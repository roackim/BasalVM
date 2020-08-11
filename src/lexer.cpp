#include <string>
#include <iostream>
#include "lexer.h"

using std::string;

namespace lexer
{

    // return lower cased string
    string to_lower( string s )
    {
        string low = s;
        for( unsigned i=0; i<s.length(); i++ )
        {
            if( s[i] >= 'A' and s[i] <= 'Z' )
            {
                low[i] += 32;
            }
        }
        return low;
    }

    bool matchOP( string op )
    {
        op = to_lower( op ); // non case sensitive op
        return( op=="add"  or op=="sub" or op=="cmp"   or op=="copy" or op=="push" or op=="pop" or op=="mul" 
             or op=="div"  or op=="mod" or op=="and"   or op=="or"   or op=="not"  or op=="xor" or op=="jump" 
             or op=="call" or op=="ret" or op=="input" or op=="disp" or op=="rand" or op=="wait" or op=="exit"
             or op=="halt" or op=="cls"  );
    }

    // return true if op is a flag from basm, case sensitive ( flags are uppercased eg : EQU, ZRO )
    bool matchFlag( const string& op )
    {
        // string flaglist = "EQU|ZRO|POS|NEG|OVF|ODD";
        return(  op=="EQU" or op=="ZRO" or op=="POS" or op=="NEG" or op=="OVF" or op=="ODD" );
    }

    // return true if op is "if" or "ifnot" , not case sensitive, used for conditionnal jump eg : jump start ifnot ZRO
    bool matchFlowCtrl( string op )
    {
        // string flowlist = "if|ifnot";
        op = to_lower( op ); // non case sensitive if and not
        return( op=="if" or op=="ifnot" );
    }

    bool matchReg( const string& op )
    {
        // string oplist = "ax|bx|cx|dx|ex|fx|di|si|ip|sp";
        return( op=="ax" or op=="bx" or op=="cx" or op=="dx" or op=="ex" 
                         or op=="fx" or op=="di" or op=="si" or op=="ip" 
                         or op=="sp" or op=="r0" or op=="r1" or op=="r2"
                         or op=="r3" or op=="r4" or op=="r5" );
    }

    bool isSpace( const char& c )
    {
        return( c == ' ' or c == '\t' );
    }
    bool isNumber( const char& c )
    {
        return( c >= '0' and c <= '9' );
    }
    bool isAlphaNumerical( const char& c )
    {
        return(( c >= 'a' and c <= 'z') or ( c >= 'A' and c <= 'Z' ) or ( c >= '0' and c <= '9'));
    }
    bool isAlpha( const char& c )
    {
        return(( c >= 'a' and c <= 'z') or ( c >= 'A' and c <= 'Z' ));
    }
    bool isHexa( const char& c )
    {
        return( ( c >= '0' and c <= '9' ) or ( c >= 'A' and c <= 'F' ));
    }

    bool matchDecimalValue( const string& s )
    {
        unsigned off = 0; // offset to start the loop approprietly ( skip special char at begining ex : -123 )

        if( s[0]  == '-' ) off = 1;
        if( not (s.length() > off )) return false; // must have atleast one number
        for( unsigned i=off; i < s.length(); i++ )
        {
            if( not isNumber( s[i] )) return false; // must have atleast one number
        }
        return true;
    }

    bool matchHexaValue( const string& s )
    {
        unsigned off = 0;

        if( s[0]  == '0' and s[1] == 'x') off = 2;
        else return false;
        if( not (s.length() > off )) return false; // must have atleast one number
        for( unsigned i=off; i < s.length(); i++ )
        {
            if( not isHexa( s[i] )) return false; // must have atleast one number
        }
        return true;
    }

    bool matchBinValue( const string& s )
    {
        unsigned off = 0;

        if( s[0]  == '0' and s[1] == 'b') off = 2;
        else return false;
        if( not (s.length() > off )) return false; // must have atleast one number
        for( unsigned i=off; i < s.length(); i++ )
        {
            if(( s[i] != '0') and ( s[i] != '1')) return false; // must have atleast one number
        }
        return true;
    }

    bool matchCharValue( const string& s )
    {
        if( s[0] != '\'' )
            return false;
        else
        {
            if( s.length() == 3 )
            {
                if( s[2] == '\'' ) 
                    return true;
                return false;
            }
            else if( s.length() == 4 )
            {
                if( s[3] == '\'' )
                    if( s[1] == '\\' )
                        return true;
                return false;
            }
            else
                return false;
        }
    }

    bool matchLabelDecl( const string& s )
    {
        unsigned off = 0;

        if( s[0]  == ':') off = 1;
        else return false;
        if( not isAlpha( s[off] ) ) return false; // must start with [a-Z] 
        for( unsigned i=off; i < s.length(); i++ )
        {
            if( not (isAlphaNumerical( s[i] ) or s[i] == '_') ) return false; // must have atleast one number
        }
        return true;
    }

    bool matchLabel( const string& s )
    {

        if( not isAlpha( s[0] ) ) return false; // must start with [a-Z] 
        for( unsigned i=0; i < s.length(); i++ )
        {
            if( not (isAlphaNumerical( s[i] ) or s[i] == '_') ) return false; // must have atleast one number
        }
        return true;
    }


    // can't have nested functions in C++ :( , this is a work-around
    void endWord( vector<string>& words, string& word ) // push word if not empty
    {
        if( word == "" ) return;
        words.push_back( word );
        word = "";  // empty variable for the next word
    }

    bool isEscaped( const string& line, const uint32_t& i )
    {
        return( i > 0 and line[i-1] == '\\');
    }

    // REFACTOR 
    // split a string with a delimiter 
    vector<string> splitLine( string line )
    {
        vector<string> words;
        string word = "";

       
        for( uint32_t i=0; i < line.length(); i++) // every char of the string
        {
            if( isSpace( line[i]) )
            {    
                endWord( words, word );
                while( isSpace( line[i+1]) ) 
                {
                    i++;    
                }
                continue;
            }
            else if( line[i] == ',' )
            {
                bool notEsc = not isEscaped( line, i );
                if( notEsc ) 
                    endWord( words, word );
                word += line[i];
                continue;
            }
            else if( line[i] == '@' )
            {
                bool notEsc = not isEscaped( line, i );
                if( notEsc ) 
                    endWord( words, word );
                word += line[i];
                if( notEsc ) 
                    endWord( words, word );
                continue;
            }
            else if( line[i] == ';' )
            {
                bool notEsc = not isEscaped( line, i );
                if( notEsc ) 
                    endWord( words, word );
                word += line[i];
                if( notEsc ) 
                    endWord( words, word );
                continue;
            }
            else if( line[i] == '(' )
            {
                bool notEsc = not isEscaped( line, i );
                if( notEsc ) 
                    endWord( words, word );
                word += line[i];
                if( notEsc ) 
                    endWord( words, word );
                continue;
            }
            else if( line[i] == ')' )
            {
                bool notEsc = not isEscaped( line, i );
                if( notEsc ) 
                    endWord( words, word );
                word += line[i];
                if( notEsc ) 
                    endWord( words, word );
                continue;
            }
            else if( line[i] == '#') // discard the rest of the line if the char is not escaped
            {
                bool notEsc = not isEscaped( line, i );
                if( notEsc )    // stop processing the line
                { 
                    endWord( words, word );
                    break; 
                }
                else            // the char is escaped, continue normaly
                    word += line[i];
                continue;
            }
            // default
            word += line[i];
        }
        endWord( words, word);
        word += ";"; // end of line token
        endWord( words, word);
        
        return words;
    }

    token tokenizeOneWord( string txt )
    {
        using namespace basm;
        Type type = UNKNOWN;
        if     ( txt == "," ) type = COMMA;
        else if( txt == "@" ) type = AROBASE;
        else if( txt == ";" ) type = ENDL;
        else if( txt == ":" ) type = COLON;
        else if( txt == "(" ) type = LPAREN;
        else if( txt == ")" ) type = RPAREN;
        else if( txt == "s" or txt == "ms"  or txt == "us") type = TIME;        // try to match time units 
        else if( txt == "int" or txt == "char" or txt == "mem" or txt == "str" 
                or txt == "hex" or txt == "bin" ) type = DISP_TYPE;         // try to display types 
        else if( matchOP( txt ))           type = OP;                // try to match op
        else if( matchFlag( txt ))         type = CPUFLAG;           // try to match Flags
        else if( matchFlowCtrl( txt ))     type = COND;              // try to match conditions for flags
        else if( matchReg( txt ))          type = REG;               // try to match registers
        else if( matchCharValue( txt ))    type = CHAR_VALUE;        // try to match char literals ex: 'a', '\n'
        else if( matchDecimalValue( txt )) type = DECIMAL_VALUE;     // try to match decimal values
        else if( matchHexaValue( txt ))    type = HEXA_VALUE;        // try to match hexa values
        else if( matchBinValue( txt ))     type = BINARY_VALUE;      // try to match binary values
        else if( matchLabelDecl( txt ))    type = LABEL_DECL;        // try to match label declaration ex:  :Hello_World_Proc
        else if( matchLabel( txt))         type = LABEL;             // try to match label call ex: jump Hello_World_Proc

        token ret( txt, type );
        return( ret ); 
    }
}
