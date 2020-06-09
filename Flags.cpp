
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

bool flags[F_COUNT];

void dispFlagsRegister( void )
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
void updateFlags( int16_t value, bool cmp = false)
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
void updateAddOverflow(  int16_t dest, int16_t src )
{
	int16_t result = dest + src;
	if( dest != result - src )	flags[OVF] = 1; 
	else flags[OVF] = 0;
}


// check if you can get back to the operand from the result, if not, result has be troncated
void updateSubOverflow(  int16_t dest, int16_t src )
{
	int16_t result = dest - src;
	if( dest != result + src )	flags[OVF] = 1; 
	else flags[OVF] = 0;
}

// check if you can get back to the operand from the result, if not, result has be troncated
void updateMulOverflow(  int16_t dest, int16_t src )
{
	int16_t result = dest * src;
	if( dest != result / src )	flags[OVF] = 1; 
	else flags[OVF] = 0;
}


