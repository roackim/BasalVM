# Basal Virtual Machine
A basic 16bits VM, written in C++ capable of interpreting basal bytecode, using 32bits instructions.
The VM automatically allocate 2^16 words of 16bits. this represent ~130ko of memory.
It is quite performant, being able to execute ~20k instructions in 0.5 ms.

# Dependencies
	- gcc
    - make
    
# Building
    make

	
The project is compiled with a very large set of warnings enabled, which you can see typing "make flags".
It should compile without any warning being prompt, except a few from Static Analysis.

# Usage

Currently the assembler and the VM are coupled together, meaning you can only use the vm on textual representation of the bytecode:

./main <basm_file>

	ex : 	./bin/main examples/GameOfLife.basm


# Basal Assembler
proto-assembler based on GNU assembly, but simplified.
can assemble 20k basal assembly lines to instruction code under 0.1s.

It has severals features such as dereferencing registers, using immediate values or direct addresses.
Check Doc.md for more details. You can also check the code sample in bin/example, you'll find an implementation
of Conway's Game Of Life as well as other programs.

#Examples :

You can find example programs in the examples folder, these have been hand programmed, but ultimately this assembler should be used as a backend for a compiler.

![Game of life running on basal VM](preview.gif?raw=true "Basal VM interpreting bin/examples/GameOfLife.basm")

Display the first elements of the Fibonacci sequence (bin/examples/fibonacci.basm) :

	#--------------------
	# Fibonacci Sequence
	#--------------------

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
		
	:Inter  # separate numbers in display
		disp    '\,',   char
		disp    '\s',   char
		ret

	:END    # end program
		disp    '}',    char
		disp    '\n',   char
		exit
