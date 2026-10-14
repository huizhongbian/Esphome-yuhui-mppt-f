# yuhui-mppt
Made for mppt charginger for Yuhui or Jvyuan ,esphome base on Homeassistant systems
# ESPHome 钰辉 MPPT 控制器组件
这是为钰辉/聚源 MPPT 太阳能充电控制器编写的 ESPHome 外部组件，支持读取实时数据和设置参数。
2026.10.12 测试通过。
#    功能说明
✅ 实时数据监控	---发送 0xB1 命令 查询PV电压、电池电压、充电电流、温度等

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



# 定义传感器、开关、number 等（详见 examples 目录YAML文件）
