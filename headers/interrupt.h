#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>

// IDT 条目（64 位模式）
struct idt_entry {
    uint16_t base_low;      // 处理程序地址低 16 位
    uint16_t sel;           // 代码段选择子（内核代码段，0x18）
    uint8_t  ist;           // IST 栈（未使用，设为 0）
    uint8_t  flags;         // 标志：P=1, DPL=0, 类型=0xE（中断门）
    uint16_t base_mid;      // 处理程序地址中 16 位
    uint32_t base_high;     // 处理程序地址高 32 位
    uint32_t reserved;      // 保留
} __attribute__((packed));

// IDT 指针（用于 lidt）
struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

// 初始化 IDT
void idt_init(void);

// 中断处理函数（由汇编调用）
void isr_handler(uint64_t int_no, uint64_t err_code);

#endif