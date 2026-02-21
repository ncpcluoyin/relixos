#include "serial.h"

#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

/* VGA 颜色属性：白字黑底 */
#define VGA_ATTRIBUTE (0x0F)

void core_main(void)
{
    //serial test



    /* 死循环 */
    while (1) {
        __asm__ volatile ("hlt");
    }
}