#pragma once

// UTL: The universal utility library
// Copyright 2019-2020 Force67.

#include "utypes.h"

#if defined(__clang__) || defined(__GNUC__)
#define NAKED __attribute__((naked))
#define PACKED __attribute__((packed))
#define PS4ABI __attribute__((sysv_abi))
#define NORETURN __attribute__((noreturn))
#define F_INLINE __attribute__((inline_always))

#define bswap16 __builtin_bswap16
#define bswap32 __builtin_bswap32
#define bswap64 __builtin_bswap64

#define rotr16 __builtin_rotateright16
#define rotr32 __builtin_rotateright32
#define rotr64 __builtin_rotateright64

#define rotl16 __builtin_rotateleft16
#define rotl32 __builtin_rotateleft32
#define rotl64 __builtin_rotateleft64

#ifdef _WIN32
#define dbg_break() __debugbreak()
#else
#define dbg_break() asm("int $3")
#endif

#define EXPORT
#define IMPORT

#else
#define NAKED naked
#define PACKED
#define PS4ABI
#define NORETURN

#define bswap16 _byteswap_ushort
#define bswap32 _byteswap_ulong
#define bswap64 _byteswap_uint64

#define dbg_break() __debugbreak()

#endif

#ifdef _WIN32
#define UTL_EXPORT __declspec(dllexport)
#define UTL_IMPORT __declspec(dllimport)
#define UTL_PATH_SEP "\\"
#elif
#define UTL_EXPORT
#define UTL_IMPORT
#define UTL_PATH_SEP "/"
#endif

#define BUGCHECK dbg_break