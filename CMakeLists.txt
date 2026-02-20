# CMakeLists.txt - 构建 Multiboot2 64 位内核 (RelixOS)

cmake_minimum_required(VERSION 3.10)

# 项目名称及使用的语言
project(RelixOS C ASM)

# 指定目标系统为通用 (freestanding) 环境，处理器为 x86_64
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# 显式禁用位置无关代码 (PIE)
set(CMAKE_POSITION_INDEPENDENT_CODE OFF)

# 通用编译标志
set(COMMON_FLAGS "-m64 -ffreestanding -fno-pie -nostdlib -nostdinc -fno-builtin -fno-stack-protector")

# C 语言标志
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS} -Wall -Wextra")

# 汇编语言标志 (通过 -Wa 传递给汇编器)
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -Wa,--64")

# 定义可执行文件目标（源文件：boot.S 和 core.c）
add_executable(core.elf boot.S core.c)

# 确保目标属性禁止 PIE（双重保险）
set_target_properties(core.elf PROPERTIES
    POSITION_INDEPENDENT_CODE OFF
)

# 链接选项：指定链接器格式、使用自定义链接脚本、禁止标准库、禁用 PIE
target_link_options(core.elf PRIVATE
    -Wl,-m,elf_x86_64          # 生成 64 位 ELF
    -T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld   # 使用自定义链接脚本
    -nostdlib                   # 禁止标准库
    -no-pie                     # 强制生成非 PIE 可执行文件
)

# 当链接脚本发生更改时强制重新链接
set_target_properties(core.elf PROPERTIES
    LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
)