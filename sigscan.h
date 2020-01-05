#pragma once

#include <iostream>
#include <string>
#include <Windows.h>

class SignatureScanner
{
public:

	static bool MemoryCompare(DWORD bData, const char* bMask, const char* szMask, int len) {
		for (int bCount = 0; bCount < len; ++bCount) {
			if (szMask[bCount] == '?' || *(char*)(bData + bCount) == bMask[bCount])
				continue;
			return false;
		}
		return true;
	}

	static bool FindSignature(DWORD* out, DWORD start, DWORD size, const char* sig, const char* mask, int len, size_t offset)
	{
		size_t s = 1000000;
		//not efficient
		for (DWORD i = 0; i < size; i++)
		{
			if (i % s == 0) {
				printf("testing address 0x%x\r\n", start + i);
			}
			if (MemoryCompare(start + i, sig, mask, len)) {
				printf("found signature at 0x%x\r\n", start+i);
				*out = start + i + offset;
				return true;
			}
		}
		printf("returned false\r\n");
		return false;
	}
};