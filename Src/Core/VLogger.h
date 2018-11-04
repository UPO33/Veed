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
};


struct VLogToken
{
	unsigned mIndex;
	bool bIsParam;
};

struct VLogEntryData
{
	static constexpr unsigned MAX_TOKEN = 1024;

	const char* mFunction;
	const char* mFilename;
	const char* mArgsStr;
	uint32_t mLineNumber;
	uint32_t mThreadId; //thread id in which log occurred
	uint32_t mClock; //clock at which log occurred
	ELogLevel mLevel;
	VLogToken mTokens[MAX_TOKEN];
	unsigned mTokenCount = 0;
	std::vector<char> mStringBuffer;

	VLogEntryData()
	{
		mStringBuffer.reserve(4096);
	}
	void PushStr(const std::string& str)
	{
		mStringBuffer.insert(mStringBuffer.end(), str.begin(), str.end());
	}
	void PushChr(char chr)
	{
		mStringBuffer.push_back(chr);
	}
	void NewToken(bool bIsParam)
	{
		mTokens[mTokenCount].mIndex = mStringBuffer.size();
		mTokens[mTokenCount].bIsParam = bIsParam;
		mTokenCount++;
		assert(mTokenCount < MAX_TOKEN);
	}
	void End()
	{
		mStringBuffer.push_back('\0');
	}
	void Print()
	{
		for (size_t i = 0; i < mTokenCount; i++)
		{
			const char* strToken = mStringBuffer.data() + mTokens[i].mIndex;
			printf("[%s]\n", strToken);
		}
	}
};

inline void ZZSPrintAuto(VLogEntryData& out, const char* format)
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
template<typename T, typename... TArgs> void ZZSPrintAuto(VLogEntryData& data, const char* format, const T& value, const TArgs&... args)
{
	while (*format)
	{
		if (*format == ('%'))
		{
			data.End();
			data.NewToken(true);
			std::ostringstream osstr;
			osstr << value;
			data.PushStr(osstr.str());
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


template<typename... TArgs> void VLogExtractData(VLogEntryData& outData, const char* format, const TArgs&... args)
{
	outData.NewToken(false);
	ZZSPrintAuto(outData, format, args...);
	outData.End();
}

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
	logData.mThreadId = 0;\
	logData.mClock = std::clock();\
	logData.mLevel = level;\
	logData.mArgsStr = #__VA_ARGS__;\
	VLogExtractData(data, format, ##__VA_ARGS__);\
	VLogger::Get().mLogs.push_back(logData);\
}\

