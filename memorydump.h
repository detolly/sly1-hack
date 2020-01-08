#pragma once
#include "pch.h"
#include <fstream>

class MemoryDump {
public:
	static void Dump(const char* fileName, DWORD start, size_t size) {
		char* bytes = (char*)malloc(size);
		if (!bytes) {
			printf("malloc failed, not enough (legal) memory?");
			return; 
		}
		for (DWORD i = 0; i < size; ++i) {
			bytes[i] = *(char*)(start+i);
		}
		std::ofstream file("C:\\test\\dump.bin", std::ios::out | std::ios::binary);
		file.write(bytes, size);
		free(bytes);
		printf("Successfully dumped memory!\r\n");
	}
};