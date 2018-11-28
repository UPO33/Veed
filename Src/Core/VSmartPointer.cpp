#include "VSmartPointer.h"
#include "VAllocators.h"

VWeakRefrenceData* gNullWeakRefrence = nullptr;

VFreeListAllocator* VGetWeakRefrenceDataAllocator()
{
	static VFreeListAllocator Ins(sizeof(VWeakRefrenceData), 512);
	return &Ins;
}

VWeakRefrenceData::~VWeakRefrenceData()
{
	if (gNullWeakRefrence != this)
	{
		VASSERT(mRefCount == 0);
		VASSERT(mObject == nullptr);
	}
}

void VWeakRefrenceData::Dec()
{
	mRefCount--;
	if (mRefCount == 0)
	{
		if (mObject == nullptr) //object is deleted then delete this too?
		{
			if (this != GetNull())	//null weak must not be deleted
				delete this;
		}
	}
}

VWeakRefrenceData* VWeakRefrenceData::GetNull()
{
	if (gNullWeakRefrence == nullptr)
		gNullWeakRefrence = new VWeakRefrenceData(nullptr, 0xFFffFF);

	return gNullWeakRefrence;
}


void* VWeakRefrenceData::operator new(size_t size)
{
	VASSERT(size == sizeof(VWeakRefrenceData));
	return VGetWeakRefrenceDataAllocator()->Alloc();
}


void VWeakRefrenceData::operator delete(void* ptr)
{
	VASSERT(ptr);
	VGetWeakRefrenceDataAllocator()->Free(ptr);
}