#include <iostream>
#include "OP.cpp"

using std::cout; 
using std::endl;


int main( void )
{
	initializeRegisters();

	// cout << "r3 :\t" << static_cast<int16_t>(reg[r3]) << endl;
	reg[r2] = 17;	
	// processInstruction( 0x01032000 ); // add r3 to r2
	// cout << "r3 :\t" << static_cast<int16_t>(reg[r3]) << endl;

	processInstruction( 0x51007FFF );
	processInstruction( 0x51000050 );
	processInstruction( 0x510000AE );

	dispMemoryStack();
	processInstruction( 0x1A820001 );
	//processInstruction( 0x10800001 );
	dispMemoryStack();
	dispFlagsRegister();

	processInstruction( 0x1A82000F );
	//processInstruction( 0x10800001 );
	dispMemoryStack();
	dispFlagsRegister();

	// dispMemoryStack();


	return 0;
}
