#pragma once

//	+------------------------------------+
//	|    Enums needed for code clarity   |
//	+------------------------------------+

// Registers Enum
enum Reg
{
	ax = 0,
	bx,
	cx,
	dx,
	ex,
	fx,
	si,	
	di,
	sp,		// 8.
	ip,		// 9.
	R_COUNT
};

// Opcode Enum
enum OP			// TODO RAND and WAIT	
{													
	IO = 0,		//	.0			// used to communicate with outside the VM
	ADD,		// 	.1
	SUB,		// 	.2
	CMP,		// 	.3
	COPY,		// 	.4
	PUSH,		// 	.5			
	POP,		// 	.6	
	MUL,		// 	.7
	DIV,		// 	.8
	MOD,		// 	.9
	AND,		// 	.10 
	OR,			// 	.11
	NOT,		// 	.12
	XOR,		// 	.13	
	JUMP,		// 	.14			// contains CALL and RET maybe HALT too ?
	PROMPT		// 	.15			// act as both a cout and a cin depending on the mode
};

// Flags Enum
enum Flag
{
	EQU = 0,
	ZRO,
	POS,
	NEG,
	OVF,
	ODD,
	F_COUNT
};

//	+---------------------------+
//	|    Virtual Machine Class  |
//	+---------------------------+

class VM
{

//	+---------------------+
//	|    VM attributes    |
//	+---------------------+

private:
	// array of word addresses  (16 bits offset)
	// ~ amount to 130 ko of memory, no need for dynamic allocation
	uint16_t memory[UINT16_MAX];	
	// allow for ~ 32700 instructions per program.
	uint32_t program[UINT16_MAX / 2];
	// amount of reserved space in memory ( maybe useful for later ? )
	const uint16_t RESERVED_SPACE = 2;
	// array containing the CPU flags, access like flags[ZRO]
	bool flags[ F_COUNT ];
	// used to generate random numbers
	uint16_t rnd_seed;
public:
	// array containing the registers access like reg[r2]
	uint16_t reg[ R_COUNT ];


//	+--------------------------+
//	|    VM basic functions    |
//	+--------------------------+

	// initialize registers to 0, initialize the seed for random numbers
	void initialize( void );

	// display the stack values
	void dispMemoryStack( bool showReserved = false );
	
	// check if the address is RESERVED
	void checkForSegfault( int16_t address );


//	+-----------------------------------+
//	|    OP Interpretation Functions    |
//	+-----------------------------------+

	// add a value (from a register or immediate) to a destination register
	// modify flags ZRO, NEG and POS
	void executeAddBasedOP( uint32_t instruction, OP op );

	// push a value ( either immediate or from a register ) to the top of the stack
	// push and pop are factorized inside the same opcode to make room for DISP instruction
	void executePUSH( uint32_t instruction );

	// take the top value, and decrement rsp, while placing ( or discarding ) the value in a register
	void executePOP( uint32_t instruction );

	// Either act as a cout or a cin, either with a value or a string
	void executePROMPT( uint32_t instruction ); // TODO subject to change input -> sfml

	// redirect to the correct function depending on the instruction code contained in the first 8 bits
	void processInstruction( uint32_t instruction );


//	+------------------------------+
//	|    Flags Update Functions    |
//	+------------------------------+

	void dispFlagsRegister( void );

	// update every flag except Overflow since it needs operands value. Special case for EQU and ZRO see below
	// sub_or_cmp : false for sub (ZRO flag) and true for cmp (EQU flag)
	void updateFlags( int16_t value, bool cmp = false);

	// check if you can get back to the operand from the result, if not, result has be troncated
	void updateAddOverflow(  int16_t dest, int16_t src );

	// check if you can get back to the operand from the result, if not, result has be troncated
	void updateSubOverflow(  int16_t dest, int16_t src );

	// check if you can get back to the operand from the result, if not, result has be troncated
	void updateMulOverflow(  int16_t dest, int16_t src );
};


