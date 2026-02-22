#include<stdint.h>
#include"serial.h"

void core_main(){
    serial_putchar(DEFAULT_COM,'A');
    char a[3] = {'M','A','\0'};
    serial_puts(DEFAULT_COM,a);
    serial_putchar(DEFAULT_COM,'A');
}
