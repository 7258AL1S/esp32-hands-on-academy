# 基线板、器件与接线

本课程基线是 **经典 ESP32-DevKitC V4 / ESP32-WROOM / 4 MB+ Flash**。
ESP32 是家族名，ESP32-S2/S3/C3 的引脚、ADC、UART、BLE/USB 能力不能按本表直接套用。
先核对模组文字、原理图与官方 pinout，再填写自己的引脚表。下图为连接关系，不是排针孔位。

## H01：按钮和外部 LED

```text
GPIO18 ── 330 Ω ── LED anode (+)
                        cathode (-) ── GND
GPIO19 ── button ── GND
          input pull-up enabled
```

四脚轻触按钮同侧两脚常常内部相连，使用万用表确认实际导通关系。
按下把 GPIO19 接到 GND：LOW=pressed；松开由内部上拉到 HIGH。
LED 极性需查器件，长脚仅是未剪脚器件的辅助判断。GPIO 不能直接驱动电机/大电流灯条。
电阻串联即可，不要求必须在 LED 阳极一侧；不能省掉电阻。

## H04–H05：分批加外设

| 实验 | 接线 | 注意 |
|---|---|---|
| ADC | 10 kΩ 电位器两端 3.3 V/GND，滑动端 GPIO34 | 经典 ESP32 ADC1_CH6；仅输入，无内部上拉；不能接 5 V |
| UART2 回环 | GPIO17 TX → GPIO16 RX | 只接本板；WROVER 的 16/17 可能被 PSRAM 占用 |
| BH1750 | 3.3 V/GND、SDA21、SCL22、ADDR low | 模块必须兼容 3.3 V；上拉到 3.3 V，不到 5 V |
| SPI 回环 | MOSI13 → MISO32，SCLK14，CS27 | 外部设备全部断开，仅测试回环；不用 GPIO12 |
| SPI 显示扩展 | 按实际屏幕资料选 DC/RST 和其他脚 | 回环代码没有屏幕初始化，不能直接当显示驱动 |

一次只做一个总线实验，确认功能后再整合。SPI/ADC/I2C 器件并非 H00 的前置购买项。
GPIO6–11 常用于 Flash，不作通用实验脚；GPIO34–39 输入专用；启动 strap GPIO 避免
新手外接不明确电平。5 V 供电引脚不意味着 GPIO 容忍 5 V。

## 每次上电前

- [ ] 断开 USB/电源后修改接线；核对真实板型与 GPIO 标签。
- [ ] 3.3 V、GND、极性、电阻、额定值正确；无电源短路。
- [ ] 按钮与电位器未把 GPIO 连接到 5 V；输入/输出能力匹配。
- [ ] 未同时用多个电源造成回灌；按板卡官方供电方式选择。
- [ ] 串口与已知稳定固件可用于恢复。

理论按实际问题补齐：H01 回路/欧姆定律/上拉，H03 抖动/采样，H04 分压/量化/校准，
H05 上拉/总线/时序。万用表不足以测 Interrupt 延迟或 PWM 波形，需要相应仪器。

官方：[DevKitC V4](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitc/user_guide.html)、
[ESP32 datasheet](https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf)、
[GPIO v5.5.1](https://docs.espressif.com/projects/esp-idf/en/v5.5.1/esp32/api-reference/peripherals/gpio.html)。
