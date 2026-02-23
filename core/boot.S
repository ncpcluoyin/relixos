/* boot.S - RelixOS 启动代码，支持 Multiboot2 并切换到 64 位长模式 */

.section .multiboot
.align 8
multiboot_header:
    .long 0xE85250D6              /* 魔数 */
    .long 0                       /* 架构：i386 (32位) */
    .long header_end - multiboot_header
    .long 0x100000000 - (0xE85250D6 + 0 + (header_end - multiboot_header)) /* 校验和 */
    /* 结束标签 */
    .word 0
    .word 0
    .long 8
header_end:

.section .bss
.align 16
stack_bottom:
    .skip 524288          // 512 KB  栈
stack_top:

.section .data
.align 4096
/* 页表：2MB 大页恒等映射，覆盖前 2GB */
pml4:
    .space 4096
pdpt:
    .space 4096
pd_low:                            /* 页目录（低2GB） */
    .space 4096

/* GDT 表 */
.align 8
gdt:
    .quad 0x0000000000000000       /* 空描述符 */
    .quad 0x00CF9A000000FFFF       /* 32位代码段 (0x08) */
    .quad 0x00CF92000000FFFF       /* 32位数据段 (0x10) */
    .quad 0x00209A0000000000       /* 64位代码段 (0x18) */
    .quad 0x0000920000000000       /* 64位数据段 (0x20) */
gdt_ptr:
    .word (gdt_ptr - gdt - 1)      /* 限长 */
    .quad gdt                      /* 基址 */

/* 保存 Multiboot2 信息地址 */
mb_info_saved:
    .quad 0

.section .text
.code32
.global _start
_start:
    /* 保存 GRUB 传递的 Multiboot2 信息地址 (ebx) */
    movl %ebx, mb_info_saved

    /* 设置临时栈 */
    mov $stack_top, %esp

    /* 检查 CPU 是否支持长模式 */
    call check_long_mode
    test %eax, %eax
    jz no_long_mode

    /* 设置页表 */
    call setup_paging

    /* 启用 PAE */
    mov %cr4, %eax
    or $(1 << 5), %eax
    mov %eax, %cr4

    /* 设置 EFER.LME 启用长模式 */
    mov $0xC0000080, %ecx
    rdmsr
    or $(1 << 8), %eax
    wrmsr

    /* 启用分页 */
    mov %cr0, %eax
    or $(1 << 31), %eax
    mov %eax, %cr0

    /* 加载 GDT (包含 64 位段) */
    lgdt gdt_ptr

    /* 远跳转进入 64 位模式 */
    ljmp $0x18, $long_mode_entry

no_long_mode:
    cli
    hlt
    jmp no_long_mode

/* 函数: check_long_mode - 检查是否支持长模式，返回 eax=1 支持，0 不支持 */
check_long_mode:
    /* 检查 CPUID 是否可用 */
    pushfl
    pushfl
    xorl $(1 << 21), (%esp)
    popfl
    pushfl
    popl %ecx
    xorl (%esp), %ecx
    popfl
    test %ecx, %ecx
    jz .Lno_cpuid

    /* 检查扩展功能 */
    mov $0x80000000, %eax
    cpuid
    cmp $0x80000001, %eax
    jb .Lno_cpuid

    mov $0x80000001, %eax
    cpuid
    test $(1 << 29), %edx
    jz .Lno_cpuid

    mov $1, %eax
    ret
.Lno_cpuid:
    xor %eax, %eax
    ret

/* 函数: setup_paging - 初始化页表 (2MB 大页恒等映射) */
setup_paging:
    /* 清零页表区域 */
    cld
    mov $pml4, %edi
    mov $(4096*3), %ecx
    xor %eax, %eax
    rep stosb

    /* PML4[0] = PDPT 物理地址 | 0x03 (存在 + 可写) */
    mov $pdpt, %eax
    or $3, %eax
    mov %eax, pml4

    /* PDPT[0] = PD 物理地址 | 0x03 */
    mov $pd_low, %eax
    or $3, %eax
    mov %eax, pdpt

    /* 填充 PD 项：256 个 2MB 页，覆盖 512MB（根据需要可增加） */
    mov $pd_low, %edi
    mov $0x000083, %eax          /* 2MB 页: 存在 + 可写 + 大页 */
    mov $256, %ecx
1:
    mov %eax, (%edi)
    add $8, %edi
    add $0x200000, %eax
    loop 1b

    /* 设置 CR3 = PML4 物理地址 */
    mov $pml4, %eax
    mov %eax, %cr3
    ret

.code64
long_mode_entry:
    /* 设置 64 位数据段寄存器 */
    mov $0x20, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    /* 重新设置栈（rsp 为 64 位） */
    mov $stack_top, %rsp

    /* 将 Multiboot2 信息地址加载到 rdi（第一个参数） */
    mov (mb_info_saved), %rdi

    /* 调用 C 主函数 */
    call core_main

    /* 若返回则停机 */
    cli
loop:
    hlt
    jmp loop


/******************************************************************************
 * 中断处理入口
 *****************************************************************************/

/* 宏：无错误码的中断 */
.macro ISR_NOERRCODE num
.global isr\num
isr\num:
    pushq $0                  # 伪错误码（占位）
    pushq $\num               # 中断号
    jmp isr_common_stub
.endm

/* 宏：有错误码的中断（CPU 已压入错误码） */
.macro ISR_ERRCODE num
.global isr\num
isr\num:
    pushq $\num               # 中断号（CPU 已压入错误码）
    jmp isr_common_stub
.endm

/* 定义 0-31 号异常（部分有错误码） */
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8               # 双重错误
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14              # 页错误
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_ERRCODE   18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_ERRCODE   30
ISR_NOERRCODE 31

/* 定义 32-47 用于 IRQ（无错误码） */
ISR_NOERRCODE 32
ISR_NOERRCODE 33
ISR_NOERRCODE 34
ISR_NOERRCODE 35
ISR_NOERRCODE 36
ISR_NOERRCODE 37
ISR_NOERRCODE 38
ISR_NOERRCODE 39
ISR_NOERRCODE 40
ISR_NOERRCODE 41
ISR_NOERRCODE 42
ISR_NOERRCODE 43
ISR_NOERRCODE 44
ISR_NOERRCODE 45
ISR_NOERRCODE 46
ISR_NOERRCODE 47

.global isr_stubs
isr_stubs:
    .quad isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7
    .quad isr8, isr9, isr10, isr11, isr12, isr13, isr14, isr15
    .quad isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23
    .quad isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31
    .quad isr32, isr33, isr34, isr35, isr36, isr37, isr38, isr39
    .quad isr40, isr41, isr42, isr43, isr44, isr45, isr46, isr47

/* 标记栈不需要可执行权限 */
.section .note.GNU-stack,"",@progbits

/* 通用中断处理程序 */

isr_common_stub:
    /* 保存所有通用寄存器 */
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    /* 传递参数给 C 函数 isr_handler：
     *   rdi = 中断号（位于 15*8+8(%rsp)）
     *   rsi = 错误码（位于 15*8(%rsp)）
     */
    movq 15*8+8(%rsp), %rdi   // 中断号 -> 第一个参数
    movq 15*8(%rsp), %rsi     // 错误码 -> 第二个参数
    call isr_handler

    /* 恢复寄存器 */
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rbp
    popq %rdi
    popq %rsi
    popq %rdx
    popq %rcx
    popq %rbx
    popq %rax

    /* 移除栈上的中断号和错误码（16 字节） */
    addq $16, %rsp
    iretq
