#pragma once

#include "VArray.h"

//////////////////////////////////////////////////////////////////////////
class VObject
{
private:
	const VClassInfo* mClass;
	VObject* mObjectParent;
	VArray<VObject*> mObjectChildren;
};