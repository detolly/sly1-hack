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
			if (MemoryCompare(start + i, sig, mask)) {
				printf("found signature at 0x%x\r\n", start + i);
				*out = start + i + offset;
				return true;
			}
		return false;
	}

	static DWORD PatternScan(const char* pattern, const char* mask, size_t offset)
	{
		printf("got here 1\r\n");
		SYSTEM_INFO sysInfo;

		GetSystemInfo(&sysInfo);

		DWORD end = (DWORD)sysInfo.lpMaximumApplicationAddress;

		DWORD currentChunk = 0x10000000;
		DWORD match = 0x0;

		while (currentChunk < end)
		{
			printf("got here 2\r\n");
			MEMORY_BASIC_INFORMATION mbi;
			if (!VirtualQuery((char*)currentChunk, &mbi, sizeof(mbi)))
			{
				return 0;
			}
			if (mbi.State == MEM_COMMIT && mbi.Protect != PAGE_NOACCESS)
			{
				printf("got here 3\r\n");
				DWORD oldprotect;
				if (VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &oldprotect))
				{
					printf("got here 3 0x%p 0x%p\r\n", mbi.BaseAddress, mbi.RegionSize);
					if (SignatureScanner::FindSignature(&match, (DWORD)mbi.BaseAddress, mbi.RegionSize, pattern, mask, offset)) {
						break;
					}
					VirtualProtect(mbi.BaseAddress, mbi.RegionSize, oldprotect, &oldprotect);
				}
			}
			currentChunk = currentChunk + mbi.RegionSize;
		}
		printf("got here 5\r\n");
		return match;
	}
};



//Internal Pattern Scan
//Should match Ex::Scan logic, any changes here, change there too
