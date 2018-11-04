#include "../Core/VLogger.h"

void VTest_Log()
{
	VLogEntryData data;
	VLogExtractData(data, "object % created at %%%", "myObject", 10, 20, 30);
	data.Print();
	VLogEntryData data2;
	VLogExtractData(data2, "object created");
	data2.Print();
	assert(1);

	VLOG_BASE(ELL_Message, "object % created at % % %", "object0", 10, 20, 30);
	VLOG_BASE(ELL_Message, "object % created at % % %", "object0", 10, 20, 30);

	std::string dumpStr;
	VLogger::Get().DumpString(dumpStr);
}