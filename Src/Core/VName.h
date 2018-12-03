#pragma once

#include <cstdint>

//////////////////////////////////////////////////////////////////////////
class VName
{
public:
	static constexpr uint64_t EMPTY_HANDLE = uint64_t(0xFFffFFff);
	using HashT = uint64_t;

	struct Instance
	{
		char*		mString;
		//size_t	mLength;
	};

	inline static uint64_t MakeHandle(int index, int suffix)
	{
		return (uint64_t(uint32_t(suffix)) | (uint64_t(uint32_t(index)) << 32));
	}
	static HashT CalcHash(const char* string, size_t len);
	static Instance& GetInstanceAt(int index);
	static uint64_t  FindCreateInstance(const char* cstr);
	static uint64_t  FindCreateInstance(const char* string, int suffix);

	//initialize and empty name
	VName() : mHandle(EMPTY_HANDLE) {}
	//initialize from the string with or without suffix number
	VName(const char* cstr) : mHandle(FindCreateInstance(cstr)) { }
	//initialize from string and suffix number.
	VName(const char* stringPart, int suffixNumber) : mHandle(FindCreateInstance(stringPart, suffixNumber)) { }
	//copy constructor
	VName(const VName& copy) : mHandle(copy.mHandle) { }
	//takes the string part from another name
	VName(VName& stringaPart, int suffixNumber) {
		mIndex = stringaPart.mIndex;
		mSuffix = suffixNumber;
	}
	bool operator == (const VName& other) const { return mHandle == other.mHandle; }
	bool operator != (const VName& other) const { return mHandle != other.mHandle; }

	//
	bool IsEmpty() const { return mHandle == EMPTY_HANDLE; }
	//returns the string part. always returns a valid pointer
	const char* StringPart() const { return GetInstanceAt(mIndex).mString; }
	//returns the suffix number if available, otherwise returns -1
	int NumberPart() const { return mSuffix; }
	//
	void ToString(char* outBuffer, size_t bufferSize) const;

private:
	
	union 
	{
		struct 
		{
			int mSuffix; //number suffix, -1 mean no number suffix
			int mIndex;	//index to access the instance
		};
		
		uint64_t mHandle;
	};

};

inline void VGetLogString(VName name, char* out, size_t size) { name.ToString(out, size); }