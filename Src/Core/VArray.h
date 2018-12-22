#pragma once

#include "VMemory.h"

static constexpr size_t VINVALID_INDEX = 0xFFffFFff;

#pragma region global functions
//removes the intended elements of a array and keep the order of elements
//this function is fast enough in case over 10% of the elements are supposed to be removed, more percent more performance
//if TElement's ctor dtor must be handled properly use 'ArrayConditionalRemove()' instead
template<typename TElement, typename TLength, typename TLambda> void VArrayConditionalRemovePOD(TElement* arrayElements, TLength& arrayLength, TLambda proc)
{
	TLength iWriteElement = 0;
	for (TLength iElement = 0; iElement < arrayLength; iElement++)
	{
		if (proc(arrayElements[iElement]))
		{
			continue;
		}
		else
		{
			//@note: we don't need to check the equality of iWriteElement and iElement for better performance
			//when they are same the memory would be same so that it seems CPU actually doesn't do any MOV
			arrayElements[iWriteElement] = arrayElements[iElement];
			iWriteElement++;
		}
	}
	arrayLength = iWriteElement;
};
template<typename TElement, typename TLength, typename TLambda> void VArrayConditionalRemove(TElement* arrayElements, TLength& arrayLength, TLambda proc)
{
	TLength iWriteElement = 0;
	for (TLength iElement = 0; iElement < arrayLength; iElement++)
	{
		if (proc(arrayElements[iElement]))
		{
			arrayElements[iElement].~TElement();
			continue;
		}
		else
		{
			if (iWriteElement != iElement)
			{
				new (arrayElements + iWriteElement) TElement(arrayElements[iElement]);
				arrayElements[iElement].~TElement();
			}

			iWriteElement++;
		}
	}
	arrayLength = iWriteElement;
};
template<typename TElement, typename TIndex> void VArrayRemoveAtSwap(TElement* elements, TIndex& length, TIndex index)
{
	VASSERT(length != 0 && index < length);
	length--;
	if (index != length)
		elements[index] = std::move(elements[length]);
	else
		elements[index].~TElement();
}
//only calls destructor on removing elements
template<typename TElement, typename TIndex> void VArrayRemoveShift(TElement* elements, TIndex& length, TIndex removeIndex, TIndex removeCount)
{
	VASSERT(removeIndex < length && removeCount > 0);
	VASSERT(removeIndex + removeCount <= length);
	TIndex n = removeIndex + removeCount;
	//calling destructor
	for (TIndex i = removeIndex; i < n; i++)
		(elements + i)->~TElement();

	VMemMove(elements + removeIndex, elements + n, (length - n) * sizeof(TElement));
	length -= removeCount;
}
template<typename TElement, typename TIndex> void VArrayReverse(TElement* elements, TIndex length)
{
	for (TIndex iIndex = 0; iIndex < length / 2; iIndex++)
	{
		auto temp = elements[length - iIndex - 1];
		elements[length - iIndex - 1] = elements[iIndex];
		elements[iIndex] = temp;
	}
}
template<typename TElement> int VArrayFindFirst(const TElement* elements, int length, const TElement& search)
{
	for (int i = 0; i < length; i++)
	{
		if (elements[i] == search)
			return i;
	}
	return -1;
}
template<typename TElement> int VArrayFindFirstRev(const TElement* elements, int length, const TElement& search)
{
	for (int i = length - 1; i >= 0; i--)
	{
		if (elements[i] == search)
			return i;
	}
	return -1;
}
template<typename TElement> int VArrayFindLast(const TElement* elements, int length, const TElement& search)
{
	int iLast = -1;
	for (int i = 0; i < length; i++)
	{
		if (elements[i] == search)
			iLast = i;
	}
	return iLast;
}
#pragma  endregion


struct VVArrayHeader
{
	size_t mLength = 0;
	size_t mCapacity = 0;

	static const VVArrayHeader EmptyInstance;
};

///////////////////////////////////
template<typename TElement> class VArray
{
public:
	using ElementT = TElement;

	struct Header
	{
		int mLength;
		int mCapacity;
		//this is fake we only use this struct for casting
		TElement mItems[0xFFffFF];
	};

	//empty initializer
	VArray() : mHeader(GetEmptyHeader())
	{}
	//initialize with initial capacity
	VArray(size_t initailCapacity) : mHeader(GetEmptyHeader())
	{
		IncCap(initailCapacity);
	}
	//copy constructor
	VArray(const VArray& copy) : VArray(copy.Elements(), copy.Length())
	{
	}
	//copy from std::initializer_list
	VArray(const std::initializer_list<TElement>& list) : VArray(list.begin(), list.size())
	{
	}
	//constructing from raw elements
	VArray(const TElement* pElements, size_t count)
	{
		mHeader = GetEmptyHeader();
		if (pElements && count)
		{
			AddUninitilized(count);
			for (size_t i = 0; i < count; i++)
			{
				new (mHeader->mItems + i) TElement(pElements[i]);
			}
		}
	}
	//move constructor
	VArray(VArray&& move)
	{
		mHeader = move.mHeader;
		move.mHeader = GetEmptyHeader();
	}
	//move assignment
	VArray& operator =(VArray&& move)
	{
		if (this != &move)
		{
			this->Empty();
			new (this) VArray(move);
		}
		return *this;
	}
	//copy assignment
	VArray& operator = (const VArray& copy)
	{
		Reset();
		Append(copy.Elements(), copy.Length());
		return *this;
	}
	//
	~VArray()
	{
		Empty();
	}
	//must be always valid
	Header* mHeader;


	int Length() const { return mHeader->mLength; }
	int Capacity() const { return mHeader->mCapacity; }
	bool IsIndexValid(int index) const { return index >= 0 && index <= Length(); }

	TElement* Elements() { return (TElement*)mHeader->mItems; }
	const TElement* Elements() const { return (const TElement*)mHeader->mItems; }

	TElement& First(int index = 0) { return Elements()[index]; }
	const TElement& First(int index = 0) const { return Elements()[index]; }

	TElement& Last(int index = 0) { return Elements()[Length() - 1 - index]; }
	const TElement& Last(int index = 0) const { return Elements()[Length() - 1 - index]; }

	const TElement& operator [] (size_t index) const { return First(index); }
	TElement& operator [] (size_t index) { return First(index); }

	//return number of available free element without any allocation
	size_t Avail() const { return mHeader->mCapacity - mHeader->mLength; }

	size_t AddUninitilized(size_t count = 1)
	{
		CheckInc(count);

		auto ret = mHeader->mLength;
		mHeader->mLength += count;
		return ret;
	}
	size_t AddZeroed(size_t count = 1)
	{
		CheckInc(count);
		VMemZero(mHeader->mItems + mHeader->mLength, sizeof(TElement) * count);

		auto ret = mHeader->mLength;
		mHeader->mLength += count;
		return ret;
	}
	//adds an element at the end of array
	template<typename... TArgs> size_t Add(TArgs... args)
	{
		CheckInc(1);
		new (mHeader->mItems + mHeader->mLength) TElement(args...);
		return mHeader->mLength++;
	}
	//
	void Append(const TElement* elements, size_t count)
	{
		auto start = AddUninitilized(count);
		for (size_t i = 0; i < count; i++)
		{
			new (mHeader->mItems + (start + i)) TElement(elements[i]);
		}
	}
	//
	void AppendMove(const TElement* elements, size_t count)
	{
		auto start = AddUninitilized(count);
		for (size_t i = 0; i < count; i++)
		{
			new (mHeader->mItems + (start + i)) TElement(std::move(elements[i]));
		}
	}
	//
	void CheckInc(size_t count)
	{
		auto avail = mHeader->mCapacity - mHeader->mLength;
		if (count > avail)
		{
			count += 128;
			IncCap(count);
		}
	}
	//
	void IncCap(size_t count)
	{
		if (count == 0) return;

		VASSERT(mHeader);
		if (mHeader == GetEmptyHeader())
		{
			VASSERT(mHeader->mCapacity == 0 && mHeader->mLength == 0);

			size_t memorySize = sizeof(VVArrayHeader) + (sizeof(TElement) * count);
			auto pNewHeader = (Header*)AllocMemory(memorySize);
			VASSERT(pNewHeader);

			pNewHeader->mLength = 0;
			pNewHeader->mCapacity = count;

			mHeader = pNewHeader;
		}
		else
		{
			auto newCapacity = mHeader->mCapacity + count;
			auto memorySize = sizeof(VVArrayHeader) + (sizeof(TElement) * newCapacity);

			auto pNewHeader = (Header*)ReallocMemory(mHeader, newCapacity);
			VASSERT(pNewHeader);

			pNewHeader->mLength = mHeader->mLength;
			pNewHeader->mCapacity = newCapacity;

			mHeader = pNewHeader;
		}
	}
	void RemoveAtSwap(int index)
	{
		VArrayRemoveAtSwap(mHeader->mItems, mHeader->mLength, index);
	}
	void RemoveAtShift(int index, int count = 1)
	{
		VArrayRemoveShift(mHeader->mItems, mHeader->mLength, index, count);
	}
	//find the first specified element and remove it by shifting array.
	int RemoveFirstShift(const TElement& item)
	{
		auto index = FindFirst(item);
		if (index != -1) RemoveAtShift(index);
		return index;
	}
	int RemoveFirstSwap(const TElement& item)
	{
		auto index = FindFirst(item);
		if (index != -1) RemoveAtSwap(index);
		return index;
	}
	template<typename TLambda> void ConditionalRemovePOD(const TLambda& proc)
	{
		VArrayConditionalRemovePOD(mHeader->mItems, mHeader->mLength, proc);
	}
	template<typename TLambda> void ConditionalRemove(const TLambda& proc)
	{
		VArrayConditionalRemove(mHeader->mItems, mHeader->mLength, proc);
	}
	void Reverse()
	{
		VArrayReverse(mHeader->mItems, mHeader->mLength);
	}
	//
	int AddUnique(const TElement& item)
	{
		auto index = FindFirst(item);
		if (index == -1)
			index = Add(item);
		return index;
	}
	//
	void AppendUnique(const TElement* elements, size_t numElement)
	{
		if (elements == nullptr) return;

		for (size_t i = 0; i < numElement; i++)
			AddUnique(elements[i]);
	}
	//
	void Shrink()
	{
		if (mHeader != GetEmptyHeader())
		{
			auto newSize = sizeof(VVArrayHeader) + (mHeader->mLength * sizeof(TElement));
			mHeader = Realloc(mHeader, newSize);
			VASSERT(mHeader);
		}
	}
	//remove an element from the end of array and calls dtor
	void Pop()
	{
		VASSERT(Length() > 0);
		mHeader->mLength--;
		(mHeader->mItems + mHeader->mLength)->~TElement();
	}
	void PopN(size_t count)
	{
		VASSERT(count <= Length());
		for (size_t i = 0; i < count; i++)
		{
			(mHeader->mItems + (mHeader->mLength - 1 - i))->~TElement();
		}
		mHeader->mLength -= count;
	}
	void Pop(TElement& out)
	{
		VASSERT(Length() > 0);
		mHeader->mLength--;
		out = std::move(mHeader->mItems[mHeader->mLength]);
	}
	//destroy all elements and free the memory
	void Empty()
	{
		Reset();
		if (mHeader != GetEmptyHeader())
		{
			FreeMemory(mHeader);
			mHeader = GetEmptyHeader();
		}
	}
	//destroy all elements but keep the memory for further use
	void Reset()
	{
		for (int i = 0; i < mHeader->mLength; i++)
		{
			mHeader->mItems[i].~TElement();
		}
		mHeader->mLength = 0;
	}

	int FindFirst(const TElement& item) const { return VArrayFindFirst(Elements(), mHeader->mLength, item); }
	int FindFirstRev(const TElement& item) const { return VArrayFindFirstRev(Elements(), mHeader->mLength, item); }
	int FindLast(const TElement& item) const { return VArrayFindLast(Elements(), mHeader->mLength, item); }

	class Iter
	{
	public:
		TElement* ptr;
		Iter(TElement* p) : ptr(p) {}
		bool operator !=(Iter rhs) const { return ptr != rhs.ptr; }
		TElement& operator*() { return *ptr; }
		void operator++() { ptr++; }
	};
	class ConstIter
	{
	public:
		const TElement* ptr;
		ConstIter(const TElement* p) : ptr(p) {}
		bool operator !=(ConstIter rhs) const { return ptr != rhs.ptr; }
		const TElement& operator*() const { return *ptr; }
		void operator++() { ptr++; }
	};

	Iter begin() { return Iter(Elements()); }
	Iter end() { return Iter(Elements() + Length()); }
	ConstIter begin() const { return ConstIter(Elements()); }
	ConstIter end() const { return ConstIter(Elements() + Length()); }
	ConstIter cbegin() const { return begin(); }
	ConstIter cend() const { return cend(); }

private:
	
	static void* AllocMemory(size_t sizeInBytes)
	{
		return VMemAlloc(sizeInBytes);
	}
	static void* ReallocMemory(void* pMemory, size_t newSize)
	{
		return VMemRealloc(pMemory, newSize);
	}
	static void FreeMemory(void* pMemory)
	{
		VMemFree(pMemory);
	}
	inline static Header* GetEmptyHeader()
	{
		return (Header*)&VVArrayHeader::EmptyInstance;
	}
};



//////////////////////////////////////////////////////////////////////////
template<typename TElement, unsigned MaxElement> struct alignas(TElement) VStackArray
{
	union 
	{
		TElement mELements[MaxElement];
	};
	int mLength = 0;

	int Length() const { return mLength; }
	static int Capacity() { return MaxElement; }

	//default constructor
	VStackArray()
	{}
	//copy constructor
	template<unsigned CopyMaxElement> VStackArray(const VStackArray<TElement, CopyMaxElement>& copy)
	{
		VASSERT(copy.mLength <= MaxElement);
		
		Append(copy.Elements(), copy.Length());
	}
	//copy from raw array
	VStackArray(const TElement* copy, int count)
	{
		VASSERT(copy.mLength <= count);
		
		Append(copy.Elements(), copy.Length());
	}

	//destructor
	~VStackArray(){ Reset(); }
	
	void Reset()
	{
		for (int i = 0; i < mLength; i++)
			Elements()[i].~TElement();
		mLength = 0;
	}
	void Empty() { Reset(); }

	bool IsIndexValid(int index) const { return index >= 0 && index <= Length(); }
	TElement* Elements() { return (TElement*)&mELements[0]; }
	const TElement* Elements() const { return (const TElement*)&mELements[0]; }

	TElement& First(int index = 0) { return Elements()[index]; }
	const TElement& First(int index = 0) const { return Elements()[index]; }

	TElement& Last(int index = 0) { return Elements()[mLength - 1 - index]; }
	const TElement& Last(int index = 0) const { return Elements()[mLength - 1 - index]; }

	const TElement& operator [] (size_t index) const { return First(index); }
	TElement& operator [] (size_t index) { return First(index); }

	int Avail() const { return MaxElement - mLength; }



	int AddUninitilized(int count = 1)
	{
		VASSERT(Avail() >= count);
		auto ret = mLength;
		mLength += count;
		return ret;
	}
	int AddZeroed(int count = 1)
	{
		VASSERT(Avail() >= count);
		VMemZero(Elements() + mLength, sizeof(TElement) * count);

		auto ret = mLength;
		mLength += count;
		return ret;
	}
	//adds an element at the end of array
	template<typename... TArgs> size_t Add(TArgs... args)
	{
		VASSERT(Avail() >= 1);
		new (Elements() + mLength) TElement(args...);
		return mLength++;
	}
	//
	void RemoveAtSwap(int index)
	{
		VArrayRemoveAtSwap(Elements(), mLength, index);
	}
	void RemoveAtShift(int index, int count = 1)
	{
		VArrayRemoveShift(Elements(), mLength, index, count);
	}
	//find the first specified element and remove it by shifting array.
	int RemoveFirstShift(const TElement& item)
	{
		auto index = FindFirst(item);
		if (index != -1) RemoveAtShift(index);
		return index;
	}
	int RemoveFirstSwap(const TElement& item)
	{
		auto index = FindFirst(item);
		if (index != -1) RemoveAtSwap(index);
		return index;
	}
	template<typename TLambda> void ConditionalRemovePOD(const TLambda& proc)
	{
		VArrayConditionalRemovePOD(Elements(), mLength, proc);
	}
	template<typename TLambda> void ConditionalRemove(const TLambda& proc)
	{
		VArrayConditionalRemove(Elements(), mLength, proc);
	}
	void Reverse()
	{
		VArrayReverse(Elements(), mLength);
	}
	//
	int AddUnique(const TElement& item)
	{
		auto index = FindFirst(item);
		if (index == -1)
			index = Add(item);
		return index;
	}
	//
	void AppendUnique(const TElement* elements, int numElement)
	{
		if (elements == nullptr) return;

		for (size_t i = 0; i < numElement; i++)
			AddUnique(elements[i]);
	}
	//
	void Append(const TElement* elements, int count)
	{
		auto start = AddUninitilized(count);
		for (int i = 0; i < count; i++)
		{
			new (Elements() + (start + i)) TElement(elements[i]);
		}
	}
	//
	void AppendMove(const TElement* elements, int count)
	{
		auto start = AddUninitilized(count);
		for (int i = 0; i < count; i++)
		{
			new (Elements() + (start + i)) TElement(std::move(elements[i]));
		}
	}
	//remove an element from the end of array and calls dtor
	void Pop()
	{
		VASSERT(Length() > 0);
		mLength--;
		(Elements() + mLength)->~TElement();
	}
	void PopN(int count)
	{
		VASSERT(count <= Length());
		for (int i = 0; i < count; i++)
		{
			(Elements() + (mLength - 1 - i))->~TElement();
		}
		mLength -= count;
	}
	void Pop(TElement& out)
	{
		VASSERT(Length() > 0);
		mLength--;
		out = std::move(Elements()[mLength]);
	}
};