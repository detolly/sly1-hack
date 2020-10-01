#pragma once

#include "stdint.h"

typedef char s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef uintptr_t uptr;
typedef intptr_t sptr;

typedef unsigned int uint;

typedef union _u128_t
{
	struct
	{
		u64 lo;
		u64 hi;
	};

	u64 _u64[2];
	u32 _u32[4];
	u16 _u16[8];
	u8 _u8[16];
} u128;

typedef union _s128_t
{
	u64 lo;
	s64 hi;
} s128;

union CP0regs {
	struct {
		u32	Index, Random, EntryLo0, EntryLo1,
			Context, PageMask, Wired, Reserved0,
			BadVAddr, Count, EntryHi, Compare;
		union {
			struct {
				u32 IE : 1;		// Bit 0: Interrupt Enable flag.
				u32 EXL : 1;		// Bit 1: Exception Level, set on any exception not covered by ERL.
				u32 ERL : 1;		// Bit 2: Error level, set on Resetm NMI, perf/debug exceptions.
				u32 KSU : 2;		// Bits 3-4: Kernel [clear] / Supervisor [set] mode
				u32 unused0 : 3;
				u32 IM : 8;		// Bits 10-15: Interrupt mask (bits 12,13,14 are unused)
				u32 EIE : 1;		// Bit 16: IE bit enabler.  When cleared, ints are disabled regardless of IE status.
				u32 _EDI : 1;		// Bit 17: Interrupt Enable (set enables ints in all modes, clear enables ints in kernel mode only)
				u32 CH : 1;		// Bit 18: Status of most recent cache instruction (set for hit, clear for miss)
				u32 unused1 : 3;
				u32 BEV : 1;		// Bit 22: if set, use bootstrap for TLB/general exceptions
				u32 DEV : 1;		// Bit 23: if set, use bootstrap for perf/debug exceptions
				u32 unused2 : 2;
				u32 FR : 1;		// (?)
				u32 unused3 : 1;
				u32 CU : 4;		// Bits 28-31: Co-processor Usable flag
			} b;
			u32 val;
		} Status;
		u32   Cause, EPC, PRid,
			Config, LLAddr, WatchLO, WatchHI,
			XContext, Reserved1, Reserved2, Debug,
			DEPC, PerfCnt, ErrCtl, CacheErr,
			TagLo, TagHi, ErrorEPC, DESAVE;
	} n;
	u32 r[32];
};


union GPR_reg {
	u128	UQ;
	s128	SQ;
	u64		UD[2];
	s64		SD[2];
	u32		UW[4];
	s32		SW[4];
	u16		US[8];
	s16		SS[8];
	u8		UC[16];
	s8		SC[16];
};
/*__pragma(pack(push, 1)) struct Regs __pragma(pack(pop)) {
	GPR_reg
		r0, at, v0, v1, a0, a1, a2, a3,
		t0, t1, t2, t3, t4, t5, t6, t7,
		s0, s1, s2, s3, s4, s5, s6, s7,
		t8, t9, k0, k1, gp, sp, s8, ra;
	GPR_reg HI, LO;
	CP0regs CP0;
	u32 sa;		
	u32 IsDelaySlot;
	u32 pc;
};
*/
struct Regs {
	GPR_reg
		r0, at, v0, v1, a0, a1, a2, a3,
		t0, t1, t2, t3, t4, t5, t6, t7,
		s0, s1, s2, s3, s4, s5, s6, s7,
		t8, t9, k0, k1, gp, sp, s8, ra;
	GPR_reg HI, LO;
	CP0regs CP0;
	u32 sa;
	u32 IsDelaySlot;
	u32 pc;
};
/*
float* _f00 = (float*)0x827E90;
float* _f01 = (float*)_f00 + 0x4;
float* _f02 = (float*)_f01 + 0x4;
float* _f03 = (float*)_f02 + 0x4;
float* _f04 = (float*)_f03 + 0x4;
float* _f05 = (float*)_f04 + 0x4;
float* _f06 = (float*)_f05 + 0x4;
float* _f07 = (float*)_f06 + 0x4;
float* _f08 = (float*)_f07 + 0x4;
float* _f09 = (float*)_f08 + 0x4;
float* _f10 = (float*)_f09 + 0x4;
float* _f11 = (float*)_f10 + 0x4;
float* _f12 = (float*)_f11 + 0x4;
float* _f13 = (float*)_f12 + 0x4;
float* _f14 = (float*)_f13 + 0x4;
float* _f15 = (float*)_f14 + 0x4;
float* _f16 = (float*)_f15 + 0x4;
float* _f17 = (float*)_f16 + 0x4;
float* _f18 = (float*)_f17 + 0x4;
float* _f19 = (float*)_f18 + 0x4;
float* _f20 = (float*)_f19 + 0x4;
float* _f21 = (float*)_f20 + 0x4;
float* _f22 = (float*)_f21 + 0x4;
float* _f23 = (float*)_f22 + 0x4;
float* _f24 = (float*)_f23 + 0x4;
float* _f25 = (float*)_f24 + 0x4;
float* _f26 = (float*)_f25 + 0x4;
float* _f27 = (float*)_f26 + 0x4;
float* _f28 = (float*)_f27 + 0x4;
float* _f29 = (float*)_f28 + 0x4;
float* _f30 = (float*)_f29 + 0x4;
float* _f31 = (float*)_f30 + 0x4;
*/