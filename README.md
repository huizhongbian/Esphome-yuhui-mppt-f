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
  

## 接线

| ESP8266 | RS485转TTL模块 | MPPT 端子 |
|---------|------------|-----------|
| TX(GPIO1) | Rx-----A | A+ |
| RX(GPIO3) | TX-----B | B- |
| GND | GND | GND |
| 5V | VCC | 可选 |

波特率：9600 8 N 1

## 设备报错：
如果连接正常，读出来的数据为“未知”，esphome窗口传回错误为B3,重点查485转TTL硬件，正常两点灯都亮。

## 更新ESPhome设备版本方法：
 如果以前安装成功的，要更新最新的版本，要重新清理以前的缓存文件，在ESPHome Builder应用里面，找到你的设备卡片，点击卡片右侧或下方的 三个点 ⋮（更多操作），在弹出菜单里找到：
中文界面：“清理构建文件” 或 “清除构建文件”
英文界面：Clean Build Files
清理文件，重新下载编译就行。如果不行，则加一行代码强制更新  refresh: 0s 。

external_components:
  - source: github://sayinzest/Esphome-yuhui-mppt@main
  - 
    components: [ yuhui_mppt ]
    
    refresh: 0s   # 强制每次编译都拉取最新



## 配置Yaml使用方法：

在您的 ESPHome YAML 配置中添加：

```yaml
external_components:
  - source: github://sayinzest/Esphome-yuhui-mppt@main
    components: [ yuhui_mppt ]

uart:
  tx_pin: GPIO1
  rx_pin: GPIO3
  baud_rate: 9600

# 定义传感器、开关、number 等（详见 examples 目录）
