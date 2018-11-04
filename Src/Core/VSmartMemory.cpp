#include "VSmartMemory.h"

VSmartMemBlock* VSmartMemBlock::Create(size_t size, size_t align /*= 16*/, bool bZeroMemory)
{
	
	auto finalSize = sizeof(VSmartMemBlock) + size + align;
	auto pAllocated = (VSmartMemBlock*)VMemAlloc(finalSize);
	VASSERT(pAllocated);
	if (bZeroMemory) VMemZero(pAllocated, finalSize);
	return new (pAllocated) VSmartMemBlock(size, (void*)VNextMul((size_t)&pAllocated->mDataHead, align));
}
