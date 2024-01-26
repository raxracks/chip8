#include <stdint.h>

#ifndef __CPU_H__
#define __CPU_H__

typedef struct CPU {
	uint16_t pc;
	uint8_t mem[4096];
	uint8_t regs[16];
	uint16_t I;
} CPU;

#endif
