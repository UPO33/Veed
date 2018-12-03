#pragma once

#include "VCoreBase.h"

//////////////////////////////////////////////////////////////////////////
template<typename T> class VFunc {};
template<typename TRet, typename... TArgs> class VFunc<TRet(TArgs...)>
{
public:
	using RetT = TRet;
	using PatternT = TRet(*)(TArgs...);
	PatternT mFunction;

	VFunc() {}
	VFunc(std::nullptr_t) { mFunction = nullptr; }
	template<typename T> VFunc(T pFunction) { mFunction = pFunction; }


	TRet operator() (TArgs... args) const
	{
		return mFunction(args...);
	}
	operator bool() const { return mFunction != nullptr; }
	explicit operator void* () const { return (void*)mFunction; }
	bool operator == (const void* p) const { return mFunction == p; }
	bool operator != (const void* p) const { return mFunction != p; }
	bool operator == (const VFunc& fp) const { return mFunction == fp.mFunction; }
	bool operator != (const VFunc& fp) const { return mFunction != fp.mFunction; }
};

//////////////////////////////////////////////////////////////////////////member function pointer
template<typename T> class VMemFunc {};
template<typename TRet, typename... TArgs> class VMemFunc<TRet(TArgs...)>
{
public:
	using RetT = TRet;
	using PatternT = TRet(VVoid::*)(TArgs...);

	PatternT mFunction;

	VMemFunc() {}
	VMemFunc(std::nullptr_t) { mFunction = nullptr; }

	template <typename T> VMemFunc(TRet(T::* pFunction)(TArgs...))
	{
		union
		{
			PatternT	a;
			TRet(T::* b)(TArgs...);
		};
		b = pFunction;
		mFunction = a;
	}

	TRet operator() (void* object, TArgs... args) const
	{
		return (((VVoid*)object)->*mFunction)(args...);
	}
	operator bool() const { return mFunction != nullptr; }
	explicit operator void* () const { return MemFunc2Ptr(mFunction); }

	bool operator == (const VMemFunc& other) const { return mFunction == other.mFunction; }
	bool operator != (const VMemFunc& other) const { return mFunction != other.mFunction; }
};