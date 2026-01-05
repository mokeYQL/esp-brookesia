# 🔧 **EchoEar V1.2 版本完整IO配置表**


## 📊 **IO引脚功能映射表（V1.2专用）**

| **GPIO** | **功能模块** | **软件信号名** | **原理图信号** | **功能描述** |
|:--------:|:------------|:---------------|:---------------|:-------------|
| **GPIO0** | 启动控制 | `ESP_BOOT` | ESP_BOOT | 启动模式选择 |
| **GPIO1** | I2C总线 | `BSP_I2C_SCL` | I2C_SCL | I2C时钟线 |
| **GPIO2** | I2C总线 | `BSP_I2C_SDA` | I2C_SDA | I2C数据线 |
| **GPIO3** | 音频输入 | `BSP_I2S_DSIN` | I2S_DI | I2S数据输入（来自ES7210） |
| **GPIO4** | UART1接收 | `BSP_UART1_RX` | U1RXD | 串口接收 |
| **GPIO5** | UART1发送 | `BSP_UART1_TX` | U1TXD | 串口发送 |
| **GPIO6** | 触摸按键 | `BSP_TOUCH_PAD2` | TOUCH_PAD2 | 触摸按键2 |
| **GPIO7** | 触摸按键 | `BSP_TOUCH_PAD1` | TOUCH_PAD1 | 触摸按键1 |
| **GPIO9** | 电源控制 | `BSP_POWER_OFF` | POWER_CTRL | 电源开关控制 |
| **GPIO10** | 触摸屏 | `BSP_LCD_TOUCH_INT` | TP_INT | 触摸屏中断信号 |
| **GPIO11** | LCD显示 | `BSP_LCD_DATA2` | LCD_SDA2 | LCD数据线2 |
| **GPIO12** | LCD显示 | `BSP_LCD_DATA3` | LCD_SDA3 | LCD数据线3 |
| **GPIO13** | LCD显示 | `BSP_LCD_DATA1` | LCD_SDA1 | LCD数据线1 |
| **GPIO14** | LCD显示 | `BSP_LCD_CS` | LCD_CS | LCD片选信号 |
| **GPIO15** | 音频功放 | `BSP_POWER_AMP_IO` | PA_CTRL | 音频功放使能控制 |
| **GPIO16** | SD卡 | `BSP_SD_CLK` | SD_CLK | SD卡时钟 |
| **GPIO17** | SD卡 | `BSP_SD_D0` | SD_DA0 | SD卡数据线0 |
| **GPIO18** | LCD显示 | `BSP_LCD_PCLK` | LCD_PCLK | LCD像素时钟 |
| **GPIO19** | USB接口 | `BSP_USB_DM` | USB_DN | USB数据- |
| **GPIO20** | USB接口 | `BSP_USB_DP` | USB_DP | USB数据+ |
| **GPIO21** | IMU传感器 | `BSP_IMU_INT` | IMU_INT1 | IMU中断信号 |
| **GPIO38** | SD卡 | `BSP_SD_CMD` | SD_CMD | SD卡命令线 |
| **GPIO39** | 音频接口 | `BSP_I2S_LCLK` | I2S_WS | I2S字选择 |
| **GPIO40** | 音频接口 | `BSP_I2S_SCLK` | I2S_BCK | I2S位时钟 |
| **GPIO41** | 音频接口 | `BSP_I2S_DOUT` | I2S_DO | I2S数据输出（到ES8311） |
| **GPIO42** | 音频接口 | `BSP_I2S_MCLK` | I2S_MCLK | I2S主时钟 |
| **GPIO43** | LED指示灯 | `BSP_HEAD_LED` | LED_G | 绿色状态LED |
| **GPIO44** | LCD背光 | `BSP_LCD_BACKLIGHT` | LCD_BLK | LCD背光控制 |
| **GPIO45** | LCD控制 | `BSP_LCD_DC` | LCD_DC | LCD数据/命令选择 |
| **GPIO46** | LCD显示 | `BSP_LCD_DATA0` | LCD_SDA0 | LCD数据线0 |
| **GPIO47** | LCD复位 | `BSP_LCD_RST` | LCD_RST | LCD复位信号 |
| **GPIO48** | 音频电源 | `BSP_POWER_CODEC_EN` | CODEC_PWR_CTRL | 编解码器电源控制 |

---

## 🎯 **功能模块连接图（V1.2）**

```
┌─────────────────────────────────────────────────────────────┐
│                      ESP32-S3 (V1.2)                        │
├─────────┬─────────┬─────────┬─────────┬─────────┬─────────┤
│   LCD   │  Audio  │   SD    │   I2C   │  USB    │  GPIO   │
├─────────┼─────────┼─────────┼─────────┼─────────┼─────────┤
│ DATA0   │ I2S_WS  │ SD_CLK  │ I2C_SCL │ USB_DN  │ TOUCH1  │
│  GPIO46 │ GPIO39  │ GPIO16  │ GPIO1   │ GPIO19  │ GPIO7   │
├─────────┼─────────┼─────────┼─────────┼─────────┼─────────┤
│ DATA1   │ I2S_BCK │ SD_CMD  │ I2C_SDA │ USB_DP  │ TOUCH2  │
│  GPIO13 │ GPIO40  │ GPIO38  │ GPIO2   │ GPIO20  │ GPIO6   │
├─────────┼─────────┼─────────┼─────────┼─────────┼─────────┤
│ DATA2   │ I2S_DI  │ SD_D0   │ IMU_INT │         │ LED_G   │
│  GPIO11 │ GPIO3   │ GPIO17  │ GPIO21  │         │ GPIO43  │
├─────────┼─────────┼─────────┼─────────┼─────────┼─────────┤
│ DATA3   │ I2S_DO  │         │         │         │ UART_TX │
│  GPIO12 │ GPIO41  │         │         │         │ GPIO5   │
├─────────┼─────────┼─────────┼─────────┼─────────┼─────────┤
│ CS      │ I2S_MCLK│         │         │         │ UART_RX │
│  GPIO14 │ GPIO42  │         │         │         │ GPIO4   │
├─────────┼─────────┼─────────┼─────────┼─────────┼─────────┤
│ DC      │ PA_CTRL │         │         │         │ PWR_OFF │
│  GPIO45 │ GPIO15  │         │         │         │ GPIO9   │
├─────────┼─────────┼─────────┼─────────┼─────────┼─────────┤
│ RST     │ CODEC_EN│         │         │         │ BOOT    │
│  GPIO47 │ GPIO48  │         │         │         │ GPIO0   │
├─────────┼─────────┼─────────┼─────────┼─────────┼─────────┤
│ PCLK    │         │         │         │         │         │
│  GPIO18 │         │         │         │         │         │
├─────────┼─────────┼─────────┼─────────┼─────────┼─────────┤
│ BLK     │         │         │         │         │         │
│  GPIO44 │         │         │         │         │         │
└─────────┴─────────┴─────────┴─────────┴─────────┴─────────┘
```

---

## 🔗 **外设连接拓扑图**

```
           ┌─────────────────┐
           │   ESP32-S3      │
           │   (V1.2)        │
           └─────┬─┬─┬─┬─────┘
                 │ │ │ │
    ┌────────────┘ │ │ └────────────┐
    │              │ │              │
┌───▼───┐    ┌────▼─▼────┐    ┌────▼────┐
│  SD   │    │   Audio   │    │   LCD   │
│ Card  │    │  System   │    │ Display │
├───────┤    ├───────────┤    ├─────────┤
│ CLK:16│    │  WS:39    │    │ DATA0:46│
│ CMD:38│    │ BCK:40    │    │ DATA1:13│
│ D0:17 │    │  DI:3     │    │ DATA2:11│
└───────┘    │  DO:41    │    │ DATA3:12│
              │ MCLK:42   │    │  CS:14  │
              │ PA_EN:15  │    │  DC:45  │
              │ PWR:48    │    │ RST:47  │
              └───────────┘    │ PCLK:18 │
                               │ BLK:44  │
                               └─────────┘
    ┌────────────┐     ┌─────────────┐
    │   I2C Bus  │     │   USB/UART  │
    ├────────────┤     ├─────────────┤
    │ SCL:1      │     │ USB_DN:19   │
    │ SDA:2      │     │ USB_DP:20   │
    │ IMU_INT:21 │     │ UART_TX:5   │
    └────────────┘     │ UART_RX:4   │
                       └─────────────┘
            ┌─────────────────┐
            │   User Controls │
            ├─────────────────┤
            │ TOUCH1:7        │
            │ TOUCH2:6        │
            │ LED_G:43        │
            │ POWER_OFF:9     │
            │ BOOT:0          │
            └─────────────────┘
```

---

## 📋 **版本关键差异点（V1.2专属）**

1. **音频数据输入**：从GPIO15改为GPIO3
2. **LCD复位**：从GPIO3改为GPIO47
3. **新增音频编解码器电源控制**：GPIO48
4. **新增第二个触摸按键**：GPIO6
5. **UART1引脚调整**：TX=GPIO5, RX=GPIO4

---

