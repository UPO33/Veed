#pragma once

#include "VCoreBase.h"

template <typename TElement, unsigned MaxElement> struct VHistoryQueue
{
	static_assert(MaxElement > 2, "");

	int mSize = 0;
	int mRead = 0;
	alignas(alignof(TElement)) char mBuffer[MaxElement * sizeof(TElement)];
	

	VHistoryQueue()
	{

	}
	~VHistoryQueue()
	{
		Reset();
	}
	bool IsEmpty() const
	{
		return mSize == 0;
	}
	bool IsFull() const
	{
		return mSize == MaxElement;
	}
	//return true if queue was full and oldest item removed. 
	bool Push(const TElement& itemToBeAdded, TElement& removedItem)
	{
		if (IsFull())
		{
			Dequeue(removedItem);
			Enqueue(itemToBeAdded);
			return true;
		}

		Enqueue(itemToBeAdded);
		return false;
	}

	template<typename... TArgs> void Enqueue(TArgs... args)
	{
		VASSERT(!IsFull());
		auto index = (mRead + mSize) % MaxElement;
		new (Elements() + index) TElement(args...);
		mSize++;
	}
	void Dequeue(TElement& out)
	{
		VASSERT(!IsEmpty());
		out = std::move(Elements()[mRead]);
		mRead = (mRead + 1) % MaxElement;
		mSize--;
	}
	void Reset()
	{
		for (int i = 0; i < mSize; i++)
			Newest(i).~TElement();

		mSize = 0;
		mRead = 0;
	}
	//
	bool IsIndexValid(int index) const { return index >= 0 && index < Length(); }
	//
	static int Capacity() { return MaxElement; }
	//returns number of elements available
	int Length() const { return mSize; }
	//0 == newest, Length -1 == oldest
	TElement& Newest(int index = 0)
	{
		VASSERT(IsIndexValid(index));
		return Oldest(mSize - index - 1);
	}
	//0 == oldest, Length - 1 == newset
	TElement& Oldest(int index = 0)
	{
		VASSERT(IsIndexValid(index));
		return Elements()[(mRead + index) % MaxElement];
	}

	TElement* Elements()
	{
		return (TElement*)mBuffer;
	}
};