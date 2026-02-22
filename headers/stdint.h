// include/stdint.h
#ifndef STDINT_H__
#define STDINT_H__

typedef unsigned char		uint8_t;
typedef unsigned short int	uint16_t;
typedef unsigned int		uint32_t;
typedef unsigned long int	uint64_t;  // x86_64 下 long 是 64 位

typedef signed char			int8_t;
typedef signed short int	int16_t;
typedef signed int			int32_t;
typedef signed long int		int64_t;

typedef unsigned long int	uintptr_t;  // 指针类型

#endif