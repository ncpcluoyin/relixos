//pic.c

#include "pic.h"
#include "serial.h"
#include "io.h"

// 重新映射 IRQ
void pic_remap(int offset1, int offset2) {
    //debug out
    serial_puts(COM1,"pic_remap called\n");

    // 保存原有屏蔽字（可选）
    uint8_t a1 = inb(PIC1_DATA);
    uint8_t a2 = inb(PIC2_DATA);

    // 初始化主 PIC
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC1_DATA, offset1);      // 主偏移
    outb(PIC2_DATA, offset2);      // 从偏移
    outb(PIC1_DATA, 4);            // 主 PIC 有从 PIC 连接到 IRQ2
    outb(PIC2_DATA, 2);            // 从 PIC 的级联 ID
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);

    // 恢复原有屏蔽字（或设置新屏蔽）
    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);

    //debug out
    serial_puts(COM1,"pic_remap done\n");
}

void pic_send_eoi(int irq) {
    //debug out
    serial_puts(COM1,"pic_send_eoi called\n");
    if (irq >= 8) {
        outb(PIC2_COMMAND, 0x20);   // 非特定 EOI
    }
    outb(PIC1_COMMAND, 0x20);
}

void pic_mask_irq(int irq) {
    uint16_t port = (irq < 8) ? PIC1_DATA : PIC2_DATA;
    uint8_t mask = inb(port);
    mask |= (1 << (irq & 7));
    outb(port, mask);
}

void pic_unmask_irq(int irq) {
    uint16_t port = (irq < 8) ? PIC1_DATA : PIC2_DATA;
    uint8_t mask = inb(port);
    mask &= ~(1 << (irq & 7));
    outb(port, mask);
}
