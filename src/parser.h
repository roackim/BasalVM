#pragma once
#include <string>

using std::string;

namespace parser
{
	// lower a string, allow for non-case-sensitive OP 
	string to_lower( string s );

	// return true if op is an instruction (not case sensitive) ex: PUSH PoP Add copy
	bool matchOP( string op );

	// return true if op is a flag from basm, case sensitive ( flags are uppercased eg : EQU, ZRO )
	bool matchFlag( const string& op );

	// return true if op is "if" or "ifnot" , not case sensitive, used for conditionnal jump eg : jump start ifnot ZRO
	bool matchFlowCtrl( string op );
	bool matchReg( const string& op );
	// helpers functions used to replaced regex, because of terrible performance (i dont know why std::regex was so slow, i put in parser.cpp/h the code that replace the regex)
	bool isSpace( const char& c );
	bool isNumber( const char& c );
	bool isAlphaNumerical( const char& c );
	bool isAlpha( const char& c );
	bool isHexa( const char& c );
	// return true if the string correctly match, false ohterwise, allow for cascade matching
	bool matchDecimalValue( const string& s );
	bool matchHexaValue( const string& s );
	bool matchBinValue( const string& s );
	bool matchCharValue( const string& s );
	bool matchLabelDecl( const string& s );
	bool matchLabel( const string& s );

}
