#include "../Core/VSmartMemory.h"
#include "../Core/VArray.h"

void VTest_Smart()
{

	struct VSmartObj0 : public VSmartBase
	{
		VSmartObj0()
		{
			printf("CTOR\n");
		}
		~VSmartObj0()
		{
			printf("DTOR\n");
		}
	};
	{
		VSharedPtr<VSmartMemBlock> memblock0 = VSmartMemBlock::Create(4096);
		VSharedPtr<VSmartBase> sp0 = memblock0;
		{
			VSharedPtr<VSmartBase> sp1 = sp0;
		}

	}
	{
		VSharedPtr<VSmartObj0> smartObj0 = new VSmartObj0;
		VASSERT(smartObj0.IsValid());
		
		VArray<VSharedPtr<VSmartObj0>> objects;
		objects.Add(smartObj0);
		objects.Add(smartObj0);
		objects.Add(smartObj0);
		objects.Reset();
	}
	
}