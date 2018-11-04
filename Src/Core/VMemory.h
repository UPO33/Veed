#pragma once

#include <cstringt.h>

#include "VCoreBase.h"

inline void* VMemZero(void* dst, size_t size)
{
	return memset(dst, 0, size);
}
inline void* VMemCopy(void* dst, const void* src, size_t size)
{
	return memcpy(dst, src, size);
}
inline void* VMemMove(void* dst, const void* src, size_t size)
{
	return memmove(dst, src, size);
}


inline void* VMemAlloc(size_t size) { return ::malloc(size); }
inline void* VMemAllocZero(size_t size)
{
	void* p = VMemAlloc(size);
	if (p) VMemZero(p, size);
	return p;
}
inline void* VMemRealloc(void* memory, size_t newSize) { return ::realloc(memory, newSize); }
inline void  VMemFree(void* memory) { ::free(memory); }
template<typename T> void VMemFreeSafe(T*& memory)
{
	if (memory) 
	{
		VMemFree(memory);
		memory = nullptr;
	}
	
}
//allocate and initialize by copying from @copyFrom
inline void* VMemAllocCopy(size_t size, const void* copyFrom)
{
	void* pMem = VMemAlloc(size);
	if (pMem) VMemCopy(pMem, copyFrom, size);
	return pMem;
}
#ifdef VCOMPILER_MSVC
inline void* VMemAllocAligned(size_t size, size_t align) { return ::_aligned_malloc(size, align); }
inline void* VMemAllocAlignedZero(size_t size, size_t align)
{
	void* p = VMemAllocAligned(size, align);
	if (p) VMemZero(p, size);
	return p;
}
inline void* VMemReallocAligned(void* memory, size_t newSize, size_t newAlignment) { return ::_aligned_realloc(memory, newSize, newAlignment); }
inline void  VMemFreeAligned(void* memory) { ::_aligned_free(memory); }
#else
//#TODO aligned allocation for other compilers
inline void* VMemAllocAligned(size_t size, size_t align) { return ::malloc(size); }
inline void* VMemAllocAlignedZero(size_t size, size_t align)
{
	void* p = VMemAllocAligned(size, align);
	if (p) VMemZero(p, size);
	return p;
}
inline void* VMemReallocAligned(void* memory, size_t newSize, size_t newAlignment) { return ::realloc(memory, newSize); }
inline void  VMemFreeAligned(void* memory) { ::free(memory); }
#endif

template<typename T> void  VMemFreeAlignedSafe(T*& memory)
{
	if (memory)
	{
		VMemFreeAligned(memory);
		memory = nullptr;
	}
}

template<typename T> void VMemZero(T& object)
{
	::memset(&object, 0, sizeof(T));
};

template<typename T> void VSafeDelete(T*& object)
{
	if (object)
	{
		delete object;
		object = nullptr;
	}
}
template<typename T> void VSafeDeleteArray(T*& objectArray)
{
	if (objectArray)
	{
		delete[] objectArray;
		objectArray = nullptr;
	}
}