#include "../Core/VNetBitStream.h"
#include <stdlib.h>
#include "../Core/VMemory.h"
#include "../Core/VCoreBase.h"

void VTes_BitSteam()
{
	struct VTestStruct
	{
		
		bool mBool0;
		uint32_t mDW;
		uint32_t m4Bit;
		uint32_t m7Bit;
		char mBuffer[32];

		VTestStruct()
		{
		}
		void SetRandom()
		{
			mBool0 = rand() % 2 == 0;
			mDW = rand();
			m4Bit = rand() & (0b1111);
			m7Bit = rand() & (0b1111111);
			for (size_t i = 0; i < sizeof(mBuffer); i++)
				mBuffer[i] = i;
		}
		void Serialize(VBitStream& stream)
		{
			stream.RWBit(mBool0);
			stream.RWNumber(mDW, 32);
			stream.RWNumber(m4Bit, 4);
			stream.RWNumber(m7Bit, 7);
			stream.RWBits (mBuffer, 0, sizeof(mBuffer) * 8);
		}
	};


	VTestStruct struct0;
	VTestStruct struct0Read;
	struct0.SetRandom();
	
	VBitWriter writer0;
	struct0.Serialize(writer0);

	VBitReader reader0(writer0.GetBytes(), writer0.Seek());
	struct0Read.Serialize(reader0);

	VBitWriterStack<1024> rewite;
	struct0Read.Serialize(rewite);

	VASSERT(rewite.Seek() == writer0.Seek());
	VASSERT(memcmp(writer0.GetBytes(), rewite.GetBytes(), writer0.Seek() / 8) == 0);


	printf("");
	
}