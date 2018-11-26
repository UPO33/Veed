#include "../Core/VLogger.h"
#include <windows.h>
#include <iostream>

void VTest_Log()
{
	//VLogTokenPack data;
	//
	//VLogExtractTokens(data, "object % created at %%% ok? % addr:%", "myObject", 10, 20, 30, true, (void*)(0xFFffFF));
	//data.Print();
	//
	//VLogTokenPack data2;
	//VLogExtractTokens(data2, "object created");
	//data2.Print();
	//

	VLOG_MSG("object % created at % % %", "object0", 10, 20, 30);
	VLOG_SUC("object % created at % % %", "object0", 10, 20, 30);
	VLOG_WRN("object % created at % % %", "object0", 10, 20, 30);

}