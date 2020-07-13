#include <iterator>
#include <fstream>
#include <cmath>
#include "basm.h"
#include "parser.h"


namespace cpl
{

	// better error message for compilation
	bool Compiler::compileError( std::string message )
	{

		std::cerr << "\n/!\\ Error line " << lineNbr << ": " << message << "." << endl;
	
		string line = "";
		unsigned marker = 0;
		unsigned i = j;
		if( current.type == ENDL ) i--;
		while( tokens[ i ].type != ENDL ) i--;
		i++;
		while( tokens[ i ].type != ENDL )
		{
			line += tokens[i].text + " ";

			if( i == j )	
			{
				marker = line.length()+1 - current.text.length() / 2;
			}
			i++;
		}	
		if( i == j and current.type == ENDL )	
		{
			marker = line.length()+3 - current.text.length() / 2;
		}

		cout << "--> " << line << endl ;
		for( unsigned k=0; k<=marker; k++)
			cout << " ";
		cout << "^" << endl;

		exit(-1); // subject to change
		return false;
	}

	// helper function, allow to get string from enum
	string getTypeStr( Type type )
	{
		switch( type )
		{
			case 0:
				return "label";
			case 1:
				return "op";
			case 2:
				return "reg";
			case 3:
				return "flag";
			case 4:
				return "flow";
			case 5:
				return "comma";
			case 6:
				return "colon";
			case 7:
				return "lparen";
			case 8:
				return "rparen";
			case 9:
				return "comment";
			case 10:
				return "decimal_value";
			case 11:
				return "hexa_value";
			case 12:
				return "binary_value";
			case 13:
				return "label_decl";
			case 14:
				return "endl";
			case 15:
				return "halt";
			case 16:
				return "time";
			case 17:
				return "arobase";
			case 18:
				return "disp_type";
			case 19:
				return "char_value";
			case 20:
				return "unkown";
		}
		return "ERROR";
	}

	// get register index
	uint8_t getRegInd( string reg )
	{
		if     ( reg == "ax" ) return 0;	
		else if( reg == "bx" ) return 1;
		else if( reg == "cx" ) return 2;
		else if( reg == "dx" ) return 3;
		else if( reg == "ex" ) return 4;
		else if( reg == "fx" ) return 5;
		else if( reg == "si" ) return 6;
		else if( reg == "di" ) return 7;
		else if( reg == "sp" ) return 8;
		else if( reg == "ip" ) return 9;

		std::cerr << "unkown register : " << reg << endl;
		return 15;
	}

	// increment j and reassign token t
	bool Compiler::readToken( void )
	{
		if( j < tokens.size() - 2 )
		{	
			current = tokens[ ++j ];
			return true; 		
		}
		else
		{
			current = token("End of file", HALT );
			return false;
		}
	}

	// parse decimals, binary and hexadecimal value, return a uint16 encoded value, and call readToken()
	uint16_t Compiler::parseValue( void )
	{
		string value = current.text;
		if( current.type == DECIMAL_VALUE )
		{
			int32_t i = atoi( value.c_str() );
			if( i > 65536 or i < -32768 )
			{
				compileError( "Value '" + current.text + "' is too big to be encoded" );
				return false;
			}
			readToken();
			return static_cast<uint16_t>( i );
		}
		else if( current.type == BINARY_VALUE )
		{
			value = value.substr( 2, value.length() - 1 ); // remove the base before number eg : 0b0101 -> 0101
			if( value.length() > 16 )
			{
				compileError( "Value '" + current.text + "' is too big to be encoded" );
				return false;
			}
			long int i = std::stol( value.c_str(), nullptr, 2);
			readToken();
			return static_cast<uint16_t>( i );
		}
		else if( current.type == HEXA_VALUE )
		{
			value = value.substr( 2, value.length() - 1 ); // remove the base before number eg : 0b0101 -> 0101
			if( value.length() > 4 )
			{
				compileError( "Value '" + current.text + "' is too big to be encoded" );
				return false;
			}
			long int i = std::stol( value.c_str(), nullptr, 16);
			readToken();
			return static_cast<uint16_t>( i );
		}
		compileError( "Expected a value" );
		return 0;

	}

	// curent token must be a ENDL, compileError and return false otherwise
	bool Compiler::readEndl( void )
	{
		if( current.type == ENDL )
		{
			readToken();
			lineNbr++;
			return true;
		}
		else
		{
			compileError("Expected End of line or ';'");
			return false;
		}
	}

	// current token must be a comma, compileError and return false otherwise
	bool Compiler::readComma( void )
	{
		if( tokens[ j ].type == COMMA )
		{
			readToken();
			return true;
		}
		else
		{
			compileError("Expected a comma, not '" + current.text + "'");
			return false;
		}
	}



	// match regex with string to find corresponding Type, and pakc both type and Txt in a token object.
	token Compiler::tokenize( string txt )
	{
		Type type = UNKNOWN;
		if     ( txt == "," ) type = COMMA;
		else if( txt == "@" ) type = AROBASE;
		else if( txt == ";" ) type = ENDL;
		else if( txt == ":" ) type = COLON;
		else if( txt == "(" ) type = LPAREN;
		else if( txt == ")" ) type = RPAREN;
		else if( txt == "!" ) type = COMMENT;
		else if( txt == "s" or txt == "ms" ) type = TIME;		// try to match time units 
		else if( txt == "int" or txt == "char" or txt == "address" ) type = DISP_TYPE;		// try to match time units 
		else if( parser::matchOP( txt ))			type = OP;				// try to match op
		else if( parser::matchFlag( txt ))			type = FLAG;			// try to match Flags
		else if( parser::matchFlowCtrl( txt ))		type = FLOW;			// try to match conditions for flags
		else if( parser::matchReg( txt ))			type = REG;				// try to match registers
		else if( parser::matchDecimalValue( txt ))  type = DECIMAL_VALUE;	// try to match decimal values
		else if( parser::matchHexaValue( txt ))		type = HEXA_VALUE;		// try to match hexa values
		else if( parser::matchBinValue( txt ))		type = BINARY_VALUE;	// try to match binary values
		else if( parser::matchLabelDecl( txt )) 	type = LABEL_DECL;		// try to match label declaration ex:  :Hello_World_Proc
		else if( parser::matchLabel( txt))			type = LABEL;			// try to match label call ex: jump Hello_World_Proc

		token ret( txt, type );
		return( ret ); 
	}

	// split a string with a delimiter 
	string Compiler::removeSpace( string line )
	{
		char del = '|';
		string delstr ; delstr += del; // used to compare txt to del 
		string txt = "";
		for( uint64_t i=0; i < line.length(); i++)
		{
			if( txt == delstr ) // avoid having a delimiter in first place
				txt = "";
			if( parser::isSpace( line[i]) )
			{	
				txt += del;
				while( parser::isSpace( line[i+1]) ) 
				{
					i++;	
				}
				continue;
			}
			else if( line[i] == ',' )
			{
				if( txt[txt.length()-1] != del ) txt += del;
				txt += ',';
				if( not parser::isSpace( line[i+1] )) txt += del;	
				continue;
			}
			else if( line[i] == '@' )
			{
				if( txt[txt.length()-1] != del ) txt += del;
				txt += '@';
				if( not parser::isSpace( line[i+1] )) txt += del;	
				continue;
			}
			else if( line[i] == ',' )
			{
				if( txt[txt.length()-1] != del ) txt += del;
				txt += ',';
				if( not parser::isSpace( line[i+1] )) txt += del;	
				continue;
			}
			else if( line[i] == '(' )
			{
				if( txt[txt.length()-1] != del ) txt += del;
				txt += '(';
				if( not parser::isSpace( line[i+1] )) txt += del;	
				continue;
			}
			else if( line[i] == ')' )
			{
				if( txt[txt.length()-1] != del ) txt += del;
				txt += ')';
				if( not parser::isSpace( line[i+1] )) txt += del;	
				continue;
			}
			if( line[i] == '!' ) break;
			// default
			txt += line[i];
		}
		if( txt[txt.length()-1] != del and txt != "" ) txt += del;
		txt += ";"; // end of line token
		txt += del;
		
		return txt;
	}

	// get one token from a string already split // called by loadAndTokenize
	bool Compiler::getOneToken( string& line )  
	{
		if( line == "" )
			return false;
		string delstr = "|";
		unsigned del;
		string tokenStr = "";

		del = line.find( delstr );
		tokenStr = line.substr(0, del);			// get first token
		line.erase(0, del + delstr.length());	// erase first token and delimiter
		
		token t = tokenize( tokenStr );

		tokens.push_back( t );
		return true;
	}

	// load a file and tokenize it
	void Compiler::loadAndTokenize( string fileName )
	{
		token first_token(";", ENDL); 
		tokens.push_back( first_token );

		char line[120];	// char * used to store line
		std::ifstream rfile;	
		rfile.open( fileName );	// Open file
		if( rfile.is_open())
		{
			while(rfile.getline( line, 120 ))	// tokenize whole line for every lines
			{
				string s = line;
				s = removeSpace( line );
				while( getOneToken( s ) ){ }
			}
		}
		rfile.close();
		token last_token("HALT", HALT); 
		tokens.push_back( last_token );
	}

	// compile the target basm file
	void Compiler::compile( string fileName )
	{
		rsp = 0;					 // isntruction count, used for LABEL_DECL
		loadAndTokenize( fileName ); // we can now use tokens
	
		if( !tokens.empty() )
		{
			current = tokens[ j ];
			while( parseOneInstr() )
			{
				if( !readEndl() )  // expect one instruction per line
				{
					break; // stop compilation
				}
			} 		
		}
		else
			cout << "Warning : No instruction found" << endl;

	}

	// parse one instruction from the token array
	bool Compiler::parseOneInstr( void )
	{
		if( current.text == "" ) // skip empty tokens. (might happens when using tabs mixed )
		{
			readToken();
			return true;
		}
		if( current.type == ENDL ) 
			return true; // do nothing

		if( current.type == HALT )
		{
			return false; // stop compiling (might not be usefull)
		}

		if( current.type == LABEL_DECL ) // if token is a label declaration, verifiy if not already defined, then define it.
		{
			string labelStr = current.text.substr(1, current.text.size() - 1);
			std::map<string, uint16_t>::iterator itr;
			for( itr = labels.begin(); itr != labels.end(); ++itr )
			{
				if( itr->first == labelStr ) // label already defined
				{
				compileError("Label '" + labelStr + "' already defined" );
					return false;
				}
			}
			labels.insert( std::pair<string, uint16_t>( labelStr, rsp ));	
			readToken();
			return true;
		}
		if( current.type == HALT )
			return false;

		if( current.type == OP )
		{
			string op = parser::to_lower( current.text );

			if( op=="add" or op=="sub" or op=="cmp" or op=="copy" or op=="mul" or op=="div" or op=="mod" )
				return parseAddBasedInstr();
			else if( op=="and" or op=="or" or op=="not" or op=="xor" )
				return parseBinBasedInstr();
			else if( op == "push" )
				return parsePushInstr();
			else if( op == "pop" )
				return parsePopInstr();
			else if( op == "rand" )
				return parseRandInstr();
			else if( op == "wait" )
				return parseWaitInstr();
			else if( op == "input" or op == "disp" )
				return parsePromptBasedInstr();
			else if( op == "jump" or op == "call" or op == "ret" )
				return parseJumpBasedInstr();
			else{
				compileError("instruction '" + current.text + "' not implemented yet");
				return false;
			}
		}
		compileError("Not an instruction : '" + current.text + "'");
		return false;
	}

	// helper function
	bool Compiler::checkForDereferencement( void )
	{
		if( current.type == DECIMAL_VALUE and tokens[ j + 1 ].type == LPAREN )
			return true;
		if( current.type == LPAREN )
			return true;
		return false;
	}


	// helper function
	bool Compiler::readDereferencedReg( uint8_t& offset, uint8_t& reg )
	{
		offset = 0; // no offset by default

		// read and parse offset if exists ifnot an offset of 0 will be used
		if( current.type == DECIMAL_VALUE )
		{

			int32_t i = atoi( current.text.c_str() );
			if( i > 7 or i < -7 )
				compileError( "Value '" + current.text + "' is too big to be encoded, offsets range is [0-7]" );
			readToken();

			uint8_t v = std::abs( i );
			v &= 0b0111;
			if( i < 0 ) v |= 0b1000;

			offset = static_cast<uint16_t>( v );
		}
		// read register inside parenthesis
		if( current.type == LPAREN )
		{
			readToken();				// read the left parenthesis
			if( current.type == REG )	// expect register name
			{
				reg = getRegInd( current.text );
				readToken();
				if( current.type == RPAREN ) 
				{
					readToken(); // expect closure parenthesis
					return true;
				}
				else
					return compileError("Closing parenthesis expected"); // terminate compilation
			}
			else 
				return compileError("Register expected after parenthesis");
		}
		else
			return compileError("Opening parenthesis expected");
		return false;

	}

//		+----------------------+
//		|  PARSE INSTRUCTIONS  |
//		+----------------------+

	// ADD, SUB, COPY, CMP, DIV, MUL, MOD
	bool Compiler::parseAddBasedInstr( void )
	{
		uint32_t instruction = 0x10000000;

		// Compute OP Code
		string op = parser::to_lower( current.text );
		if     ( op == "add" ) instruction = 0x10000000;
		else if( op == "sub" ) instruction = 0x20000000;
		else if( op == "copy") instruction = 0x30000000;
		else if( op == "cmp" ) instruction = 0x40000000;
		else if( op == "mul" ) instruction = 0x50000000;
		else if( op == "div" ) instruction = 0x60000000;
		else if( op == "mod" ) instruction = 0x70000000;

		readToken();

		uint8_t l_mode   = 0; // 0: immediate value | 1: address | 2: register | 3: dereferenced register
		uint8_t r_mode   = 0; // 0: this mode is not possible (destination operand), all other are possible
		uint8_t l_offset = 0; // offset of left operand
		uint8_t l_reg	 = 0; // register index of left operand
		uint8_t r_offset = 0; // ..
		uint8_t r_reg	 = 0; // ..

		// branch on address as left operand ex:	add @150, 34
		if( current.type == AROBASE )
		{
			readToken();							// skip @ token
			uint16_t src_address = parseValue();	// expect a value after @ symbol
			readComma();							// expect a comma

			l_mode = 1;
			instruction |= src_address;				// last 4 bits are immediate address
		} // branch on register as left operand ex:			add cx, ax
		else if( current.type == REG )
		{
			l_reg = getRegInd( current.text );	
			readToken();
			readComma();
			l_mode = 2;
			instruction |= l_reg << 12;
			instruction |= l_reg << 20;

		} //
		else if( checkForDereferencement() )
		{
			readDereferencedReg( l_offset, l_reg );
			readComma();
			instruction |= l_reg    << 12;
			instruction |= l_offset <<  8;
			instruction |= l_reg    << 20;
			instruction |= l_offset << 16;
			l_mode = 3;
		} // branch on immediate value as left operand ex:	add 123, cx
		else if( current.type == DECIMAL_VALUE or current.type == HEXA_VALUE or current.type == BINARY_VALUE )
		{
			uint16_t imm_value = parseValue();
			readComma();
			instruction |= imm_value;				// add immediate value to the last 4 bits
			l_mode = 0;
		} 

		else
			return compileError("Unexpected token");

		// we can assume the first operand and the comma to be already parsed
		// branch on address as right operand ex:	add 231, @34
		if( current.type == AROBASE )
		{
			readToken();							// skip @ token
			uint16_t dest_address = parseValue();	// expect a value after @ symbol
			r_mode = 1;
			instruction &= 0xFFFF0000;
			instruction |= dest_address;				// last 4 bits are immediate address
			// compatibility problems ex: add 0xFF, @123
			if( l_mode == 0 )
				return compileError("Cannot use immediate value, with an immediate address in the same instruction");
			if( l_mode == 1 )
				return compileError("Cannot use two immediate addresses in the same instruction");
		} // branch on register as right operand ex: add ax, cx
		else if( current.type == REG )
		{
			r_reg = getRegInd( current.text );	
			readToken();
			instruction |= r_reg << 20;
			r_mode = 2;
		} // branch on dereferenced register as right operand ex: add 45, (dx) 
		else if( checkForDereferencement() )
		{
			if( l_mode == 3 )
				return compileError("Cannot use two dereferencement in the same instruction");
			readDereferencedReg( r_offset, r_reg );
			instruction |= r_reg    << 20;
			instruction |= r_offset << 16;
			r_mode = 3;
		}

		// add the modes to the instruction
		char modes = (l_mode << 2) + r_mode;
		instruction |= modes << 24;

		program.push_back( instruction );
		return true;
	}

	// TODO function body
	// opcode 8.  AND, OR, NOT, XOR
	bool Compiler::parseBinBasedInstr( void )
	{
		return false;
	}

	// opcode 9, PUSH
	bool Compiler::parsePushInstr( void )
	{
		uint32_t instruction = 0x90000000;
		readToken(); // skip push token
		if( current.type == DECIMAL_VALUE or current.type == HEXA_VALUE or current.type == BINARY_VALUE )
		{
			uint16_t v = parseValue();
			instruction |= v;
			instruction |= 0x01000000;
		}
		else if( current.type == REG )
		{
			int src = getRegInd( current.text );	
			src = src << 12;
			instruction |= src;
			readToken(); // skip register
		}
		else if( current.type == ENDL )
			return compileError("Missing operands after push instruction");
		else
			return compileError("Junk after pop instruction");

		program.push_back( instruction );
		return true;
	}

	// opcode 10, POP
	bool Compiler::parsePopInstr( void )
	{
		uint32_t instruction = 0xA0000000;
		readToken(); // skip pop token
		token t = tokens[j];
		if( current.type == REG ) // pop to a register
		{
			int dest = getRegInd( current.text );	
			dest = dest << 20;
			instruction |= dest;
			readToken();
		}
		else if( current.type == ENDL ) // just pop, discarding the value
		{
			instruction |= 0x01000000; 
		}
		else
		{
			compileError("Junk after pop instruction");
			return false;
		}
		program.push_back( instruction );
		return true;
	}

	// TODO
	// opcode 11, JUMP, CALL, RET
	bool Compiler::parseJumpBasedInstr( void )
	{
		return false;
	}

	// TODO  finish second operand for disp, do input
	// opcode 12, INPUT, DISP
	bool Compiler::parsePromptBasedInstr( void )
	{
		uint32_t instruction = 0xC0000000;
		uint8_t l_mode = 0; // source  mode, 0: address | 1: reg | 2: dereferenced reg
		uint8_t r_mode = 0;	// display mode, 0: char | 1: integer | 2: address ( unsigned int )
		uint8_t reg	   = 0;
		uint8_t offset = 0;		
		uint16_t value = 0;

		if( current.text == "disp" )
		{
			readToken(); // skip disp token
		}
		else // should not happen
			return compileError("Expected disp instruction");

		if( current.type == AROBASE ) // disp @256, char  
		{
			readToken();							// skip @ token
			value = parseValue();	// expect a value after @ symbol
			readComma();							// expect a comma
			l_mode = 0;

		}
		else if( current.type == DECIMAL_VALUE ) // disp 97, char
		{
			string value_str = current.text;
			if( current.type == DECIMAL_VALUE )
			{
				int32_t i = atoi( value_str.c_str() );
				if( i > 65536 or i < -32768 )
				{
					compileError( "Value '" + current.text + "' is too big to be encoded" );
					return false;
				}
				value = static_cast<uint16_t>( i );
				readToken();
				readComma();
				l_mode = 3;
			}
		}
		else if( current.type == REG )
		{
			value = getRegInd(current.text);
			readToken();
			readComma();
			l_mode = 1;
		}
		else if( checkForDereferencement() )
		{
			readDereferencedReg( offset, reg ); 	
			readComma();
			l_mode = 2;
		}
		else
			return compileError("Unexpected operand : '" + current.text + "'");

		// we can assume that the first operand and the comma has been correctly processed
		// process second operand
		if( current.type == DISP_TYPE )
		{
			if	   ( current.text == "char" )
				r_mode = 0;
			else if( current.text == "int" )
				r_mode = 1;
		}	

	cout << "Parsed : " << value << endl;

	program.push_back( instruction );
	return true;
	}

	// RAND
	bool Compiler::parseRandInstr( void )
	{
		uint32_t instruction = 0xD0000000;
		readToken(); // skip rand token

		if( current.type == REG )
		{
			readToken();
			if( current.type == COMMA ) // rand ax, 265     ax -> rand(0, 256)
			{
				instruction |= 0x02000000; // mode
				readComma();
				uint16_t max_value = parseValue();  // does not increment j (maybe it should ?)
				instruction |= max_value;  // immediate value

				readToken(); // skip immediate value, go to next token
				program.push_back( instruction ); // store instruction
				return true;
			}
			else if( tokens[ j ].type == ENDL ) // rand ax
			{
				program.push_back( instruction ); // store instruction
				return true;
			}
			else
				return compileError("junk after rand instruction, maybe a comma is missing between operands");
		}
		else
			return compileError("expected register after rand instruction" );

		return false;
	}

	// WAIT
	bool Compiler::parseWaitInstr( void )
	{
		uint32_t instruction = 0xE0000000;
		readToken(); // skip wait token
		uint16_t time_value = parseValue();
		instruction |= time_value;

		bool comma = readComma();
		if( current.type == TIME )
		{
			if( current.text == "ms" ) ;
				// nothing to change in instruction ( mode == 0 )
			else if( current.text == "s" )
				instruction |= 0x01000000;
			readToken(); // skip unit token
			program.push_back( instruction );
			return true and comma;
		}
		else
			return compileError("Expected time units, either 'ms' or 's' operand");
	}


}

// usage
// cpl::Compiler compiler;

// compiler.compile( "asm/test.basm" );

