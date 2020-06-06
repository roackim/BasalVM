#include <iostream>
#include <vector>

using std::cout;
using std::endl;

// array of word addresses  (16 bits offset)
// ~ amount to 130 ko of memory, no need for dynamic allocation
uint16_t memory[UINT16_MAX];	
// amount of reserved space in memory ( for flags for example )
const uint16_t RESERVED_SPACE = 4;

// registers order inside the reg array
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
	rsp,
	R_COUNT
};

// array containing the registers access like reg[r2]
uint16_t reg[ R_COUNT ];
// should not be modifiable with ADD, SUB and MOV
uint16_t rip; 


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
	cout << "┌────────────────────\n│ -- Memory Stack" << endl;
	for( int i = reg[rsp]; i >= RESERVED_SPACE; i-- )
	{
		cout << "│ " << i << ":\t  " << static_cast<int16_t>(memory[i]) << endl;
	}

	cout << "│ -- Reserved Memory" << endl;
	if( showReserved )
	{
		for( int i = RESERVED_SPACE-1; i >= 0; i-- )
		{
			cout << "│ " << i << ":\t  " << static_cast<int16_t>(memory[i]) << endl;
		}
	}
	cout << "└────────────────────" << endl;

}


// enum for op codes ! subject to change !
enum OP
{
	BR = 0,		// 0. branch 
	ADD,    	// 1. add  
	SUB,		// 2. substract
	MOV,		// 3. copy
	PUSH,		// 4. push on the stack
	POP,		// 5. pop the stack
	CMP,		// 6. compare two values
	AND,    	// . bitwise AND 
	OR,			// . bitwise OR
	NOT,		// . bitwise NOT
	JUMP,		// . jump 
	LEA,    	// . load effective address 
	TRAP    	// . execute trap 
};


enum Flag
{
	ZRO = 1,
	EQU = 2,
	POS = 3,
	NEG = 4
};


