#pragma once

#include "VClassInfo.h"


struct VClassTypeInfo;


//////////////////////////////////////////////////////////////////////////
struct VObject
{
	VCLASS_BASE(VObject);

	VObject* mObjectParant;


	VObject()
	{

	}

	virtual void OnPostPropertyChange() {}
	virtual void OnPrePropertyChange() {}
	virtual void OnSerialize(VArchiveBase& archive) {}
	virtual VClassTypeInfo* GetClass() { return nullptr; }
};


//////////////////////////////////////////////////////////////////////////
template<class TObjectClass> struct VObjectPtr
{

	VObject* Get() const { return nullptr;  }
	//#TODO
	
};
