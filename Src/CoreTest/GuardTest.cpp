#include <windows.h>
#include "../Core/VLogger.h"

struct STestAsset
{
	int n = 123;

	STestAsset()
	{
		printf("%s\n", __FUNCTION__);
	}
	~STestAsset()
	{
		printf("%s\n", __FUNCTION__);
	}
	void Print()
	{
		printf("%s %d\n", __FUNCTION__, n);
	}
};
struct SGameObject
{
	STestAsset* Asset = nullptr;
	void Print()
	{
		Asset->Print();
	}
};

void VCreateAssets()
{
}
void VUpdateGame()
{
	STestAsset ass0;
	SGameObject go0;
	go0.Asset = &ass0;
	SGameObject go1;
	go0.Print();
	go1.Print();
}
void VTest_Guard()
{
	return;

	__try
	{
		VUpdateGame();
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		printf("\nError Occurred\n");
		system("pause");
	}
}