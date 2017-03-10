#pragma once
#include "Token.h"

class Scanner
    {
    public:
        Scanner( void );

        void Init( void );
     
        void MoveToNext( void );

        const Token& GetCurrToken( void ) const{ return m_CurrToken; };
    private:
        void ReadString( void );
        void ReadToken( void );
        
    private:
        std::string m_CurrStr;
        unsigned int m_CurrIdx;
        Token m_CurrToken;
    };
