#include "interrupt.h"
#include "serial.h"
#include "io.h"

#define IDT_SIZE 256
static struct idt_entry idt[IDT_SIZE];
static struct idt_ptr idtp;

extern void irq0_handler(void);   // 来自 boot.S

static void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low  = base & 0xFFFF;
    idt[num].base_mid  = (base >> 16) & 0xFFFF;
    idt[num].base_high = (base >> 32) & 0xFFFFFFFF;
    idt[num].sel       = sel;
    idt[num].ist       = 0;
    idt[num].flags     = flags;   // 直接使用传入的 flags，应包含存在位
    idt[num].reserved  = 0;
}

void idt_init(void) {
    // 先清零所有条目（可选，但确保未使用的条目 P=0 会导致 GPF，所以最好设置一个默认处理）
    // 为简化，我们只设置向量32，其他条目保持未使用（P=0）。但 CPU 可能会因为未初始化而触发异常。
    // 为了安全，可以将所有条目设为同一个处理函数（比如一个空处理），但这里先跳过。
    // 我们假设在开启中断前不会发生其他中断，所以可以只设置需要的。
    for (int i = 0; i < IDT_SIZE; i++) {
        // 将所有条目初始化为空（P=0），这样如果意外触发会引发 GPF，但我们可以捕捉到？
        // 更好的做法是设置一个默认处理，但为了简化，暂时留空。
        idt[i].flags = 0; // 存在位为0，任何未设置的中断都会引起异常
    }

    uint64_t addr = (uint64_t)irq0_handler;
    idt_set_gate(32, addr, 0x18, 0x8E); // 0x18 是64位代码段选择子，0x8E 是存在+中断门

    idtp.limit = sizeof(idt) - 1;
    idtp.base  = (uint64_t)&idt;
    __asm__ volatile ("lidt %0" : : "m"(idtp));
}