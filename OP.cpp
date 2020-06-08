#include <iostream>
#include "VM.cpp"
#include <string>

using std::cout;
using std::endl;
using std::string;

void Error( string message )
{
	std::cerr << "/!\\ Error : " << message << ". \nTerminating the program." << endl;
	exit( -1 );
}

// transform a bool for [0;1] to [1;-1]
short coef( bool value )
{
	return -(value*2 -1);
}

// update every flag except Overflow since it needs operands value. Special case for EQU and ZRO see below
// sub_or_cmp : false for sub (ZRO flag) and true for cmp (EQU flag)
void updateFlags( int16_t value, bool cmp = false)
{
	if( value == 0 )
	{
		if( cmp == false )
			flags[ZRO] = 1;
		else 
			flags[EQU] = 1;
	}
	else if( value < 0 )
	{
		flags[NEG] = 1;
		flags[POS] = 0;
	}
	else
	{
		flags[NEG] = 0;
		flags[POS] = 1;
	}
}

// check if you can get back to the operand from the result, if not, result has be troncated
void updateAddOverflow(  int16_t dest, int16_t src )
{
	int16_t result = dest + src;
	if( dest != result - src )	flags[OVF] = 1; 
	else flags[OVF] = 0;
}

// check if you can get back to the operand from the result, if not, result has be troncated
int16_t updateSubOverflow(  int16_t dest, int16_t src )
{
	int16_t result = dest - src;
	if( dest != result + src ) cout << "Overflow destected" << endl;
	return result;
}

void checkForSegfault( int16_t address )
{
	if( address < RESERVED_SPACE )
	{
		Error( "Segmentation fault, cannot access to reserved memory addresses");
		exit( -1 );
	}
}


// add a value (from a register or immediate) to a destination register
// modify flags ZRO, NEG and POS
void executeADD( uint32_t instruction )
{
	// 4 bits for sign and mod : 1bit of sign, 3bits of mode value
	short mode		= ( instruction & 0x07000000 ) >> 24;	// mode of the instruction
	bool  sign		= ( instruction & 0x08000000 ) >> 24;	// sign of the offset
	short dest		= ( instruction & 0x00F00000 ) >> 20;	// destination register
	short dest_off	= ( instruction & 0x000F0000 ) >> 16;	// destination offset
	short src		= ( instruction & 0x0000F000 ) >> 12;	// source register
	short src_off	= ( instruction & 0x00000F00 ) >>  8;	// source offset
	int16_t value	= ( instruction & 0x0000FFFF );			// immediate value

	// multiple dereferencement not possible eg: add 4(rsp), 5(rsp)
	// like a real assembly
	
	uint16_t* dest_p = NULL;
	uint16_t* src_p  = NULL; // if still null after the switch, use the imediate value instead
	uint16_t address = 0;

	switch( mode ) // only check for segfault when dereferencing 
	{			// example with a few values and registers
		case 0:	// add  3, rsp
			dest_p = &reg[dest]; break;
			
		case 1:	// add	3, (r7)
			address = reg[dest];
			dest_p	= &memory[ address ];
			checkForSegfault( address ); break;

		case 2:	// add	3, 4(r7)
			address = reg[dest] + coef(sign) * dest_off;
			dest_p	= &memory[ address ] ; 
			checkForSegfault( address ); break;
			
		case 3: // add	r1, r7
			dest_p = &reg[dest];
			src_p  = &reg[src]; break;

		case 4:	// add	r1, (r7)
			address = reg[dest];
			dest_p	= &memory[ address ];
			src_p	= &reg[src];
			checkForSegfault( address ); break;

		case 5:	// add	r1, 4(r7)
			address = reg[dest] + coef(sign) * dest_off;
			dest_p	= &memory[ address ] ; 
			src_p	= &reg[src]; 
			checkForSegfault( address ); break;

		case 6: // add	(r1), r7
			dest_p	= &reg[dest];
			address = reg[src];
			src_p	= &memory[ address ]; 
			checkForSegfault( address ); break;

		case 7:	// add	4(r1), r7
			dest_p	= &reg[dest]; 
			address = reg[src] + coef(sign) * src_off;
			src_p	= &memory[ address ]; 
			checkForSegfault( address ); break;
	}
	if( mode > 7 ) 
		Error( "Unknown mode for instruction 'add'" );

	if( src_p != NULL )
	{
		updateAddOverflow( *dest_p, *src_p );
		*dest_p += *src_p;
	}
	else if( src_p == NULL )
	{
		updateAddOverflow( *dest_p, value );
		*dest_p += value;
	}
	updateFlags( *dest_p );
}


// push a value ( either immediate or from a register ) to the top of the stack
void executePUSH( uint32_t instruction )
{
	uint16_t mode	= ( instruction & 0x0F000000 ) >> 24;	// mode of the instruction
	uint16_t src 	= ( instruction & 0x0000F000 ) >> 12;	// source register

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
		Error( "Out of memory.");
}

// take the top value, and decrement rsp, while placing ( or discarding ) the value in a register
void executePOP( uint32_t instruction ) 
{
	if( reg[rsp] >= RESERVED_SPACE )
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
		std::cerr << "Stack is empty, cannot execute instruction 'pop', exiting program." << endl;
		exit(-1);
	}
}

// take the intruction code from the instruction
OP getInstruction( uint32_t instruction )
{
	// only keep the 4 left hand bits
	instruction = ( instruction & 0xF0000000 ) >> 28;
	
	// convert value as an OP 
	OP op = OP( instruction );
	return op;	
}

// redirect to the correct function depending on the instruction code contained in the first 8 bits
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
			// executeSUB( instruction );
			break;
		case COPY :
			// executeMOV( instruction );
			break;
		case PUSH:
			executePUSH( instruction );
			break;
		case POP :
			executePOP( instruction );
			break;
	}
}
