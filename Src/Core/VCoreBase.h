#pragma once

#include <iostream>
#include <algorithm>
#include <utility>
#include <assert.h>
#include <cmath>
#include <iostream>
#include <time.h>
#include <type_traits>
#include <functional>
#include <cstdint>
#include <sstream>
#include <cstdlib>


#define VASSERT(expression) assert(expression)





#if defined(_MSC_VER)
#define VCOMPILER_MSVC
#define VMODULE_IMPORT __declspec(dllimport)
#define VMODULE_EXPORT __declspec(dllexport)
#elif defined(__GNUC__)
#define VCOMPILER_GCC
#define VMODULE_IMPORT
#define VMODULE_EXPORT  __attribute__((visibility("default")))
#else
#error not implemented yet
#endif


#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
#define VPLATFORM_WIN 1
#define VPLATFORM_ID  VPLATFORM_WIN
#endif
#if defined(__linux__) || defined(__gnu_linux__)
#define VPLATFORM_LINUX 2
#define VPLATFORM_ID  VPLATFORM_LINUX
#endif
#if defined(__ANDROID__)
#define VPLATFORM_ANDROID 3
#ifdef VPLATFORM_ID
#undef VPLATFORM_ID
#endif
#define VPLATFORM_ID  VPLATFORM_ANDROID
#endif




#ifdef VCOMPILER_MSVC
//#define UBREAK_DEBUGGER() (*((int*)0) = 0)
#define UBREAK_DEBUGGER() __debugbreak()
#else
#define UBREAK_DEBUGGER() assert(false)
#endif


#define VARRAY_LEN(Array)  (sizeof(Array) / sizeof(Array[0]))


//VWELD(prefix, __COUNTER__) will results  prefix__COUNTER__
#define VWELD(Prefix, Suffix) Prefix##Suffix
//VWELDINNER(prefix, __COUNTER__) will results  prefix1
#define VWELDINNER(Prefix, Suffix) VWELD(Prefix, Suffix)

#define VCACHE_ALIGN	128

/*
in order to check a class has the specified member function or to get pointer to member function whether available or not,
first u must declare that with UDECLARE_MEMBERFUNCTION_CHECKING(DeclarationName, FunctionName, ReturnType, Args...)
then u are able to use UCLASS_HAS_MEMBERFUNCTION and UCLASS_GET_MEMBERFUNCTION
note:
it doesn't take the parent's class functions into account
*/
#define VDECLARE_MEMBERFUNCTION_CHECKING(DeclarationName, FunctionName, ReturnType, ...)\
	template <class Type> struct Z_##DeclarationName##HasFuncChecker\
	{\
		using TFunc = ReturnType (Type::*) (__VA_ARGS__);\
		struct FakeType { ReturnType FunctionName(__VA_ARGS__) { return exit(0); } };\
		template <typename T, T> struct TypeCheck; \
		template <typename T> struct FuncCheck { typedef ReturnType(T::*fptr)(__VA_ARGS__); }; \
		template <typename T> static char HasFunc(TypeCheck< typename FuncCheck<T>::fptr, &T::FunctionName >*); \
		template <typename T> static long  HasFunc(...); \
		static bool const value = (sizeof(HasFunc<Type>(0)) == sizeof(char)); \
	}; \
	template<typename T> struct Z_##DeclarationName##HasFunc\
	{\
		using TFunc = ReturnType (UCore::Void::*) (__VA_ARGS__);\
		struct FakeType { ReturnType FunctionName(__VA_ARGS__) { return exit(0); } };\
		struct FakeT { static bool const value = 0; };\
		static bool const value = std::conditional<std::is_class<T>::value, Z_##DeclarationName##HasFuncChecker<T>, FakeT>::type::value;\
		static TFunc GetFunc() \
		{\
			using TargetClass = typename std::conditional<value, T, FakeType>::type;\
			if (std::is_same<TargetClass, FakeType>::value) return nullptr;\
			else return (TFunc)&TargetClass::FunctionName;\
		}\
	}; \

//return true if 'Class' has specified function function must be declared with UDECLARE_MEMBERFUNCTION_CHECKING once
#define VCLASS_HAS_MEMBERFUNCTION(DeclarationName, Class) Z_##DeclarationName##HasFunc<Class>::value
//return pointer to member function as ReturnType(Void::*)(Args), u can cast result to any type
//or use MFP<ReturnType, Args> for convenience, returns null if not exist
//function must be declared with UDECLARE_MEMBERFUNCTION_CHECKING once
#define VCLASS_GET_MEMBERFUNCTION(DeclarationName, Class) Z_##DeclarationName##HasFunc<Class>::GetFunc()

//disable needs to have dll interface
#pragma warning(disable:4251)




class VVoid {};

using VVoidFuncPtr = void(*)();
using VVoidMemFuncPtr = void(VVoid::*)();

struct VInitDefault {};
struct VInitConfig {};
struct VInitZero {};
struct VNoInit {};


template <typename TFunction = VVoidMemFuncPtr> void* MemFunc2Ptr(TFunction function)
{
	// 		union
	// 		{
	// 			TFunction f;
	// 			void* p;
	// 		};
	// 		f = function;
	// 		return p;
	return (void*&)function;
}
template<typename TFunction = VVoidMemFuncPtr> TFunction Ptr2MemFunc(const void* function)
{
	union
	{
		TFunction f;
		const void* p;
	};
	p = function;
	return f;
}
constexpr bool VIsPow2(size_t x)
{
	return x && ((x & (x - 1)) == 0);
}
//round up to next multiple of 'align'. 'align' must be power of 2
inline size_t VNextAlign(size_t numToRound, size_t align)
{
	VASSERT(VIsPow2(align));
	return (numToRound + align - 1) & ~(align - 1);
}
inline size_t VNextMul(size_t numToRound, size_t multiple)
{
	VASSERT(multiple);
	return ((numToRound + multiple - 1) / multiple) * multiple;
}
inline unsigned VNextPow2(unsigned v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}

static const float VPI = 3.141592653589f;
static float const VRAD2DEG = 180.0f / VPI;
static float const VDEG2RAD = VPI / 180.0f;
static float const VFLOAT_EPSILON = 0.000001f;

template<typename T> T VMax(T a, T b) { return a > b ? a : b; }
template<typename T> T VMin(T a, T b) { return a < b ? a : b; }

inline float VClamp(float value, float min, float max) { return value < min ? min : value > max ? max : value; }

inline float VSqrt(float x) { return sqrtf(x); }
inline float VRSqrt(float x) { return 1.0f / sqrt(x); }

inline float VSin(float x) { return sinf(x); }
inline float VCos(float x) { return cosf(x); }
inline void VSinCos(float angleRad, float& outSin, float& outCos)
{
	outSin = sin(angleRad);
	outCos = cos(angleRad);
}
inline void VSinCosDeg(float angleDeg, float& outSin, float& outCos)
{
	outSin = ::sin(angleDeg * VDEG2RAD);
	outCos = ::cos(angleDeg * VDEG2RAD);
}
inline float VACos(float f) { return acos(f); }
inline float VASin(float f) { return asin(f); }

inline float VAbs(float x) { return x < 0 ? -x : x; }

inline float VTan(float x)
{
	return ::tanf(x);
}
inline float VFloor(float f) { return std::floor(f); }
inline float VCeil(float f) { return std::ceil(f); }
inline float VRound(float f) { return std::round(f); }
inline bool VIsFinite(float f) { return std::isfinite(f); }
inline bool VIsNan(float f) { return std::isnan(f); }
inline bool VIsInf(float f) { return std::isinf(f); }
inline int VTruct(float f) { return (int)f; }
inline float VDistance(float x, float y) { return VAbs(x - y); }
inline float VLerp(float v0, float v1, float t)
{
	return (1 - t) * v0 + t * v1;
}
template<typename T> T VSign(T value)
{
	return value < 0 ? -1 : value > 0 ? 1 : 0;
}

inline bool VRandBool() { return std::rand() % 2 == 0; }
inline int VRandInt() { return std::rand(); }
inline float VRandFraction() { return ((float)std::rand()) / RAND_MAX; }
inline float VRandFloat(float min, float max) { return VLerp(min, max, VRandFraction()); }

//e.g TT_IsSpecialization<TArray, TArray<int>>::value === true
template <class T, template <class...> class Template> struct TT_IsSpecialization : std::false_type {};
template <template <class...> class Template, class... Args> struct TT_IsSpecialization<Template<Args...>, Template> : std::true_type {};
