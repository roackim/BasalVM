#include <iostream>
#include "VirtualMachine/VM.h"
#include "Assembler/Assembler.h"

using std::cout; 
using std::endl;


int main( void )
{


	// Instanciate Assambler, assemble instructions
	Asm::Assembler basm;
	basm.assemble( "program.basm" );
	
	// For Debugging purposes
	cout << "\n - assembler -" << endl;
	for( unsigned i=0; i<basm.program.size(); i++)
	{
		cout << std::hex << std::uppercase << basm.program[i] << endl;
	}
	cout << " - ok - " << endl;
	
	// Instanciate Virtual Machine
	VM vm;
	vm.initialize();
	vm.load( basm.program );

	vm.executeProgram();



	return 0;
}
