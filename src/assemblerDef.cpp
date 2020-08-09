#include "assemblerDef.h"

#include <string>
#include <iostream>

using std::string;
using std::cout;
using std::endl;


namespace basm
{

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
        else if( reg == "r0" ) return 10;
        else if( reg == "r1" ) return 11;
        else if( reg == "r2" ) return 12;
        else if( reg == "r3" ) return 13;
        else if( reg == "r4" ) return 14;
        else if( reg == "r5" ) return 15;
        

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
                return "stop";
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

}

