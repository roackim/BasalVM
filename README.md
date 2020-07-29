# Basal Virtual Machine
basic 16bits VM, written in C++ capable of interpreting basal bytecode, using 32bits instructions.
the VM automatically allocate 2^16 words of 16bits. this represent ~130ko of memory.
the program is loaded by the compiler in a std::vector, containing instructions encoded in 32 bits.

# Dependencies
	- gcc
	- cppclean
	- cppcheck
	
you can also just remove the # Static Analysis in the makefile instead if you dont want to use cppclean and cppcheck.

	The project is compiled with a very large set of warnings enabled, which you can see typing "make flags", it also should compile without any warning being prompt, except a few from Static Analysis.

# Usage

Currently the assembler and the VM are coupled together, meaning you can only use both at the same time, meaning there is only one way to run the project:

./main <basm_file>

	ex : 	./bin/main bin/examples/GameOfLife.basm
	
	or :	cd bin && ./main examples/GameOfLife.basm


# Basal Assembler
proto Assembler based on GNU assembly, but simplified.
can assemble 20k basal assembly lines to instruction code under 0.1s

it has severals features such as dereferencing registers, using immediate values or direct addresses.

# BASM Syntax

The syntax is based on GAS, generally follow this model : <Instr> <source>, <desination>
Instructions are not case-sensitive, although registers, and CPU flags are.

~~ TODO in depth documentation for every instruction


Registers:

	ax   |
	bx	 |
	cx	 |> General purpose registers
	dx	 |
	ex	 |
	fx	 |
	si	 -> Source Index register  
	di	 -> Destination Index register
	sp	 -> Stack Pointer register
	ip	 -> Instruction Pointer register

Instructions:
	
	add, sub, copy, cmp, mul, div, mod, bin, push, pop, jump, prompt, rand, wait		

	~~ Todo: detail every instructions

CPU flags:

	EQU, ZRO, POS, NEG, OVF, ODD


Examples :

	! this BASM programm displays the first 10 iteration of the fibonacci sequence


	:BEGIN

		copy    0,  ax
		copy    1,  bx
		copy    15, cx  

		disp    '{', char
		disp     bx, mem

	:LOOP   # loop to calculate fibonacci sequence
		sub     1,  cx
		cmp     0,  cx
		jump END if EQU 

		copy    bx, dx
		add     ax, bx
		copy    dx, ax

		call Inter
		disp    bx, mem

		jump LOOP

	:END    # end program
		disp    '}',    char
		disp    '\n',   char
		exit
		
	:Inter  # separate numbers in display
		disp    '\,',   char
		disp    '\s',   char
		ret
