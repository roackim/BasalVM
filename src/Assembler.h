#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>

using std::string;
using std::cout;
using std::cerr;
using std::endl;


		
namespace Asm // keep things contained in a namespace.  basm = Basal Assembly
{
	enum Type	// types of tokens
	{
		LABEL,
		OP,
		REG,
		FLAG,
		FLOW,
		COMMA,
		COLON,
		LPAREN,
		RPAREN,
		COMMENT,
		DECIMAL_VALUE,
		HEXA_VALUE,
		BINARY_VALUE,
		LABEL_DECL,
		ENDL,
		HALT,
		TIME,
		AROBASE,
		DISP_TYPE,
		CHAR_VALUE,
		UNKNOWN
	};

	class token
	{
	public:
		string text;
		Type type = UNKNOWN;
		token( string s, Type t )
		: text( s )
		, type( t ) { }
		token()
		{
			text = ""; type = UNKNOWN;
		}
	};

	// helper function, allow to get string from enum
	string getTypeStr( Type type );

	// get register index
	uint8_t getRegInd( string reg ); 

	class Assembler
	{
	public:
		std::vector<uint32_t> program;			// store all the instructions

	private:
		uint64_t rsp = 0;						// pointer to next instructions while compiling, increment every time an instruction is parsed
		uint64_t   j = 0;						// used to count tokens
		uint64_t lineNbr = 1;					// one empty line is always artifially added at the begining
		std::vector<token> tokens;				// store all tokens
		std::map<string, uint16_t> labels;		// store addresses of labels
		token current;							// used as current token

	public:
		// assemble instructions
		bool assemble( string fileName );
	
	private:
		// increment j and reassign token t
		bool readToken( void );

		// better error message for compilation
		bool compileError( string message );

		// parse decimals, binary and hexadecimal values
		uint16_t parseValue( void ); 

		// check if nexts tokens can be interpreted as a dereferencement
		bool checkForDereferencement( void );

		// read and expect a dereferenced register
		bool readDereferencedReg( uint8_t& offset, uint8_t& reg );

		// curent token must be a ENDL, compileError and return false otherwise
		bool readEndl( void );

		// current token must be a comma, compileError and return false otherwise
		bool readComma( void );

		// match regex with string to find corresponding Type, and pakc both type and Txt in a token object.
		token tokenize( string txt );

		// split a string with a delimiter 
		string removeSpace( string line );

		// get one token from a string already split 
		bool getOneToken( string& line );

		// load a file and tokenize it
		bool loadAndTokenize( string fileName );

		// look at a token and redirect toward the appropriatre function, eg : ADD -> call parseAddBasedInstr()		
		bool parseOneInstr( void );

		// ADD, SUB, COPY, CMP, DIV, MUL, MOD
		bool parseAddBasedInstr( void );

		// AND, OR, NOT, XOR
		bool parseBinBasedInstr( void );

		// PUSH
		bool parsePushInstr( void );

		// POP
		bool parsePopInstr( void );

		// JUMP, CALL, RET
		bool parseJumpBasedInstr( void );

		// Called by parsePromptInstr
		bool parseDispInstr( void );

		// Called by parsePromptInstr
		bool parseInputInstr( void );

		// INPUT, DISP
		bool parsePromptInstr( void );

		// RAND
		bool parseRandInstr( void );

		// WAIT
		bool parseWaitInstr( void );




	};
}
