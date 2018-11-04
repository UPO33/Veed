#pragma once

#include "VLogger.h"

//////////////////////////////////////////////////////////////////////////
//base class for serialization
class VArchiveBase
{
protected:
	uint8_t* mBytes;
	size_t mSeek;
	size_t mCapacity;
	bool mIsSaving;
	bool mIsValid;
	bool mOwnsMemory;

public:
	bool IsValid() const { return mIsValid; }
	bool IsSaving() const { return mIsSaving; }
	size_t Avail() const { return mCapacity - mSeek; }
	size_t GetSeek() const { return mSeek; }
	size_t GetCapacity() const { return mCapacity; }
	uint8_t* GetBytes() const { return mBytes; }
	
	void Reset() { mSeek = 0; }

	virtual void RWBits(void* bits, size_t sizeInBits) {}
	virtual void RWBytes(void* bytes, size_t sizeInBytes) {}

	virtual void RWBit(bool& b) {};
	virtual void RWByte(uint8_t& byte) {};
	virtual void RWWord(uint16_t& word) {};
	virtual void RWDWord(uint32_t& dw) {};
	virtual void RWQWord(uint64_t& qw) {};
};
inline VArchiveBase& operator && (VArchiveBase& archive, bool& value)
{
	archive.RWBytes(&value, sizeof(bool));
	return archive;
}
inline VArchiveBase& operator && (VArchiveBase& archive, int& value)
{
	archive.RWBytes(&value, sizeof(int));
	return archive;
}
inline VArchiveBase& operator && (VArchiveBase& archive, float& value)
{
	archive.RWBytes(&value, sizeof(float));
	return archive;
}
//////////////////////////////////////////////////////////////////////////
class VArchiveByteReader : public VArchiveBase
{
public:
	VArchiveByteReader(void* pData, size_t dataSize, bool bCopyData);
	VArchiveByteReader(const VArchiveByteReader& copy);
	~VArchiveByteReader();
	void RWBytes(void* bytes, size_t sizeInBytes) override;
};

//////////////////////////////////////////////////////////////////////////
class VArchiveByteWriter : public VArchiveBase
{
public:
	//default constructor
	VArchiveByteWriter(size_t initialCapacity = 0);
	//copy constructor
	VArchiveByteWriter(const VArchiveByteWriter& copy);
	//move constructor
	VArchiveByteWriter(VArchiveByteWriter&& move);
	//destructor
	~VArchiveByteWriter();

	void RWBytes(void* bytes, size_t sizeInBytes) override;
	void CheckInc(size_t size);
};

//////////////////////////////////////////////////////////////////////////
template<size_t SizeInByte> class VArchiveByteWriterStack : public VArchiveByteWriter
{
public:
	VArchiveByteWriterStack()
	{
		mCapacity = SizeInByte;
		mSeek = 0;
		mOwnsMemory = false;
		mIsValid = true;
		mIsSaving = true;
		mBytes = mBuffer;
	}
	~VArchiveByteWriterStack() 
	{
		mCapacity = mSeek = 0;
		mBytes = nullptr;
	}
	
	VArchiveByteWriterStack(const VArchiveByteWriter&) = delete;
	VArchiveByteWriterStack(VArchiveByteWriter&&) = delete;


	uint8_t mBuffer[SizeInByte];
};

