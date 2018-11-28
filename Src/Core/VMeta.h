#pragma once

#include "VClassInfo.h"


#include "VCoreBase.h"
#include "VName.h"
#include "VArray.h"
#include "VObject.h"
#include "VVectorMath.h"
#include "VString.h"



VDECLARE_MEMBERFUNCTION_CHECKING(MetaSerialize, MetaSerialize, void, VArchiveBase&);


VName VGetTypeidNameFixed(const std::type_info& ti);



//this struct helps to extract the required information of a type, will static assert if the type is not supported to be de|serialized
//null specialization
template < typename T /*the type of property*/, typename = void> struct TPropertyTypeExtract
{
	static const EMetaTypeId Value = EMT_Unknown;
	static VTypeInfo* Get() { return nullptr; }
};
//bool
template<> struct TPropertyTypeExtract<bool>
{
	static const EMetaTypeId Value = EMT_Bool;
	static VTypeInfo* Get() { return VMetaSys::GetIns()->GetBaseTypeById(Value); }
};
//int32
template<> struct TPropertyTypeExtract<int>
{
	static const EMetaTypeId Value = EMT_Int32;
	static VTypeInfo* Get() { return VMetaSys::GetIns()->GetBaseTypeById(Value); }
};
//float
template<> struct TPropertyTypeExtract<float>
{
	static const EMetaTypeId Value = EMT_Float;
	static VTypeInfo* Get() { return VMetaSys::GetIns()->GetBaseTypeById(Value); }
};

//#TODO add int32 char, double

//VVec2
template<> struct TPropertyTypeExtract<VVec2>
{
	static const EMetaTypeId Value = EMT_VVec2;
	static VTypeInfo* Get() { return VMetaSys::GetIns()->GetBaseTypeById(Value); }
};
//VVec
template<> struct TPropertyTypeExtract<VVec3>
{
	static const EMetaTypeId Value = EMT_VVec3;
	static VTypeInfo* Get() { return VMetaSys::GetIns()->GetBaseTypeById(Value); }
};
//VVec4
template<> struct TPropertyTypeExtract<VVec4>
{
	static const EMetaTypeId Value = EMT_VVec4;
	static VTypeInfo* Get() { return VMetaSys::GetIns()->GetBaseTypeById(Value); }
};
//VInt2
template<> struct TPropertyTypeExtract<VInt2>
{
	static const EMetaTypeId Value = EMT_VInt2;
	static VTypeInfo* Get() { return VMetaSys::GetIns()->GetBaseTypeById(Value); }
};
//VString
template<> struct TPropertyTypeExtract<VString>
{
	static const EMetaTypeId Value = EMT_VString;
	static VTypeInfo* Get() { return VMetaSys::GetIns()->GetBaseTypeById(Value); }
};
//VName
template<> struct TPropertyTypeExtract<VName>
{
	static const EMetaTypeId Value = EMT_VName;
	static VTypeInfo* Get() { return VMetaSys::GetIns()->GetBaseTypeById(Value); }
};
//VArray<>
template<typename T> struct TPropertyTypeExtract<VArray<T>>
{
	typedef TPropertyTypeExtract<T> TArg0;

	static const EMetaTypeId Value = EMT_VArray;
	static const EMetaTypeId ElementType = TArg0::Value;

	static_assert(ElementType != EMetaTypeId::EMT_Unknown, "unknown array element");
	static_assert(ElementType != EMetaTypeId::EMT_VArray, "array as array element is not supported");

	static VTypeInfo* Get()
	{
		VName nameOfType = VGetTypeidNameFixed(typeid(VArray<T>));

		//@if pArrayType if valid its registered
		VTypeInfo* pArrayType = VMetaSys::GetIns()->FindTypeByName(nameOfType);
		if (pArrayType)
		{
			VASSERT(pArrayType->mId == Value);
			return pArrayType;
		}

		VArrayTypeInfo* pNewType = new VArrayTypeInfo;
		pNewType->mId = EMT_VArray;
		pNewType->mAlign = alignof(VArray<T>);
		pNewType->mSize = sizeof(VArray<T>);
		pNewType->mName = nameOfType;

		pNewType->mElementType = TArg0::Get();

		VMetaSys::GetIns()->RegisterNewType(pNewType);

		return pNewType;
	}
};
//VStackArray<>
template<typename T, size_t MaxLength> struct TPropertyTypeExtract<VStackArray<T, MaxLength>>
{
	typedef TPropertyTypeExtract<T> TArg0;

	static const EMetaTypeId Value = EMT_VStackArray;
	static const EMetaTypeId ElementType = TArg0::Value;

	static_assert(ElementType != EMetaTypeId::EMT_Unknown, "unknown array element");

	static VTypeInfo* Get()
	{
		VName nameOfType = VGetTypeidNameFixed(typeid(VStackArray<T,MaxLength>));

		VTypeInfo* pArrayType = VMetaSys::GetIns()->FindTypeByName(nameOfType);
		if (pArrayType)
		{
			VASSERT(pArrayType->mId == Value);
			return pArrayType;
		}

		VStackArrayTypeInfo* pNewType = new VStackArrayTypeInfo;
		pNewType->mId = EMT_VStackArray;
		pNewType->mAlign = alignof(VStackArray<T, MaxLength>);
		pNewType->mSize = sizeof(VStackArray<T, MaxLength>);
		pNewType->mName = nameOfType;

		pNewType->mElementType = TArg0::Get();
		pNewType->mMaxElement = MaxLength;

		VMetaSys::GetIns()->RegisterNewType(pNewType);

		return pNewType;
	}
};

//VSubclassOf
template<typename T> struct TPropertyTypeExtract<VSubclassOf<T>>
{
	static const EMetaTypeId Value = EMT_VSubclassOf;
	static VTypeInfo* Get()
	{
		VName nameOfType = VGetTypeidNameFixed(typeid(VSubclassOf<T>));
		VTypeInfo* pType = VMetaSys::GetIns()->FindTypeByName(nameOfType);
		if (pType)
		{
			VASSERT(pType->mId == EMT_VSubclassOf);
			return pType;
		}


		VSubClassOfTypeInfo* pNewType = new VSubClassOfTypeInfo();
		pNewType->mId = EMT_VSubclassOf;
		pNewType->mName = nameOfType;
		pNewType->mSize = sizeof(VSubclassOf<T>);
		pNewType->mAlign = alignof(VSubclassOf<T>);

		VName className = VGetTypeidNameFixed(typeid(T));
		pNewType->mClassName = className;
		pNewType->mClass = VMetaSys::GetIns()->FindClassByName(className);

		return pNewType;
	}

};
//VObjectPtr
template<typename T> struct TPropertyTypeExtract<VObjectPtr<T>>
{
	static const EMetaTypeId Value = EMT_VObjectPtr;

	static VTypeInfo* Get()
	{
		VName nameOfType = VGetTypeidNameFixed(typeid(VObjectPtr<T>));
		VTypeInfo* pType = VMetaSys::GetIns()->FindTypeByName(nameOfType);
		if (pType)
		{
			VASSERT(pType->mId == EMT_VObjectPtr);
			return pType;
		}

		VObjectPtrTypeInfo* pNewType = new VObjectPtrTypeInfo;
		pNewType->mName = nameOfType;
		pNewType->mId = EMT_VObjectPtr;
		pNewType->mSize = sizeof(VObjectPtr<T>);
		pNewType->mAlign = alignof(VObjectPtr<T>);

		VName className = VGetTypeidNameFixed(typeid(T));
		pNewType->mObjectClassName = className;
		pNewType->mObjectClass = VMetaSys::GetIns()->FindClassByName(className);
		return pNewType;
	}
};
//Object*
template<typename T> struct TPropertyTypeExtract<T, typename std::enable_if<std::is_pointer<T>::value && std::is_base_of<VObject, typename std::remove_pointer<T>::type>::value>::type>
{
	static const EMetaTypeId Value = EMT_ObjectPointer;

	static VTypeInfo* Get()
	{
		VName nameOfType = VGetTypeidNameFixed(typeid(T));

		VTypeInfo* pType = VMetaSys::GetIns()->FindTypeByName(nameOfType);
		if (pType)
		{
			VASSERT(pType->mId == Value);
			return pType;
		}

		VObjectPointerTypeInfo* pNewType = new VObjectPointerTypeInfo;
		pNewType->mName = nameOfType;
		pNewType->mId = Value;
		pNewType->mSize = sizeof(VObject*);
		pNewType->mAlign = alignof(VObject*);

		VName objectClassName = VGetTypeidNameFixed(typeid(typename std::remove_pointer<T>::type));
		pNewType->mObjectClassName = objectClassName;
		pNewType->mObjectClass = VMetaSys::GetIns()->FindClassByName(objectClassName);
		return pType;
	}
};

//enum
template<typename T> struct TPropertyTypeExtract<T, typename std::enable_if<std::is_enum<T>::value>::type>
{
	static const EMetaTypeId Value = EMT_Enum;

	static VTypeInfo* Get()
	{
		VName nameofType = VGetTypeidNameFixed(typeid(T));
		if (VTypeInfo* pType = VMetaSys::GetIns()->FindTypeByName(nameofType))
		{
			VASSERT(pType->mId == EMT_Enum);
			return pType;
		}

		//if the enum dosen't exist at the moment we register and empty one.
		//enum registration should add parameter to it finally

		VEnumTypeInfo* pNewType = new VEnumTypeInfo();
		pNewType->mId = EMT_Enum;
		pNewType->mName = nameofType;
		pNewType->mAlign = alignof(T);
		pNewType->mSize = sizeof(T);

		VMetaSys::GetIns()->RegisterNewType(pNewType);
		return pNewType;
	}
};


//MetaClass
template<typename T> struct TPropertyTypeExtract<T, typename std::enable_if<TT_IsMetaClass<T>::value>::type>
{
	static const EMetaTypeId Value = EMT_Class;

	static VTypeInfo* Get()
	{
		VName nameOfType = VGetTypeidNameFixed(typeid(T));
		if (VTypeInfo* pType = VMetaSys::GetIns()->FindTypeByName(nameOfType))
		{
			VASSERT(pType->mId == Value);
			return pType;
		}

		//if the class in not available yet we create it here 
		VClassTypeInfo* pNewType = new VClassTypeInfo();
		pNewType->mId = EMT_Class;
		pNewType->mName = nameOfType;
		pNewType->mAlign = alignof(T);
		pNewType->mSize = sizeof(T);

		VMetaSys::GetIns()->RegisterNewType(pNewType);
		return pNewType;
	}
};


#if 0

//////////////////////////////////////////////////////////////////////////
struct ZZ_ClassTypeCheckResult
{
	VName									mClassName;	//eg VEntityStaticMesh
	VName									mParentClassName;	//eg VEntity
	bool									mHasParent = false;
	size_t									mClassSize = 0;
	size_t									mClassAlign = 0;

	VClassTypeInfo::FPDefaultConstructor		mDefaulConstructor = nullptr;
	VClassTypeInfo::FPDestructor				mDestructor = nullptr;
	VClassTypeInfo::FPCopyConstructor			mCopyConstructor = nullptr;

	decltype(VClassTypeInfo::mSerialize)		mMetaSerialize = nullptr;

	const char*					mHeaderFileName = nullptr;
	const char*					mSourceFileName = nullptr;
	int							mHeaderFileLine = 0;
	int							mSourceFileLine = 0;
	ZZ_PropertyRegParam			mProperties[MAX_PROPERTY_COUNT];
	size_t						mNumProperty = 0;
	AttributePack				mAttributes;

	//fill the properties based on template parameter and return the result
	static template<typename TClass, typename TParent> ZZ_ClassTypeCheckResult Make()
	{
		ZZ_ClassTypeCheckResult ret;

		static_assert(std::is_class<TClass>::value, "must be a class");
		//static_assert(std::is_default_constructible<TClass>::value && std::is_destructible<TClass>::value, "Class must be default constructible and destructible");

		ret.mHasParent = !std::is_void<TParent>::value;
		ret.mParentClassName = mHasParent ? ZZGetClassName<TParent>() : nullptr;
		ret.mClassName = ZZGetClassName<TClass>();
		ret.mClassSize = sizeof(TClass);
		ret.mClassAlign = alignof(TClass);

		using IsDCT = typename std::is_default_constructible<TClass>;

		//if a class is not default constructible we set, C/D/CC tor to null, treat as  abstract class and cant be instantiated
		if (!IsDCT::value)
		{
			ret.mDefaulConstructor = nullptr;
			ret.mDestructor = nullptr;
			ret.mCopyConstructor = nullptr;
		}
		else
		{
			typedef typename std::conditional<IsDCT::value, TClass, VVoid>::type TargetClassT;

			ret.mDefaulConstructor = [](void* object) { new (object) TargetClassT(); };
			ret.mDestructor = [](void* object) { ((TargetClassT*)object)->~TargetClassT(); };
			ret.mCopyConstructor = [](void* object, const void* copyObject) { new (object) TargetClassT(*((const TargetClassT*)copyObject)); };

		}

		//member functions
		{
			ret.mMetaSerialize = VCLASS_GET_MEMBERFUNCTION(MetaSerialize, TClass);
		}

		return ret;
	}
};
#endif // 


//////////////////////////////////////////////////////////////////////////
template<class TClass, class TParent> VClassTypeInfo*  ZZZClassRegister()
{
	VClassTypeInfo* pClass = nullptr;

	VName nameOfClass = VGetTypeidNameFixed(typeid(TClass));
	//already available?
	if (VTypeInfo* pFoundType = VMetaSys::GetIns()->FindTypeByName(nameOfClass))
	{
		VASSERT(pFoundType->GetAsClass());
		VASSERT(!pFoundType->GetAsClass()->mInitialized);
		pClass = (VClassTypeInfo*)pFoundType;
	}
	else
	{
		pClass = new VClassTypeInfo();
	}

	bool bbHasParent = !std::is_void<TParent>::value;
	VTypeInfo* pParentType = bbHasParent ? TPropertyTypeExtract<TParent>::Get() : nullptr;
	if (bbHasParent)
	{
		VASSERT(pParentType && pParentType->GetAsClass());
		pClass->mParentClass = pParentType->GetAsClass();
	}

	pClass->mId = EMT_Class;
	pClass->mName = VGetTypeidNameFixed(typeid(TClass));
	pClass->mSize = sizeof(TClass);
	pClass->mAlign = alignof(TClass);

	pClass->mHeaderFilename = TClass::ZZFileName();
	pClass->SourceLineNumber = TClass::ZZLineNumber();

	using IsDCT = typename std::is_default_constructible<TClass>;

	//if a class is not default constructible we set, C/D/CC tor to null, treat as  abstract class and cant be instantiated
	if (!IsDCT::value)
	{
		pClass->mDefaultConstructor = nullptr;
		pClass->mDestructor = nullptr;
		pClass->mCopyConstructor = nullptr;
	}
	else
	{
		typedef typename std::conditional<IsDCT::value, TClass, VVoid>::type TargetClassT;

		pClass->mDefaultConstructor = [](void* object) { new (object) TargetClassT(); };
		pClass->mDestructor = [](void* object) { ((TargetClassT*)object)->~TargetClassT(); };
		pClass->mCopyConstructor = [](void* object, const void* copyObject) { new (object) TargetClassT(*((const TargetClassT*)copyObject)); };

	}

	//meta functions
	{
		pClass->mSerialize = VCLASS_GET_MEMBERFUNCTION(MetaSerialize, TClass);
	}
	return pClass;
}



#define VCLASS_BEGIN(Class, ...)\
	struct ZZZ_##Class\
	{\
		typedef Class TClass;\
		typedef typename Class::ParentT TParentClass;\
		VClassTypeInfo* mClassInfo = nullptr;\
		ZZZ_##Class()\
		{\
			mClassInfo = ZZZClassRegister<TClass, TParentClass>();\
			mClassInfo->mSourceFilename = __FILE__;\
			mClassInfo->SourceLineNumber = __LINE__;\
			VMetaSys::GetIns()->RegisterNewType(mClassInfo);\





#define VPROPERTY(Property, ...)\
			{\
				typedef decltype(TClass::Property) TPropertyType;\
				VPropertyInfo* pPrp = new VPropertyInfo();\
				pPrp->mOwner = mClassInfo;\
				pPrp->mName = #Property;\
				pPrp->mOffset = offsetof(TClass, Property);\
				pPrp->mLineNumber = __LINE__;\
				pPrp->mTypeInfo = TPropertyTypeExtract<TPropertyType>::Get();\
				VASSERT(pPrp->mTypeInfo);\
				mClassInfo->mProperties.Add(pPrp);\
			}\



#define VCLASS_END(Class)\
			mClassInfo->mInitialized = true;\
		}\
		~ZZZ_##Class()\
		{\
			VMetaSys::GetIns()->UnregType(mClassInfo);\
		}\
	} ZZZ_##Class##Instance;\
	const VClassTypeInfo* Class::GetClassStatic() { return ZZZ_##Class##Instance.mClassInfo; }\


//hot reload process
//save the scene
//free the old dll
//load the new one

