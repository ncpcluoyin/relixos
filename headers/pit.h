#ifndef PIT_H
#define PIT_H

#include <stdint.h>

// 初始化 PIT 以指定频率（Hz）
void pit_init(uint32_t frequency);

// 获取当前 tick 计数（由时钟中断递增）
uint64_t pit_get_ticks(void);

void pit_handler(void);

#define PIT_CH0_DATA     0x40
#define PIT_CH1_DATA     0x41
#define PIT_CH2_DATA     0x42
#define PIT_COMMAND      0x43

#define PIT_CMD_CH0      0x00
#define PIT_CMD_CH1      0x40
#define PIT_CMD_CH2      0x80
#define PIT_CMD_RW_LATCH 0x00
#define PIT_CMD_RW_LOW   0x10
#define PIT_CMD_RW_HIGH  0x20
#define PIT_CMD_RW_BOTH  0x30
#define PIT_CMD_MODE0    0x00
#define PIT_CMD_MODE2    0x04   // 速率发生器（用于周期性中断）
#define PIT_CMD_BCD      0x01


#endif