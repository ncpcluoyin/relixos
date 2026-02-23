#include "serial.h"
#include "io.h"

void serial_init(uint16_t port) {
    outb(port + 1, 0x00);    // 关闭中断
    outb(port + 3, 0x80);    // 设置波特率除数锁存
    outb(port + 0, 0x03);    // 除数低字节 (38400 波特)
    outb(port + 1, 0x00);    // 除数高字节
    outb(port + 3, 0x03);    // 8 位数据，无校验，1 停止位
    outb(port + 2, 0xC7);    // 启用 FIFO，清空队列
    outb(port + 4, 0x0B);    // 开启 IRQ，设置 RTS/DTR
}

void serial_putchar(uint16_t port, char c) {
    // 等待发送缓冲区空
    while ((inb(port + 5) & 0x20) == 0);
    outb(port, c);
}

void serial_puts(uint16_t port, const char *str) {
    while (*str) {
        serial_putchar(port, *str++);
    }
}

static const char hexdigits[] = "0123456789ABCDEF";

void serial_puthex(uint16_t port, uint32_t val) {
    serial_putchar(port, '0');
    serial_putchar(port, 'x');
    for (int i = 28; i >= 0; i -= 4) {
        serial_putchar(port, hexdigits[(val >> i) & 0xF]);
    }
}

int serial_data_available(uint16_t port) {
    return (inb(port + 5) & 0x01) != 0;
}

char serial_getchar(uint16_t port) {
    while ((inb(port + 5) & 0x01) == 0);
    return inb(port);
}