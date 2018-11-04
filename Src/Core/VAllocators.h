#pragma once


/*
only grows and the allocated memory for the chunks will be freed by DTor or Destroy()
*/
class FreeListAllocator
{
public:
	FreeListAllocator(const FreeListAllocator&) = delete;
	FreeListAllocator& operator = (const FreeListAllocator&) = delete;

	FreeListAllocator()
		: mChunkHead(nullptr), mFirstFree(nullptr), mElementSize(0), mNumElementInChunk(0) {}

	FreeListAllocator(int elementSize, int numElementInChunk)
		: mChunkHead(nullptr), mFirstFree(nullptr), mElementSize(0), mNumElementInChunk(0)
	{
		Reset(elementSize, numElementInChunk);
	}
	~FreeListAllocator() { Destroy(); }

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
class LinearAllocator
{
public:
	LinearAllocator(const LinearAllocator&) = delete;
	LinearAllocator& operator = (const LinearAllocator&) = delete;

	LinearAllocator() : mCurrentSeek(0), mCurrentEnd(0), mChunkSize(0), mChunkHead(nullptr) {}
	LinearAllocator(size_t chunkSize) : mCurrentSeek(0), mCurrentEnd(0), mChunkSize(0), mChunkHead(nullptr) { Reset(chunkSize); }

	~LinearAllocator() { Destroy(); }

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
