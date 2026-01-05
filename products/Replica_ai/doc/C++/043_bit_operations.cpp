/*
 * 知识点：位操作与寄存器访问
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\system.cpp 第534-547行
 * C++ Primer中文版（第五版）：第4章 表达式 - 4.8 位运算符
 * Effective+C++中文版（第三版）：无直接对应章节
 */
#include <iostream>
#include <cstdint>

// 模拟寄存器地址和位定义
#define USB_SERIAL_JTAG_CONF0_REG    0x3FF4F020
#define USB_SERIAL_JTAG_PAD_PULL_OVERRIDE    (1U << 22)
#define USB_SERIAL_JTAG_DP_PULLUP            (1U << 23)
#define USB_SERIAL_JTAG_DP_PULLDOWN          (1U << 24)

// 模拟寄存器读写函数
static inline void SET_PERI_REG_MASK(uint32_t reg, uint32_t mask) {
    std::cout << "SET_PERI_REG_MASK(0x" << std::hex << reg << ", 0x" << mask << ")" << std::dec << std::endl;
    // 实际实现：*(volatile uint32_t*)reg |= mask;
}

static inline void CLEAR_PERI_REG_MASK(uint32_t reg, uint32_t mask) {
    std::cout << "CLEAR_PERI_REG_MASK(0x" << std::hex << reg << ", 0x" << mask << ")" << std::dec << std::endl;
    // 实际实现：*(volatile uint32_t*)reg &= ~mask;
}

// 模拟延时函数
static inline void vTaskDelay(uint32_t ticks) {
    std::cout << "vTaskDelay(" << ticks << ")" << std::endl;
    // 实际实现：延时指定的tick数
}

// 模拟宏
#define pdMS_TO_TICKS(ms)    ((ms) / portTICK_PERIOD_MS)
#define portTICK_PERIOD_MS    1

// 模拟USB SERIAL JTAG LL函数
namespace usb_serial_jtag_ll {
    void phy_enable_external(bool enable) {
        std::cout << "usb_serial_jtag_ll_phy_enable_external(" << (enable ? "true" : "false") << ")" << std::endl;
    }
    
    void phy_enable_pad(bool enable) {
        std::cout << "usb_serial_jtag_ll_phy_enable_pad(" << (enable ? "true" : "false") << ")" << std::endl;
    }
    
    void phy_set_defaults() {
        std::cout << "usb_serial_jtag_ll_phy_set_defaults()" << std::endl;
    }
}

// 模拟宏定义
#define USB_SERIAL_JTAG_LL_EXT_PHY_SUPPORTED    1

static void _usb_serial_jtag_phy_init() {
    // 覆盖USB PAD的上下拉设置
    SET_PERI_REG_MASK(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_PAD_PULL_OVERRIDE);
    
    // 清除上拉，设置下拉
    CLEAR_PERI_REG_MASK(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_DP_PULLUP);
    SET_PERI_REG_MASK(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_DP_PULLDOWN);
    
    // 延时10ms
    vTaskDelay(pdMS_TO_TICKS(10));
    
#if USB_SERIAL_JTAG_LL_EXT_PHY_SUPPORTED
    // 使用内部PHY
    usb_serial_jtag_ll::phy_enable_external(false);
    // 启用USB PHY pads
    usb_serial_jtag_ll::phy_enable_pad(true);
#else
    // 外部PHY不支持，设置默认值
    usb_serial_jtag_ll::phy_set_defaults();
#endif
    
    // 清除下拉，设置上拉
    CLEAR_PERI_REG_MASK(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_DP_PULLDOWN);
    SET_PERI_REG_MASK(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_DP_PULLUP);
    
    // 取消覆盖，恢复默认上下拉控制
    CLEAR_PERI_REG_MASK(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_PAD_PULL_OVERRIDE);
}

// 演示位操作的其他用法
void demonstrate_bit_operations() {
    uint32_t reg_value = 0x12345678;
    
    std::cout << "\n=== Demonstrating Bit Operations ===" << std::endl;
    std::cout << "Initial reg_value: 0x" << std::hex << reg_value << std::dec << std::endl;
    
    // 设置特定位
    reg_value |= (1U << 8);
    std::cout << "After setting bit 8: 0x" << std::hex << reg_value << std::dec << std::endl;
    
    // 清除特定位
    reg_value &= ~(1U << 12);
    std::cout << "After clearing bit 12: 0x" << std::hex << reg_value << std::dec << std::endl;
    
    // 切换特定位
    reg_value ^= (1U << 16);
    std::cout << "After toggling bit 16: 0x" << std::hex << reg_value << std::dec << std::endl;
    
    // 检查特定位是否设置
    bool bit20_set = (reg_value & (1U << 20)) != 0;
    std::cout << "Bit 20 is " << (bit20_set ? "set" : "clear") << std::endl;
    
    // 设置多个位
    reg_value |= ((1U << 4) | (1U << 5) | (1U << 6));
    std::cout << "After setting bits 4-6: 0x" << std::hex << reg_value << std::dec << std::endl;
    
    // 清除多个位
    reg_value &= ~((1U << 24) | (1U << 25) | (1U << 26));
    std::cout << "After clearing bits 24-26: 0x" << std::hex << reg_value << std::dec << std::endl;
    
    // 提取特定位字段
    uint32_t bit_field = (reg_value >> 8) & 0xFF;
    std::cout << "Bits 8-15: 0x" << std::hex << bit_field << std::dec << std::endl;
}

int main() {
    std::cout << "=== Testing USB Serial JTAG PHY Initialization ===" << std::endl;
    _usb_serial_jtag_phy_init();
    
    demonstrate_bit_operations();
    
    return 0;
}
