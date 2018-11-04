#pragma once

#include "VCoreBase.h"

template<typename T> class VSharedPtr;
template<typename T> class VWeakPtr;

//this is newed by TBaseSmart::GetWeakData and will be deleted when mObject && mRefCount are zero
//newing and deleting is done by a FreeList allocator fast enough
struct VWeakRefrenceData
{
	void*		mObject;	//pointer to the object that TWeakPtr is pointing to, null means the object has been deleted
	unsigned	mRefCount;	//number of alive TWPtr

	VWeakRefrenceData(void* obj, unsigned initialRefCount = 0) : mObject(obj), mRefCount(initialRefCount)
	{
	}
	~VWeakRefrenceData();

	inline void Inc() { mRefCount++; }
	//decrease the refcount and delete this if needed
	void Dec();

	//return the null weak, increasing or decreasing of its refcount has no effect
	static VWeakRefrenceData* GetNull();

	static void* operator new (size_t);
	static void operator delete(void*);
};

//////////////////////////////////////////////////////////////////////////
class VWeakBase
{
public:
	VWeakBase() : mWeakData(nullptr) {}
	VWeakBase(const VWeakBase& copy) : mWeakData(nullptr) {}

	//return pointer to the WeakData of this object, allocate if needed, doesn't increase the refcount
	VWeakRefrenceData* GetWeakData() const
	{
		if (mWeakData == nullptr)
			mWeakData = new VWeakRefrenceData((void*)this, 0);

		return mWeakData;
	}
	virtual ~VWeakBase()
	{
		if (mWeakData)
		{
			VASSERT(mWeakData->mObject);
			mWeakData->mObject = nullptr;
			if (mWeakData->mRefCount == 0)	//there is no TWeakPtr alive?
			{
				if (mWeakData != VWeakRefrenceData::GetNull())
					delete mWeakData;
			}
			mWeakData = nullptr;
		}
	}
private:
	mutable VWeakRefrenceData*	mWeakData;
};


/*
inherit from this then  TSharedPtr and TWeakPtr can handle your class
*/
class VSmartBase : public VWeakBase
{
public:
	VSmartBase() : mRefCount(0) {}
	virtual ~VSmartBase()
	{
		VASSERT(mRefCount == 0);
	}

	inline void AddRef()
	{
		mRefCount++;
	}
	inline void Release()
	{
		VASSERT(mRefCount != 0);
		
		mRefCount--;
		if (mRefCount == 0)
			delete this;
	}
private:
	unsigned mRefCount;
};



//////////////////////////////////////////////////////////////////////////weak pointer
template<typename T> class VWeakPtr
{
public:
	using ClassT = T;

	VWeakPtr()
	{
		mWeakData = VWeakRefrenceData::GetNull();
	}
	VWeakPtr(std::nullptr_t)
	{
		mWeakData = VWeakRefrenceData::GetNull();
	}
	VWeakPtr(const VWeakPtr& copy)
	{
		mWeakData = copy.mWeakData;
		mWeakData->Inc();
	}
	VWeakPtr(const T* ptr)
	{
		if (ptr)
		{
			mWeakData = ptr->GetWeakData();
			mWeakData->Inc();
		}
		else
		{
			mWeakData = VWeakRefrenceData::GetNull();
		}

	}
	VWeakPtr& operator = (const VWeakPtr& wptr)
	{
		if (wptr.mWeakData != mWeakData)
		{
			wptr.mWeakData->Inc();
			mWeakData->Dec();
			mWeakData = wptr.mWeakData;
		}
		return *this;
	}
	VWeakPtr& operator = (const T* bptr)
	{
		if (bptr)
		{
			VWeakRefrenceData* newWeak = bptr->GetWeakData();
			newWeak->Inc();
			mWeakData->Dec();
			mWeakData = newWeak;
		}
		else
		{
			mWeakData->Dec();
			mWeakData = VWeakRefrenceData::GetNull();
		}
		return *this;
	}
	VWeakPtr& operator = (std::nullptr_t)
	{
		mWeakData->Dec();
		mWeakData = VWeakRefrenceData::GetNull();
		return *this;
	}
	~VWeakPtr()
	{
		mWeakData->Dec();
		mWeakData = nullptr;
	}

	bool		IsValid() const { return mWeakData->mObject != nullptr; }
	T*			Get() const { return (T*)mWeakData->mObject; }

	operator bool() const { return mWeakData->mObject != nullptr; }
	operator T* () const { return (T*)mWeakData->mObject; }

	template<typename U> operator VWeakPtr<U>() const { return VWeakPtr<U>(Get()); }

	bool operator == (const VWeakPtr& wptr) const
	{
		return mWeakData == wptr.mWeakData;
	}
	bool operator != (const VWeakPtr& wptr) const
	{
		return mWeakData != wptr.mWeakData;
	}
	T* operator -> () const { return (T*)mWeakData->mObject; }

private:
	VWeakRefrenceData * mWeakData;	//always valid
};


//////////////////////////////////////////////////////////////////////////shared pointer
template<typename T> class VSharedPtr
{
public:
	using ClassT = T;

	VSharedPtr()
	{
		mPtr = nullptr;
	}
	VSharedPtr(std::nullptr_t)
	{
		mPtr = nullptr;
	}
	~VSharedPtr()
	{
		if (mPtr) mPtr->Release();
		mPtr = nullptr;
	}
	VSharedPtr(T* ptr)
	{
		if (ptr)
		{
			mPtr = ptr;
			mPtr->AddRef();
		}
		else
		{
			mPtr = nullptr;
		}
	}
	VSharedPtr(const VSharedPtr& copy)
	{
		mPtr = copy.mPtr;
		if (mPtr) mPtr->AddRef();
	}

	VSharedPtr& operator = (const VSharedPtr& other)
	{
		return this->operator=(other.mPtr);
	}
	VSharedPtr& operator = (T* ptr)
	{
		if (ptr)
		{
			ptr->AddRef();
			if (mPtr) mPtr->Release();
			mPtr = ptr;
		}
		else
		{
			if (mPtr) mPtr->Release();
			mPtr = nullptr;
		}
		return *this;
	}

	VWeakPtr<T> GetWeak() const
	{
		return VWeakPtr<T>(mPtr);
	}
	T* Get() const { return mPtr; }

	bool IsValid() const { return mPtr != nullptr; }

	operator bool() const { return mPtr != nullptr; }
	operator T* () const { return mPtr; }

	T* operator -> () const { return mPtr; }

	template<typename U> operator VSharedPtr<U>() const { return VSharedPtr<U>(mPtr); }

	bool operator == (const VSharedPtr& other) const { return mPtr == other.mPtr; }
	bool operator != (const VSharedPtr& other) const { return mPtr != other.mPtr; }

private:
	T* mPtr;
};

// template<typename T, typename U> VSharedPtr<T> PtrStaticCast(const VSharedPtr<U>& sptr)
// {
// 	auto p = static_cast<T*>(sptr.Get());
// 	return VSharedPtr<T>(p);
// }
// template<typename T, typename U> VWeakPtr<T> PtrStaticCast(const VWeakPtr<U>& sptr)
// {
// 	auto p = static_cast<T*>(sptr.Get());
// 	return VWeakPtr<T>(p);
// }
// 
// template<typename T, typename U> VSharedPtr<T> PtrDynamicCast(const VSharedPtr<U>& sptr)
// {
// 	auto p = dynamic_cast<T*>(sptr.Get());
// 	return VSharedPtr<T>(p);
// }
// template<typename T, typename U> VWeakPtr<T> PtrDynamicCast(const VWeakPtr<U>& sptr)
// {
// 	auto p = dynamic_cast<T*>(sptr.Get());
// 	return VWeakPtr<T>(p);
//}