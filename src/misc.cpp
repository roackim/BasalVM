#include "misc.h"
#include <iostream>
#include <string>

using std::string;

void Error( const std::string& message ) // TODO Subject to change !
{
	std::cerr << "/!\\ Error : " << message << ". \nTerminating the program." << std::endl;
	exit( -1 );
}

// helper function : transform a bool for [0;1] to [1;-1]
short coef( bool value )
{
	return -(value*2 -1);
}

// allow to clear the console output depending on the OS
void ClearConsole()
{
#if defined _WIN32
    system("cls");
#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
    system("clear");
#elif defined (__APPLE__)
    system("clear");
#endif
}
