#pragma once

#include "declarations.h"
#include "../PCSX2structs/registers.h"

#define ps2(a) a+0x20000000

inline static void callNative(void* hook) {
	printf("Calling native function! pc: 0x%08x\n", r->pc);
	r->sp.UW[0] -= 0x10;
	GPR_reg temp;
	memset(&temp, 0, sizeof(decltype(temp)));
	temp.UW[0] = r->ra.UW[0];
	memcpy(reinterpret_cast<void*>(ps2(r->sp.UW[0])), &temp, sizeof(decltype(temp)));
	r->ra.UW[0] = r->pc+0x4;
	DWORD old;
	VirtualProtect(hook, 1024, MEM_COMMIT | PAGE_EXECUTE_READWRITE, &old);
	__asm {
		jmp hook
	}
	r->sp.UW[0] += 0x10;
	printf("Finished calling native function! pc: 0x%08x\n", r->pc);
}

inline static void call_native_through_cheat(uint32_t addr, uint32_t arg)
{
	uint32_t tmp = *(uint32_t*)(ps2(0x00262a88));
	uint32_t tmp_arg = *(uint32_t*)(ps2(0x00262a8c));
	*(uint32_t*)(ps2(0x00262a88)) = addr;
	*(uint32_t*)(ps2(0x00262a8C)) = arg;
	*(uint32_t*)(ps2(0x00262a90)) = 15;
	while (*(uint32_t*)(ps2(0x00262a90)) != 0) {
		Sleep(500);
	}
	*(uint32_t*)(ps2(0x00262a88)) = tmp;
	*(uint32_t*)(ps2(0x00262a8C)) = tmp_arg;
}

inline static void print_on_screen(const char *str) {
	memcpy((void*)0x20260204, str, strlen(str)+1);
	call_native_through_cheat(0x260294, 0x26ec70);
}

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