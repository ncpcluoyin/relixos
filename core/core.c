#include "serial.h"

// 可选：如果未来需要解析 Multiboot2 信息，可以使用参数 mb_info_addr
void core_main(uint64_t  mb_info_addr) {
    // 初始化串口（使用 COM1）
    serial_init(COM1);

    // 输出欢迎信息
    serial_puts(COM1, "\n=== RelixOS ===\n");
    serial_puts(COM1, "64bit core has been loaded successfully\n");
    serial_puts(COM1, "welcome! --by ZYC\n");

    // 测试十六进制输出
    serial_puts(COM1, "Multiboot2 info address: ");
    serial_puthex(COM1, (uint32_t)mb_info_addr);
    serial_puts(COM1, "\n");
}