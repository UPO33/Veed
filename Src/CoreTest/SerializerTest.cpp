#include "../Core/VSerializer.h"
#include "../Core/VCoreBase.h"

struct VTestSer0
{
	int mNumber = 0;
	float mFloat = 0;
	bool mBool = false;

	void FillRandom()
	{
		mNumber = rand();
		mFloat = rand() * 0.123f;
		mBool = rand() % 2 == 0;
	}
	inline bool operator == (const VTestSer0& other)
	{
		return mNumber == mNumber
			&& mFloat == other.mFloat
			&& mBool == other.mBool;
	}
};
inline VArchiveBase& operator && (VArchiveBase& ar, VTestSer0& data)
{
	ar && data.mNumber && data.mFloat && data.mBool;
	return ar;
}
void VTest_Serializer()
{
	VArchiveByteWriter writer;

	constexpr unsigned Count = 1024;
	VTestSer0 dataStructs[Count];
	VTestSer0 dataStructs2[Count];

	for (size_t i = 0; i < Count; i++)
	{
		dataStructs[i].FillRandom();
		writer && dataStructs[i];
	}

	VArchiveByteWriter writer2;
	VArchiveByteReader reader(writer.GetBytes(), writer.GetSeek(), true);
	for (size_t i = 0; i < Count; i++)
	{
		reader && dataStructs2[i];
		writer2 && dataStructs2[i];
		VASSERT(reader.IsValid());
		VASSERT(dataStructs[i] == dataStructs2[i]);
	}
	
	VASSERT(writer.GetSeek() == writer2.GetSeek());
	VASSERT(memcmp(writer.GetBytes(), writer2.GetBytes(), writer.GetSeek()) == 0);

}