#include <iostream>
#include <string>

using std::cout;
using std::endl;

// array of word addresses  (16 bits offset)
// ~ amount to 130 ko of memory, no need for dynamic allocation
uint16_t memory[UINT16_MAX];	

// amount to ~ 130 ko of memory, no need for dynamic allocation
// allow for ~ 32700 instructions per program.
uint32_t program[UINT16_MAX / 2];

// amount of reserved space in memory ( for flags for example )
const uint16_t RESERVED_SPACE = 2;

// registers order inside the reg array
// cannot be higher than 16, as we use 4bits to address registers in instructions
enum 
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

// array containing the registers access like reg[r2]
uint16_t reg[ R_COUNT ];


// initialize registers to 0
void initializeRegisters( void )
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
void dispMemoryStack( bool showReserved = false )
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



