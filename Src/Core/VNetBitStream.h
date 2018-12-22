#pragma once

#include "VCoreBase.h"

void VBitCopy(void* __restrict dst, size_t dstOffset, const void* __restrict src, size_t srcOffset, size_t bitCount);

//////////////////////////////////////////////////////////////////////////
//base class for serializing bits
struct VBitStream
{

protected:
	VBitStream(){}
	~VBitStream();

public:
	//returns number of bits available to read or write
	size_t Avail() const { return mCapacity - mSeek; }
	//returns current size of buffer in bits
	size_t Seek() const { return mSeek; }
	//returns capacity in bits
	size_t Capacity() const { return mCapacity; }
	//return size of current data in bytes
	size_t GetSizeByte() const;
	//if any reading on this buffer failed. it returns false. we won't be able to read any more if its not valid.
	bool IsValid() const { return mIsValid; }

	void* GetBytes() { return mBytes; }
	const void* GetBytes() const { return mBytes; }
	
	void CheckInc(size_t count);
	void RWBit(bool& bit);
	void RWBits(void* bits, size_t offset, size_t bitcount);
	//read/write a 32bit integer with specified bit length. dosen't check for boundary
	void RWNumber(uint32_t& n, size_t bitCount);
	//read/write a 32 bit integer as packed. the first bit of each bytes indicated more.
	void RWNumerBytePacked(uint32_t& n);

protected:
	size_t		mSeek;
	size_t		mCapacity;
	uint8_t*	mBytes;
	bool		mIsSaving;
	bool		mShouldFreeMemory;
	bool		mIsValid;

	

};
//////////////////////////////////////////////////////////////////////////
struct VBitReader : VBitStream
{
	//#note it dosen't take a copy from @pBits so u have to keep it yourself
	VBitReader(const void* pBits, size_t bitCount)
	{
		mIsSaving = false;
		mIsValid = true;
		mShouldFreeMemory = false;
		mBytes = (uint8_t*)pBits;
		mCapacity = bitCount;
		mSeek = 0;
	}
};
//////////////////////////////////////////////////////////////////////////
struct VBitWriter : VBitStream
{
	VBitWriter(size_t initialCapacityInByte = 0);
};
//////////////////////////////////////////////////////////////////////////
template<size_t CapacityInByte> struct VBitWriterStack : VBitStream
{
	VBitWriterStack()
	{
		mIsSaving = true;
		mIsValid = true;
		mShouldFreeMemory = false;
		mBytes = mBuffer;
		mCapacity = CapacityInByte * 8;
		mSeek = 0;
		//zero the last byte
		mBytes[CapacityInByte - 1] = 0;
	}

	uint8_t mBuffer[CapacityInByte];
};

inline VBitStream& operator << (VBitStream& stream, bool& b)
{
	stream.RWBit(b);
	return stream;
}
inline VBitStream& operator << (VBitStream& stream, uint32_t& n)
{
	stream.RWNumerBytePacked(n);
	return stream;
}