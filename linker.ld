/* linker.ld - 改进链接脚本，区分代码段和数据段权限 */

ENTRY(_start)

PHDRS
{
    text PT_LOAD FLAGS(5);   /* 读 + 执行 */
    data PT_LOAD FLAGS(6);    /* 读 + 写 */
}

SECTIONS
{
    . = 1M;

    .multiboot : {
        *(.multiboot)
    } :text

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