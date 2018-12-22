#pragma once

#include "VCoreBase.h"

/*
only grows and the allocated memory for the chunks will be freed by DTor or Destroy()
*/
class VFreeListAllocator
{
public:
	VFreeListAllocator(const VFreeListAllocator&) = delete;
	VFreeListAllocator& operator = (const VFreeListAllocator&) = delete;

	VFreeListAllocator()
		: mChunkHead(nullptr), mFirstFree(nullptr), mElementSize(0), mNumElementInChunk(0) {}

	VFreeListAllocator(int elementSize, int numElementInChunk)
		: mChunkHead(nullptr), mFirstFree(nullptr), mElementSize(0), mNumElementInChunk(0)
	{
		Reset(elementSize, numElementInChunk);
	}
	~VFreeListAllocator() { Destroy(); }

	void*	Alloc();
	//make sure that ptr allocated using this allocator, no debugging is available currently
	void	Free(void* ptr);

	void	Reset(int elementSize, int numElementInChunk);

	//free all the elements but keep allocated chunks
	void	FreeAll();
	//free all the elements and allocated chunks
	void	Destroy();

	int		ElementSize() const { return mElementSize; }
	int		NumElementInChunk() const { return mNumElementInChunk; }





private:
	//this is just for casting, for the real structure consider the commented mELement[mElementSize] as well
	union Elem
	{
		Elem*		mNextFree;	//is used when this element is free
								//byte		mElement[mElementSize];
	};

	/*
	the structure of chunk is as follow:
	struct alignof(VCACHE_ALIGN) Chunk
	{
	Elem			mElements[mNumElementInChunck];
	Chunk*			mNextChunk;
	};
	*/

	Elem*	mFirstFree;
	void*	mChunkHead;
	int		mElementSize;
	int		mNumElementInChunk;


	void*& ChunkGetNext(const void* chunk) const;
	void* AllocChunk() const;
	void FreeChunkElements(void* chunkElements);
	void AddNewChunk();
	void DebugCheckFree(void* memory);
};


/*
only allocates and grows. the allocated memories will be released all by DTor or Destroy()
*/
class VLinearAllocator
{
public:
	VLinearAllocator(const VLinearAllocator&) = delete;
	VLinearAllocator& operator = (const VLinearAllocator&) = delete;

	VLinearAllocator() : mCurrentSeek(0), mCurrentEnd(0), mChunkSize(0), mChunkHead(nullptr) {}
	VLinearAllocator(size_t chunkSize) : mCurrentSeek(0), mCurrentEnd(0), mChunkSize(0), mChunkHead(nullptr) { Reset(chunkSize); }

	~VLinearAllocator() { Destroy(); }

	void*	Alloc(size_t size, size_t align = sizeof(void*));
	void	Reset(size_t chunkSize);
	//free all the elements and allocated chunks
	void	Destroy();


private:
	struct Chunk
	{
		Chunk*	mNext;
	};

	size_t		mCurrentSeek;
	size_t		mCurrentEnd;
	Chunk*		mChunkHead;
	size_t		mChunkSize;
};
