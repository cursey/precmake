#pragma once

#include "utl/type.h"
#include "utl/whoami.h"

static const u64 max_u64 = 0xffffffffffffffffull;
static const u32 max_u32 = 0xffffffff;
static const u16 max_u16 = 0xffff;
static const u8 max_u8 = 0xff;

#if ARCH_X86_64
static const usize max_usize = 0xffffffffffffffffull;
#endif

static const i64 max_i64 = 0x7fffffffffffffffull;
static const i32 max_i32 = 0x7fffffff;
static const i16 max_i16 = 0x7fff;
static const i8 max_i8 = 0x7f;

#if ARCH_X86_64
static const isize max_isize = 0x7fffffffffffffffull;
#endif

static const i64 min_i64 = 0xffffffffffffffffull;
static const i32 min_i32 = 0xffffffff;
static const i16 min_i16 = 0xffff;
static const i8 min_i8 = 0xff;

#if ARCH_X86_64
static const isize min_isize = 0xffffffffffffffffull;
#endif

static const u32 bitmask1 = 0x00000001;
static const u32 bitmask2 = 0x00000003;
static const u32 bitmask3 = 0x00000007;
static const u32 bitmask4 = 0x0000000f;
static const u32 bitmask5 = 0x0000001f;
static const u32 bitmask6 = 0x0000003f;
static const u32 bitmask7 = 0x0000007f;
static const u32 bitmask8 = 0x000000ff;
static const u32 bitmask9 = 0x000001ff;
static const u32 bitmask10 = 0x000003ff;
static const u32 bitmask11 = 0x000007ff;
static const u32 bitmask12 = 0x00000fff;
static const u32 bitmask13 = 0x00001fff;
static const u32 bitmask14 = 0x00003fff;
static const u32 bitmask15 = 0x00007fff;
static const u32 bitmask16 = 0x0000ffff;
static const u32 bitmask17 = 0x0001ffff;
static const u32 bitmask18 = 0x0003ffff;
static const u32 bitmask19 = 0x0007ffff;
static const u32 bitmask20 = 0x000fffff;
static const u32 bitmask21 = 0x001fffff;
static const u32 bitmask22 = 0x003fffff;
static const u32 bitmask23 = 0x007fffff;
static const u32 bitmask24 = 0x00ffffff;
static const u32 bitmask25 = 0x01ffffff;
static const u32 bitmask26 = 0x03ffffff;
static const u32 bitmask27 = 0x07ffffff;
static const u32 bitmask28 = 0x0fffffff;
static const u32 bitmask29 = 0x1fffffff;
static const u32 bitmask30 = 0x3fffffff;
static const u32 bitmask31 = 0x7fffffff;
static const u32 bitmask32 = 0xffffffff;

static const u64 bitmask33 = 0x00000001ffffffffull;
static const u64 bitmask34 = 0x00000003ffffffffull;
static const u64 bitmask35 = 0x00000007ffffffffull;
static const u64 bitmask36 = 0x0000000fffffffffull;
static const u64 bitmask37 = 0x0000001fffffffffull;
static const u64 bitmask38 = 0x0000003fffffffffull;
static const u64 bitmask39 = 0x0000007fffffffffull;
static const u64 bitmask40 = 0x000000ffffffffffull;
static const u64 bitmask41 = 0x000001ffffffffffull;
static const u64 bitmask42 = 0x000003ffffffffffull;
static const u64 bitmask43 = 0x000007ffffffffffull;
static const u64 bitmask44 = 0x00000fffffffffffull;
static const u64 bitmask45 = 0x00001fffffffffffull;
static const u64 bitmask46 = 0x00003fffffffffffull;
static const u64 bitmask47 = 0x00007fffffffffffull;
static const u64 bitmask48 = 0x0000ffffffffffffull;
static const u64 bitmask49 = 0x0001ffffffffffffull;
static const u64 bitmask50 = 0x0003ffffffffffffull;
static const u64 bitmask51 = 0x0007ffffffffffffull;
static const u64 bitmask52 = 0x000fffffffffffffull;
static const u64 bitmask53 = 0x001fffffffffffffull;
static const u64 bitmask54 = 0x003fffffffffffffull;
static const u64 bitmask55 = 0x007fffffffffffffull;
static const u64 bitmask56 = 0x00ffffffffffffffull;
static const u64 bitmask57 = 0x01ffffffffffffffull;
static const u64 bitmask58 = 0x03ffffffffffffffull;
static const u64 bitmask59 = 0x07ffffffffffffffull;
static const u64 bitmask60 = 0x0fffffffffffffffull;
static const u64 bitmask61 = 0x1fffffffffffffffull;
static const u64 bitmask62 = 0x3fffffffffffffffull;
static const u64 bitmask63 = 0x7fffffffffffffffull;
static const u64 bitmask64 = 0xffffffffffffffffull;

static const u32 bit1 = 1 << 0;
static const u32 bit2 = 1 << 1;
static const u32 bit3 = 1 << 2;
static const u32 bit4 = 1 << 3;
static const u32 bit5 = 1 << 4;
static const u32 bit6 = 1 << 5;
static const u32 bit7 = 1 << 6;
static const u32 bit8 = 1 << 7;
static const u32 bit9 = 1 << 8;
static const u32 bit10 = 1 << 9;
static const u32 bit11 = 1 << 10;
static const u32 bit12 = 1 << 11;
static const u32 bit13 = 1 << 12;
static const u32 bit14 = 1 << 13;
static const u32 bit15 = 1 << 14;
static const u32 bit16 = 1 << 15;
static const u32 bit17 = 1 << 16;
static const u32 bit18 = 1 << 17;
static const u32 bit19 = 1 << 18;
static const u32 bit20 = 1 << 19;
static const u32 bit21 = 1 << 20;
static const u32 bit22 = 1 << 21;
static const u32 bit23 = 1 << 22;
static const u32 bit24 = 1 << 23;
static const u32 bit25 = 1 << 24;
static const u32 bit26 = 1 << 25;
static const u32 bit27 = 1 << 26;
static const u32 bit28 = 1 << 27;
static const u32 bit29 = 1 << 28;
static const u32 bit30 = 1 << 29;
static const u32 bit31 = 1 << 30;
static const u32 bit32 = 1 << 31;

static const u64 bit33 = 1ull << 32;
static const u64 bit34 = 1ull << 33;
static const u64 bit35 = 1ull << 34;
static const u64 bit36 = 1ull << 35;
static const u64 bit37 = 1ull << 36;
static const u64 bit38 = 1ull << 37;
static const u64 bit39 = 1ull << 38;
static const u64 bit40 = 1ull << 39;
static const u64 bit41 = 1ull << 40;
static const u64 bit42 = 1ull << 41;
static const u64 bit43 = 1ull << 42;
static const u64 bit44 = 1ull << 43;
static const u64 bit45 = 1ull << 44;
static const u64 bit46 = 1ull << 45;
static const u64 bit47 = 1ull << 46;
static const u64 bit48 = 1ull << 47;
static const u64 bit49 = 1ull << 48;
static const u64 bit50 = 1ull << 49;
static const u64 bit51 = 1ull << 50;
static const u64 bit52 = 1ull << 51;
static const u64 bit53 = 1ull << 52;
static const u64 bit54 = 1ull << 53;
static const u64 bit55 = 1ull << 54;
static const u64 bit56 = 1ull << 55;
static const u64 bit57 = 1ull << 56;
static const u64 bit58 = 1ull << 57;
static const u64 bit59 = 1ull << 58;
static const u64 bit60 = 1ull << 59;
static const u64 bit61 = 1ull << 60;
static const u64 bit62 = 1ull << 61;
static const u64 bit63 = 1ull << 62;
static const u64 bit64 = 1ull << 63;
