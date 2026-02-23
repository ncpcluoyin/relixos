//interrupt.c

#include "interrupt.h"
#include "pit.h"
#include "serial.h"

// 外部数组，指向所有中断入口（在 boot.S 中定义）
extern void* isr_stubs[];

#define IDT_SIZE 256
static struct idt_entry idt[IDT_SIZE];
static struct idt_ptr idtp;

// 设置一个 IDT 条目
static void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low  = base & 0xFFFF;
    idt[num].base_mid  = (base >> 16) & 0xFFFF;
    idt[num].base_high = (base >> 32) & 0xFFFFFFFF;
    idt[num].sel       = sel;
    idt[num].ist       = 0;
    idt[num].flags     = flags | 0x80; // 设置 Present 位
    idt[num].reserved  = 0;
}

void idt_init(void) {
    idtp.limit = sizeof(idt) - 1;
    idtp.base  = (uint64_t)&idt;

    // 设置 0-31 号异常（使用 64 位代码段 0x18，中断门标志 0x8E）
    for (int i = 0; i < 32; i++) {
        idt_set_gate(i, (uint64_t)isr_stubs[i], 0x18, 0x8E);
        //debug out
        serial_puts(COM1,"set gate:");
        serial_putnum(COM1,i);
        serial_puts(COM1," addr:");
        serial_putnum(COM1,isr_stubs[i]);
        serial_putchar(COM1,'\n');
    }
    // 设置 32-47 号 IRQ（同样使用中断门）
    for (int i = 32; i < 48; i++) {
        idt_set_gate(i, (uint64_t)isr_stubs[i], 0x18, 0x8E);
        //debug out
        serial_puts(COM1,"set gate:");
        serial_putnum(COM1,i);
        serial_puts(COM1," addr:");
        serial_putnum(COM1,isr_stubs[i]);
        serial_putchar(COM1,'\n');
    }

    // 加载 IDT
    __asm__ volatile ("lidt %0" : : "m"(idtp));
}

// 中断处理 C 函数

void isr_handler(uint64_t int_no, uint64_t err_code) {
    serial_puts(COM1, "ISR: ");
    serial_puthex(COM1, int_no);
    if (err_code) {
        serial_puts(COM1, " err=");
        serial_puthex(COM1, err_code);
    }
    serial_putchar(COM1, '\n');

    if (int_no == 32) {
        pit_handler();
    }
}