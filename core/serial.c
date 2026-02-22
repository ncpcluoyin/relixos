#include "serial.h"

void serial_putchar(uint16_t port, uint8_t data) {
    __asm__ volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}

void serial_puts(uint16_t port,const char * str){
    char tmp;
    while(*str)
    {
        tmp = *str;
        serial_putchar(port,tmp);
        str++;
    }
}
