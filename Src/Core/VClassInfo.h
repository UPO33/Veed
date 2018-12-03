#pragma once
#include "VName.h"
#include "VModule.h"
#include "VArray.h"
#include "VFunction.h"

#include <bitset>

//////////////////////////////////////////////////////////////////////////
struct VClassTypeInfo;
struct VEnumTypeInfo;
struct VArrayTypeInfo;
struct VStackArrayTypeInfo;
struct VSubClassOfTypeInfo;
struct VObjectPtrTypeInfo;
struct VClassTypeInfo;

struct VArchiveBase;
struct VBitStream;

struct VAttrBase
{
	int mId = 0;
	virtual ~VAttrBase() {}
};
//inline comment for class, property etc
struct VAttrComment : VAttrBase
{
	const char* mComment;
};
//specifies external comment file
struct VAttrCommentFile : VAttrBase
{
	const char* mFile;
	const char* mSection;
	const char* mKey;
};
struct VAttrNetClass : VAttrBase
{
};
struct VAttrReplicate : VAttrBase
{
};
struct VAttrTransient : VAttrBase
{
};
struct VAttrProperty : VAttrBase
{
	union 
	{
		const char* mComment;
		float mCalmpMinMax[2];
	};
};
struct VAttrClass : VAttrBase
{
	union
	{
		const char* mComment;
	};
};

enum EMetaTypeId
{
	EMT_Unknown,

	//numeric types
	EMT_Bool,
	EMT_Int32,
	EMT_Float,

	//default structures
	EMT_VString,
	EMT_VVec2,
	EMT_VVec3,
	EMT_VVec4,
	EMT_VColor32,
	EMT_VColor,
	EMT_VName,
	EMT_VInt2,
	EMT_VRect,
	
	EMT_ObjectPointer,
	EMT_VObjectPtr,

	EMT_Class,
	EMT_VSubclassOf,
	EMT_VArray,
	EMT_VStackArray,

	EMT_Enum,
	EMT_Max,
};

enum EPropertyFlag
{
	EPF_None,

	EPF_Transient, //property won't be serialized
	EPF_Hidden,  //hidden in editor
	EPF_Disabled, //user can't change the value
	EPF_DontCook, //property won't be serialized for cooking build
	EPF_Comment,
	EPF_Clamp,
	EPF_Replicate,
	EPF_ReplicateCustom,
	EPF_IgnoreGC,	//our property will not be nullified automatically
};
enum EClassFlag
{
	ECF_None,

	ECF_Comment,
	ECF_NetClass,
};


struct VTypeInfo
{
	VClassTypeInfo* GetAsClass() const { return mId == EMT_Class ? (VClassTypeInfo*)this : nullptr; }
	VEnumTypeInfo* GetAsEnum() const { return mId == EMT_Enum ? (VEnumTypeInfo*)this : nullptr; }
	VArrayTypeInfo* GetAsArray() const { return mId == EMT_VArray ? (VArrayTypeInfo*)this : nullptr; }
	VStackArrayTypeInfo* GetAsStackArray() const { return mId == EMT_VStackArray ? (VStackArrayTypeInfo*)this : nullptr; }
	VSubClassOfTypeInfo* GetAsSubClassOf() const { return mId == EMT_VSubclassOf ? (VSubClassOfTypeInfo*)this : nullptr; }
	VObjectPtrTypeInfo* GetAsObjectPtr() const { return mId == EMT_VObjectPtr ? (VObjectPtrTypeInfo*)this : nullptr; }

	//id of this type
	EMetaTypeId	mId;
	//size of the type. for classes its size of the class
	size_t		mSize;
	//the align required for the type
	size_t		mAlign;
	//basic data types: float, int32, uint32, ...
	//default types : String, Name, Color, Vec3 , ...
	//meta classes: VMyClass0, VMyClass1, ...
	//ObjectPointers : VObject*,  VMyObject* , ...
	//remplates: VArray<float>, VArray<VMyClass>, VStackArray<VObject*>, VObjectPtr<VMyObject> VSubclassOf<VMyClass>
	VName		mName;
	//module that this type comes from
	VModule*	mModule;

	virtual void PrintToLog(int indent) const;
};

struct VArrayTypeInfo : VTypeInfo
{
	//pointer to the type of element
	VTypeInfo* mElementType = nullptr;
};

struct VStackArrayTypeInfo : VTypeInfo
{
	//pointer to type of the element
	VTypeInfo*	mElementType = nullptr;
	size_t		mMaxElement;
};
//type info for VObjectPtr<>
struct VObjectPtrTypeInfo : VTypeInfo
{
	VName			mObjectClassName;
	VClassTypeInfo* mObjectClass = nullptr;
};
//type info for VSubclassOf<>
struct VSubClassOfTypeInfo : VTypeInfo
{
	VName			mClassName;
	VClassTypeInfo* mClass = nullptr;
};
//
struct VEnumTypeInfo : VTypeInfo
{
	bool mInitilized;
};
//VObject*
struct VObjectPointerTypeInfo : VTypeInfo
{
	VName mObjectClassName;
	VClassTypeInfo* mObjectClass = nullptr;
};

//
struct VPropertyInfo
{
	//the real name of the property. e.g mPosition, mNumber, ...
	VName mName;
	//class that this property belongs to
	VClassTypeInfo* mOwner;
	//offset from the base class
	size_t mOffset;
	//information about type of this property
	VTypeInfo* mTypeInfo;
	//line number at VPROPERTY()
	int mLineNumber;
};

//maximum number of class we support
static constexpr unsigned VMAX_CLASS = 512;

//////////////////////////////////////////////////////////////////////////
struct VClassTypeInfo : VTypeInfo
{
	using FPDefaultConstructor = void(*)(void* pInstance);
	using FPCopyConstructor = void(*)(void* pInstance, const void* pCopy);
	using FPDestructor = void(*)(void* pInstance);

	//return true if this class is either @pClass or a child of @pClass
	bool IsChildOf(const VClassTypeInfo* pClass) const
	{
		return mCastBits.test(pClass->mClassIndex);
	}

	bool IsObjectClass() const;

	void PrintToLog(int indent) const override;


	//pointer to parent class if we are not root class
	VClassTypeInfo* mParentClass;
	//name of the parent class
	VName mParentClassName;
	//array of all registered properties for this class
	VStackArray<VPropertyInfo*, 256> mProperties;
	//file at VCLASS
	const char* mHeaderFilename;
	//file at VCLASS_BEGIN
	const char* mSourceFilename;
	//line number at VCLASS()
	int HeaderLineNumber;
	//lineNumber at VLCASS_BEGIN
	int SourceLineNumber;
	//
	bool mInitialized; //
	//index for networking
	uint16_t mNetIndex;
	//
	uint16_t mClassIndex;
	std::bitset<VMAX_CLASS> mCastBits;

	

	FPDefaultConstructor mDefaultConstructor;
	FPCopyConstructor mCopyConstructor;
	FPDestructor mDestructor;

	VMemFunc<void(VArchiveBase&)> mSerialize;
	VMemFunc<void(VBitStream&)> mNetSerialize;
};

template<typename T> struct VSubclassOf
{
	T* mClass;
};




///////////////////////////////////////////////
struct VMetaSys
{
	static VMetaSys* GetIns();
	
	VArray<VTypeInfo*>	mTypes;

	VMetaSys();
	~VMetaSys();

	VTypeInfo* mBaseTypes[EMetaTypeId::EMT_Max] = {};

	VTypeInfo*		GetBaseTypeById(EMetaTypeId id);
	VTypeInfo*		FindTypeByName(VName name);
	VClassTypeInfo* FindClassByName(VName name);
	VEnumTypeInfo*	FindEnumByName(VName name);
	void			RegisterNewType(VTypeInfo* pNewType);
	void			UnregType(VTypeInfo* pType);

	void			PrintAllTypes();
};


template<typename T = void> struct ZZParentCatch { typedef T TT; };


#define VCLASS_BASE(Class, ...)\
public:\
	static int ZZLineNumber() { return __LINE__; }\
	static const char* ZZFileName() { return __FILE__; }\
	void ZZIsMeta() {}\
	friend struct ZZZ_##Class;\
	typedef typename ZZParentCatch<__VA_ARGS__>::TT ParentT;\
	typedef Class SelfT;\
	static const VClassTypeInfo* GetClassStatic();\


#define VCLASS_POD(Class, ...) VCLASS_BASE(Class, __VA_ARGS__ )


#define VCLASS_OBJECT(Class, ...) \
	VCLASS_BASE(Class, __VA_ARGS__ )\
	virtual VClassTypeInfo* GetClass() override { return GetClassStatic(); }\

