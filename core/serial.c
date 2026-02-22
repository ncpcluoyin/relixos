#include "serial.h"

static uint16_t com_port = 0x3F8;  // 默认 COM1

// 内联汇编 I/O 函数

static inline uint8_t inb(int port)
{
	int ret;
	asm ("xorl %eax,%eax");
	asm ("inb %%dx,%%al":"=a" (ret):"d"(port));
	return ret;
}

static inline void outb(unsigned short port, uint8_t data) {
    __asm__ volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}


void serial_init(uint16_t port) {
    com_port = port;

    outb(com_port + 1, 0x00);    // 关闭中断
    outb(com_port + 3, 0x80);    // 设置波特率除数锁存
    outb(com_port + 0, 0x03);    // 除数低字节 (38400 波特)
    outb(com_port + 1, 0x00);    // 除数高字节
    outb(com_port + 3, 0x03);    // 8 位数据，无校验，1 停止位
    outb(com_port + 2, 0xC7);    // 启用 FIFO，清空队列
    outb(com_port + 4, 0x0B);    // 开启 IRQ，设置 RTS/DTR
}

inline void serial_putchar(char c) {
    // 等待发送缓冲区空
    //while ((inb(com_port + 5) & 0x20) == 0);
    outb(com_port, c);
}

void serial_puts(const char *str) {
    while (*str) {
        serial_putchar(*str++);
    }
}

char serial_getchar(void) {
    // 等待数据到达
    while ((inb(com_port + 5) & 0x01) == 0);
    return inb(com_port);
}

int serial_data_available(void) {
    return (inb(com_port + 5) & 0x01) != 0;
}