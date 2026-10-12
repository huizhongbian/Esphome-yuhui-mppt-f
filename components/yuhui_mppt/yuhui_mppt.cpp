#include "yuhui_mppt.h"
#include "esphome/core/log.h"
#include <cstdlib>
#include <ctime>   // 用于 time/localtime

namespace esphome {
namespace yuhui_mppt {

static const char *const TAG = "yuhui_mppt";

void YuhuiMppt::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Yuhui MPPT...");
  while (available()) read();
  // 初始化开关状态
  if (disable_charge_switch_) {
    disable_charge_switch_->add_on_state_callback([this](bool state) {
      this->send_control_command_(state ? 0x01 : 0x02);
    });
    disable_charge_switch_->publish_state(true);      // true开false关
  }
  if (dc_output_switch_) {
    dc_output_switch_->add_on_state_callback([this](bool state) {
      this->send_control_command_(state ? 0x03 : 0x04);
    });
    dc_output_switch_->publish_state(true);      // true开false关
  }
  // 初始化Button
  if (clock_calibration_button_) {
    clock_calibration_button_->add_on_press_callback([this]() {
        this->send_clock_calibration_command();
    });
  }
  // 初始化NUMBER
  if (max_current_number_) {
    max_current_number_->add_on_state_callback([this](float value) {
      if (!skip_number_callback_) {
        this->on_max_current_value(value);
      }
    });
  }
  if (max_voltage_number_) {
    max_voltage_number_->add_on_state_callback([this](float value) {
      if (!skip_number_callback_) {
        this->on_max_voltage_value(value);
      }
    });
  }

}
  // 发送 0xB1 命令查询数据
void YuhuiMppt::update() {
  std::vector<uint8_t> command;
  command.push_back(address_);
  command.push_back(0xB1);
  command.push_back(0x01);
  for (int i = 0; i < 4; i++) command.push_back(0x00);
  uint8_t sum = 0;
  for (auto b : command) sum += b;
  command.push_back(sum);
  send_command_(command);
  read_and_parse_response_();
}

void YuhuiMppt::send_command_(const std::vector<uint8_t> &data) {
  write_array(data.data(), data.size());
  flush();
}
  // 发送控制命令
void YuhuiMppt::send_control_command_(uint8_t command_code) {
  std::vector<uint8_t> cmd;
  cmd.push_back(address_);
  cmd.push_back(0xC0);
  cmd.push_back(command_code);
  for (int i = 0; i < 4; i++) cmd.push_back(0x00);
  uint8_t sum = 0;
  for (auto b : cmd) sum += b;
  cmd.push_back(sum);
  send_command_(cmd);
  delay(100);
  while (available()) read();
}
  // 发送时间同步命令
void YuhuiMppt::send_clock_calibration_command() {
  time_t now = time(nullptr);
  if (now < 1000000000) {
    ESP_LOGW(TAG, "System time not synced yet, clock calibration skipped");
    return;
  }
  
  struct tm *timeinfo = localtime(&now);
  
  uint8_t year   = (timeinfo->tm_year + 1900) % 100;
  uint8_t month  = timeinfo->tm_mon + 1;
  uint8_t day    = timeinfo->tm_mday;
  uint8_t hour   = timeinfo->tm_hour;
  uint8_t minute = timeinfo->tm_min;
  
  std::vector<uint8_t> cmd;
  cmd.push_back(0x00);
  cmd.push_back(0xDF);
  cmd.push_back(year);
  cmd.push_back(month);
  cmd.push_back(day);
  cmd.push_back(hour);
  cmd.push_back(minute);
  
  uint8_t sum = 0;
  for (auto b : cmd) sum += b;
  cmd.push_back(sum);
  
  send_command_(cmd);
  
  ESP_LOGI(TAG, "Clock calibration sent: 20%02d-%02d-%02d %02d:%02d", 
           year, month, day, hour, minute);
  
  delay(100);
  while (available()) read();
}
  // 发送参数命令
void YuhuiMppt::send_parameter_command_(uint8_t param_code, uint16_t value) {
  std::vector<uint8_t> cmd;
  cmd.push_back(address_);
  cmd.push_back(0xD0);
  cmd.push_back(param_code);
  cmd.push_back(0x00);
  cmd.push_back(0x00);
  cmd.push_back((value >> 8) & 0xFF);
  cmd.push_back(value & 0xFF);
  uint8_t sum = 0;
  for (auto b : cmd) sum += b;
  cmd.push_back(sum);
  send_command_(cmd);
  delay(150);
  while (available()) read();
}
  // 读取参数
bool YuhuiMppt::read_parameter_(uint8_t param_code, uint16_t &result) {
  std::vector<uint8_t> cmd;
  cmd.push_back(address_);
  cmd.push_back(0xD0);
  cmd.push_back(param_code);
  for (int i = 0; i < 4; i++) cmd.push_back(0x00);
  uint8_t sum = 0;
  for (auto b : cmd) sum += b;
  cmd.push_back(sum);
  send_command_(cmd);
  uint32_t start = millis();
  std::vector<uint8_t> response;
  while (response.size() < 8 && millis() - start < 800) {
    while (available()) {
      response.push_back(read());
    }
    delay(5);
  }
  if (response.size() != 8) {
    ESP_LOGW(TAG, "Parameter read timeout, got %d bytes", response.size());
    return false;
  }
  uint8_t calc = 0;
  for (size_t i = 0; i < 7; i++) calc += response[i];
  if (calc != response[7]) {
    ESP_LOGW(TAG, "Parameter read checksum mismatch");
    return false;
  }
  if (response[0] != address_ || response[1] != 0xD0) {
    ESP_LOGW(TAG, "Parameter read address/command mismatch");
    return false;
  }
  result = (response[5] << 8) | response[6];
  return true;
}
  // 最大电流值
void YuhuiMppt::on_max_current_value(float value) {
  if (value < 0.1f) value = 0.1f;
  if (value > 130.0f) value = 130.0f;
  uint16_t current_ma = (uint16_t)(value * 100);
  send_parameter_command_(0x25, current_ma);
  ESP_LOGI(TAG, "Set max charging current to %.1fA", value);
}
  // 浮充电压
void YuhuiMppt::on_max_voltage_value(float value) {
  if (value < 0.1f) value = 0.1f;
  if (value > 140.0f) value = 140.0f;
  uint16_t voltage_ma = (uint16_t)(value * 100);
  send_parameter_command_(0x22, voltage_ma);
  ESP_LOGI(TAG, "Set max charging voltage to %.1fV", value);
}
  // 读取并解析响应
bool YuhuiMppt::read_and_parse_response_() {
  for (int retry = 0; retry < 2; retry++) {
    uint32_t start = millis();
    std::vector<uint8_t> buffer;
    while (millis() - start < 500) {
      while (available()) {
        buffer.push_back(read());
      }
      if (buffer.size() >= 3) delay(10);
    }

    if (buffer.empty()) {
      ESP_LOGW(TAG, "No data received, retry %d", retry);
      delay(100);
      continue;
    }

    for (size_t i = 0; i + 2 < buffer.size(); i++) {
      if (buffer[i] == address_ && buffer[i+1] == 0xB1) {
        size_t frame_len = buffer.size() - i;
        if (frame_len < 93) continue;
        uint8_t calc = 0;
        for (size_t j = i; j < i + frame_len - 1; j++) calc += buffer[j];
        if (calc != buffer[i + frame_len - 1]) continue;
        std::vector<uint8_t> frame(buffer.begin() + i, buffer.begin() + i + frame_len);
        return parse_frame_(frame);
      }
    }
    ESP_LOGW(TAG, "No valid frame, retry %d", retry);
    delay(100);
  }
  ESP_LOGW(TAG, "Failed to parse B1 response after retries");
  return false;
}
  // 解析帧
bool YuhuiMppt::parse_frame_(const std::vector<uint8_t> &data) {
  if (data.size() < 93) {
    ESP_LOGW(TAG, "Parse frame too short (%d bytes)", data.size());
    return false;
  }
  uint16_t raw_fcv = (data[20] << 8) | data[21];
  float charging_voltage = raw_fcv / 100.0f;
  
  uint16_t raw_max_cur = (data[26] << 8) | data[27];
  float maxcharging_current = raw_max_cur / 100.0f;

  uint16_t raw_pv = (data[30] << 8) | data[31];
  float pv_voltage = raw_pv / 10.0f;

  uint16_t raw_bat = (data[32] << 8) | data[33];
  float battery_voltage = raw_bat / 100.0f;

  uint16_t raw_cur = (data[34] << 8) | data[35];
  float charging_current = raw_cur / 100.0f;

  uint16_t raw_temp_int = (data[36] << 8) | data[37];
  float internal_temp = raw_temp_int / 10.0f;

  uint16_t raw_temp_ext = (data[40] << 8) | data[41];
  float external_temp = raw_temp_ext / 10.0f;

  uint32_t raw_daily = (data[44] << 24) | (data[45] << 16) | (data[46] << 8) | data[47];
  float daily_energy_kwh = raw_daily / 1000.0f;

  uint32_t raw_total = (data[48] << 24) | (data[49] << 16) | (data[50] << 8) | data[51];
  float total_energy_kwh = raw_total / 1000.0f;

  float charging_power = battery_voltage * charging_current;

  if (charging_voltage_sensor_) charging_voltage_sensor_->publish_state(charging_voltage);
  if (pv_voltage_sensor_) pv_voltage_sensor_->publish_state(pv_voltage);
  if (battery_voltage_sensor_) battery_voltage_sensor_->publish_state(battery_voltage);
  if (charging_current_sensor_) charging_current_sensor_->publish_state(charging_current);
  if (maxcharging_current_sensor_) maxcharging_current_sensor_->publish_state(maxcharging_current);
  if (charging_power_sensor_) charging_power_sensor_->publish_state(charging_power);
  if (internal_temperature_sensor_) internal_temperature_sensor_->publish_state(internal_temp);
  if (external_temperature_sensor_) external_temperature_sensor_->publish_state(external_temp);
  if (daily_energy_sensor_) daily_energy_sensor_->publish_state(daily_energy_kwh);
  if (total_energy_sensor_) total_energy_sensor_->publish_state(total_energy_kwh);

  ESP_LOGI(TAG, "Data: Fcv=%.2fV PV=%.1fV Bat=%.2fV Cur=%.2fA MCur=%.2fA Pow=%.1fW IntT=%.1f°C ExtT=%.1f°C Daily=%.2fkWh Total=%.2fkWh",
           charging_voltage, pv_voltage, battery_voltage, charging_current,maxcharging_current, charging_power,
           internal_temp, external_temp, daily_energy_kwh, total_energy_kwh);
  return true;
}
  // 转储配置
void YuhuiMppt::dump_config() {
  ESP_LOGCONFIG(TAG, "Yuhui MPPT:");
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", address_);
}

}  // namespace yuhui_mppt
}  // namespace esphome