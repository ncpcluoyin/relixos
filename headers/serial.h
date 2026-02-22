#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

// 串口端口定义
#define COM1 0x3F8
#define COM2 0x2F8

// 初始化串口
void serial_init(uint16_t port);

// 输出一个字符
void serial_putchar(uint16_t port, char c);

// 输出字符串
void serial_puts(uint16_t port, const char *str);

// 输出十六进制数
void serial_puthex(uint16_t port, uint32_t val);

// 检查是否有数据可读（非阻塞）
int serial_data_available(uint16_t port);

// 读取一个字符（阻塞）
char serial_getchar(uint16_t port);

#endif