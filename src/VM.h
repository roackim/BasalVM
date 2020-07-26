#pragma once
#include <vector>
#include <string>

#include "basmDefinition.h"


//  +---------------------------+
//  |    Virtual Machine Class  |
//  +---------------------------+

// take the intruction code from the instruction
OP getInstruction( const uint32_t& instruction );

// take a string and return if possible the correct 16 bits number
uint16_t parseInputValue( std::string value );

class VM
{

//  +---------------------+
//  |    VM attributes    |
//  +---------------------+


public:
    // array containing the registers access like reg[r2], public so you can do : vm.reg[ax] = 238; in main()
    uint16_t reg[ R_COUNT ];

private:
    // array of word addresses  (16 bits offset)
    // ~ amount to 130 ko of memory, no need for dynamic allocation
    uint16_t memory[UINT16_MAX];    
    // no program size limit, dinamic array for less memory impact on average
    std::vector<uint32_t> program;
    // amount of reserved space in memory ( maybe useful for later ? )
    const uint16_t RESERVED_SPACE = 0;
    // array containing the CPU flags, access like flags[ZRO]
    bool flags[ F_COUNT ];
    // used to generate random numbers
    uint16_t rnd_seed;



//    All functions are public
//  +--------------------------+
//  |    VM basic functions    |
//  +--------------------------+

public:
    // initialize registers to 0, initialize the seed for random numbers
    void initialize( void );

    // load instructions in program vector from another vector (passed by the compiler)
    void load( const std::vector<uint32_t>& instructionArray );

    // execute the program
    void start( void );

    // display the stack values
    void dispMemoryStack( bool showReserved = false ) const;

    // display the stack values without a box
    void dispMemoryStackLight( void ) const;


    // display the content of Flags byte, with the corresponding flags eg ZRO, EQU, ODD etc..
    void dispFlagsRegister( void ) const;
    

private:
    // check if the address is RESERVED
    void checkForSegfault( const uint16_t& address ) const;

    // generate a 16bit random number
    uint16_t xorshift16( void );

//  +-----------------------------------+
//  |    OP Interpretation Functions    |
//  +-----------------------------------+

    // add a value (from a register or immediate) to a destination register
    // modify flags ZRO, NEG and POS
    void executeAddBasedOP( const uint32_t& instruction, OP op );

    // push a value ( either immediate or from a register ) to the top of the stack
    // push and pop are factorized inside the same opcode to make room for DISP instruction
    void executePUSH( const uint32_t& instruction );

    // take the top value, and decrement rsp, while placing ( or discarding ) the value in a register
    void executePOP( const uint32_t& instruction );

    // Either act as a cout or a cin, either with a value or a string
    void executePROMPT( const uint32_t& instruction ); // TODO subject to change input -> sfml

    // Binary Operator : Either act as AND, OR, NOT or XOR, used to compress 4 instructions in 1 opcode
    // Because of the compression, they cannot be used with dereferenced operands
    void executeBinBasedOP( const uint32_t& instruction );

    // take a register and set its value to a (pseudo) random one
    void executeRAND( const uint32_t& instruction );

    // sleep for a certain amount of time before going to the next instruction
    void executeWAIT( const uint32_t& instruction );

    // contains jump, conditionnal jump, call and ret
    void executeJUMP( const uint32_t& instruction );

    // redirect to the correct function depending on the instruction code contained in the first 8 bits
    bool processInstruction( const uint32_t& instruction );


//  +------------------------------+
//  |    Flags Update Functions    |
//  +------------------------------+

    // Each function will cast the argument values as int16_t

    // update every flag except Overflow since it needs operands value. Special case for EQU and ZRO see below
    // sub_or_cmp : false for sub (ZRO flag) and true for cmp (EQU flag)
    void updateFlags( const uint16_t& value );

    // update flags for cmp because destination is unchanged, ( OVF flag is unchanged )
    void updateCmpFlags( const uint16_t& dest, const uint16_t& src );

    // check if you can get back to the operand from the result, if not, result has overflowed
    void updateAddOverflow( const uint16_t& dest, const uint16_t& src );

    // check if you can get back to the operand from the result, if not, result has overflowed
    void updateSubOverflow( const uint16_t& dest, const uint16_t& src );

    // check if you can get back to the operand from the result, if not, result has overflowed
    void updateMulOverflow( const uint16_t& dest, const uint16_t& src );

    // analyse second hex value to execute appropriate instruction 
    void selectMISC( const uint32_t& instruction );

};



