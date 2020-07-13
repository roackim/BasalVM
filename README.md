# Basal Virtual Machine
basic 16bits VM, written in C++ capable of interpreting basal bytecode, using 32bits instructions.
the VM automatically allocate 2^16 words of 16bits. this represent ~130ko of memory.
the program is loaded by the compiler in a std::vector, containing instructions encoded in 32 bits.
Its size is proportionnal to the number of instruction in the program.

# Basal Assembler
proto Assembler based on GNU assembly, but simplified.
can "compile" 20k basal assembly lines to bytecode under 0.1s

it has severals features such as dereferencing registers, using immediate values or addresses.

# BASM Syntax

The syntax is based on GAS, generally follow this model : <Instr> <source>, <desination>
Instructions are not case-sensitive, although registers, and CPU flags are.

~~ TODO define explicitly every syntax and functionnality


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

	mainly used after a cmp instruction, 

	EQU, ZRO, POS, NEG, OVF, ODD


Examples :

	! this BASM programm displays the first 10 iteration of the fibonacci sequence

	:Fibonacci_Seq

		copy 10, cx
		copy  0, ax
		copy  1, bx
		
		disp ax, int

	:loopStart				
		disp bx, int
	
		copy bx, dx
		add  bx, ax
		copy ax, bx		! bx -> ax + bx
		copy dx, ax		! ax -> bx

		sub   1, cx
		jmp loopStart if ZRO

	ret
