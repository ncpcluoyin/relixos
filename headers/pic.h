#ifndef PIC_H
#define PIC_H

// 重新映射 IRQ 0-15 到 IDT 向量 32-47
void pic_remap(int offset1, int offset2);

// 发送 EOI（中断结束）给主/从 PIC
void pic_send_eoi(int irq);

// 屏蔽/取消屏蔽 IRQ
void pic_mask_irq(int irq);
void pic_unmask_irq(int irq);

#define PIC1        0x20    // IO 基地址（主）
#define PIC2        0xA0    // IO 基地址（从）
#define PIC1_COMMAND PIC1
#define PIC1_DATA   (PIC1+1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA   (PIC2+1)

#define ICW1_ICW4       0x01        // 需要 ICW4
#define ICW1_SINGLE     0x02        // 单级模式
#define ICW1_INTERVAL4  0x04        // 调用地址间隔 4
#define ICW1_LEVEL      0x08        // 电平触发
#define ICW1_INIT       0x10        // 初始化

#define ICW4_8086       0x01        // 8086/88 模式
#define ICW4_AUTO       0x02        // 自动 EOI
#define ICW4_BUF_SLAVE  0x08        // 缓冲模式从
#define ICW4_BUF_MASTER 0x0C        // 缓冲模式主
#define ICW4_SFNM       0x10        // 特殊完全嵌套

#endif