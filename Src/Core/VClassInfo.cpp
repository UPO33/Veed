#include "VClassInfo.h"
#include "VLogger.h"
#include "VVectorMath.h"
#include "VString.h"

VMetaSys* VMetaSys::GetIns()
{
	static VMetaSys Ins;
	return &Ins;
}

VMetaSys::VMetaSys()
{
	mTypes.IncCap(4096);

	//registering base types
	{

#define REG_BASE_TYPE(type, name, id){ \
		VTypeInfo* pType = new VTypeInfo();\
		pType->mId = id;\
		pType->mSize = sizeof(type);\
		pType->mAlign = alignof(type);\
		pType->mName = name;\
		this->RegisterNewType(pType);\
		mBaseTypes[id] = pType;\
		}\

		
		REG_BASE_TYPE(bool, "bool", EMT_Bool);
		REG_BASE_TYPE(int32_t, "int32", EMT_Int32);
		REG_BASE_TYPE(float, "float", EMT_Float);

		REG_BASE_TYPE(VVec2, "VVec2", EMT_VVec2);
		REG_BASE_TYPE(VVec3, "VVec3", EMT_VVec3);
		REG_BASE_TYPE(VVec4, "VVec4", EMT_VVec4);
		REG_BASE_TYPE(VInt2, "VInt2", EMT_VInt2);
		REG_BASE_TYPE(VName, "VName", EMT_VName);
		REG_BASE_TYPE(VString, "VString", EMT_VString);

#undef REG_BASE_TYPE

	}
}

VMetaSys::~VMetaSys()
{

}

VTypeInfo* VMetaSys::GetBaseTypeById(EMetaTypeId id)
{
	return mBaseTypes[id];
}

VTypeInfo* VMetaSys::FindTypeByName(VName name)
{
	for (int i = 0; i < mTypes.Length(); i++)
	{
		if (mTypes[i] && mTypes[i]->mName == name)
			return mTypes[i];
	}
	return nullptr;
}

VClassTypeInfo* VMetaSys::FindClassByName(VName name)
{
	auto pType = FindTypeByName(name);
	return pType ? pType->GetAsClass() : nullptr;
}

VEnumTypeInfo* VMetaSys::FindEnumByName(VName name)
{
	auto pType = FindTypeByName(name);
	return pType ? pType->GetAsEnum() : nullptr;
}

void VMetaSys::RegisterNewType(VTypeInfo* pNewType)
{
	VASSERT(pNewType);
	VASSERT(FindTypeByName(pNewType->mName) == nullptr);

	VLOG_SUC("type % registered.", pNewType->mName);

	mTypes.Add(pNewType);
}

void VMetaSys::UnregType(VTypeInfo* pType)
{
	mTypes.RemoveFirstShift(pType);
}

void VMetaSys::PrintAllTypes()
{
	for (int i = 0; i < mTypes.Length(); i++)
	{
		if (VTypeInfo* pType = mTypes[i])
		{
			
			//VLOG_MSG("----------------------------");
			pType->PrintToLog(0);
			//VLOG_MSG("----------------------------");
		}
	}
}

void VTypeInfo::PrintToLog(int indent) const
{
#ifndef VBUILD_SHIPPING
	//VLOG_MSG("%name: % size: % align: % id:%", VlogIndent(indent), mName, mSize, mAlign, mId);
#endif
}

void VClassTypeInfo::PrintToLog(int indent) const
{
	VTypeInfo::PrintToLog(indent);

	for (int i = 0; i < mProperties.Length(); i++)
	{
		if (VPropertyInfo* pProperty = mProperties[i])
		{
			//VLOG_MSG("%[%] name % offset %", VlogIndent(indent), i, pProperty->mName, pProperty->mOffset);
			pProperty->mTypeInfo->PrintToLog(indent + 1);
		}
	}
}
