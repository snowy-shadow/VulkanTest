// VulkanTest.cpp : Defines the entry point for the application.
//

#include "App.h"

int main()
{
	try
	{
		VT::App App;
		App.run();
	}
	catch (const std::exception& E)
	{
		std::cout << E.what();
	}

	return 0;
}