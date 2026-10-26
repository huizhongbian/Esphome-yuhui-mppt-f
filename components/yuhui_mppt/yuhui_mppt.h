#pragma once
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/number/number.h"
#include "esphome/components/button/button.h"
#include <vector>

namespace esphome {
namespace yuhui_mppt {
  // 公共轮询组件
class YuhuiMppt : public PollingComponent, public uart::UARTDevice {
 public:
  void set_address(uint8_t addr) { address_ = addr; }

  // 传感器
  void set_rated_voltage_sensor(sensor::Sensor *s) { rated_voltage_sensor_ = s; }
  void set_charging_voltage_sensor(sensor::Sensor *s) { charging_voltage_sensor_ = s; }
  void set_pv_voltage_sensor(sensor::Sensor *s) { pv_voltage_sensor_ = s; }
  void set_battery_voltage_sensor(sensor::Sensor *s) { battery_voltage_sensor_ = s; }
  void set_charging_current_sensor(sensor::Sensor *s) { charging_current_sensor_ = s; }
  void set_rated_current_sensor(sensor::Sensor *s) { rated_current_sensor_ = s; }
  void set_maxcharging_current_sensor(sensor::Sensor *s) { maxcharging_current_sensor_ = s; }
  void set_charging_power_sensor(sensor::Sensor *s) { charging_power_sensor_ = s; }
  void set_internal_temperature_sensor(sensor::Sensor *s) { internal_temperature_sensor_ = s; }
  void set_external_temperature_sensor(sensor::Sensor *s) { external_temperature_sensor_ = s; }
  void set_daily_energy_sensor(sensor::Sensor *s) { daily_energy_sensor_ = s; }
  void set_total_energy_sensor(sensor::Sensor *s) { total_energy_sensor_ = s; }

  // 开关
  void set_charge_switch(switch_::Switch *s) { charge_switch_ = s; }
  void set_dc_output_switch(switch_::Switch *s) { dc_output_switch_ = s; }

  // Number 实体
  void set_max_current_number(number::Number *n) { max_current_number_ = n; }
  void set_max_voltage_number(number::Number *n) { max_voltage_number_ = n; }

  // Button 实体
  void set_clock_calibration_button(button::Button *b) { clock_calibration_button_ = b; }
  
  void setup() override;
  void update() override;
  void dump_config() override;

 protected:
  uint8_t address_{1};
  // 传感器
  sensor::Sensor *rated_voltage_sensor_{nullptr};
  sensor::Sensor *charging_voltage_sensor_{nullptr};
  sensor::Sensor *pv_voltage_sensor_{nullptr};
  sensor::Sensor *battery_voltage_sensor_{nullptr};
  sensor::Sensor *charging_current_sensor_{nullptr};
  sensor::Sensor *rated_current_sensor_{nullptr};
  sensor::Sensor *maxcharging_current_sensor_{nullptr};
  sensor::Sensor *charging_power_sensor_{nullptr};
  sensor::Sensor *internal_temperature_sensor_{nullptr};
  sensor::Sensor *external_temperature_sensor_{nullptr};
  sensor::Sensor *daily_energy_sensor_{nullptr};
  sensor::Sensor *total_energy_sensor_{nullptr};
  // 开关
  switch_::Switch *charge_switch_{nullptr};
  switch_::Switch *dc_output_switch_{nullptr};
  // Number 实体
  number::Number *max_current_number_{nullptr};
  number::Number *max_voltage_number_{nullptr};

  // Button 实体
  button::Button *clock_calibration_button_{nullptr};

  void send_command_(const std::vector<uint8_t> &data);
  void send_control_command_(uint8_t command_code);
  void send_parameter_command_(uint8_t param_code, uint16_t value);
  bool read_parameter_(uint8_t param_code, uint16_t &result);
  bool read_and_parse_response_();
  bool parse_frame_(const std::vector<uint8_t> &frame);
  void on_max_current_value(float value);
  void on_max_voltage_value(float value);
  void send_clock_calibration_command();

};

}  // namespace yuhui_mppt
}  // namespace esphome