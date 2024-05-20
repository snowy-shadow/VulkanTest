#include "App.h"

#include <iostream>

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
