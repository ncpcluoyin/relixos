# CMakeLists.txt - RelixOS 构建配置

cmake_minimum_required(VERSION 3.10)
project(RelixOS C ASM)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# 禁用位置无关代码（内核需要绝对地址）
set(CMAKE_POSITION_INDEPENDENT_CODE OFF)

# 通用编译标志
set(COMMON_FLAGS "-m64 -ffreestanding -fno-pie -nostdlib -nostdinc -fno-builtin -fno-stack-protector")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS} -Wall -Wextra")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -Wa,--64")

#include headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/headers)

# 内核可执行文件
add_executable(core.elf 
    core/core.c
    core/boot.S
)

set_target_properties(core.elf PROPERTIES
    POSITION_INDEPENDENT_CODE OFF
)

target_link_options(core.elf PRIVATE
    -Wl,-m,elf_x86_64
    -T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
    -nostdlib
    -no-pie
    -Wl,--build-id=none
)

set_target_properties(core.elf PROPERTIES
    LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
)

# ------------------ ISO 镜像生成 (UEFI) ------------------
set(ISO_ROOT ${CMAKE_CURRENT_BINARY_DIR}/iso_root)
set(ISO_BOOT ${ISO_ROOT}/boot)
set(ISO_BOOT_GRUB ${ISO_BOOT}/grub)
set(GRUB_CFG_SRC ${CMAKE_CURRENT_SOURCE_DIR}/grub.cfg)

add_custom_target(iso
    COMMAND ${CMAKE_COMMAND} -E make_directory ${ISO_BOOT_GRUB}
    COMMAND ${CMAKE_COMMAND} -E copy core.elf ${ISO_BOOT}/
    COMMAND ${CMAKE_COMMAND} -E copy ${GRUB_CFG_SRC} ${ISO_BOOT_GRUB}/
    COMMAND ${CMAKE_COMMAND} -E env GRUB_PLATFORMS="efi-64" grub-mkrescue -o ${CMAKE_CURRENT_BINARY_DIR}/relixos.iso ${ISO_ROOT}
    DEPENDS core.elf
    COMMENT "Generating bootable ISO image (UEFI Mode)"
    VERBATIM
)

# ------------------ QEMU 运行目标 ------------------
find_program(QEMU_EXECUTABLE qemu-system-x86_64)
if(NOT QEMU_EXECUTABLE)
    message(WARNING "qemu-system-x86_64 not found. 'run' target will not work.")
endif()

# 查找 OVMF 固件
find_file(OVMF_FW
    NAMES OVMF.fd OVMF_CODE.fd
    PATHS /usr/share/ovmf /usr/share/edk2/x64 /usr/share/edk2/ovmf
    DOC "Path to OVMF firmware file"
)

if(OVMF_FW)
    message(STATUS "Found OVMF firmware: ${OVMF_FW}")
else()
    message(WARNING "OVMF firmware not found. 'run' target will fail.")
endif()

# UEFI 启动
add_custom_target(run
    COMMAND ${QEMU_EXECUTABLE} -bios ${OVMF_FW} -cdrom ${CMAKE_CURRENT_BINARY_DIR}/relixos.iso -no-reboot -no-shutdown -serial stdio
    DEPENDS iso
    COMMENT "Starting RelixOS in QEMU (UEFI mode)"
    VERBATIM
)