#pragma 

#include "VClassInfo.h"

//serializer for VArray
struct VMetaArrayView
{
	VMetaArrayView(void * pArray, VArrayTypeInfo* pType)
	{}
};


//serializer for VStackArray
struct VMetStackArrayView
{
	VMetStackArrayView(void* pStackArray, VStackArrayTypeInfo* pType)
	{}
};
