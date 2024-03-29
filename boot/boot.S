#include "boot.h"

.section .multiboot
    .long MB_MAGIC
    .long MB_ALIGNED_4K_MEM_MAP
    .long CHECKSUM(MB_ALIGNED_4K_MEM_MAP)
.section .bss
    .align 16
    stack_bottom:
        .skip 65536, 0
    stack_top:
.section .text
    _start:
        movl $stack_top, %esp
        call _relix_core_init
        pushl %ebx
        call _relix_core_main
        cli
    _j:
        hlt
        jmp _j