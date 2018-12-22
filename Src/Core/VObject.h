#pragma once

#include "VClassInfo.h"


struct VClassTypeInfo;

enum EObjectFlag
{
	EOF_None,
	EOF_Alive = 1,
};
//////////////////////////////////////////////////////////////////////////
struct VObject
{
	VCLASS_BASE(VObject);

	uint32_t mObjectIndex;
	uint32_t mObjectFlags;

	//pointer to the parent object if any
	VObject* mObjectParant;
	//pointer to the first child. if valid we have at least one child. mObjectFirstChild->mObjectUp pooints to the last child
	VObject* mObjectFirstChild;
	//the object at the bottom of this. null means this is the last child.
	VObject* mObjectUp;
	//the object at the top of this. if null
	VObject* mObjectDown;


	VObject();
	VObject(VObject* pParent);

	bool IsObjectAlive() const { return mObjectFlags & EOF_Alive; }

	virtual void OnPostPropertyChange() {}
	virtual void OnPrePropertyChange() {}
	virtual void OnSerialize(VArchiveBase& archive) {}
	virtual VClassTypeInfo* GetClass() { return nullptr; }
};

struct VObjectSlot
{
	union
	{
		VObject* mObject;
		uint32_t mNextFree;
	};

	uint32_t mSerial : 31;
	uint32_t mIsFree : 1;
	
	

	

	bool IsFree() const { return mIsFree; }
};

struct VObjectContext
{
	VObjectSlot*	mSlots;
	uint32_t		mMaxSlots;
	uint32_t		mFirstFreeSlot;
	uint32_t		mNumAliveObject;
	uint32_t		mObjectPlaceIter;

	template<typename T> void ForechAliveObject(T proc)
	{
		for (uint32_t i = 0; i < mMaxSlots; i++)
		{
			if (mSlots[i].IsFree())
				continue;
			//..
		}
	}
	void AddObject(VObject* pObj)
	{
		VASSERT(mFirstFreeSlot != 0);
		pObj->mObjectIndex = mFirstFreeSlot;
		VObjectSlot& slot = mSlots[mFirstFreeSlot];
		mFirstFreeSlot = slot.mNextFree;
		slot.mSerial++;
		slot.mObject = pObj;
		mNumAliveObject++;
	}
	void RemoveObject(VObject* pObj)
	{
		VASSERT(pObj && pObj->mObjectIndex != 0);

		VObjectSlot& slot = mSlots[pObj->mObjectIndex];
		slot.mNextFree = mFirstFreeSlot;
		slot.mSerial++;
		mFirstFreeSlot = pObj->mObjectIndex;
		pObj->mObjectIndex = 0;
		mNumAliveObject--;
		
	}
};
extern VObjectContext* gObjectContext;

//////////////////////////////////////////////////////////////////////////
struct VObjectPtrBase
{
	uint32_t mObjectIndex;
	uint32_t mSerial;

	VObject* Get() const
	{
		const VObjectSlot& slot = gObjectContext->mSlots[mObjectIndex];
		if (slot.mSerial == mSerial && mSerial != 0)
			return slot.mObject;
		return nullptr;
	}
	VObjectPtrBase() : mObjectIndex(0), mSerial(0) 
	{
	}
	VObjectPtrBase(const VObject* pObject)
	{
		if (pObject && pObject->IsObjectAlive())
		{
			mObjectIndex = pObject->mObjectIndex;
			mSerial = gObjectContext->mSlots[pObject->mObjectIndex].mSerial;
		}
		else
		{
			mObjectIndex = mSerial = 0;
		}
	}
};

//////////////////////////////////////////////////////////////////////////
template<class TObjectClass> struct VObjectPtr : VObjectPtrBase
{
	
	
	
};
