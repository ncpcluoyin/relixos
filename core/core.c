//core.c

#include "serial.h"
#include "interrupt.h"
#include "pic.h"
#include "pit.h"
#include "io.h"

void core_main(uint64_t mb_info_addr) {
    serial_init(COM1);
    serial_puts(COM1, "RelixOS started\n");

    idt_init();
    pic_remap(32, 40);

    outb(PIC1_DATA, 0xFF);   // 屏蔽所有 IRQ（PIT 初始化会取消屏蔽 IRQ0）
    outb(PIC2_DATA, 0xFF);

    pit_init(100);            // 初始化 PIT，频率 100 Hz
    

    serial_puts(COM1, "Testing software interrupt...\n");
    __asm__ volatile ("int $0x19");
    serial_puts(COM1, "Software interrupt returned.\n");

    //debug out
    serial_puts(COM1,"sti\n");
    __asm__ volatile ("sti"); // 开启中断

    //debug out
    serial_puts(COM1,"seeking ticks\n");
    uint64_t last_ticks = 0;
    while (1) {
        uint64_t ticks = pit_get_ticks();
        if (ticks != last_ticks) {
            serial_putchar(COM1, '.');
            last_ticks = ticks;
        }
        //debug out
        serial_puts(COM1,"trying\n");
    }
}