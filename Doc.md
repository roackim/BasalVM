
#Basal Assembly Syntax

The syntax is based on GAS, generally follow this model : Instr source, desination
Instructions are not case-sensitive, although registers, and CPU flags are.

Labels:

    labels declaration starts with ':' and then a letter, afterwhat any number of any letter, number or underscore will be accepted.

    ex:  :START_LOOP
    ex:  :HelloThis_is_123123_aValidLABel
    
Registers:
    
    ax, bx, cx, dx, ex, fx, si, di, sp, ip
    
they are all general purpose register except sp and ip which are respectively the stack pointer and the            instruction pointer

CPU Flags:

    EQU, ZRO, POS, NEG, OVF, ODD
    
EQU and ZRO always have the same value
NEG and POS can have the same value, if the result is 0, both NEG and POS flag will be false.    


Instructions:

    Flags Modification meaning:
    Basic = EQU, ZRO, POS, NEG, ODD          # Does not update overflow
    All      = EQU, ZRO, POS, NEG, OVF, ODD    

    src  = either a register, a dereferenced register, an immediate value or an address        |> depend on the instruction 
    dest = either a register, a dereferenced register or an address                            |

    +-----------+---------------+----------------------+--------------------------+-------------------------------------------------+
    |    Instr  | Modify Flags  |      Usage           |           Mode           |              Description                        |
    +-----------+---------------+----------------------+--------------------------+-------------------------------------------------+
    | ADD       |    All        | add  src, dest       | operands can be deref    | add          a source value to a dest           |
    | SUB       |    All        | sub  src, dest       | operands can be deref    | substract a source value to a dest              |
    | CMP       |    Basic      | cmp  src, dest       | operands can be deref    | compare   a source value to a dest              |
    | COPY      |    Basic      | copy src, dest       | operands can be deref    | copy      a source value to a dest              |
    | MUL       |    All        | mul  src, dest       | operands can be deref    | multiply  a source value to a dest              |
    | DIV       |    Basic      | div  src, dest       | operands can be deref    | divide      a source value to a dest            |
    | MOD       |    Basic      | mod  src, dest       | operands can be deref    | modulus   a source value to a dest              |
    | PUSH      |    None       | push src             |                          | push a register or a value to the stack         |
    | POP       |    None       | pop (dest)           |                          | pop the stack, to a destination registion (opt) |
    | AND       |    Basic      | and  src, dest       |                          | bitwise AND a source value to a dest            |
    | OR        |    Basic      | or   src, dest       |                          | bitwise OR  a source value to a dest            |
    | NOT       |    Basic      | not  src, dest       |                          | bitwise NOT a source value to a dest            |
    | XOR       |    Basic      | xor  src, dest       |                          | bitwise XOR a source value to a dest            |
    | JUMP      |    None       | jump label (if FLAG) | if, ifnot                | goto address, can be conditional                |
    | CALL      |    None       | call label (if FLAG) | if. ifnot                | push ip then goto address, can be conditional   |
    | RET       |    None       | ret (if FLAG)        | if, ifnot                | pop ip, can be conditional                      |
    | INPUT     |    None       |                      |                          | input a value or a string                       |
    | DISP      |    None       | disp src, mode       | int, mem, hex, char, str | display a value or a string                     |
    | RAND      |    Basic      | rand src             |                          | randomize a register                            |
    | WAIT      |    None       | wait value, mode     | s, ms or us              | sleep for a certain amount of time              |
    | EXIT      |    None       | exit                 |                          | stop the program                                |
    | CLS       |    None       | cls                  |                          | clear the console screen                        |
    +-----------+---------------+----------------------+--------------------------+-------------------------------------------------+





    
    
