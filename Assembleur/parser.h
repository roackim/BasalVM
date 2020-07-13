#pragma once
#include <string>

using std::string;

namespace parser
{
	// lower a string, allow for non-case-sensitive OP
	string to_lower( string s );

	bool matchOP( string op );

	// return true if op is a flag from basm, case sensitive ( flags are uppercased eg : EQU, ZRO )
	bool matchFlag( string op );

	// return true if op is "if" or "ifnot" , not case sensitive, used for conditionnal jump eg : jump start ifnot ZRO
	bool matchFlowCtrl( string op );
	bool matchReg( string op );
	// helpers functions
	bool isSpace( char c );
	bool isNumber( char c );
	bool isAlphaNumerical( char c );
	bool isAlpha( char c );
	bool isHexa( char c );
	// return true if the string correctly match, false ohterwise, allow for cascade matching
	bool matchDecimalValue( string s );
	bool matchHexaValue( string s );
	bool matchBinValue( string s );
	bool matchLabelDecl( string s );
	bool matchLabel( string s );

}
