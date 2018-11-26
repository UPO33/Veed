#include "VNetBitStream.h"
#include "VMemory.h"
#include "VCoreBase.h"

void VBitCopy(void* dst, size_t dstOffset, const void* src, size_t srcOffset, size_t bitCount)
{
	uint8_t* pDst = (uint8_t*)dst;
	const uint8_t* pSrc = (const uint8_t*)src;

	static const uint8_t BitRevs[8] = {
		1 << 7,
		1 << 6,
		1 << 5,
		1 << 4,
		1 << 3,
		1 << 2,
		1 << 1,
		1
	};
	static const uint8_t BitRevsNeg[8] = {
		~(1 << 7),
		~(1 << 6),
		~(1 << 5),
		~(1 << 4),
		~(1 << 3),
		~(1 << 2),
		~(1 << 1),
		~(1)
	};

	for (size_t i = 0; i < bitCount; i++)
	{
		uint8_t srcByte = pSrc[(srcOffset + i) / 8];

		if (srcByte & BitRevs[(srcOffset + i) % 8])
		{
			size_t dstByteIndex = (dstOffset + i) / 8;
			pDst[dstByteIndex] |= BitRevs[(dstOffset + i) % 8];
		}
		else
		{
			size_t dstByteIndex = (dstOffset + i) / 8;
			pDst[dstByteIndex] &= BitRevsNeg[(dstOffset + i) % 8];
		}
	}
}


VBitStream::~VBitStream()
{
	if (mShouldFreeMemory && mBytes)
	{
		VMemFree(mBytes);
	}

	mCapacity = mSeek = 0;
	mBytes = nullptr;
	mIsValid = false;
}

size_t VBitStream::GetSizeByte() const
{
	return VNextAlign(mSeek, 8) / 8;
}

void VBitStream::CheckInc(size_t count)
{
	if (Avail() < count)
	{
		if (!mShouldFreeMemory)
			return;

		{
			//we always grow more than this to not reallocate for little increases
			const size_t MinGrow = 4096;

			mCapacity = mCapacity + count + MinGrow;
			size_t byteNeeded = VNextAlign(mCapacity, 8) / 8;
			mBytes = (uint8_t*)VMemRealloc(mBytes, byteNeeded);
			VASSERT(mBytes);

			//zero the last byte so that we can compare the memory byte by byte.
			mBytes[byteNeeded - 1] = 0;
		}
	}
}

void VBitStream::RWBit(bool& bit)
{
	if (mIsSaving)
	{
		CheckInc(1);

		if (bit)
		{
			mBytes[mSeek / 8] |= (1 << (7 - (mSeek % 8)));
		}
		else
		{
			mBytes[mSeek / 8] &= ~(1 << (7 - (mSeek % 8)));
		}
		mSeek++;
	}
	else
	{
		if (mIsValid == false)
			return;

		if (Avail() == 0)
		{
			mIsValid = false;
			return;
		}

		auto byte = mBytes[mSeek / 8];
		auto bitIndex = (1 << (7 - (mSeek % 8)));
		bit = byte & bitIndex;
		mSeek++;
	}
}

void VBitStream::RWBits(void* bits, size_t offset, size_t bitcount)
{
	if (mIsSaving)
	{
		CheckInc(bitcount);
		VBitCopy(mBytes, mSeek, bits, offset, bitcount);
		mSeek += bitcount;
	}
	else
	{
		if (!mIsValid) return;

		if (bitcount > Avail())
		{
			mIsValid = false;
			return;
		}

		VBitCopy(bits, offset, mBytes, mSeek, bitcount);
		mSeek += bitcount;
	}
}

void VBitStream::RWNumber(uint32_t& n, size_t bitCount)
{
	VASSERT(bitCount < 33);

	//zero the vale if we are reading.
	if (!mIsSaving && mIsValid) 
		n = 0;
	RWBits(&n, 0, bitCount);
}

void VBitStream::RWNumerBytePacked(uint32_t& n)
{
	//#TODO
	RWNumber(n, 33);
}

VBitWriter::VBitWriter(size_t initialCapacityInByte)
{
	mIsSaving = true;
	mIsValid = true;
	mShouldFreeMemory = true;
	mSeek = mCapacity = 0;
	mBytes = nullptr;

	if (initialCapacityInByte)
	{
		CheckInc(initialCapacityInByte * 8);
	}
}
