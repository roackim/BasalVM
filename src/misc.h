#pragma once
#include <iostream>
#include <string>


void Error( const std::string& message ); // TODO Subject to change !


// helper function : transform a bool for [0;1] to [1;-1]
short coef( bool value );

// allow to clear the console output depending on the OS
void ClearConsole();


