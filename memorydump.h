#pragma once
#include "pch.h"
#include <fstream>

class MemoryDump {
public:
	static void Dump(const char* fileName, DWORD start, size_t size) {
		char* bytes = new char[size];
		if (!bytes) {
			printf("failed, not enough memory?\r\n");
			return; 
		}
		for (unsigned int i = 0; i < size; ++i) {
			bytes[i] = *(char*)(start+i);
		}
		std::ofstream file(fileName, std::ios::out | std::ios::binary);
		file.write(bytes, size);
		file.close();
		delete[] bytes;
	}
};