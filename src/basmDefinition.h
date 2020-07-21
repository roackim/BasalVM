#pragma once

//  +------------------------------------+
//  |    Enums used for code clarity     |
//  +------------------------------------+
//    -- Registers --
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
    sp,     // 8.
    ip,     // 9.
    R_COUNT
};

//    -- OP codes --
enum OP             
{
    IO = 0,     //  .0          // used to communicate with outside the VM, not implemented yet
    ADD,        //  .1          // Addition
    SUB,        //  .2          // Substraction
    COPY,       //  .3          // Copy
    CMP,        //  .4          // Comparison
    MUL,        //  .5          // Multiplication       ex :  MUL 5, ax         C : ax *= 5;
    DIV,        //  .6          // Division
    MOD,        //  .7          // Modulus
    BIN,        //  .8          // contains AND, OR, NOT and XOR
    PUSH,       //  .9
    POP,        //  .10         
    JUMP,       //  .11         // contains CALL and RET 
    PROMPT,     //  .12         // act as both a cout and a cin depending on the mode
    RAND,       //  .13
    WAIT,       //  .14         // implementation must depend on the context
    HALT        //  .15         // halt programm execution
};

//    -- CPU Flags --
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

