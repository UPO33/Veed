#include "VHash.h"
#include "VString.h"


VHash_CRC32::HashT VHash_CRC32::GetPrime()
{
	VASSERT(false);
	return 0;
}

VHash_CRC32::HashT VHash_CRC32::HashBuffer(const void* buffer, size_t length, HashT hash /*= GetPrime()*/)
{
	VASSERT(false);
	return 0;
}

VHash_CRC32::HashT VHash_CRC32::HashStr(const char* str, VHash_CRC32::HashT hash /*= GetPrime()*/)
{
	return HashBuffer(str, VStrLen(str), hash);
}

VHash_FNV32::HashT VHash_FNV32::HashBuffer(const void* buffer, size_t length, VHash_FNV32::HashT hash)
{
	VASSERT(buffer);
	unsigned char* p = (unsigned  char*)buffer;
	for (size_t i = 0; i < length; i++)
	{
		hash = (hash * 16777619) ^ p[i];
	}
	return hash;
}

VHash_FNV32::HashT VHash_FNV32::HashStr(const char* str, VHash_FNV32::HashT hash /*= GetPrime()*/)
{
	return HashBuffer(str, VStrLen(str), hash);
}

VHash_FNV64::HashT VHash_FNV64::HashBuffer(const void* buffer, size_t length, VHash_FNV64::HashT hash)
{
	VASSERT(buffer);
	unsigned char* p = (unsigned char*)buffer;
	for (size_t i = 0; i < length; i++)
	{
		hash = (hash * 1099511628211ULL) ^ p[i];
	}
	return hash;
}

VHash_FNV64::HashT VHash_FNV64::HashStr(const char* str, VHash_FNV64::HashT hash)
{
	return HashBuffer(str, VStrLen(str), hash);
}