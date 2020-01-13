#pragma once

#include <iostream>
#include <string>
#include <Windows.h>

class SignatureScanner
{
public:

	static bool MemoryCompare(DWORD bData, const char* bMask, const char* szMask) {
		for (size_t bCount = 0; bCount < strlen(szMask); ++bCount) { 
			//printf("Read byte: 0x%x,  Byte Mask: 0x%x\r\n", *(BYTE*)(bData + bCount), (BYTE)bMask[bCount]);
			if (szMask[bCount] == 'x' && *(BYTE*)(bData + bCount) != (BYTE)bMask[bCount])
				return false;
		}
		return true;
	}

	static bool FindSignature(DWORD* out, DWORD start, DWORD size, const char* sig, const char* mask, size_t offset)
	{
		for (DWORD i = 0; i < size; i++)
		{
			if (MemoryCompare(start + i, sig, mask)) {
				printf("found signature at 0x%x\r\n", start + i);
				*out = start + i + offset;
				return true;
			}
		}
		return false;
	}
};