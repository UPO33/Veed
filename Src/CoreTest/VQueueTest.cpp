#include "../Core/VHistoryQueue.h"
#include "../Core/VLogger.h"

static int gTestN = 0;

void VTest_Queue()
{
	
	{
		struct S00
		{
			int mNum = 0;
			S00(int num) : mNum(num)
			{
				gTestN++;
				printf("CT\n");
			}
			S00(const S00& copy) : mNum(copy.mNum)
			{
				gTestN++;
				printf("CCT\n");
			}
			~S00()
			{
				printf("DT\n");
				gTestN--;
			}
		};

		VHistoryQueue<S00, 4> intQ0;

		for (size_t i = 0; i < intQ0.Capacity() * 2; i++)
		{
			S00 rm(0);
			if (intQ0.Push(S00(i), rm))
			{
				VLOG_MSG("% removed", rm.mNum);
			}
		}
		//for (size_t i = 0; i < intQ0.Capacity(); i++)
		//{
		//	VASSERT(intQ0.Oldest(i).mNum == i);
		//}
		//for (size_t i = 0; i < 4; i++)
		//{
		//	VASSERT(intQ0.Dequeue().mNum == i);
		//}
		//VASSERT(intQ0.Length() == 4);
	}

	//VASSERT(gTestN == 0);

 	//VLOG_MSG("");
}