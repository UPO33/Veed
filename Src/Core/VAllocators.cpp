#include "VAllocators.h"
#include "VMemory.h"

//////////////////////////////////////////////////////////////////////////
void*& FreeListAllocator::ChunkGetNext(const void* chunk) const
{
	return *((void**)(((size_t)chunk) + mElementSize * mNumElementInChunk + sizeof(void*)));
}

void* FreeListAllocator::AllocChunk() const
{
	return VMemAllocAligned(mElementSize * mNumElementInChunk + sizeof(void*), VCACHE_ALIGN);
}

void FreeListAllocator::FreeChunkElements(void* chunkElements)
{
	for (int i = mNumElementInChunk - 1; i >= 0; i--)
	{
		Elem* elem = reinterpret_cast<Elem*>(((char*)chunkElements) + (i * mElementSize));
		elem->mNextFree = mFirstFree;
		mFirstFree = elem;
	}
}

void FreeListAllocator::AddNewChunk()
{
	void* newChunk = AllocChunk();
	FreeChunkElements(newChunk);
	ChunkGetNext(newChunk) = mChunkHead;
	mChunkHead = newChunk;
}

void FreeListAllocator::DebugCheckFree(void* memory)
{
	//#TODO: check whether 'memory' is allocated and belongs to this allocator

	VASSERT(memory);
}

void FreeListAllocator::Reset(int elementSize, int numElementInChunk)
{
	VASSERT(elementSize >= sizeof(void*));	//because of Elem.mNextFree
	VASSERT(numElementInChunk > 0);

	Destroy();
	mElementSize = elementSize;
	mNumElementInChunk = numElementInChunk;
}

void* FreeListAllocator::Alloc()
{
	VASSERT(mElementSize && mNumElementInChunk);

	if (mFirstFree == nullptr) AddNewChunk();

	auto tmp = mFirstFree;
	mFirstFree = mFirstFree->mNextFree;
	return tmp;
}

void FreeListAllocator::Free(void* ptr)
{
	DebugCheckFree(ptr);

	Elem* elem = reinterpret_cast<Elem*>(ptr);
	elem->mNextFree = mFirstFree;
	mFirstFree = elem;
}

void FreeListAllocator::FreeAll()
{
	mFirstFree = nullptr;

	void* chunk = mChunkHead;
	while (chunk)
	{
		FreeChunkElements(chunk);
		chunk = ChunkGetNext(chunk);
	}
}

void FreeListAllocator::Destroy()
{
	void* chunk = mChunkHead;
	while (chunk)
	{
		auto tmp = chunk;
		chunk = ChunkGetNext(chunk);
		VMemFreeAligned(tmp);
	}

	mChunkHead = nullptr;
	mFirstFree = nullptr;
}

void* LinearAllocator::Alloc(size_t size, size_t align)
{
	VASSERT(size && align);
	VASSERT(size < mChunkSize);

	size_t start = VNextAlign(mCurrentSeek, align);
	if (start + size < mCurrentEnd)
	{
		mCurrentSeek = start + size;
		return (void*)start;
	}
	else
	{
		Chunk* newChunk = (Chunk*)VMemAlloc(mChunkSize + sizeof(void*));
		newChunk->mNext = mChunkHead;
		mChunkHead = newChunk;

		size_t base = ((size_t)newChunk) + sizeof(void*);
		mCurrentEnd = base + mChunkSize;

		size_t newStart = VNextAlign(base, align);
		VASSERT(newStart + size < mCurrentEnd);
		mCurrentSeek = newStart + size;
		return (void*)newStart;
	}
}

void LinearAllocator::Reset(size_t chunkSize)
{
	Destroy();
	mChunkSize = chunkSize;
}

void LinearAllocator::Destroy()
{
	Chunk* iter = mChunkHead;
	while (iter)
	{
		Chunk* tmp = iter;
		iter = iter->mNext;
		VMemFree(tmp);
	}

	mChunkHead = nullptr;
	mCurrentSeek = mCurrentEnd = 0;
}
