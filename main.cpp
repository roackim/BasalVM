#include <iostream>
#include "OP.cpp"

using std::cout; 
using std::endl;


int main( void )
{
	initializeRegisters();

	reg[r3] = -3;  
	reg[r2] = 1;	

	// cout << static_cast<int16_t>(reg[r2]) << endl;

	processInstruction( 0x041000FF );
	processInstruction( 0x041000FE );
	processInstruction( 0x0410000E );
	processInstruction( 0x041000FD );

	dispMemoryStack();
	processInstruction( 0x05020000 ); 
	dispMemoryStack();
	cout << "after the top: " << static_cast<int16_t>(memory[reg[rsp]+1]) << endl;

	// value still exist after decrement of reg[rsp]
	// cout << static_cast<int16_t>(memory[reg[rsp]+1]) << endl;



	return 0;
}
