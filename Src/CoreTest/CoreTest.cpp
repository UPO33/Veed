#include <stdexcept>
#include <windows.h>
#include <csignal>

void MainGuarded()
{
	extern void VTest_Array();
	extern void VTest_Log();
	extern void VTest_Serializer();
	extern void VTest_Smart();

	VTest_Array();
	VTest_Log();
	VTest_Serializer();
	VTest_Smart();
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