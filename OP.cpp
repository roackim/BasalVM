#include <iostream>
#include "VM.cpp"

using std::cout;
using std::endl;

OP getInstruction( uint32_t instruction )
{
	// only keep the 4 left hand bits
	instruction = ( instruction & 0xFF000000 ) >> 24;
	// convert value as an OP 
	OP op = OP( instruction );
	return op;	
}

void executeADD( uint32_t instruction );
void executeSUB( uint32_t instruction );
void executeMOV( uint32_t instruction );
void executePUSH( uint32_t instruction );
void executePOP( uint32_t instruction );

void processInstruction( uint32_t instruction )
{
	// get the current opcode
	OP op = getInstruction( instruction );

	switch( op )
	{
		case ADD :
			executeADD( instruction );
			break;
		case SUB :
			executeSUB( instruction );
			break;
		case MOV :
			executeMOV( instruction );
			break;
		case PUSH:
			executePUSH( instruction );
			break;
		case POP :
			executePOP( instruction );
			break;
	}
}


// 4b: opcode (already striped) | 3b: destination register | 1b: mode | either 3:b source register or 8b: immediate value
// immediate value is encoded with Signed magnitude representation -> 1bit for sign and 7bits for value
void executeADD( uint32_t instruction )
{
	short mode	= ( instruction & 0x00F00000 ) >> 20;	// mode of the instruction
	short dest	= ( instruction & 0x000F0000 ) >> 16;	// destination register
	short src	= ( instruction & 0x0000F000 ) >> 12;	// source register

	mode &= 0x01;	// only look at the last bit

	if( mode == 0 )	// two register mode
	{
		if( src > R_COUNT )
			cout << "Unknown register: '" << src << "'\n -> Instruction discarded" << endl;
		reg[dest] += reg[src];
	}

	else if( mode == 1 ) // one register and an imediate value mode
	{
		signed short value = ( instruction & 0x0000FFFF );
		reg[dest] += value;
	}	
	else 
		cout << "Unknown mode for ADD: '" << mode << "'\n -> Instruction discarded" << endl;
}

// same as executeADD but substract instead
void executeSUB( uint32_t instruction )
{
	short mode	= ( instruction & 0x00F00000 ) >> 20;	// mode of the instruction
	short dest	= ( instruction & 0x000F0000 ) >> 16;	// destination register
	short src	= ( instruction & 0x0000F000 ) >> 12;	// source register

	mode &= 0x01;	// only look at the last bit

	if( mode == 0 )	// two register mode
	{
		if( src > R_COUNT )
			cout << "Unknown register: '" << src << "'\n -> Instruction discarded" << endl;
		reg[dest] -= reg[src];
	}

	else if( mode == 1 ) // one register and an imediate value mode
	{
		signed short value = ( instruction & 0x0000FFFF );
		reg[dest] -= value;
	}	
	else 
		cout << "Unknown mode for SUB: '" << mode << "'\n -> Instruction discarded" << endl;
}

// same as executeADD but replace instead
void executeMOV( uint32_t instruction )
{
	short mode	= ( instruction & 0x00F00000 ) >> 20;	// mode of the instruction
	short dest	= ( instruction & 0x000F0000 ) >> 16;	// destination register
	short src	= ( instruction & 0x0000F000 ) >> 12;	// source register

	mode &= 0x01;	// only look at the last bit

	if( mode == 0 )	// two register mode
	{
		if( src > R_COUNT )
			cout << "Unknown register: '" << src << "'\n -> Instruction discarded" << endl;
		reg[dest] = reg[src];
	}

	else if( mode == 1 ) // one register and an imediate value mode
	{
		signed short value = ( instruction & 0x0000FFFF );
		reg[dest] = value;
	}	
	else 
		cout << "Unknown mode for MOV: '" << mode << "'\n -> Instruction discarded" << endl;
}


void executePUSH( uint32_t instruction )
{
	uint16_t mode	= ( instruction & 0x00F00000 ) >> 20;	// mode of the instruction
	uint16_t src 	= ( instruction & 0x000F0000 ) >> 16;	// source register

	mode &= 0x01;	// only look at the last bit

	if( reg[rsp] < UINT16_MAX ) // check for room in VM memory
	{
		if( mode == 0 )	// push src
		{
			memory[++reg[rsp]] = reg[src];
		}
		else if( mode == 1 ) // push immediate value
		{
			int16_t value = ( instruction & 0x0000FFFF );
			memory[++reg[rsp]] = value;
		}
	}
	else // not enough memory left to push
	{
		std::cerr << "Out of memory, exiting program." << endl;
		exit(-1);
	}
}

void executePOP( uint32_t instruction ) // TODO registers
{
	if( reg[rsp] > 0 )
	{
		uint16_t mode	= ( instruction & 0x00F00000 ) >> 20;	// mode of the instruction
		uint16_t dest 	= ( instruction & 0x000F0000 ) >> 16;	// dest register
		if( mode == 0 ) // if mode is at 0, save top value to a dest reg, otherwise just discard it
		{
			reg[dest] = memory[reg[rsp]]; // mov top of stack in destination register
		}
		reg[rsp] -= 1; // decrease rsp
	}
	else 
	{
		std::cerr << "Stack is empty, cannot POP, exiting program." << endl;
		exit(-1);
	}
}
