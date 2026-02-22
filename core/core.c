#include<stdint.h>

#define COM1 0x3F8
#define DEFAULT_COM COM1

static inline uint8_t inb(int port)
{
	int ret;
	asm ("xorl %eax,%eax");
	asm ("inb %%dx,%%al":"=a" (ret):"d"(port));
	return ret;
}
static inline void outb(int port, uint8_t data)
{
	asm ("outb %%al,%%dx"::"a" (data), "d"(port));
}

void core_main(){
    outb(DEFAULT_COM,'a');
}