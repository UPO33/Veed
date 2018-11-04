#pragma once

#include "VSmartPointer.h"
#include "VMemory.h"

//////////////////////////////////////////////////////////////////////////
class VSmartMemBlock : public VSmartBase
{
public:
	//create a new object with specified parameters
	//the main purpose of this class is to allocate all necessary data in one allocation instead of 3 separate objects (RefCountData , Object , Memory)
	static VSmartMemBlock* Create(size_t size, size_t align = 16, bool bZeroMemory = true);
	//return the size of memory block in bytes
	size_t GetSize() const { return mSize; }
	//returns pointer to the memory block
	void* GetData() { return mMemory; }
	const void* GetData() const { return &mMemory; }

	void operator delete(void * p) { VMemFree(p); }

private:
	VSmartMemBlock(size_t size, void* pMem) : mSize(size), mMemory(pMem) 
	{}
	
	size_t mSize;
	void* mMemory;
	size_t mDataHead;
};