#include "pch.h"
#include <stdio.h>
#include <windows.h>
#include "sigscan.h"

/* There is no ANSI ustrncpy */
unsigned char* ustrncpy(unsigned char* dest, const unsigned char* src, int len) {
	while (len--)
		dest[len] = src[len];

	return dest;
}

/* //////////////////////////////////////
	CSigScan Class
	////////////////////////////////////// */
unsigned char* CSigScan::base_addr;
size_t CSigScan::base_len;
void* (*CSigScan::sigscan_dllfunc)(const char* pName, int* pReturnCode);

/* Initialize the Signature Object */
void CSigScan::Init(unsigned char* sig, char* mask, size_t len) {
	is_set = 0;

	sig_len = len;
	sig_str = new unsigned char[sig_len];
	ustrncpy(sig_str, sig, sig_len);

	sig_mask = new char[sig_len + 1];
	strncpy(sig_mask, mask, sig_len);
	sig_mask[sig_len + 1] = 0;

	printf("imagine\r\n");
	if (!base_addr)
		return; // GetDllMemInfo() Failed

	printf("imagine 2\r\n");
	if ((sig_addr = FindSignature()) == NULL)
		return; // FindSignature() Failed

	is_set = 1;
	// SigScan Successful!
}

/* Destructor frees sig-string allocated memory */
CSigScan::~CSigScan(void) {
	delete[] sig_str;
	delete[] sig_mask;
}

/* Get base address of the server module (base_addr) and get its ending offset (base_len) */
bool CSigScan::GetDllMemInfo(void) {
	void* pAddr = (void*)sigscan_dllfunc;
	printf("got here bro\r\n");
	base_addr = 0;
	base_len = 0;
	MEMORY_BASIC_INFORMATION mem;

	if (!pAddr)
		return false; // GetDllMemInfo failed!pAddr

	if (!VirtualQuery(pAddr, &mem, sizeof(mem)))
		return false;

	base_addr = (unsigned char*)mem.AllocationBase;
	printf("%p\r\n", base_addr);

	IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)mem.AllocationBase;
	IMAGE_NT_HEADERS* pe = (IMAGE_NT_HEADERS*)((unsigned long)dos + (unsigned long)dos->e_lfanew);
;
	if (pe->Signature != IMAGE_NT_SIGNATURE) {
		base_addr = 0;
		return false; // GetDllMemInfo failedpe points to a bad location
	}

	base_len = (size_t)pe->OptionalHeader.SizeOfImage;
	printf("%d\r\n", base_len);

	return true;
}

/* Scan for the signature in memory then return the starting position's address */
void* CSigScan::FindSignature(void) {
	unsigned char* pBasePtr = base_addr;
	unsigned char* pEndPtr = base_addr + base_len;
	size_t i;

	while (pBasePtr < pEndPtr) {
		for (i = 0; i < sig_len; i++) {
			if ((sig_mask[i] != '?') && (sig_str[i] != pBasePtr[i]))
				break;
		}

		// If 'i' reached the end, we know we have a match!
		if (i == sig_len)
			return (void*)pBasePtr;

		pBasePtr++;
	}

	return NULL;
}

/* Signature Objects */
CSigScan CBaseAnimating_Ignite_Sig;

/* Set the static base_addr and base_len variables then initialize all Signature Objects */
void InitSigs(void) {
	CSigScan::GetDllMemInfo();



	return;
}