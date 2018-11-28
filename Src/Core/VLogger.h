#pragma once

#include <sstream>
#include <assert.h>
#include <vector>
#include <ctime>

enum ELogColor
{
	ELC_Black,
	ELC_Red,
	ELC_Green,
	ELC_Blue,
	ELC_Cyan,
	ELC_Magenta,
	ELC_Yellow,
	ELC_White,
	ELC_Gray,

	ELC_HiRed,
	ELC_HiGreen,
	ELC_HiBlue,
	ELC_HiCyan,
	ELC_HiMagenta,
	ELC_HiYellow,
	ELC_HiWhite,

	//default foreground color for normal text
	ELC_DefaultFG = ELC_HiWhite,
	//default background color for normal text
	ELC_DefaultBG = ELC_White,
	//default foreground color for parameters
	ELC_DefaultParamFG = ELC_HiGreen
};

enum ELogLevel
{
	ELL_Message,
	ELL_Success,
	ELL_Warning,
	ELL_Error,
	ELL_Debug,
	ELL_Hint,
};

//a simple linear allocator on the stack which only grows
template<size_t SizeInByte> struct VLinerAllocStack
{
	size_t mSize = 0;
	uint8_t mBuffer[SizeInByte];

	size_t Avail() const { return mSize - SizeInByte; }

	void* Alloc(size_t size)
	{
		if (size <= Avail())
		{
			auto ret = &mBuffer[mSize];
			mSize += size;
			return ret;
		}
		return nullptr;
	}
};
struct VLogToken
{
	//index of the first character of string in the buffer. the token is null terminated.
	unsigned mIndex : 31;
	//whether its parameter or normal text
	unsigned mIsParam : 1;
};
struct VLogTokenPack
{
	static constexpr unsigned MAX_TOKEN = 1024;
	static constexpr unsigned BUFFER_SIZE = 8192;
	
	unsigned	mTokenCount = 0;
	unsigned	mStringBufferSize = 0;
	char		mStringBuffer[BUFFER_SIZE];
	VLogToken	mTokens[MAX_TOKEN];
	
	const char* GetTokenStr(int index) const 
	{
		return &mStringBuffer[mTokens[index].mIndex];
	}
	
	void NewToken(bool bIsParam)
	{
		mTokens[mTokenCount].mIndex = mStringBufferSize;
		mTokens[mTokenCount].mIsParam = bIsParam;
		mTokenCount++;
		assert(mTokenCount < MAX_TOKEN);
	}
	void PushChr(char chr)
	{
		assert(mStringBufferSize < BUFFER_SIZE - 1);
		mStringBuffer[mStringBufferSize++] = chr;
	}
	void PushStr(const char* str, size_t len)
	{
		memcpy(&mStringBuffer[mStringBufferSize], str, len);
		mStringBufferSize += len;
		assert(mStringBufferSize < BUFFER_SIZE);
	}
	void PushStr(const char* str)
	{
		PushStr(str, strlen(str));
	}
	void End()
	{
		PushChr(0);
	}
	void Print() const;
};

struct VlogIndent
{
	int mIndent;
	explicit VlogIndent(int indent) : mIndent(indent) {}
};
//////////////////////////////////////////////////////////////////////////
void VGetLogString(bool b, char* out, size_t size );
void VGetLogString(int i, char* out, size_t size);
void VGetLogString(double d, char* out, size_t size);
void VGetLogString(unsigned i, char* out, size_t size);
void VGetLogString(unsigned long long i, char* out, size_t size);
void VGetLogString(char c, char* out, size_t size);
void VGetLogString(const void* p, char* out, size_t size);
void VGetLogString(const char* str, char* out, size_t size);
void VGetLogString(VlogIndent indent, char* out, size_t size);

struct VLogEntryData
{
	const char* mFunction;
	const char* mFilename;
	const char* mArgsStr;
	uint32_t mLineNumber;
	uint32_t mThreadId; //thread id in which log occurred
	uint32_t mClock; //clock at which log occurred
	ELogLevel mLevel;
	VLogTokenPack mTokens;


};

inline void ZZSPrintAuto(VLogTokenPack& out, const char* format)
{
	while (*format)
	{
		if (*format == ('%'))
		{
			assert(false);
		}
		out.PushChr(*format);
		format++;
	}
}
template<typename T, typename... TArgs> void ZZSPrintAuto(VLogTokenPack& data, const char* format, const T& value, const TArgs&... args)
{
	while (*format)
	{
		if (*format == ('%'))
		{
			/*

			data.End();
			data.NewToken(true);
			std::ostringstream osstr;
			osstr << value;
			data.PushStr(osstr.str());
			data.End();

			*/
			data.End();
			data.NewToken(true);
			char itemBuffer[256];
			VGetLogString(value, itemBuffer, sizeof(itemBuffer));
			data.PushStr(itemBuffer);
			data.End();


			data.NewToken(false);
			format++;
			ZZSPrintAuto(data, format, args...);

			return;
		}

		data.PushChr(*format);
		format++;
	}
}


template<typename... TArgs> void VLogExtractTokens(VLogTokenPack& outTokens, const char* format, const TArgs&... args)
{
	outTokens.NewToken(false);
	ZZSPrintAuto(outTokens, format, args...);
	outTokens.End();
}

void VPrintLogEntryToConsole(const VLogEntryData& data);

struct VLogger
{
	static VLogger& Get();

	//returns true if we must catch this log
	bool Check(const char* function);
	
	std::vector<VLogEntryData> mLogs;

	void DumpString(std::string& out);
	
};

#define VLOG_BASE(level, format, ...) \
if(VLogger::Get().Check(__FUNCTION__))\
{\
	VLogEntryData logData;\
	logData.mFunction = __FUNCTION__;\
	logData.mLineNumber = __LINE__;\
	logData.mFilename = __FILE__;\
	logData.mThreadId = 0; /*#TODO*/ \
	logData.mClock = std::clock();\
	logData.mLevel = level;\
	logData.mArgsStr = #__VA_ARGS__;\
	VLogExtractTokens(logData.mTokens, format, ##__VA_ARGS__);\
	VLogger::Get().mLogs.push_back(logData);\
	VPrintLogEntryToConsole(logData);\
}\

#define VLOG_MSG(format, ...) VLOG_BASE(ELL_Message, format, __VA_ARGS__ )
#define VLOG_ERR(format, ...) VLOG_BASE(ELL_Error  , format, __VA_ARGS__ )
#define VLOG_WRN(format, ...) VLOG_BASE(ELL_Warning, format, __VA_ARGS__ )
#define VLOG_SUC(format, ...) VLOG_BASE(ELL_Success, format, __VA_ARGS__ )
#define VLOG_DBG(format, ...) VLOG_BASE(ELL_Debug  , format, __VA_ARGS__ )

