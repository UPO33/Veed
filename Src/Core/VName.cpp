#include "VName.h"
#include "VAllocators.h"
#include "VHash.h"
#include "VMemory.h"

static constexpr unsigned TABLE_SIZE = 0xFFff;


//////////////////////////////////////////////////////////////////////////
struct VNameContext
{
	VName::Instance		mInstances[TABLE_SIZE] = {};
	VName::HashT		mHashes[TABLE_SIZE] = {};
	int					mInstancesCount = 0;
	VLinearAllocator	mStringPool;

	VNameContext()
		: mStringPool(1024 * 100)
	{
		mInstances[0].mString = "";
		mHashes[0] = VName::CalcHash("", 0);
		mInstancesCount = 1;
	}


	int FindByHash(VName::HashT hash) const
	{
		for (int i = 0; i < mInstancesCount; i++)
		{
			if (mHashes[i] == hash)
				return i;
		}
		return -1;
	}
	int AddInstance(const char* string, size_t stringLen, VName::HashT stringHash)
	{
		VASSERT(string);
		VASSERT(mInstancesCount < TABLE_SIZE);
		char* pNewString = (char*)mStringPool.Alloc(stringLen + 1);
		VASSERT(pNewString);
		VMemCopy(pNewString, string, stringLen);
		//null terminates
		pNewString[stringLen] = 0;

		mInstances[mInstancesCount].mString = pNewString;
		//mInstances[mInstancesCount].mLength = stringLen;
		mHashes[mInstancesCount] = stringHash;

		return mInstancesCount++;
	}
	static VNameContext* Get()
	{
		static VNameContext Ins;
		return &Ins;
	}
};

//calculates the hash using FNV64 on lower case characters
VName::HashT VName::CalcHash(const char* string, size_t len)
{
	static const uint64_t FNV_HASH_PRIME = 14695981039346656037ULL;

	uintptr_t hash = FNV_HASH_PRIME;
	unsigned char* p = (unsigned char*)string;
	for (size_t i = 0; i < len; i++)
	{
		hash = (hash * 1099511628211ULL) ^ tolower(p[i]);
	}
	return hash;
}

//////////////////////////////////////////////////////////////////////////
VName::Instance& VName::GetInstanceAt(int index)
{
	return VNameContext::Get()->mInstances[index];
}


//////////////////////////////////////////////////////////////////////////
uint64_t VName::FindCreateInstance(const char* cstr)
{
	if (cstr == nullptr || cstr[0] == 0)
		return VName::EMPTY_HANDLE;

	const char* strIter = cstr;
	const char* lastDigit = nullptr;
	const char* lastNonDigit = nullptr;

	while (*strIter)
	{
		if (isdigit(*strIter))
			lastDigit = strIter;
		else
			lastNonDigit = strIter;

		strIter++;
	}

	int suffix = -1;
	auto len = strIter - cstr;
	
	//digit only?
	if (lastDigit && lastNonDigit == nullptr)
	{
		VASSERT(len <= 10); //should not be a very long number 
		suffix = atoi(cstr);
		return VName::MakeHandle(0, suffix);
	}

	auto stringPartLen = lastNonDigit + 1 - cstr;
	auto hashStr = VName::CalcHash(cstr, stringPartLen);
	int index = VNameContext::Get()->FindByHash(hashStr);

	//ends with digit?
	if (lastDigit != nullptr && lastNonDigit < lastDigit)
	{
		suffix = atoi(lastNonDigit + 1);
	}
	else
	{

	}

	if (index == -1)
	{
		index = VNameContext::Get()->AddInstance(cstr, stringPartLen, hashStr);
	}
	else
	{
	}
	return VName::MakeHandle(index, suffix);
	


	/*
	Cases:
	inputs:
		 Input		  StringPart	NumberPart
		object_0		object_			 0
		object_1		object_			 1
		object-22		object-			 22
		object			object			-1
		123				  				123
		123_asd			123_asd			-1
		qwe_123			 qwe_			123
	*/
}
//////////////////////////////////////////////////////////////////////////
uint64_t VName::FindCreateInstance(const char* string, int suffix)
{
	if (string == nullptr || string[0] == 0)
		return VName::MakeHandle(0, suffix);

	size_t len = strlen(string);
	auto strHash = VName::CalcHash(string, len);

	int index = VNameContext::Get()->FindByHash(strHash);
	if (index == -1)
		index = VNameContext::Get()->AddInstance(string, len, strHash);

	return VName::MakeHandle(index, suffix);
}
//////////////////////////////////////////////////////////////////////////
void VName::ToString(char* outBuffer, size_t bufferSize) const
{
	if (NumberPart() != -1)
	{
		snprintf(outBuffer, bufferSize, "%s%d", StringPart(), NumberPart());
	}
	else
	{
		snprintf(outBuffer, bufferSize, "%s", StringPart());
	}
}

