#include "pit.h"
#include "serial.h"
#include "io.h"
#include "interrupt.h"  // 包含中断处理声明

static volatile uint64_t pit_ticks = 0;

void pit_init(uint32_t frequency) {
    // 设置 PIT 通道 0 为模式 2（速率发生器）
    uint32_t divisor = 1193182 / frequency;  // PIT 输入时钟 1.193182 MHz
    outb(PIT_COMMAND, PIT_CMD_CH0 | PIT_CMD_RW_BOTH | PIT_CMD_MODE2);
    outb(PIT_CH0_DATA, divisor & 0xFF);       // 低字节
    outb(PIT_CH0_DATA, (divisor >> 8) & 0xFF); // 高字节

    // 启用 IRQ0 中断（对应 PIC 偏移后的向量）
    pic_unmask_irq(0);
}

uint64_t pit_get_ticks(void) {
    return pit_ticks;
}

// 时钟中断处理函数（由 isr_handler 调用，或通过专门的 IRQ 处理）
void pit_handler(void) {
    pit_ticks++;
    // 可输出一个字符用于调试（但频繁输出会拖慢速度）
    // serial_putchar(COM1, '.');
    pic_send_eoi(0);   // 发送 EOI 给 IRQ0
}

void irq0_c_handler(void) {
    pit_ticks++;
    pic_send_eoi(0);
}
