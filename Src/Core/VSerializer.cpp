#include "VSerializer.h"
#include "VMemory.h"
#include "VLogger.h"

VArchiveByteWriter::VArchiveByteWriter(VArchiveByteWriter&& move)
{
	if (move.mOwnsMemory)
	{
		mCapacity = move.mCapacity;
		mSeek = move.mSeek;
		mOwnsMemory = move.mOwnsMemory;
		mIsValid = move.mIsValid;
		mIsSaving = move.mIsSaving;
		mBytes = move.mBytes;

		move.mSeek = 0;
		move.mCapacity = 0;
		move.mBytes = nullptr;
	}
	else
	{
		mCapacity = move.mSeek;
		mSeek = move.mSeek;
		mOwnsMemory = true;
		mIsValid = move.mIsValid;
		mIsSaving = true;
		mBytes = nullptr;
		if (move.mSeek)
		{
			VASSERT(move.mBytes);
			mBytes = (uint8_t*)VMemAllocCopy(move.mSeek, move.mBytes);
			VASSERT(mBytes);
		}
		move.mBytes = nullptr;
		move.mSeek = 0;

	}
}

VArchiveByteWriter::VArchiveByteWriter(size_t initialCapacity /*= 0*/)
{
	mCapacity = initialCapacity;
	mSeek = 0;
	mOwnsMemory = true;
	mIsValid = true;
	mIsSaving = true;
	mBytes = nullptr;
	if (initialCapacity)
	{
		mBytes = (uint8_t*)VMemAlloc(initialCapacity);
		VASSERT(mBytes);
	}
}

VArchiveByteWriter::VArchiveByteWriter(const VArchiveByteWriter& copy)
{
	mCapacity = copy.mSeek;
	mSeek = copy.mSeek;
	mOwnsMemory = true;
	mIsValid = copy.mIsValid;
	mIsSaving = copy.mIsSaving;
	mBytes = nullptr;
	if (copy.mSeek)
	{
		mBytes = (uint8_t*)VMemAllocCopy(copy.mSeek, copy.mBytes);
		VASSERT(mBytes);
	}
}

VArchiveByteWriter::~VArchiveByteWriter()
{
	if (mOwnsMemory && mBytes)
	{
		VMemFree(mBytes);
		mBytes = nullptr;
	}
	mCapacity = mSeek = 0;
}

void VArchiveByteWriter::RWBytes(void* bytes, size_t sizeInBytes)
{
	if (sizeInBytes == 0) return;
	VASSERT(bytes);
	CheckInc(sizeInBytes);
	VMemCopy(mBytes + mSeek, bytes, sizeInBytes);
	mSeek += sizeInBytes;
}

void VArchiveByteWriter::CheckInc(size_t size)
{
	if (size > Avail())
	{
		if (mOwnsMemory)
		{
			mCapacity = mCapacity + size + 512;
			mBytes = mBytes ? (uint8_t*)VMemRealloc(mBytes, mCapacity) : (uint8_t*)VMemAlloc(mCapacity);
			VASSERT(mBytes);
		}
		else
		{
			VASSERT(false && "buffer overflow");
		}
	}
}

VArchiveByteReader::VArchiveByteReader(void* pData, size_t dataSize, bool bCopyData)
{
	mCapacity = dataSize;
	mSeek = 0;
	mOwnsMemory = bCopyData;
	mIsValid = true;
	mIsSaving = false;
	mBytes = (uint8_t*)pData;
	if (bCopyData)
	{
		mBytes = (uint8_t*)VMemAllocCopy(dataSize, pData);
		VASSERT(mBytes);
	}
}

VArchiveByteReader::VArchiveByteReader(const VArchiveByteReader& copy)
{
	mCapacity = copy.mCapacity;
	mIsSaving = copy.mIsSaving;
	mIsValid = copy.mIsValid;
	mOwnsMemory = copy.mOwnsMemory;
	mSeek = copy.mSeek;
	mCapacity = copy.mCapacity;
}

VArchiveByteReader::~VArchiveByteReader()
{
	if (mOwnsMemory && mBytes)
	{
		VMemFree(mBytes);
		mBytes = 0;
	}
	mCapacity = mSeek = 0;
}

void VArchiveByteReader::RWBytes(void* bytes, size_t sizeInBytes)
{
	if (!mIsValid) return;

	if (sizeInBytes > Avail())
	{
		mIsValid = false;
		return;
	}

	VMemCopy(bytes, mBytes + mSeek, sizeInBytes);
	mSeek += sizeInBytes;
}
