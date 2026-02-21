/* core.c - 64 位内核，输出文本到屏幕 */

#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

/* VGA 颜色属性：白字黑底 */
#define VGA_ATTRIBUTE (0x0F)

void core_main(void)
{
    char *video_memory = (char *) VGA_ADDRESS;
    const char *message = "Hello, RelixOS!";
    int i;

    /* 清屏 */
    for (i = 0; i < VGA_WIDTH * VGA_HEIGHT * 2; i += 2) {
        video_memory[i] = ' ';
        video_memory[i + 1] = VGA_ATTRIBUTE;
    }

    /* 输出消息 */
    for (i = 0; message[i] != '\0'; i++) {
        video_memory[i * 2] = message[i];
        video_memory[i * 2 + 1] = VGA_ATTRIBUTE;
    }

    /* 死循环 */
    while (1) {
        __asm__ volatile ("hlt");
    }
}