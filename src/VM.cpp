#include <iostream>
#include <string>
#include <time.h> // used for seeding PRNG with entropy
#include <chrono> // used for cross platform sleep call
#include <thread>
#include <vector>
#include <bitset> // used for binary display of number

#include "misc.h"
#include "VM.h"
#include "parser.h"

using std::string;
using std::cout;
using std::endl;
using std::cin;
using std::flush;

// take the intruction code from the instruction
OP getInstruction( const uint32_t& instruction )
{
    // convert value as an OP 
    OP op = OP( (instruction >> 28) );
    return op;  
}



// take a string and return if possible the correct 16 bits number
uint16_t parseInputValue( string value )
{
    if( parser::matchBinValue( value ) )
    {
        value = value.substr( 2, value.length() - 1 ); // remove the base before number eg : 0b0101 -> 0101
        if( value.length() > 16 )
        {
            Error( "Value '" + value + "' is too big to be encoded" );
            return false;
        }
        long int i = std::stol( value.c_str(), nullptr, 2);
        return static_cast<uint16_t>( i );
    }
    else if( parser::matchHexaValue( value ) )
    {
        value = value.substr( 2, value.length() - 1 ); // remove the base before number eg : 0x0FA2 -> 0FA2
        if( value.length() > 4 )
        {
            Error( "Value '" + value + "' is too big to be encoded" );
            return false;
        }
        long int i = std::stol( value.c_str(), nullptr, 16);
        return static_cast<uint16_t>( i );
    }
    else if( parser::matchDecimalValue( value) )
    {
        int32_t i = atoi( value.c_str() );
        if( i > 65536 or i < -32768 )
        {
            Error( "Value '" + value + "' is too big to be encoded" );
            return false;
        }
        return static_cast<uint16_t>( i );
    }
    Error( "Expected a value" );
    return 0;
}



//  +--------------------------+
//  |    VM basic functions    |
//  +--------------------------+

// initialize registers to 0, initialize PRNG generator with entropy
void VM::initialize( void )
{
    for( int i = 0; i < R_COUNT; i++ )
    {
        reg[i] = 0;
    }
    // push will increment reg[sp] before assignement
    reg[sp] = RESERVED_SPACE; // save space for flags
    reg[ip] = 0;

    program.clear();     // clear current program

    srand(time(NULL));
    rnd_seed = rand();   // seed the xorshift PRNG
}

// load instructions in program vector from another vector (passed by the compiler)
void VM::load( const std::vector<uint32_t>& instructionArray )
{
    program = instructionArray; // should copy every element from the vector, works differently than standard array pointer
}

// execute the program // TODO use ip register instead of a for loop
void VM::start( void )
{
    while( processInstruction( program[reg[ip]] ))
    { 
        // cout << std::hex << std::uppercase <<  program[reg[ip]] <<  std::dec <<endl;
    } 
}

// display the stack values
void VM::dispMemoryStack( bool showReserved ) const
{
    cout << "\n┌────────────────────┐\n│ -- Memory Stack    │" << endl;

    for( int32_t i = reg[sp]; i >= RESERVED_SPACE; i-- )
    {
        int16_t value = static_cast<int16_t>(memory[i]);
        unsigned s = 11 - std::to_string( value ).length(); 

        cout << "│ " << i << "\t";

        for( unsigned j=0; j<s; j++ ) // align numbers on the right
        {
            cout << " ";
        }

        cout << value << "  │" << endl;
    }
    cout << "│ -- Reserved Memory │" << endl;
    if( showReserved and RESERVED_SPACE > 0) // only display reserved memory if there actually is reserved memory
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
void VM::checkForSegfault( const uint16_t& address ) const
{
    if( address < RESERVED_SPACE )
    {
        Error( "Segmentation fault, cannot access to reserved memory addresses");
    }
}

// generate a 16bit random number
uint16_t VM::xorshift16( void )
{
    /* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
    uint16_t x = rnd_seed;
    x ^= x << 7;
    x ^= x >> 9;
    x ^= x << 8;
    rnd_seed = x;
    return x;
}

// generate a 16bit random number
// uint16_t VM::xorshift16( void )
// {   /* Mixed algorithm from Marsaglia xorshift algorithm and other LFSR */
//  uint16_t x = rnd_seed;
//  x ^= x >> 7;
//  x ^= x << 9;
//  x ^= x >> 13;
//  rnd_seed = x + 1;
//  return x;
// }




//  +-----------------------------------+
//  |    OP Interpretation Functions    |
//  +-----------------------------------+

// Macro Function : ADD, SUB. CMP, COPY, MUL, DIV and MOD
// theses intrcutions are fatorized because they function strictly the same and are encoded identically by the assembler.
// add a value (from a register or immediate) to a destination register
// modify flags ZRO, NEG and POS
void VM::executeAddBasedOP( const uint32_t& instruction, OP op )
{
    // 4 bits for sign and offset : 1bit of sign, 3bits of offset value, offset therefore range from -7 to 7
    uint16_t  mode       = ( instruction & 0x0F000000 ) >> 24;   // mode of the instruction
    uint16_t  dest       = ( instruction & 0x00F00000 ) >> 20;   // destination register
    bool     dest_sign  = ( instruction & 0x00080000 ) >> 19;   // destination offset sign
    uint16_t  dest_off   = ( instruction & 0x00070000 ) >> 16;   // destination offset
    uint16_t  src        = ( instruction & 0x000000F0 ) >>  4;   // source register
    bool     src_sign   = ( instruction & 0x00000008 ) >>  0;   // source offset sign
    uint16_t  src_off    = ( instruction & 0x00000007 ) >>  0;   // source offset
    uint16_t src_value  = ( instruction & 0x0000FFFF );         // immediate value or address
    uint16_t dest_value = ( instruction & 0x00FFFF00 ) >>  8;   // immediate address

    // src_value will be used as the variable containing the source value.
    
    uint16_t l_mode = mode >> 2;
    uint16_t r_mode = mode & 0b0011;

    // multiple dereferencement not possible eg: add 4(rsp), 5(rsp)
    // like a real assembly, but there are no limitations to enfore this other than a if statement in Assembler.cpp
    
    uint16_t* dest_p = nullptr ;
    uint16_t* src_p  = nullptr ;    // if still null after the switch, use the imediate value instead
    uint16_t address = 0;       // used in case of dereferencement

    switch( l_mode ) // prepare the source
    {
        case 0:     // immediate value 
                    // do nothing : if src_p is null after switch statement, use l_value
            break;  
        case 1:     // immediate address 
            address = src_value;
            src_p = &memory[ address ];
            checkForSegfault( address ); break; 

        case 2:     // register
            src_p = &reg[src]; break;

        case 3:     // dereferenced register
            address = reg[src] + coef(src_sign) * src_off;  
            src_p = &memory[ address ];
            checkForSegfault( address ); break;

        default:
            Error("Unexpected value in instruction");
    }
    switch( r_mode ) // prepare the destination
    {
        case 0:     // immediate value
            Error("Cannot use immediate value as a destination"); break;

        case 1:     // immediate address
            address = dest_value;
            dest_p = &memory[ address ];
            checkForSegfault( address ); break; 

        case 2:     // register
            dest_p = &reg[dest]; break;

        case 3:     // dereferenced register
            address = reg[dest] + coef(dest_sign) * dest_off;   
            dest_p = &memory[ address ];
            checkForSegfault( address ); break;

        default:
            Error("Cannot use immediate value as a destination"); break;

    }

    if( src_p  != nullptr )     // not an immediate value 
        src_value =  *src_p;    // -> put the actual source value inside src_value
                                // this avoid different case.

    // execute different operator based on the instruction

    switch( op )
    {
        case ADD :
            updateAddOverflow( *dest_p, src_value );
            *dest_p += src_value;
            updateFlags( *dest_p ); break;

        case SUB:
            updateSubOverflow( *dest_p, src_value );
            *dest_p -= src_value;
            updateFlags( *dest_p ); break;

        case COPY:
            *dest_p = src_value;
            updateFlags( *dest_p ); break;

        case CMP: // same as sub except dest operand is unchanged 
            updateSubOverflow( *dest_p, src_value );
            updateCmpFlags( *dest_p, src_value ); // replace normal updateFlags, because here destination is unchanged.
            break;

        case MUL:
            updateMulOverflow( *dest_p, src_value );
            *dest_p *= src_value;
            updateFlags( *dest_p ); break;

        case DIV:
            *dest_p /= src_value;
            updateFlags( *dest_p ); break;

        case MOD:
            *dest_p %= src_value;
            updateFlags( *dest_p ); break;

        default:
            Error("Unexpected OP code");
    }
}

// push a value ( either immediate or from a register ) to the top of the stack
// push and pop are factorized inside the same opcode to make room for DISP instruction
void VM::executePUSH( const uint32_t& instruction )
{
    uint16_t mode   = ( instruction & 0x0F000000 ) >> 24;   // mode of the instruction
    uint16_t src    = ( instruction & 0x0000F000 ) >> 12;   // source register

    if( reg[sp] < UINT16_MAX ) // check for room in VM memory
    {
        if( mode == 0 ) // push source register
        {
            memory[++reg[sp]] = reg[src];
        }
        else if( mode == 1 ) // push immediate value
        {
            int16_t value = ( instruction & 0x0000FFFF );
            memory[++reg[sp]] = static_cast<uint16_t>( value );
        }
    }
    else // not enough memory left to push
        Error("Out of memory");
}

// take the top value, and decrement rsp, while placing ( or discarding ) the value in a register
void VM::executePOP( const uint32_t& instruction ) 
{
    if( reg[sp] >= RESERVED_SPACE ) // check if there is something on the stack
    {
        uint16_t mode   = ( instruction & 0x0F000000 ) >> 24;   // mode of the instruction
        uint16_t dest   = ( instruction & 0x00F00000 ) >> 20;   // dest register
        if( mode == 0 ) // if mode is at 0, save top value to a dest reg, otherwise just discard it
        {
            reg[dest] = memory[reg[sp]]; // mov top of stack in destination register
        }
        reg[sp] -= 1; // decrease rsp
    }
    else 
        Error("Stack is empty");
}

// Either act as a cout or a cin, either with a value or a string 
// TODO : REFACTOR according to assembler.cpp
void VM::executePROMPT( const uint32_t& instruction ) 
{
    uint8_t  select  = ( instruction & 0x0F000000 ) >> 24;  // 1: disp | 2: input 
    uint8_t  l_mode  = ( instruction & 0x00F00000 ) >> 20;  // 0: value | 1: address | 2: register | 3: deref register  
    uint8_t  r_mode  = ( instruction & 0x000F0000 ) >> 16;  // 0: char | 1: int | 2: mem ( unsigned ) | 3:hex  | 4: bin | 5: str
    uint8_t  src_reg = ( instruction & 0x000000F0 ) >>  4;  // source register ( int16, or char ) or register containing address of string
    bool     src_sign   = ( instruction & 0x00000008 ) >>  0;   // source offset sign
    uint8_t  src_off    = ( instruction & 0x00000007 ) >>  0;   // source offset
    uint16_t src_value = ( instruction & 0x0000FFFF );      // immediate value or address   

    // src_value will be used as the variable containing the source value.

                                    // after switch case, copy the value pointed by src_p to src_value
    uint16_t* src_p  = nullptr ;    // if still null after the switch, use the imediate value instead
    uint16_t address = 0;           // used in case of dereferencement

    if( select == 1 ) // disp  
    {
        switch( l_mode )
        {
            case 0: // value
                break;

            case 1: // address
                address = src_value;
                src_p = &memory[ address ];
                checkForSegfault( address ); 
                break; 

            case 2: // register
                src_p = &reg[src_reg]; 
                break;

            case 3: // dereferenced register
                address = reg[src_reg] + coef(src_sign) * src_off;  
                src_p = &memory[ address ];
                checkForSegfault( address ); 
                break;
                
            default:
                Error("Unexpected value in instruction"); 
                break;
        }
        if( src_p  != nullptr )     // not an immediate value 
            src_value =  *src_p;    // -> put the actual source value inside src_value

        switch( r_mode )
        {
            case 0: // char
                cout << static_cast<char>( src_value ); 
                break;
            case 1: // int
                cout << static_cast<int16_t>( src_value );
                break;
            case 2: // mem
                cout << src_value;
                break;
            case 3: // hex
                cout << std::hex << std::uppercase << src_value << std::dec;
                break; 
            case 4: // bin
            {
                std::bitset<16> binNbr( src_value );
                cout << binNbr ;
                break;
            }
            case 5: // str
            {
                if( l_mode != 1 and l_mode != 3 )
                    Error("Runtime error which should be compile-time error: Cannot only display string on addresses");
                
                string mess = "";
                uint16_t cursor = address; // initialize at start of string
                while( memory[ cursor ] != 0 )
                {
                    mess += static_cast<char>( memory[cursor] );
                    if( cursor < UINT16_MAX ) // avoid overflow
                        cursor += 1;
                }
                cout << mess ;
                break;
            }
            default:
                Error("Unexpected value in instruction");
        }
    }
    else if( select == 2 ) // input
    {
        switch( l_mode )
        {
            case 0: // value
                Error("(Assembler Error) Cannot use immediate value with input instruction"); 
                break;

            case 1: // address
                address = src_value;
                src_p = &memory[ address ];
                checkForSegfault( address ); 
                break; 

            case 2: // register
                src_p = &reg[src_reg]; 
                break;

            case 3: // dereferenced register
                address = reg[src_reg] + coef(src_sign) * src_off;  
                src_p = &memory[ address ];
                checkForSegfault( address ); 
                break;
                
            default:
                Error("Unexpected value in instruction"); 
                break;
        }
        switch( r_mode )
        {
            case 0: // char
            {
                char input_value;
                cin >> input_value;
                *src_p = static_cast<uint16_t>( input_value );
                break;
            }
            case 1: // int
            {
                int16_t input_value;
                cin >> input_value;
                *src_p = static_cast<uint16_t>( input_value );
                break;
            }
            case 2: // mem
            {
                uint16_t input_value;
                cin >> input_value;
                *src_p = input_value;
                break;
            }
            case 3: // hex
            {
                uint16_t input_value;
                string input_string;
                cin >> input_string;
                if( parser::matchHexaValue( input_string.c_str() ))
                    input_value = parseInputValue( input_string );
                else
                    input_value = 0;
                *src_p = input_value ;
                break; 
            }
            case 4: // bin
            {
                uint16_t input_value;
                string input_string;
                cin >> input_string;
                if( parser::matchBinValue( input_string.c_str() ))
                    input_value = parseInputValue( input_string );
                else
                    input_value = 0;
                *src_p = input_value;
                break; 
            }
            case 5: // str
            {
                string input_string;
                std::getline(cin, input_string);
                if( address + input_string.length() >= UINT16_MAX )
                    Error("Not enough memory to store input string");
                else
                {
                    for( unsigned i=0; i<input_string.length(); i++)
                    {
                        memory[address + i] = static_cast<uint16_t>( input_string[i] );
                    }
                    memory[address + input_string.length()] = static_cast<uint16_t>( 0 );
                }
                break;
            }
            default:
                Error("Unexpected value in instruction");
        }
    }
}

// take a register and set its value to a (pseudo) random one
void VM::executeRAND( const uint32_t& instruction )
{
    uint16_t mode       = ( instruction & 0x0F000000 ) >> 24;   // chose range 
    uint16_t dest       = ( instruction & 0x00F00000 ) >> 20;   // destination register 
    uint16_t max_value  = ( instruction & 0x0000FFFF );         // choose max value

    if( mode == 0 )       // no max value
        reg[dest] = xorshift16();
    else if( mode == 1 )  // -max_value < x < max_value
        reg[dest] = xorshift16() % max_value;
    else if( mode == 2 )  // 0 <= x < max_value     
        reg[dest] = (xorshift16() % max_value / 2 ) + ( max_value / 2 );
}

// TODO REFACTOR akin to AddBasedInstr
// Binary Operator : Either act as AND, OR, NOT or XOR, used to compress 4 instructions in 1 opcode
void VM::executeBinBasedOP( const uint32_t& instruction ) 
{
    uint16_t mode   = ( instruction & 0x0F000000 ) >> 24;   // mode of the instruction either AND, OR, NOT or XOR
    uint16_t l_mode = ( instruction & 0x00F00000 ) >> 20;   // choose between immediate value or source register
    uint16_t dest   = ( instruction & 0x000F0000 ) >> 16;   // destination register
    uint16_t src    = ( instruction & 0x0000F000 ) >> 12;   // source register
    uint16_t value  = ( instruction & 0x0000FFFF );         // immediate value

    uint16_t* dest_p = &reg[dest];

    if( l_mode == 2 ) // source register, not an immediate value
    {
        value = reg[src];
    }

    switch( mode ) // none of those operations can overflow
    {
        case 1: // AND
            *dest_p &= value; 
            updateFlags( *dest_p ); 
            break;

        case 2: // OR
            *dest_p |= value;
            updateFlags( *dest_p ); 
            break;

        case 3: // NOT
            *dest_p = ~(value);
            updateFlags( *dest_p ); 
            break;

        case 4: // XOR
            *dest_p ^= (value);
            updateFlags( *dest_p ); 
            break;

        default:
            Error("Unexpected value in instruction");
            break;
    }
}

// sleep for a certain amount of time before going to the next instruction
void VM::executeWAIT( const uint32_t& instruction )
{
    uint16_t mode       = ( instruction & 0x0F000000 ) >> 24;   // chose range 
    uint16_t value      = ( instruction & 0x0000FFFF );         // choose max value
    // uncomment below to display precisely the time slept.
    using namespace std::chrono_literals;
    // auto start = std::chrono::high_resolution_clock::now();
    if( mode == 0 )
        std::this_thread::sleep_for(std::chrono::milliseconds( value ));
    if( mode == 1 )
        std::this_thread::sleep_for(std::chrono::seconds( value ));
    // auto end = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double, std::milli> elapsed = end-start;
    // cout << "Waited " << elapsed.count() << " ms\n";
}

// contains jump, conditionnal jump, call and ret
void VM::executeJUMP( const uint32_t& instruction )
{
    uint16_t mode   = ( instruction & 0x0F000000 ) >> 24;   // select operator( unconditionnal jump, call, ret or conditionnal jump )
    bool      sign  = ( instruction & 0x00F00000 ) >> 20;   // choose the sign of the condition ( 0: if | 1: ifnot )
    uint8_t cpuFlag = ( instruction & 0x000F0000 ) >> 16;   // cpu flag used as condition
    uint16_t value  = ( instruction & 0x0000FFFF );         // destination address 

    if( mode == 0 ) // unconditionnal jump
    {
       reg[ip] = value; 
    }
    else if( mode == 1 ) // call
    {
        executePUSH(0x90009000); // push ip
        reg[ip] = value;         // ip already points to the next instruction
    }
    else if( mode == 2 ) // ret
    {
        executePOP(0xA0900000);
    }
    else if( mode == 3 ) // conditionnal jump
    {
        if( sign == false ) // ifnot
        {
            if( not flags[ cpuFlag ] )
                reg[ip] = value;
        }
        if( sign == true )
        {
            if( flags[ cpuFlag ] )
                reg[ip] = value;
        }
    }
    else if( mode == 4 ) // conditionnal call
    {
        if( sign == false ) // ifnot
        {
            if( not flags[ cpuFlag ] )
            {
                executePUSH(0x90009000); // push ip
                reg[ip] = value;         // ip already points to the next instruction
            }
        }
        if( sign == true )
        {
            if( flags[ cpuFlag ] )
            {
                executePUSH(0x90009000); // push ip
                reg[ip] = value;         // ip already points to the next instruction
            }
        }
    }

}
 


// redirect to the correct function depending on the instruction code contained in the first 8 bits
bool VM::processInstruction( const uint32_t& instruction )
{
    // get the current opcode
    OP op = getInstruction( instruction );
    
    switch( op )
    {
        case PUSH:
            reg[ip]++; 
            executePUSH( instruction );
            break;
        case POP:
            reg[ip]++;
            executePOP( instruction ); 
            break;
        case BIN:
            reg[ip]++;
            executeBinBasedOP( instruction ); 
            break;
        case RAND:
            reg[ip]++;
            executeRAND( instruction ); 
            break;
        case WAIT:
            reg[ip]++;
            executeWAIT( instruction ); 
            break;
        case JUMP:
            reg[ip]++;
            executeJUMP( instruction );
            break;
        case PROMPT:
            reg[ip]++;
            executePROMPT( instruction ); 
            break;
        case HALT:
            return false; 
            break;
        default :
            reg[ip]++;
            executeAddBasedOP( instruction, op ); 
            break;
    }
    return true;
}


//  +------------------------------+
//  |    Flags Update Functions    |
//  +------------------------------+

void VM::dispFlagsRegister( void ) const
{
    cout << "┌─────┬─────┬─────┬─────┬─────┬─────┐" << endl;
    cout << "│ EQU │ ZRO │ POS │ NEG │ OVF │ ODD │" << endl;
    for( int i=0; i<F_COUNT; i++ )
    {
        cout << "│  " << flags[i] <<  "  ";
    }
    cout << "│ \n" << "└─────┴─────┴─────┴─────┴─────┴─────┘" << endl;
}

// update every flag except Overflow since it needs operands value.  ZRO and EQU flags are identical in practice.
void VM::updateFlags( const uint16_t& value )
{
    int16_t val = static_cast<int16_t>( value );

    flags[NEG] = 0;
    flags[POS] = 0;
    flags[ZRO] = 0;
    flags[EQU] = 0;

    if( val == 0 )
    {
        flags[ZRO] = 1;
        flags[EQU] = 1;
    }
    else if( val < 0 )
        flags[NEG] = 1;
    else if( val > 0 )
        flags[POS] = 1;

    flags[ODD] = val % 2;
}

// update flags for cmp because destination is unchanged, ( OVF flag is unchanged )
void VM::updateCmpFlags( const uint16_t& dest, const uint16_t& src )
{
    int16_t dest_val = static_cast<int16_t>( dest );
    int16_t src_val  = static_cast<int16_t>( src  );
    int16_t result = dest_val - src_val;

    flags[NEG] = 0;
    flags[POS] = 0;
    flags[ZRO] = 0;
    flags[EQU] = 0;

    if( result == 0 )
    {
        flags[ZRO] = 1;
        flags[EQU] = 1;
    }
    else if( result < 0 )
        flags[NEG] = 1;
    else if( result > 0 )
        flags[POS] = 1;

    flags[ODD] = result % 2;
}

// check if you can get back to the operand from the result, if not, result has overflowed
void VM::updateAddOverflow( const uint16_t& dest, const uint16_t& src )
{
    int16_t dest_val = static_cast<int16_t>( dest );
    int16_t src_val  = static_cast<int16_t>( src  );
    int16_t result = dest_val + src_val;
    if( dest_val != result - src_val )  flags[OVF] = 1; 
    else flags[OVF] = 0;
}

// check if you can get back to the operand from the result, if not, result has overflowed
void VM::updateSubOverflow( const uint16_t& dest, const uint16_t& src )
{
    int16_t dest_val = static_cast<int16_t>( dest );
    int16_t src_val  = static_cast<int16_t>( src  );
    int16_t result = dest_val - src_val;
    if( dest_val != result + src_val )  flags[OVF] = 1; 
    else flags[OVF] = 0;
}

// check if you can get back to the operand from the result, if not, result has overflowed
void VM::updateMulOverflow( const uint16_t& dest, const uint16_t& src )
{
    int16_t dest_val = static_cast<int16_t>( dest );
    int16_t src_val  = static_cast<int16_t>( src  );
    int16_t result = dest_val * src_val;
    if( dest_val != result / src_val )  flags[OVF] = 1; 
    else flags[OVF] = 0;
}






