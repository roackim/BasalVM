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
    int res;
#if defined _WIN32
    res = system("cls");
#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
    res = system("clear");
#elif defined (__APPLE__)
    res = system("clear");
#endif

    if (res != 0) std::cerr << "/!\\ Warning : cannot clear screen" << std::endl;
}
