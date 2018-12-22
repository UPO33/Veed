#include "VObject.h"
#include "VMeta.h"

VCLASS_BEGIN(VObject)
VCLASS_END(VObject)

VObject::VObject()
{
	mObjectUp = mObjectDown = mObjectFirstChild = mObjectParant = nullptr;
	gObjectContext->AddObject(this);
}

VObject::VObject(VObject* pParent)
{
	mObjectParant = pParent;
	pParent->mObjectFirstChild = this;
	
}

VObjectContext* gObjectContext;