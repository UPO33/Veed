#include "../Core/VMeta.h"
#include "../Core/VLogger.h"
#include "../Core/VArray.h"
#include "../Core/VVectorMath.h"
#include "../Core/VString.h"
#include "../Core/VName.h"

struct VMetaClass0
{

	VCLASS_POD(VMetaClass0);

	int mInt;
	bool mBool;
	float mFloat;
	VObject mObjectPointer;
	VArray<int> mIntArray;
	VArray<bool> mBoolArray;
	VStackArray<VObject*, 8> mArrayObjectStar;
	VObjectPtr<VObject> mObjectPtr;
	VSubclassOf<VObject> mSubclass;
	VVec2 mVec2;
	VVec3 mVec3;
	VVec4 mVec4;
	VInt2 mInt2;
	VName mName;
	VString mString;
	VArray<VName> mArrayName;
	VMetaClass0()
	{
	}

};

VCLASS_BEGIN(VMetaClass0)
VPROPERTY(mInt)
VPROPERTY(mBool)
VPROPERTY(mFloat)
VPROPERTY(mObjectPointer)
VPROPERTY(mIntArray)
VPROPERTY(mBoolArray);
//VPROPERTY(mArrayObjectStar)
VPROPERTY(mObjectPtr)
VPROPERTY(mSubclass)
VPROPERTY(mVec2)
VPROPERTY(mVec3)
VPROPERTY(mVec4)
VPROPERTY(mInt2)
VPROPERTY(mName)
VPROPERTY(mString)
VPROPERTY(mArrayName)
VCLASS_END(VMetaClass0)
//
enum ETest00
{
};
enum class ETest11
{

};

#define CHECK_NAME_EXACT(t)  VASSERT(VGetTypeidNameFixed(typeid(t)) == VName(#t));

template<int N> struct VVPar {};

void VTest_Meta()
{
	CHECK_NAME_EXACT(ETest00);
	CHECK_NAME_EXACT(ETest11);

	CHECK_NAME_EXACT(int);
	CHECK_NAME_EXACT(VMetaClass0);
	CHECK_NAME_EXACT(VMetaClass0*);
	CHECK_NAME_EXACT(VObject**);
	CHECK_NAME_EXACT(VArray<VSubclassOf<VObject>>);
	CHECK_NAME_EXACT(VArray<VObject*>);
	CHECK_NAME_EXACT(VVPar<8>);
	
	VMetaSys::GetIns()->PrintAllTypes();

	VLOG_MSG("");
}