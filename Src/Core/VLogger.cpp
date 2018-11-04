#include "VLogger.h"

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
