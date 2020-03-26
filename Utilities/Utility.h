#pragma once
#include "../PCSX2structs/registers.h"

typedef void(__cdecl* stdHook)();

#define ps2(a) a+0x20000000

static Regs* r;

inline static void printStack(const int start = -160, const int stop = 160) {
	const auto stackpointer = r->sp.UW[0];
	const int size = 4;
	printf("-- start -- 0x%x 0x%x 0x%x 0x%x\r\n", r->sp.UW[3], r->sp.UW[2], r->sp.UW[1], r->sp.UW[0]);
	for (int i = start; i < stop; i += size) {
		if (i % 16 == 0)
			printf("%d\t", i);
		printf("%08x ", *(DWORD*)(ps2(stackpointer - i)));
		if ((i + size) % 16 == 0)
			printf("\n");
	}
	printf("-- stop --\r\n");
}