#include <iterator>
#include <fstream>
#include <cmath>
#include "Assembler.h"
#include "parser.h"


namespace Asm
{

    // better error message for compilation
    bool Assembler::compileError( std::string message )
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
            default:
                return "unkown";
        }
        return "ERROR";
    }

    // get register index
    uint8_t getRegInd( const string& reg )
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

        // not supposed to happen.
        std::cerr << "unkown register : " << reg << endl;
        return 15;
    }

    // get CPU flag index from string
    uint8_t getFlagInd( const string& flag )
    {
        if     ( flag == "EQU" ) return 0;    
        else if( flag == "ZRO" ) return 1;
        else if( flag == "POS" ) return 2;
        else if( flag == "NEG" ) return 3;
        else if( flag == "OVF" ) return 4;
        else if( flag == "ODD" ) return 5;

        // not supposed to happen
        std::cerr << "unkown flag : " << flag << endl;
        return 6;
    }

    // increment j and reassign token t
    bool Assembler::readToken( void )
    {
        if( j < tokens.size() - 2 )
        {    
            current = tokens[ ++j ];
            return true;         
        }
        else
        {
            current = token("End of file", STOP );
            return false;
        }
    }

    // parse decimals, binary and hexadecimal value, return a uint16 encoded value, and call readToken()
    uint16_t Assembler::parseValue( void )
    {
        string value = current.text;
        if( current.type == DECIMAL_VALUE )
        {
            int32_t i = atoi( value.c_str() );
            if( i > 65535 or i < -32768 )
            {
                compileError( "Value '" + current.text + "' is too big to be encoded.\n\trange: [0, 65535] or [-32768, 32767]" );
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
                compileError( "Value '" + current.text + "' is too big to be encoded.\n\trange: [0, 65535] or [-32768, 32767]" );
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
                compileError( "Value '" + current.text + "' is too big to be encoded.\n\trange: [0, 65535] or [-32768, 32767]" );
                return false;
            }
            long int i = std::stol( value.c_str(), nullptr, 16);
            readToken();
            return static_cast<uint16_t>( i );
        }
        compileError( "Expected a value" );
        return 0;
    }

    // parse char values    
    char Assembler::parseCharValue( void )
    {
        string s = current.text;
        string c = "";

        c += s[1];
        if( s.length() == 3 )
        {
            readToken();
            return  s[1] ;
        }
        if( s.length() == 4 )
        {
            c += s[2];
            if     ( c == "\\n" ) 
            {
                readToken();
                return '\n' ;
            }
            else if( c == "\\t" ) 
            {
                readToken();
                return '\t' ;
            }
            else if( c == "\\v" ) 
            {
                readToken();
                return '\v' ;
            }
            else if( c == "\\s" ) 
            {
                readToken();
                return ' ' ;
            }
            else if( c == "\\," )
            {
                readToken();
                return ',';
            }
            else if( c == "\\!" ) // avoid comment
            {
                readToken();
                return '!';
            }
            else compileError("Unrecognized character");
        }
        return '?' ; // unkown character
    }

    // curent token must be a ENDL, compileError and return false otherwise
    bool Assembler::readEndl( void )
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
    bool Assembler::readComma( void )
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
    token Assembler::tokenize( string txt )
    {
        Type type = UNKNOWN;
        if     ( txt == "," ) type = COMMA;
        else if( txt == "@" ) type = AROBASE;
        else if( txt == ";" ) type = ENDL;
        else if( txt == ":" ) type = COLON;
        else if( txt == "(" ) type = LPAREN;
        else if( txt == ")" ) type = RPAREN;
        else if( txt == "s" or txt == "ms" ) type = TIME;        // try to match time units 
        else if( txt == "int" or txt == "char" or txt == "mem" or txt == "str" 
                or txt == "hex" or txt == "bin" ) type = DISP_TYPE; // try to match time units 
        else if( parser::matchOP( txt ))            type = OP;                // try to match op
        else if( parser::matchFlag( txt ))            type = CPUFLAG;            // try to match Flags
        else if( parser::matchFlowCtrl( txt ))        type = COND;            // try to match conditions for flags
        else if( parser::matchReg( txt ))            type = REG;                // try to match registers
        else if( parser::matchCharValue( txt ))        type = CHAR_VALUE;        // try to match char literals ex: 'a', '\n'
        else if( parser::matchDecimalValue( txt ))  type = DECIMAL_VALUE;    // try to match decimal values
        else if( parser::matchHexaValue( txt ))        type = HEXA_VALUE;        // try to match hexa values
        else if( parser::matchBinValue( txt ))        type = BINARY_VALUE;    // try to match binary values
        else if( parser::matchLabelDecl( txt ))     type = LABEL_DECL;        // try to match label declaration ex:  :Hello_World_Proc
        else if( parser::matchLabel( txt))            type = LABEL;            // try to match label call ex: jump Hello_World_Proc

        token ret( txt, type );
        return( ret ); 
    }

    // split a string with a delimiter 
    string Assembler::removeSpace( string line )
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
                if( i > 0  and line[i-1] == '\\' ); // avoid this case
                else if( txt[txt.length()-1] != del ) txt += del;
                txt += ',';
                if( i > 0  and line[i-1] == '\\' ); // avoid this case
                else if( not parser::isSpace( line[i+1] )) txt += del;    
                continue;
            }
            else if( line[i] == '@' )
            {
                if( txt[txt.length()-1] != del ) txt += del;
                txt += '@';
                if( not parser::isSpace( line[i+1] )) txt += del;    
                continue;
            }
            else if( line[i] == ';' )
            {
                if( txt[txt.length()-1] != del ) txt += del;
                txt += ';';
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
            else if( line[i] == '!')
            {
                if( i != 0 and line[i-1] != '\\' ) break;
                else if( i == 0 ) break;
            }

            // default
            txt += line[i];
        }
        if( txt[txt.length()-1] != del and txt != "" ) txt += del;
        txt += ";"; // end of line token
        txt += del;
        
        return txt;
    }

    // get one token from a string already split // called by loadAndTokenize
    bool Assembler::getOneToken( string& line )  
    {
        if( line == "" )
            return false;
        string delstr = "|";
        unsigned del;
        string tokenStr = "";

        del = line.find( delstr );
        tokenStr = line.substr(0, del);            // get first token
        line.erase(0, del + delstr.length());    // erase first token and delimiter
        
        token t = tokenize( tokenStr );

        tokens.push_back( t );
        return true;
    }

    // load a file and tokenize it
    bool Assembler::loadAndTokenize( string fileName )
    {

        char line[120];    // char * used to store line
        std::ifstream rfile;    
        rfile.open( fileName );    // Open file
        if( rfile.is_open())
        {
            token first_token(";", ENDL);  // used for error message in case or error on line 1
            tokens.push_back( first_token );

            while(rfile.getline( line, 120 ))    // tokenize whole line for every lines
            {
                string s = removeSpace( line );
                while( getOneToken( s ) ){ }
            }
        }
        else
        {
            rfile.close();
            cerr << "/!\\ Error while assembling : Cannot open file '" << fileName << "'." << endl;
            exit(-1);
            return false;

        }
        rfile.close();
        token last_token("STOP", STOP); 
        tokens.push_back( last_token );
        return true;
    }

    // assemble instructions from the target basm file
    bool Assembler::assemble( string fileName )
    {
        rsp = 0;                     // isntruction count, used for LABEL_DECL
        if( !loadAndTokenize( fileName )) // we can now use tokens
            return false;
    
        if( !tokens.empty() )
        {
            
            while( parseLabelDecl() ){}

            while( parseOneInstr() )
            {
                if( !readEndl() )  // expect one instruction per line
                {
                    break; // stop compilation
                }
            }         
            return true;
        }
        else
        {
            cerr << "/!\\ Error while assembling : No instruction found in file '" << fileName << "'." << endl;
            return false;
        }
    }


    bool Assembler::parseLabelDecl( void )
    {
        if( current.type == LABEL_DECL ) // if token is a label declaration, verifiy if not already defined, then define it.
        {
            string labelStr = current.text.substr(1, current.text.size() - 1); // remove ':' char at the begining

            // label is already defined, throw error
            if( declared_labels.count( current.text ) == 1 ) 
                return compileError("Label '" + labelStr + "' already defined" );

            declared_labels.insert( std::pair<string, uint16_t>( labelStr, rsp ));    
            readToken();
            return true;
        }    
        else if( current.type == STOP )
        {
            j = 0;            // reset cursor
            rsp = 0;
            current = tokens[ j ];
            return false;    // stop
        }
        else if( current.type == OP )
        {
            readToken();
            rsp++;
            return true;
        }
        else
        {
            readToken();
               return true;
        }
        return false; // avoid warnings
    }

    // parse one instruction from the token array
    bool Assembler::parseOneInstr( void )
    {
        if( current.text == "" ) // skip empty tokens. (might happens when using tabs mixed )
        {
            readToken();
            return true;
        }
        if( current.type == ENDL ) 
        {
            return true; // do nothing
        }
        else if( current.type == STOP ) // End of file
        {
            program.push_back( 0xF0000000 ); // halt instruction
            return false;                    // stop compilation
        }
        else if( current.type == LABEL_DECL ) // skip, already parsed by parseLabelDecl()
        {
            readToken();
            return true;
        }
        else if( current.type == OP )
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
            else if( op == "disp" or op == "input")
                return parsePromptInstr();
            else if( op == "jump" or op == "call" or op == "ret" )
                return parseJumpBasedInstr();
            else if( op == "exit" )
            {
                readToken();
                program.push_back( 0xF0000000 ); // halt instruction
                return true;                     // continue compilation
            }
            else{
                return compileError("Unknown instruction '" + current.text + "'");
            }
        }
        return compileError("Not an instruction : '" + current.text + "'");
    }

    // helper function
    bool Assembler::checkForDereferencement( void ) const
    {
        if( current.type == DECIMAL_VALUE and tokens[ j + 1 ].type == LPAREN )
            return true;
        if( current.type == LPAREN )
            return true;
        return false;
    }


    // helper function
    bool Assembler::readDereferencedReg( uint8_t& offset, uint8_t& reg )
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
            if( i < 0 ) v |= 0b1000; // first bit -> sign

            // offset = static_cast<uint16_t>( v ); // why the cast ?
            offset = v; // seems simpler
        }
        // read register inside parenthesis
        if( current.type == LPAREN )
        {
            readToken();                // read the left parenthesis
            if( current.type == REG )    // expect register name
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

//        +----------------------+
//        |  PARSE INSTRUCTIONS  |
//        +----------------------+

    // ADD, SUB, COPY, CMP, DIV, MUL, MOD
    bool Assembler::parseAddBasedInstr( void )
    {
        uint32_t instruction = 0x00000000;

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
        uint8_t l_reg    = 0; // register index of left operand
        uint8_t r_offset = 0; // ..
        uint8_t r_reg    = 0; // ..

        // branch on address as left operand ex: add @150, 34
        if( current.type == AROBASE )
        {
            readToken();                            // skip @ token
            uint16_t src_address = parseValue();    // expect a value after @ symbol
            readComma();                            // expect a comma

            l_mode = 1;
            instruction |= src_address;                // last 4 bits are immediate address
        } // branch on register as left operand ex:    add cx, ax
        else if( current.type == REG )
        {
            l_reg = getRegInd( current.text );    
            readToken();
            readComma();
            l_mode = 2;
            instruction |= static_cast<uint32_t>( l_reg << 4 );

        } // left operand is a dereferenced register
        else if( checkForDereferencement() )
        {
            readDereferencedReg( l_offset, l_reg );
            readComma();
            instruction |= static_cast<uint32_t>( l_reg << 4 ); // need explicit cast because of implicit int cast
            instruction |= l_offset << 0;
            l_mode = 3;
        } // branch on immediate value as left operand ex:    add 123, cx
        else if( current.type == DECIMAL_VALUE or current.type == HEXA_VALUE or current.type == BINARY_VALUE )
        {
            uint16_t imm_value = parseValue();
            readComma();
            instruction |= imm_value;                // add immediate value to the last 4 bits
            l_mode = 0;
        } 

        else
            return compileError("Unexpected token");

        // we can assume the first operand and the comma to be already parsed
        // branch on address as right operand ex:    add 231, @34
        if( current.type == AROBASE )
        {
            readToken();                            // skip @ token
            r_mode = 1;
            // compatibility problems ex: add 0xFF, @123, not enought place in instruction to have both an address and a value
            if( l_mode == 0 )
                return compileError("Cannot use immediate value with an immediate address in the same instruction");
            if( l_mode == 1 )
                return compileError("Cannot use two immediate addresses in the same instruction");

            uint16_t dest_address = parseValue();    // expect a value after @ symbol
            instruction &= 0xFF0000FF;
            instruction |= static_cast<uint32_t>( dest_address << 8 );                // immediate address 

        } // branch on register as right operand ex: add ax, cx
        else if( current.type == REG )
        {
            r_reg = getRegInd( current.text );    
            readToken();
            instruction |= static_cast<uint32_t>( r_reg << 20 );
            r_mode = 2;
        } // branch on dereferenced register as right operand ex: add 45, (dx) 
        else if( checkForDereferencement() )
        {
            if( l_mode == 3 ) // took this from assembly, but no real limitation force this. Might be subject to change.
                return compileError("Cannot use two dereferencement in the same instruction");
            readDereferencedReg( r_offset, r_reg );
            instruction |= static_cast<uint32_t>( r_reg << 20 );
            instruction |= static_cast<uint32_t>( r_offset << 16 );
            r_mode = 3;
        }

        // add the modes to the instruction
        short modes = (l_mode << 2) + r_mode;
        instruction |= static_cast<uint32_t>( modes << 24 );

        program.push_back( instruction );
        return true;
    }

    // TODO function body
    // opcode 8.  AND, OR, NOT, XOR        // only work with registers and immediate value
    bool Assembler::parseBinBasedInstr( void )
    {
        uint32_t instruction = 0x80000000;

        // Compute OP Code
        string op = parser::to_lower( current.text );
        if     ( op == "and" ) instruction |= 0x01000000;
        else if( op == "or" )  instruction |= 0x02000000;
        else if( op == "not")  instruction |= 0x03000000;
        else if( op == "xor" ) instruction |= 0x04000000;

        readToken(); // read instruction token

        uint8_t l_mode   = 0; // 0: immediate value | xxxx | 2: register | xxxx

        if( current.type == REG )
        {
            uint8_t l_reg = getRegInd( current.text );    
            readToken();
            l_mode = 2;
            instruction |= static_cast<uint32_t>( l_reg << 12 );
        }
        else if( current.type == DECIMAL_VALUE or current.type == HEXA_VALUE or current.type == BINARY_VALUE )
        {
            uint16_t imm_value = parseValue();
            instruction |= imm_value;                // add immediate value to the last 4 bits
            l_mode = 0;
        } 
        else if( current.type == AROBASE or checkForDereferencement() )
            return  compileError("Cannot use address or dereferencement with binary operator instructions");
        else
            return compileError("Unexpected token");

        readComma(); 

        if( current.type == REG )
        {
            uint8_t r_reg = getRegInd( current.text );    
            instruction |= static_cast<uint32_t>( r_reg << 16 ); // after immediate value
        }
        else
            return compileError("Expected register as right hand operand");

        instruction |= static_cast<uint32_t>( l_mode << 20 );
        readToken();
        program.push_back( instruction );
        return true;
        
    }

    // opcode 9, PUSH
    bool Assembler::parsePushInstr( void )
    {
        uint32_t instruction = 0x90000000;
        readToken(); // skip push token
        if( current.type == DECIMAL_VALUE or current.type == HEXA_VALUE or current.type == BINARY_VALUE )
        {
            uint16_t v = parseValue();
            instruction |= v;
            instruction |= 0x01000000; // if value is immediate
        }
        else if( current.type == REG )
        {
            int src = getRegInd( current.text );    
            instruction |= static_cast<uint32_t>( src << 12 );
            readToken(); // skip register
        }
        else if( current.type == CHAR_VALUE )
        {
            char c = parseCharValue(); 
            instruction |= static_cast<uint16_t>( c );
            instruction |= 0x01000000;
        }
        else if( current.type == ENDL )
            return compileError("Missing operands after push instruction");
        else
        {
            cout << getTypeStr( current.type ) << endl;
            return compileError("Junk after push instruction");
        }

        program.push_back( instruction );
        return true;
    }

    // opcode 10, POP
    bool Assembler::parsePopInstr( void )
    {
        uint32_t instruction = 0xA0000000;
        readToken(); // skip pop token
        if( current.type == REG ) // pop to a register
        {
            int dest = getRegInd( current.text );    
            instruction |= static_cast<uint32_t>( dest << 20 );
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
    bool Assembler::parseJumpBasedInstr( void )
    {
        uint32_t instruction = 0xB0000000;
        string op = parser::to_lower( current.text );
        if( op == "jump" )
        {
            readToken();
            if( current.type == LABEL )
            {
                if( declared_labels.count( current.text ) == 1 ) // the label has been declared
                {
                    uint16_t address = declared_labels[ current.text ]; // get the instruction address
                    instruction |= address;
                    readToken();

                    if( current.type == ENDL ) // avoid being catched by else statement
                    {
                        // instruction == 0xB000@@@@
                        instruction |= 0x00000000; // useless, but put for code clarity
                        //                ^ 0 here means it is a non conditionnal juml
                    }
                    else if( current.type == COND ) // either 'if' or 'ifnot'
                    {
                        instruction |= 0x03000000; // 3 means it is a conditionnal jump
                        if( current.text == "if" ) 
                        {
                            instruction |= 0x00100000; // 1 by default ex: jump .. if EQU | 0: if negated ex: jump .. ifnot ZRO
                        }
                        readToken();
                        if( current.type == CPUFLAG )
                        {
                            uint8_t flag_ind = getFlagInd( current.text );
                            instruction |= static_cast<uint32_t>(flag_ind << 16 );
                            readToken();
                        }
                        else
                            return compileError("Expected CPU flag after 'if' or 'ifnot'");
                    }
                    else
                        return compileError("Unexpected token");

                    program.push_back( instruction );
                    return true;
                }    
                else
                    return compileError("Undeclared label");
            }
            else return compileError("Expected a label after jump instruction");
        } // TODO call and ret
        else if( op == "call" )
        {
            readToken();
            if( current.type == LABEL ) // verify it has been declared
            {
                if( declared_labels.count( current.text ) == 1 ) // the label has been declared
                {
                    uint16_t address = declared_labels[ current.text ]; // get the instruction address

                    instruction |= 0x90009000;  // push ip
                    program.push_back( instruction );

                    instruction = 0x32900000;        // copy immediate, reg
                    instruction |= address;
                    program.push_back( instruction ); // copy address, ip
                    
                    readToken();
                    return true;
                }
                else
                    return compileError("Undeclared label");
            }
            else 
                return compileError("Expected a label after call instruction");
        }
        else if( op == "ret" )
        {

        }
        return false;
    }



    // Called by parsePromptInstr
    bool Assembler::parseDispInstr( void )
    {
        uint32_t instruction = 0xC1000000; // 1 for disp and 2 for input
        uint8_t l_mode = 0; // source  mode, 0: value | 1: address | 2: reg | 3: dereferenced reg
        uint8_t r_mode = 0;    // display mode, 0: char | 1: integer | 2: address (unsigned int) | 3: string
        uint8_t reg       = 0;
        uint8_t offset = 0;        

        readToken(); // skip disp token


        if( current.type == AROBASE )    // disp @256, char  
        {
            readToken();                // skip @ token
            uint16_t value = parseValue();        // expect a value after @ symbol
            readComma();                // expect a comma
            l_mode = 1;
            instruction |= value;

        }
        else if( current.type == REG )
        {
            reg = getRegInd( current.text );
            readToken();
            readComma();
            l_mode = 2;
            instruction |= static_cast<uint32_t>( reg << 4 );
        }
        else if( checkForDereferencement() )
        {
            readDereferencedReg( offset, reg );     
            readComma();
            l_mode = 3;
            instruction |= static_cast<uint32_t>( reg << 4 );
            instruction |= offset;
        }
        else if( current.type == DECIMAL_VALUE or current.type == HEXA_VALUE 
            or current.type == BINARY_VALUE )
        {
            uint16_t val = parseValue();
            readComma();
            l_mode = 0;
            instruction |= val;
        }
        else if( current.type == CHAR_VALUE ) // allow char values as left operands, ex: disp 'a', char or disp 'h', int
        {
            char c = parseCharValue(); 
            readComma();
            l_mode = 0;
            instruction |= static_cast<uint16_t>( c );
            instruction |= 0x01000000;
        }
        else
            return compileError("Unexpected operand : '" + current.text + "'");

        // we can assume that the first operand and the comma has been correctly processed
        // process second operand
        if( current.type == DISP_TYPE )
        {
            if     ( current.text == "char") r_mode = 0;
            else if( current.text == "int" ) r_mode = 1;
            else if( current.text == "mem" ) r_mode = 2;
            else if( current.text == "hex" ) r_mode = 3;
            else if( current.text == "bin" ) r_mode = 4;
            else if( current.text == "str" ) r_mode = 5;
            readToken();
        }
        else
            return compileError("Expected Display type, not '" + current.text + "'" );


        if( r_mode == 5 )
        {
            if( l_mode == 0 )
                return compileError("Cannot display immediate value as str, use char instead");
            if( l_mode == 2 )
                return compileError("Cannot display register as str, use char instead");
        }

        // add the modes to the instruction
        short modes = (l_mode << 4) + r_mode;
        instruction |= static_cast<uint32_t>( modes << 16 );

        program.push_back( instruction );
        return true;
    }

    // Called by parsePromptInstr
    bool Assembler::parseInputInstr( void )
    {
        uint32_t instruction = 0xC2000000; // 1 for disp and 2 for input
        uint8_t l_mode = 0; // source mode 0: immediate value | 1: address | 2: register 
        uint8_t r_mode = 0; // dest   mode 0: char | 1: int | 2: mem | 3: str    
        uint8_t reg       = 0;

        readToken(); // read input token

        if( current.type == REG )
        {
            l_mode = 2;
            reg = getRegInd( current.text );
            readToken(); // read reg
            readComma(); // expect a comma

            instruction |= static_cast<uint32_t>( reg << 4 );

            if( current.type == DISP_TYPE )
            {
                if     ( current.text == "char") r_mode = 0;
                else if( current.text == "int" ) r_mode = 1;
                else if( current.text == "mem" ) r_mode = 2;
                else if( current.text == "hex" ) r_mode = 3;
                else if( current.text == "bin" ) r_mode = 4;
                else if( current.text == "str" ) // not usable with a register destination
                    return compileError("Cannot use string input with a register");
                readToken();
            }
            else
                return compileError("Expected input type, not '"+current.text+"'");

        }
        else if( checkForDereferencement() )
        {
            l_mode = 3;
            uint8_t offset;
            readDereferencedReg( offset, reg );
            instruction |= offset;
            instruction |= static_cast<uint16_t>(reg << 4);
            readComma();

            if( current.type == DISP_TYPE )
            {
                if( current.text == "str" or current.text == "string")
                {
                    r_mode = 5;
                    readToken();
                }
                else
                    return compileError("Expected string type after address operand, not '" + current.text + "'");
            }
            else 
                return compileError("Expected string type after address operand, not '" + current.text + "'");

        }
        else if( current.type == AROBASE  )
        {
            l_mode = 1;
            readToken(); // read arobase
            uint16_t value = parseValue(); // read address value, and store it
            readComma();

            instruction |= value;

            if( current.type == DISP_TYPE )
            {
                if( current.text == "str" or current.text == "string")
                {
                    r_mode = 5;
                    readToken();
                }
                else
                    return compileError("Expected string type after address operand, not '" + current.text + "'");
            }
            else 
                return compileError("Expected string type after address operand, not '" + current.text + "'");
        }
        else
            return compileError("Expected registers token or adress, not '" + current.text + "'");

        // add the modes to the instruction 
        short modes = (l_mode << 4) + r_mode; // take 4bits each
        instruction |= static_cast<uint32_t>( modes << 16 );

        program.push_back( instruction );

        return true;
    }


    // opcode 12, DISP, INPUT
    bool Assembler::parsePromptInstr( void )
    {
        string op = parser::to_lower( current.text );
        if( op == "disp" )
            return parseDispInstr();
        else if( op == "input" )
            return parseInputInstr();
        return false;
    }

    // opcode 13, RAND
    bool Assembler::parseRandInstr( void )
    {
        uint32_t instruction = 0xD0000000;
        readToken(); // skip rand token

        if( current.type == REG )
        {
            uint16_t reg = getRegInd( current.text );
            instruction |= static_cast<uint32_t>( reg << 20 );
            readToken();
            if( current.type == COMMA ) // rand ax, 265     ax -> rand(0, 256)
            {
                instruction |= 0x02000000; // mode
                readComma();
                uint16_t max_value = parseValue();
                instruction |= max_value;  // immediate value

                program.push_back( instruction ); // store instruction
                return true;
            }
            else if( tokens[ j ].type == ENDL ) // rand ax
            {
                program.push_back( instruction ); // store instruction
                return true;
            }
            else
                return compileError("Junk after rand instruction, maybe a comma is missing between operands");
        }
        else
            return compileError("Expected register after rand instruction" );

        return false;
    }

    // opcode 14, WAIT
    bool Assembler::parseWaitInstr( void )
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


