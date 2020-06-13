#include <iostream>
#include "OP.cpp"

using std::cout; 
using std::endl;


int main( void )
{
	initializeRegisters();

	reg[cx] = 17;	
	reg[ax] = 7;
	reg[bx] = 3;

	// processInstruction( 0xF7200000 ); // input cx
	// processInstruction( 0xF0210000 ); // disp cx

	processInstruction( 0x510000FF ); // push


	processInstruction( 0x43208000 ); // copy sp, cx
	processInstruction( 0x10200001 ); // add   1, cx

	cout << "input string :\n-> " ;
	processInstruction( 0xF8200000 ); // input str

	 // processInstruction( 0x51000061 ); // push a
	 // processInstruction( 0x51000062 ); // push b
	 // processInstruction( 0x51000063 ); // push c
	 // processInstruction( 0x51000064 ); // push d
	 // processInstruction( 0x51000000 ); // push NULL

	// processInstruction( 0xF4210000 ); // disp value at memory[cx] then disp "\n"

	dispMemoryStack();
	
	cout << "Printing string from memory stack : \n" << endl;
	processInstruction( 0xF3210000 ); // disp string starting at memory[cx], until NULL then disp "\n"
	//dispFlagsRegister();

	// dispMemoryStack();


	return 0;
}
