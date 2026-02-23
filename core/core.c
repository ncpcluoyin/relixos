#include "serial.h"
#include "interrupt.h"
#include "pic.h"
#include "pit.h"
#include "io.h"

void core_main(uint64_t mb_info_addr) {
    serial_init(COM1);
    serial_puts(COM1,"RelixOS started\n");
    serial_puts(COM1,"run idt_init\n");
    idt_init();
    serial_puts(COM1,"run pic_remap\n");
    pic_remap(32, 40);       // 将 IRQ0-7 映射到 32-39，IRQ8-15 映射到 40-47
    serial_puts(COM1,"ignore IRQ\n");
    outb(PIC1_DATA, 0xFF);  // 屏蔽主片所有 IRQ
    outb(PIC2_DATA, 0xFF);  // 屏蔽从片所有 IRQ
    serial_puts(COM1,"run pit_init\n");
    pit_init(100);            // 100 Hz 时钟
    serial_puts(COM1,"start int\n");

    __asm__ volatile ("int $0x32");   // 触发 IRQ0 对应的中断向量
    
    __asm__ volatile ("sti"); // 开启中断
    uint64_t last_ticks = 0;
    while (1) {
        uint64_t ticks = pit_get_ticks();
        if (ticks != last_ticks) {
            serial_putchar(COM1, '.');
            last_ticks = ticks;
        }
        __asm__ volatile ("hlt");
    }
}