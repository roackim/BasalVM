#include <iostream>
#include "VM.h"
#include "Assembler.h"

using std::cout; 
using std::cerr;
using std::endl;


int main( int argc, char *argv[] )
{

	if( argc < 2 )
	{
		cerr << "No target to assemble. Terminating program." << endl;
		return 1;
	}

	string file = argv[1];
	
	// Instanciate Assambler, assemble instructions
	Asm::Assembler basm;
	bool s = basm.assemble( file );
	if( s == false )
	{
		exit( -1 );
	}
	
	// For Debugging purposes

	// cout << "\nAssembled :" << endl;
	// for( unsigned i=0; i<basm.program.size(); i++)
	// {
	// 	cout << "\t" << std::hex << std::uppercase << basm.program[i] << endl;
	// }
	// cout << endl;
	
	// Instanciate Virtual Machine
	VM vm;
	vm.initialize();
	vm.load( basm.program );

	vm.executeProgram();

	vm.dispMemoryStack();


	return 0;
}
