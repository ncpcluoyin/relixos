ENTRY(_start)

PHDRS
{
    text PT_LOAD FLAGS(5);   /* 读 + 执行 */
    data PT_LOAD FLAGS(6);   /* 读 + 写 */
}

SECTIONS
{
    /* 从 1 MiB 开始加载 */
    . = 1M;

    /* 将 .multiboot 节放在最前面，确保其文件偏移尽可能小 */
    .multiboot : {
        *(.multiboot)
    } :text

    /* 其他代码节紧随其后 */
    .text : {
        *(.text)
    } :text

    .rodata : {
        *(.rodata)
    } :text

    .data : {
        *(.data)
    } :data

    .bss : {
        *(.bss)
        *(COMMON)
    } :data
}
