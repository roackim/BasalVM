#include <string>
#include "parser.h"
#include <algorithm>

using std::string;

namespace parser
{
	// lower a string, allow for non-case-sensitive OP
	string to_lower( string s ) 
	{
		std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c)
			{ return std::tolower(c); } // correct
					  );
		return s;
	}

	bool matchOP( string op )
	{
		op = to_lower( op ); // non case sensitive op
		return( op=="add" or op=="sub" or op=="cmp" or op=="copy" or op=="push" or op=="pop" or op=="mul" 
			 or op=="div" or op=="mod" or op=="and" or op=="or" or op=="not" or op=="xor" or op=="jump" 
			 or op=="call" or op=="ret" or op=="input" or op=="disp" or op=="rand" or op=="wait" );
	}

	// return true if op is a flag from basm, case sensitive ( flags are uppercased eg : EQU, ZRO )
	bool matchFlag( string op )
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


	bool matchReg( string op )
	{
		// string oplist = "ax|bx|cx|dx|ex|fx|di|si|ip|sp";
		return( op=="ax" or op=="bx" or op=="cx" or op=="dx" or op=="ex" or op=="fx" or op=="di" or op=="si" or op=="ip" or op=="sp" );
	}


	bool isSpace( const char& c )
	{
		if( c == ' ' or c == '\t' ) 
			return true;
		return false;
	}
	bool isNumber( const char& c )
	{
		return( c >= '0' and c <= '9' );
	}
	bool isAlphaNumerical( const char& c ) 	{
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
		if( not isNumber( s[off] )) return false; // must have atleast one number
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
		if( not isHexa( s[off] )) return false; // must have atleast one number
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
		if( s[off] != '0' and s[off] != '1' ) return false; // must have atleast one number
		for( unsigned i=off; i < s.length(); i++ )
		{
			if(( s[i] != '0') and ( s[i] != '1')) return false; // must have atleast one number
		}
		return true;
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
}
