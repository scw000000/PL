#pragma once

#include <string>

enum TokenTypes
    {
    TokenTypes_LiteralAtoms,
    TokenTypes_NumericAtoms,
    TokenTypes_OpenParentheses,
    TokenTypes_ClosingParentheses,
    TokenTypes_Error,
    TokenTypes_EOF
    };

  class Token
    {
    public:
        Token( TokenTypes type, const std::string& str = "", int num = 0 ) : 
            m_Num( num ), m_Type( type ),  m_Str( str ){}
        
    public:
        int m_Num;
        TokenTypes m_Type;
        std::string m_Str;
    };
  
  