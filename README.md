# yuhui-mppt
Made for mppt charginger for Yuhui or Jvyuan ,esphome base on Homeassistant systems
# ESPHome 钰辉 MPPT 控制器组件

[![ESPHome](https://img.shields.io/badge/ESPHome-2026.4.5-blue)](https://esphome.io)

这是为钰辉/聚源 MPPT 太阳能充电控制器编写的 ESPHome 外部组件，支持读取实时数据和设置参数。
欢迎大家TEST,ESPHome Device Builder 当前版本：2026.10.12 测试通过。


## 功能

     功能	           说明
✅ 实时数据监控	---发送 0xB1 命令 查询 电流限制、电压限制、PV电压、电池电压、充电电流、温度、电量等

✅ 最大充电电流调节---number 实体，发送 0xD0 0x25 命令

✅ 最大充电电压调节---number 实体，发送 0xD0 0x22 命令

✅ 禁止充电开关---发送 0xC0 0x01/0x02 命令

✅ DC输出开关---发送 0xC0 0x03/0x04 命令

✅ MPPT时间同步---按下按钮发送 0xDF 命令
  
<img width="398" height="1103" alt="yuhuimppt" src="https://github.com/user-attachments/assets/984b63e4-bba8-4f49-8856-3b06d673c2f5" />


| ESP32 | RS485转TTL模块 | MPPT 端子 |
|---------|------------|-----------|
| TX(GPIO16) | Rx-----A | A+ |
| RX(GPIO17) | TX-----B | B- |
| GND | GND | GND |
| 3.3V | VCC | 可选 |

波特率：9600 8 N 1


## 配置Yaml使用方法：

在您的 ESPHome YAML 配置中添加：

```yaml
external_components:
  - source: github://huizhongbian/Esphome-yuhui-mppt-f@main
    components: [ yuhui_mppt ]

uart:
  tx_pin: GPIO16
  rx_pin: GPIO17
  baud_rate: 9600

# 定义传感器、开关、number 等（详见 examples 目录）
