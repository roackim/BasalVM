#include <iostream>
#include "VM.h"

using std::cout; 
using std::endl;


int main( void )
{
	VM vm;

	vm.initialize();

	vm.reg[cx] = 17;	
	vm.reg[ax] = 7;
	vm.reg[bx] = 3;

	// vm.processInstruction( 0xF7200000 ); // input cx
	// vm.processInstruction( 0xF0210000 ); // disp cx

	// cout << "hey" << endl;
	vm.processInstruction( 0x510000FF ); // push

	// cout << "hey" << endl;
	vm.processInstruction( 0x43208000 ); // copy sp, cx
	vm.processInstruction( 0x10200001 ); // add   1, cx

	cout << "input string :\n-> " ;
	vm.processInstruction( 0xF8200000 ); // input str

	 // vm.processInstruction( 0x51000061 ); // push a
	 // vm.processInstruction( 0x51000062 ); // push b
	 // vm.processInstruction( 0x51000063 ); // push c
	 // vm.processInstruction( 0x51000064 ); // push d
	 // vm.processInstruction( 0x51000000 ); // push NULL

	// vm.processInstruction( 0xF4210000 ); // disp value at memory[cx] then disp "\n"

	vm.dispMemoryStack();
	
	cout << "Printing string from memory stack : " << endl;
	vm.processInstruction( 0xF3210000 ); // disp string starting at memory[cx], until NULL then disp "\n"
	//dispFlagsRegister();

	// dispMemoryStack();


	return 0;
}
