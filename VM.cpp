#include <iostream>
#include <string>
#include "misc.hpp"

#include "VM.h"

using std::string;
using std::cout;
using std::endl;
using std::cin;

// take the intruction code from the instruction
OP getInstruction( uint32_t instruction )
{
	// only keep the 4 left hand bits
	instruction = ( instruction & 0xF0000000 ) >> 28;
	
	// convert value as an OP 
	OP op = OP( instruction );
	return op;	
}


//	+--------------------------+
//	|    VM basic functions    |
//	+--------------------------+

// initialize registers to 0
void VM::initialize( void )
{
	for( int i = 0; i < R_COUNT; i++ )
	{
		reg[i] = 0;
	}
	// push will increment reg[sp] before assignement
	reg[sp] = RESERVED_SPACE-1; // save space for flags
	reg[ip] = 0;
}

// display the stack values
void VM::dispMemoryStack( bool showReserved )
{
	cout << "┌────────────────────┐\n│ -- Memory Stack    │" << endl;

	for( int i = reg[sp]; i >= RESERVED_SPACE; i-- )
	{
		int16_t value = static_cast<int16_t>(memory[i]);
		int s = 11 - std::to_string( value ).length(); 
		cout << "│ " << i << "\t";
		for( int j=0; j<s; j++ ) // align numbers on the right
		{
			cout << " ";
		}
		cout << value << "  │" << endl;
	}

	cout << "│ -- Reserved Memory │" << endl;
	if( showReserved )
	{
			for( int i = RESERVED_SPACE-1; i >= 0; i-- )
			{
			int16_t value = static_cast<int16_t>(memory[i]);
			int s = 11 - std::to_string( value ).length(); 
			cout << "│ " << i << "\t";
			for( int j=0; j<s; j++ ) // align numbers on the right
			{
				cout << " ";
			}
			cout << value << "  │" << endl;
		}
	}
	cout << "└────────────────────┘" << endl;
}

// check if the address is RESERVED
void VM::checkForSegfault( int16_t address )
{
	if( address < RESERVED_SPACE )
	{
		Error( "Segmentation fault, cannot access to reserved memory addresses");
		exit( -1 );
	}
}

//	+-----------------------------------+
//	|    OP Interpretation Functions    |
//	+-----------------------------------+

// add a value (from a register or immediate) to a destination register
// modify flags ZRO, NEG and POS
void VM::executeAddBasedOP( uint32_t instruction, OP op )
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
		value = *src_p;
	
	switch( op )
	{
		case ADD :
			updateAddOverflow( *dest_p, value );
			*dest_p += value;
			updateFlags( *dest_p ); break;

		case SUB:
			updateSubOverflow( *dest_p, value );
			*dest_p -= value;
			updateFlags( *dest_p ); break;

		case COPY:
			*dest_p = value;
			updateFlags( *dest_p ); break;

		case CMP:
			if( static_cast<int16_t>(*dest_p) < 0 and value > 0 ) // avoid overflow
			{
				flags[POS] = 0;
				flags[NEG] = 1;
			}
			else if( static_cast<int16_t>(*dest_p) > 0 and value < 0 )
			{
				flags[POS] = 1;
				flags[NEG] = 0;	
			}
			else // no overflow possible left, substract normally
			{	// process result in a local variable, in order to preserve destination 
				uint16_t result = *dest_p - value;
				updateFlags( result );
			}
			break;

		case MUL:
			updateMulOverflow( *dest_p, value );
			*dest_p *= value;
			updateFlags( *dest_p ); break;

		case DIV:
			*dest_p /= value;
			updateFlags( *dest_p ); break;

		case MOD:
			*dest_p %= value;
			updateFlags( *dest_p ); break;

		case AND:
			*dest_p &= value; 
			updateFlags( *dest_p ); break;

		case OR:
			*dest_p |= value;
			updateFlags( *dest_p ); break;

		case NOT:
			*dest_p = ~(value);
			updateFlags( *dest_p ); break;

		case XOR:
			*dest_p ^= (value);
			updateFlags( *dest_p ); break;

		default:
			Error("Unexpected OP code");

	}
}


// push a value ( either immediate or from a register ) to the top of the stack
// push and pop are factorized inside the same opcode to make room for DISP instruction
void VM::executePUSH( uint32_t instruction )
{
	uint16_t mode	= ( instruction & 0x0F000000 ) >> 24;	// mode of the instruction
	uint16_t src 	= ( instruction & 0x0000F000 ) >> 12;	// source register

	if( reg[sp] < UINT16_MAX ) // check for room in VM memory
	{
		if( mode == 0 )	// push source register
		{
			memory[++reg[sp]] = reg[src];
		}
		else if( mode == 1 ) // push immediate value
		{
			int16_t value = ( instruction & 0x0000FFFF );
			memory[++reg[sp]] = value;
		}
	}
	else // not enough memory left to push
		Error("Out of memory");
}

// take the top value, and decrement rsp, while placing ( or discarding ) the value in a register
void VM::executePOP( uint32_t instruction ) 
{
	if( reg[sp] >= RESERVED_SPACE ) // check if there is something on the stack
	{
		uint16_t mode	= ( instruction & 0x00F00000 ) >> 20;	// mode of the instruction
		uint16_t dest 	= ( instruction & 0x000F0000 ) >> 16;	// dest register
		if( mode == 0 ) // if mode is at 0, save top value to a dest reg, otherwise just discard it
		{
			reg[dest] = memory[reg[sp]]; // mov top of stack in destination register
		}
		reg[sp] -= 1; // decrease rsp
	}
	else 
		Error("Stack is empty");
}

// Either act as a cout or a cin, either with a value or a string // TODO subject to change input -> sfml
void VM::executePROMPT( uint32_t instruction ) 
{
	uint16_t mode	= ( instruction & 0x0F000000 ) >> 24;	// mode of the instruction
	uint16_t src 	= ( instruction & 0x00F00000 ) >> 20;	// source register ( int16, or char ) or register containing address of string
	bool newline	= ( instruction & 0x00010000 ) >> 16;


	switch( mode )
	{
		// DISPLAY MODES
		case 0: // display as INT16 value
			cout << static_cast<int16_t>(reg[src]) ; break;
		case 1: // display as UINT16 value
			cout << reg[src] ; break;
		case 2: // display as char
			cout << static_cast<char>(reg[src]) ; break;

				// in this mode src register is dereferenced, containing the address of the string in memory
		case 3: // display a string : [ char16 ]* [ NULL ], 
		{
			string message = "";
			for( int i=0; i < 65536; i++ ) // max size of string
			{
				if( memory[ reg[src] + i ] == 0 ) // NULL terminated earlier
					break;
				char c = static_cast<char>( memory[ reg[src] + i ] );
				message += c;
			}
			cout << message; break;
		}
		case 4: // in this mode src register is dereferenced, containing the address of the value in memory
			cout << memory[ reg[src]]; break;
			
		case 6: // only print a newline
			cout << endl; break;

		// INPUT MODES
		case 7: // input a value in a register
			int16_t value; cin >> value;
			reg[src] = static_cast<uint16_t>(value); break;

		case 8: // input a string, to be pushed on the stack
			char str[256]; cin.get(str, 256, '\n'); // null terminated by default
			if( reg[sp] + 1 < UINT16_MAX - 256 ) // check if there is enough room
			{
				for( int i=0; i<256; i++)
				{
					memory[ ++reg[sp] ] = static_cast<uint16_t>( str[i] );
					if( str[i] == 0 ) break; // Null Terminated before end of buffer
				}
			}
			break;
	}
	// print a new line if requested
	if( newline ) cout << endl;
}

// redirect to the correct function depending on the instruction code contained in the first 8 bits
void VM::processInstruction( uint32_t instruction )
{
	// get the current opcode
	OP op = getInstruction( instruction );
	
	switch( op )
	{
		case PUSH:
			executePUSH( instruction ); break;
		case POP:
			executePOP( instruction ); break;

		case JUMP:
			//TODO
			break;
		case PROMPT:
			executePROMPT( instruction ); break;
		default :
			executeAddBasedOP( instruction, op ); break;
	}
}


//	+------------------------------+
//	|    Flags Update Functions    |
//	+------------------------------+

void VM::dispFlagsRegister( void )
{
	cout << "┌─────┬─────┬─────┬─────┬─────┬─────┐" << endl;
	cout << "│ EQU │ ZRO │ POS │ NEG │ OVF │ ODD │" << endl;
	for( int i=0; i<F_COUNT; i++ )
	{
		cout << "│  " << flags[i] <<  "  ";
	}
	cout << "│ \n" << "└─────┴─────┴─────┴─────┴─────┴─────┘" << endl;
}

// update every flag except Overflow since it needs operands value. Special case for EQU and ZRO see below
// sub_or_cmp : false for sub (ZRO flag) and true for cmp (EQU flag)
void VM::updateFlags( int16_t value, bool cmp )
{
	flags[NEG] = 0;
	flags[POS] = 0;
	if( value == 0 )
	{
		flags[ZRO] = 1;
		if( cmp == true )
			flags[EQU] = 1;
	}
	else if( value < 0 )
		flags[NEG] = 1;
	else
		flags[POS] = 1;

	flags[ODD] = value % 2;
}

// check if you can get back to the operand from the result, if not, result has be troncated
void VM::updateAddOverflow(  int16_t dest, int16_t src )
{
	int16_t result = dest + src;
	if( dest != result - src )	flags[OVF] = 1; 
	else flags[OVF] = 0;
}

// check if you can get back to the operand from the result, if not, result has be troncated
void VM::updateSubOverflow(  int16_t dest, int16_t src )
{
	int16_t result = dest - src;
	if( dest != result + src )	flags[OVF] = 1; 
	else flags[OVF] = 0;
}

// check if you can get back to the operand from the result, if not, result has be troncated
void VM::updateMulOverflow(  int16_t dest, int16_t src )
{
	int16_t result = dest * src;
	if( dest != result / src )	flags[OVF] = 1; 
	else flags[OVF] = 0;
}






