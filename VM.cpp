#include <iostream>
#include <string>

using std::cout;
using std::endl;

// array of word addresses  (16 bits offset)
// ~ amount to 130 ko of memory, no need for dynamic allocation
uint16_t memory[UINT16_MAX];	
// amount of reserved space in memory ( for flags for example )
const uint16_t RESERVED_SPACE = 2;

// registers order inside the reg array
// cannot be higher than 16, as we use 4bits to address registers in instructions
enum 
{
	r0 = 0,
	r1,
	r2,
	r3,
	r4,
	r5,
	r6,
	r7,
	rsp,		// 8.
	rip,		// 9.
	R_COUNT
};

// array containing the registers access like reg[r2]
uint16_t reg[ R_COUNT ];


// initialize registers to 0
void initializeRegisters( void )
{
	for( int i = 0; i < R_COUNT; i++ )
	{
		reg[i] = 0;
	}
	// push will increment reg[rsp] before assignement
	reg[rsp] = RESERVED_SPACE-1; // save space for flags
}

// display the stack values
void dispMemoryStack( bool showReserved = false )
{
	cout << "┌────────────────────┐\n│ -- Memory Stack    │" << endl;

	for( int i = reg[rsp]; i >= RESERVED_SPACE; i-- )
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

// encoded with 4 bit : maximum 16 different opcode
// enum for op codes ! subject to change !
enum OP												
{													
	BR = 0,		
	ADD,    	
	SUB,		
	CMP,
	COPY,		
	PUSH,		
	POP,		
	MUL,
	DIV,
	AND,    	
	OR,			
	NOT,		
	JUMP,		
	TRAP    	
};


enum Flag
{
	EQU = 0,
	ZRO,
	POS,
	NEG,
	OVF,
	F_COUNT
};

bool flags[F_COUNT];

void dispFlagsRegister( void )
{
	cout << "┌─────┬─────┬─────┬─────┬─────┐" << endl;
	cout << "│ EQU │ ZRO │ POS │ NEG │ OVF │" << endl;
	for( int i=0; i<F_COUNT; i++ )
	{
		cout << "│  " << flags[i] <<  "  ";
	}
	cout << "│ \n" << "└─────┴─────┴─────┴─────┴─────┘" << endl;
;
}
