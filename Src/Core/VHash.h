#pragma once

#include "VCoreBase.h"

//////////////////////////////////////////////////////////////////////////
struct VHash_CRC32
{
	typedef uint32_t HashT;

	static HashT GetPrime();
	static HashT HashBuffer(const void* buffer, size_t length, HashT hash = GetPrime());
	static HashT HashStr(const char* str, HashT hash = GetPrime());
};
//////////////////////////////////////////////////////////////////////////
struct VHash_FNV32
{
	typedef uint32_t HashT;

	static const uint32_t HASH_PRIME = 2166136261;

	static HashT GetPrime() { return HASH_PRIME; }
	static HashT HashBuffer(const void* buffer, size_t length, HashT hash = GetPrime());
	static HashT HashStr(const char* str, HashT hash = GetPrime());
};
//////////////////////////////////////////////////////////////////////////
struct VHash_FNV64
{
	typedef uint64_t HashT;

	static const uint64_t HASH_PRIME = 14695981039346656037ULL;

	inline static HashT GetPrime() { return HASH_PRIME; }
	static HashT HashBuffer(const void* buffer, size_t length, HashT hash = GetPrime());
	static HashT HashStr(const char* str, HashT hash = GetPrime());
};