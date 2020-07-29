#pragma once

#include <string>
#include <iostream>

using std::string;


namespace basm
{

    // get register index
    uint8_t getRegInd( const string& reg );
    // get CPU flag index from string
    uint8_t getFlagInd( const string& flag );

    enum Type   // types of tokens
    {
        LABEL,
        OP,
        REG,
        CPUFLAG,
        COND,
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
        STOP,
        TIME,
        AROBASE,
        DISP_TYPE,
        CHAR_VALUE,
        UNKNOWN
    };

    class token // contains the type and the text 
    {
    public:
        string text;
        Type type = UNKNOWN;
        token( const string& s, Type t )
        : text( s )
        , type( t ) { }
        token()
        {
        }
    };

    // helper function, allow to get string from enum
    string getTypeStr( Type type );
}
