#include "interrupt.h"
#include "serial.h"
#include "pit.h"    // 为了 pit_handler

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
    }
    // 设置 32-47 号 IRQ（同样使用中断门）
    for (int i = 32; i < 48; i++) {
        idt_set_gate(i, (uint64_t)isr_stubs[i], 0x18, 0x8E);
    }

    // 加载 IDT
    __asm__ volatile ("lidt %0" : : "m"(idtp));


    // 打印 isr0 和 isr32 的地址
    serial_puts(COM1, "isr0 addr: ");
    serial_puthex(COM1, (uint64_t)isr_stubs[0]);
    serial_putchar(COM1, '\n');
    serial_puts(COM1, "isr32 addr: ");
    serial_puthex(COM1, (uint64_t)isr_stubs[32]);
    serial_putchar(COM1, '\n');
}

// 中断处理 C 函数
/*
void isr_handler(uint64_t int_no, uint64_t err_code) {
    // 简单打印中断号和错误码（通过串口）
    serial_puts(COM1, "ISR: ");
    serial_puthex(COM1, int_no);
    if (err_code) {
        serial_puts(COM1, " err=");
        serial_puthex(COM1, err_code);
    }
    serial_putchar(COM1, '\n');

    // 如果是时钟中断（IRQ0，映射到 32），调用 pit_handler
    if (int_no == 32) {
        pit_handler();
        return;
    }

    // 如果是页错误（14），停机
    if (int_no == 14) {
        serial_puts(COM1, "Page fault! Halting.\n");
        while(1) __asm__("hlt");
    }
    // 其他异常可添加处理
}
*/
void isr_handler(uint64_t int_no, uint64_t err_code) {//暂用简化版
    serial_putchar(COM1, 'I');
    if (int_no == 32) {
        pit_handler();   // 确保 pit_handler 也只做最小操作
    }
}