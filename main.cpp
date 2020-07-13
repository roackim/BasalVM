#include <iostream>
#include "VM.h"

using std::cout; 
using std::endl;


int main( void )
{
	VM vm;

	vm.reg[bx] = 123;

	vm.initialize();

	vm.processInstruction( 0x51000020 );
	vm.processInstruction( 0x510000FF );

	vm.dispMemoryStack();
	vm.processInstruction( 0x61000000 );
	vm.processInstruction( 0x60500000 );

	vm.dispMemoryStack();
	vm.processInstruction( 0x50003000 );

	vm.dispMemoryStack();
	vm.processInstruction( 0xD0000000 );
	vm.processInstruction( 0xE00003E8 );
	vm.processInstruction( 0x510000FF );




	return 0;
}
