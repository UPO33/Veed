#include "../Core/VArray.h"

void VTest_Array()
{
	{
		VArray<int> arr0;
		arr0.AddZeroed(1024);
		VASSERT(arr0.Length() == 1024);
		for (int i = 0; i < 1024; i++)
		{
			VASSERT(arr0.Last(i) == 0 && arr0.First(i) == 0);
		}
		arr0.Reset();
		for (int i = 0; i < 512; i++)
		{
			arr0.Add(i);
		}
		for (int i = 0; i < 512; i++)
		{
			arr0[i] == i;
		}

		VArray<int> arrMove = std::move(arr0);
		VArray<int> arrCopy = arrMove;
		VASSERT(arr0.Length() == 0 && arr0.Capacity() == 0);
		VASSERT(arr0.Length() == 0 && arr0.Length() == 0);
		VASSERT(arrMove.Length() == arrCopy.Length());

		arrCopy.ConditionalRemovePOD([](int i) { return i % 2 == 0; });
		for(int& item : arrCopy)
		{
			VASSERT(item % 2 != 0);
			item = 123;
		}
	}

	struct SStruct0
	{
		int i = 0;
	};

	{
		VArray<std::string> arrString(1024);
		for (size_t i = 0; i < 32; i++)
		{
			arrString.Add("hello");
		}
		for (size_t i = 0; i < 32; i++)
		{
			arrString.Add("world");
		}
		
		
		VArray<std::string> copyArr = arrString;
		VArray<std::string> moveArr = std::move(copyArr);
		
	}
}