#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

#define DEFAULT_COM COM1
#define COM1 0x3F8//unit16_t
#define COM2 0x2F8

void serial_putchar(uint16_t port, uint8_t data);
void serial_puts(uint16_t port,const char * str);

#endif