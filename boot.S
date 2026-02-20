/* boot.S - 64 位启动代码，Multiboot2 头指定 x86_64 架构 */

.section .multiboot
.align 8

multiboot_header:
    /* 魔数 */
    .long 0xE85250D6
    /* 架构：x86_64 (4) */
    .long 4
    /* 头部长度 */
    .long header_end - multiboot_header
    /* 校验和：32位求和为零 */
    .long 0x100000000 - (0xE85250D6 + 4 + (header_end - multiboot_header))

    /* 结束标签 */
    .word 0    /* 类型 */
    .word 0    /* 标志 */
    .long 8    /* 大小 */
header_end:

.section .text
.code64
.global _start
_start:
    /* 在长模式下，数据段寄存器仍然需要设置为 0 以使用平坦模型 */
    mov $0, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    /* 设置栈指针（栈位于 .bss 中） */
    mov $stack_top, %rsp

    /* 调用 C 主函数（64 位调用约定，参数在 rdi 等寄存器） */
    call core_main

    /* 如果返回，则停机 */
    cli
    hlt
loop:
    jmp loop

.section .bss
.align 16
stack_bottom:
    .skip 16384          # 16 KB 栈空间（64 位下足够）
stack_top:

/* 标记栈不需要可执行权限 */
.section .note.GNU-stack,"",@progbits