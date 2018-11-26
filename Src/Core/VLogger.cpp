#include "VLogger.h"
#include <iostream>
#include <iomanip>

#ifdef _WIN32

#include <Windows.h>

namespace Console
{
	static const WORD bgMask(BACKGROUND_BLUE |
		BACKGROUND_GREEN |
		BACKGROUND_RED |
		BACKGROUND_INTENSITY);
	static const WORD fgMask(FOREGROUND_BLUE |
		FOREGROUND_GREEN |
		FOREGROUND_RED |
		FOREGROUND_INTENSITY);

	static const WORD fgBlack(0);
	static const WORD fgLoRed(FOREGROUND_RED);
	static const WORD fgLoGreen(FOREGROUND_GREEN);
	static const WORD fgLoBlue(FOREGROUND_BLUE);
	static const WORD fgLoCyan(fgLoGreen | fgLoBlue);
	static const WORD fgLoMagenta(fgLoRed | fgLoBlue);
	static const WORD fgLoYellow(fgLoRed | fgLoGreen);
	static const WORD fgLoWhite(fgLoRed | fgLoGreen | fgLoBlue);

	static const WORD fgGray(fgBlack | FOREGROUND_INTENSITY);
	static const WORD fgHiWhite(fgLoWhite | FOREGROUND_INTENSITY);
	static const WORD fgHiBlue(fgLoBlue | FOREGROUND_INTENSITY);
	static const WORD fgHiGreen(fgLoGreen | FOREGROUND_INTENSITY);
	static const WORD fgHiRed(fgLoRed | FOREGROUND_INTENSITY);
	static const WORD fgHiCyan(fgLoCyan | FOREGROUND_INTENSITY);
	static const WORD fgHiMagenta(fgLoMagenta | FOREGROUND_INTENSITY);
	static const WORD fgHiYellow(fgLoYellow | FOREGROUND_INTENSITY);

	static const WORD bgBlack(0);
	static const WORD bgLoRed(BACKGROUND_RED);
	static const WORD bgLoGreen(BACKGROUND_GREEN);
	static const WORD bgLoBlue(BACKGROUND_BLUE);
	static const WORD bgLoCyan(bgLoGreen | bgLoBlue);
	static const WORD bgLoMagenta(bgLoRed | bgLoBlue);
	static const WORD bgLoYellow(bgLoRed | bgLoGreen);
	static const WORD bgLoWhite(bgLoRed | bgLoGreen | bgLoBlue);

	static const WORD bgGray(bgBlack | BACKGROUND_INTENSITY);
	static const WORD bgHiWhite(bgLoWhite | BACKGROUND_INTENSITY);
	static const WORD bgHiBlue(bgLoBlue | BACKGROUND_INTENSITY);
	static const WORD bgHiGreen(bgLoGreen | BACKGROUND_INTENSITY);
	static const WORD bgHiRed(bgLoRed | BACKGROUND_INTENSITY);
	static const WORD bgHiCyan(bgLoCyan | BACKGROUND_INTENSITY);
	static const WORD bgHiMagenta(bgLoMagenta | BACKGROUND_INTENSITY);
	static const WORD bgHiYellow(bgLoYellow | BACKGROUND_INTENSITY);

};

inline WORD VToWin32Color(ELogColor bg, ELogColor fg)
{
	auto LGetBG = [=]()
	{
		switch (bg)
		{
		case ELC_Black: return Console::bgBlack;
		case ELC_Red: return Console::bgLoRed;
		case ELC_Green:return Console::bgLoGreen;
		case ELC_Blue: return Console::bgLoBlue;
		case ELC_Cyan: return Console::bgLoCyan;
		case ELC_Magenta: return Console::bgLoMagenta;
		case ELC_Yellow: return Console::bgLoYellow;
		case ELC_White: return Console::bgLoWhite;
		case ELC_Gray: return Console::bgGray;

		case ELC_HiRed: return Console::bgHiRed;
		case ELC_HiGreen:return Console::bgHiGreen;
		case ELC_HiBlue: return Console::bgHiBlue;
		case ELC_HiCyan: return Console::bgHiCyan;
		case ELC_HiMagenta: return Console::bgHiMagenta;
		case ELC_HiYellow: return Console::bgHiYellow;
		case ELC_HiWhite: return Console::bgHiWhite;
		}
		return Console::bgBlack;
	};

	auto LGetFG = [=]()
	{
		switch (fg)
		{
		case ELC_Black: return Console::fgBlack;
		case ELC_Red: return Console::fgLoRed;
		case ELC_Green:return Console::fgLoGreen;
		case ELC_Blue: return Console::fgLoBlue;
		case ELC_Cyan: return Console::fgLoCyan;
		case ELC_Magenta: return Console::fgLoMagenta;
		case ELC_Yellow: return Console::fgLoYellow;
		case ELC_White: return Console::fgLoWhite;
		case ELC_Gray: return Console::fgGray;

		case ELC_HiRed: return Console::fgHiRed;
		case ELC_HiGreen:return Console::fgHiGreen;
		case ELC_HiBlue: return Console::fgHiBlue;
		case ELC_HiCyan: return Console::fgHiCyan;
		case ELC_HiMagenta: return Console::fgHiMagenta;
		case ELC_HiYellow: return Console::fgHiYellow;
		case ELC_HiWhite: return Console::fgHiWhite;
		}
		return Console::fgLoWhite;
	};

	return LGetBG() | LGetFG();
}

void VConsoleSetColor(ELogColor bg, ELogColor fg)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), VToWin32Color(bg, fg));
}
#endif


VLogger& VLogger::Get()
{
	static VLogger Ins;
	return Ins;
}

bool VLogger::Check(const char* function)
{
	return true;
}

void VLogger::DumpString(std::string& out)
{
	char lineBuffer[4096];
	for (size_t i = 0; i < mLogs.size(); i++)
	{
		//sprintf(lineBuffer, "%s  %s %d\n %s", mLogs[i].mFilename, mLogs[i].mFunction, mLogs[i].mLineNumber,
		//	mLogs[i].mArgsStr, mLogs[i])

		out += "============\n";
		out += mLogs[i].mFunction;
		out += "============\n";
	}
}

void VLogTokenPack::Print() const
{
	for (size_t i = 0; i < mTokenCount; i++)
	{
		const char* strToken = &mStringBuffer[mTokens[i].mIndex];
		printf("[%s] isParam:%d\n", strToken, mTokens[i].mIsParam);
	}
}

inline ELogColor VLogGetLevelColor(ELogLevel level)
{
	switch (level)
	{
	case ELL_Message: return ELC_HiWhite;
	case ELL_Success: return ELC_HiGreen;
	case ELL_Warning: return ELC_HiYellow;
	case ELL_Error: return ELC_HiRed;
	case ELL_Debug: return ELC_HiBlue;
	case ELL_Hint: return ELC_HiMagenta;
	}
	return ELC_HiWhite;
}

void VPrintLogEntryToConsole(const VLogEntryData& data)
{
	VConsoleSetColor(ELC_Black, VLogGetLevelColor(data.mLevel));
	std::cout <<  data.mFunction << std::setw(29);

	for (int i = 0; i < data.mTokens.mTokenCount; i++)
	{
		const char* str = data.mTokens.GetTokenStr(i);
		if(!str || str[0] == 0) 
			continue;

		const VLogToken& tk = data.mTokens.mTokens[i];
		VConsoleSetColor(ELC_Black, tk.mIsParam ? ELC_HiGreen : ELC_HiWhite);
		std::cout << str;
		
	}
	VConsoleSetColor(ELC_Black, ELC_HiWhite);
	std::cout << std::endl;
}
