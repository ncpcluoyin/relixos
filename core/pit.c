//pit.c

#include "pit.h"
#include "serial.h"
#include "io.h"
#include "interrupt.h"  // 包含中断处理声明
#include "pic.h"

static volatile uint64_t pit_ticks = 0;

void pit_init(uint32_t frequency) {
    // 使用四舍五入计算除数，使实际频率更接近期望值
    uint32_t divisor = (1193182 + frequency/2) / frequency;
    outb(PIT_COMMAND, PIT_CMD_CH0 | PIT_CMD_RW_BOTH | PIT_CMD_MODE2);
    outb(PIT_CH0_DATA, divisor & 0xFF);
    outb(PIT_CH0_DATA, (divisor >> 8) & 0xFF);
    pic_unmask_irq(0);
}

uint64_t pit_get_ticks(void) {
    return pit_ticks;
}

// 时钟中断处理函数（由 isr_handler 调用，或通过专门的 IRQ 处理）
void pit_handler(void) {
    pit_ticks++;
    serial_putchar(COM1, 'P');   // 每次时钟中断输出 'P'
    pic_send_eoi(0);
}