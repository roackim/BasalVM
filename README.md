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


registers:

	ax, bx, cx, dx, ex, fx, si, di, sp, ip
	[ax-fx] are genral purpose registers
	si and di respectively stands for source index and destination index 
	sp stands for stack pointer
	ip stand for instruction pointer

CPU flags:

	mainly used after a cmp instruction

	EQU : true if both operands are equals
	ZRO : true if the result of the comparison ( substraction ) is 0, virtually equal to EQU flag.
	POS : true if the result of the comparison is positive
	NEG : true if the result of the comparison is negative
	OVF : true if the result of an instruction overflowed or underflowed the destination.
	ODD : true if the result of an instruction is odd


examples :

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
