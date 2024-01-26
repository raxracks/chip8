#include <stdio.h>
#include <slibs/slibs.h>
#include <assert.h>
#include <stdint.h>
#include <cpu.h>
#include <raylib.h>
#include <math.h>
#include <time.h>

#define __DEBUG

#ifndef __DEBUG
#define printf
#endif

CPU cpu = { 0 };
uint8_t display[64 * 32];

int main(int argc, char** argv) {
	assert(argc > 1);

	srand(time(0));
	
	sl_string code = { 0 };
	sl_read_file(argv[1], &code);
	memcpy(cpu.mem + 512, code.data, code.size);

	InitWindow(1280, 640, "Chip8");
	SetTargetFPS(60);

	for(cpu.pc = 512; cpu.pc < 512 + code.size; cpu.pc += 2) {	
		uint8_t current = cpu.mem[cpu.pc] & 0xFF;
		uint8_t a = (current >> 4) & 0xF;
		uint8_t b = current & 0xF;

		uint8_t next = cpu.mem[cpu.pc + 1] & 0xFF;
		uint8_t c = (next >> 4) & 0xF;
		uint8_t d = next & 0xF;

		uint16_t address = b << 8 | c << 4 | d;

		switch(a) {
		case 0x0:
			switch(d) {
			case 0x0:
				printf("CLS\n");
				BeginDrawing();
				ClearBackground(BLACK);
				EndDrawing();
				break;
			case 0xE:
				printf("RET\n");
				break;
			}
			break;
		case 0x1:
			cpu.pc = address - 2;
			printf("JP 0x%X\n", address);
			break;
		case 0x3:
			if(cpu.regs[b] == next)
				cpu.pc += 2;
			printf("SE V%X, 0x%X\n", b, next);
			break;
		case 0x4:
			if(cpu.regs[b] != next)
				cpu.pc += 2;
			printf("SNE V%X, 0x%X\n", b, next);
			break;
		case 0x5:
			if(cpu.regs[b] == cpu.regs[c])
				cpu.pc += 2;
			break;
		case 0x6:
			cpu.regs[b] = next;
			printf("LD V%X, 0x%X\n", b, next);
			break;
		case 0x7:
			cpu.regs[b] += next;
			printf("ADD V%X, 0x%X\n", b, next);
			break;
		case 0x8:
			switch(d) {
			case 0x0:
				cpu.regs[b] = cpu.regs[c];
				printf("LD V%X, V%X\n", b, c);
				break;
			case 0x1:
				cpu.regs[b] |= cpu.regs[c];
				printf("OR V%X, V%X\n", b, c);
				break;
			case 0x2:
				cpu.regs[b] &= cpu.regs[c];
				printf("AND V%X, V%X");
				break;
			case 0x3:
				cpu.regs[b] ^= cpu.regs[c];
				break;
			case 0x4:
				cpu.regs[b] += cpu.regs[c];
				printf("ADD V%X, V%X\n", b, c);
				break;
			case 0x5:
				cpu.regs[b] -= cpu.regs[c];
				printf("SUB V%X, V%X\n", b, c);
				break;
			case 0x6:
				cpu.regs[0xF] = cpu.regs[b] & 1;
				cpu.regs[b] >>= 1;
				break;
			case 0x7:
				cpu.regs[b] = cpu.regs[c] - cpu.regs[b];
				break;
			case 0xE:
				cpu.regs[0xF] = (cpu.regs[b] & 0xFF != 0) ? 1 : 0;
				cpu.regs[b] <<= 1;
				break;
			}
			break;
		case 0x9:
			if(cpu.regs[b] != cpu.regs[c])
				cpu.pc += 2;
			break;
		case 0xA:
			cpu.I = address;
			printf("LD I, 0x%016X\n", address);
			break;
		case 0xB:
			cpu.pc = cpu.regs[0] + address;
		case 0xC:
			cpu.regs[b] = rand() & next;
			printf("RND V%X, 0x%X (out: 0x%08X)\n", b, next, cpu.regs[b]);
			break;
		case 0xD:
			int xPos = cpu.regs[b];
			int yPos = cpu.regs[c];
			int height = d;

			BeginDrawing();
			for(int y = 0; y < height; y++) {
				uint8_t line = cpu.mem[cpu.I + y];
				for(int x = 0; x < 8; x++) {
					uint8_t pixel = line & (0x80 >> x);
					if(pixel != 0) {
						int totalX = xPos + x;
						int totalY = yPos + y;
						
						totalX = totalX % 64;
						totalY = totalY % 32;
						
						int index = (totalY * 64) + totalX;
						
						if(display[index] == 1)
							cpu.regs[0xF] = 1;
						
						display[index] ^= 1;

						DrawRectangle((xPos + x) * 20, (yPos + y) * 20, 20, 20, display[index] == 0 ? BLACK : WHITE);
					}
				}
			}
			EndDrawing();

			printf("DRW V%X, V%X, %d\n", b, c, height);

			break;
		case 0xF:
			switch(next) {
			case 0x1E:
				cpu.I += cpu.regs[b];
				printf("ADD I, V%X\n", b);
				break;
			case 0x29:
				// TODO: unimplemented
				break;
			case 0x33:
				// TODO: unimplemented
				break;
			case 0x55:
				int offset = 0;
				for(int i = 0; i <= b; i++) {
					cpu.mem[cpu.I + offset++] = cpu.regs[i];
				}
				break;
			case 0x65:
				offset = 0;
				for(int i = 0; i <= b; i++) {
					cpu.regs[i] = cpu.mem[cpu.I + offset++];
				}
				printf("RSTR V%X\n", b);
				break;
			}
			break;
		}
	}

	printf("\nRegisters:\n");
	for(int i = 0; i < sl_array_len(cpu.regs); i++) {
		printf("V%X:\t0b%08b\t\t0x%08X\n", i, cpu.regs[i], cpu.regs[i]);
	}
	printf("I:\t0b%016b\t0x%016X\n", cpu.I, cpu.I);

	CloseWindow();

	return 0;
}
