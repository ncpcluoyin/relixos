#include "serial.h"

// 可选：如果未来需要解析 Multiboot2 信息，可以使用参数 mb_info_addr
void core_main(unsigned long mb_info_addr) {
    // 初始化串口（使用 COM1）
    serial_init(COM1);

    // 输出欢迎信息
    serial_puts(COM1, "\r\n=== RelixOS ===\r\n");
    serial_puts(COM1, "Kernel entered 64-bit long mode successfully.\r\n");

    // 测试十六进制输出
    serial_puts(COM1, "Multiboot2 info address: ");
    serial_puthex(COM1, (uint32_t)mb_info_addr);
    serial_puts(COM1, "\r\n");

    // 如果未来需要 VGA 文本输出（在 GRUB 配置为文本模式时可用）
    // char *video = (char *)0xB8000;
    // const char *msg = "RelixOS VGA";
    // for (int i = 0; msg[i]; i++) {
    //     video[i*2] = msg[i];
    //     video[i*2+1] = 0x0F;
    // }

    // 主循环
    while (1) {
        // 可以在这里添加命令处理等
        __asm__ volatile ("hlt");
    }
}