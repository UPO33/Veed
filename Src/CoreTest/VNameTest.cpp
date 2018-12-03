#include "../Core/VName.h"
#include "../Core/VLogger.h"
#include "../Core/VCoreBase.h"

void VTest_Name()
{
	{
		auto h0 = VName::MakeHandle(0, 0);
		auto h1 = VName::MakeHandle(1, -1);

		VName empty;
		VName namecopy = empty;
		VName nameHello("Hello", -1);
		VName nameHello2("HELLO", 2);
		VName nameHello3("HellO", 3);
		VName nameHello4("heLLO", 4);
		VName nameHello5(nameHello4, 5);
		VName nameHello6(nameHello4, 6);

		VASSERT(nameHello.StringPart() == nameHello2.StringPart());
		VASSERT(nameHello2.StringPart() == nameHello3.StringPart());
		VASSERT(nameHello3.StringPart() == nameHello4.StringPart());
		VASSERT(nameHello4.StringPart() == nameHello5.StringPart());
		VASSERT(nameHello5.StringPart() == nameHello6.StringPart());

		{
			VName name_asd123("asd123");
			VName name_asd256("asd256");
			VASSERT(name_asd123.StringPart() == name_asd256.StringPart());
			VASSERT(name_asd123.NumberPart() == 123 && name_asd256.NumberPart() == 256);

			VName name_123("123");
			VASSERT(name_123.StringPart()[0] == 0);
			VASSERT(name_123.NumberPart() == 123);
			VName name_123_1("123-1");
			VName name_123_2("123-2");
			VASSERT(name_123_1.StringPart() == name_123_2.StringPart());
			VASSERT(name_123_1.NumberPart() == 1 && name_123_2.NumberPart() == 2);

			VName name_22_33_a("22_33_a0-2");
			VName name_22_44_a("22_44_a0-2");
			VASSERT(name_22_33_a.StringPart() != name_22_44_a.StringPart());

		}

 		VLOG_MSG("");
	}
}