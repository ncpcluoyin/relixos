#include<stdint.h>
#include"serial.h"

void core_main(){
    char m = 'm';
    serial_putchar(DEFAULT_COM,m);
    char * a = "suck";
    serial_puts(DEFAULT_COM,a);
    serial_putchar(DEFAULT_COM,'A');
}
