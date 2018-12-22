#include <stdexcept>
#include <csignal>

#define VRUN_TEST(funcname) \
	extern void funcname(); \
	printf("\n------%s-----\n", #funcname);\
	funcname();\


void MainGuarded()
{
	VRUN_TEST(VTest_Queue);
	VRUN_TEST(VTest_Meta);
	VRUN_TEST(VTest_Name);
	VRUN_TEST(VTest_Array);
	VRUN_TEST(VTest_Log);
	VRUN_TEST(VTest_Serializer);
	VRUN_TEST(VTest_Smart);
	VRUN_TEST(VTes_BitSteam);
	

	//VTest_Guard();
}
void signal_handler(int signal)
{
	printf("sig\n");
	system("pause");
}

int main(int argc, char** argv)
{
	// Install a signal handler
	std::signal(SIGABRT, signal_handler);
	MainGuarded();
	system("pause");
	/*
	__try
	{
		MainGuarded();
		*((int*)nullptr) = 456;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		printf("\nError Occurred\n");
		system("pause");
	}*/
	
}