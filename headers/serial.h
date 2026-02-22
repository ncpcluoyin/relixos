#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

// 初始化串口（默认 COM1，可修改端口）
void serial_init(uint16_t port);

// 输出一个字符
void serial_putchar(char c);

// 输出字符串
void serial_puts(const char *str);

// 输入一个字符（阻塞，直到收到字符）
char serial_getchar(void);

// 检查是否有字符可读（非阻塞）
int serial_data_available(void);

#endif