//#include "common.h"
#include "Scanner.h"
#include <stdlib.h>
#include <iostream>

using namespace std;

Scanner::Scanner( void ) : m_CurrToken( TokenTypes_Error )
   {
   m_CurrIdx = 0u;
   }

void Scanner::Init()
    {
    ReadString();
    
    ReadToken();
    }

void Scanner::MoveToNext( void  )
    {
   /*  cout << "------- MoveToNext ---------" << endl; */
    if( m_CurrIdx >= m_CurrStr.size() )
        {
        ReadString();
        }
/*     cout << "!!!!!!!!!!!!!!!!!!!!" << endl;
    cout << "currStr-" << m_CurrStr << endl;
    cout << "strSize-" << m_CurrStr.size() << endl;
    cout << "currIdx-" << m_CurrIdx << endl;
    cout << "reading-" << m_CurrStr.substr( m_CurrIdx ) << endl;
    cout << "!!!!!!!!!!!!!!!!!!!!" << endl; */
    ReadToken();
    }
    
void Scanner::ReadString( void )
    {
    if( !( cin >> m_CurrStr ) )
        {
        m_CurrStr.clear();
        }
   /*  cout << "readStringFromCin-" << m_CurrStr << endl; */
    m_CurrIdx = 0u;
    }
    
void Scanner::ReadToken( void )
    {
   /*      cout << "------- ReadToken ---------" << endl; */
    if( !m_CurrStr.size() )
        {
        m_CurrToken = Token( TokenTypes_EOF );
        return;
        }
    unsigned int tempIndex = m_CurrIdx;
   /*  cout << "-------------------" << endl;
    cout << "currStr-" << m_CurrStr << endl;
    cout << "strSize-" << m_CurrStr.size() << endl;
    cout << "currIdx-" << m_CurrIdx << endl;
    cout << "reading-" << m_CurrStr.substr( m_CurrIdx ) << endl;
    cout << "-------------------" << endl; */
    // test for lexial token
    if( isalpha( m_CurrStr[ tempIndex ] ) )
        {
        ++tempIndex;
        
        // loop until 
        while( tempIndex < m_CurrStr.size() && ( isalpha( m_CurrStr[ tempIndex ] ) || isdigit( m_CurrStr[ tempIndex ] ) ) )
            {
            ++tempIndex;
            }
        string str( m_CurrStr.substr( m_CurrIdx, tempIndex - m_CurrIdx ) );
        // cout << str.substr( m_CurrIdx, tempIndex - m_CurrIdx ) << endl;
        m_CurrIdx = tempIndex;
        m_CurrToken = Token( TokenTypes_LiteralAtoms, str );
        }
    else if( isdigit( m_CurrStr[ tempIndex ] ) )
        {
        ++tempIndex;
        while( tempIndex < m_CurrStr.size() && ( isalpha( m_CurrStr[ tempIndex ] ) || isdigit( m_CurrStr[ tempIndex ] ) ) )
            {
            ++tempIndex;
            }
        unsigned int i = m_CurrIdx;
        while( i < tempIndex )
            {
            if( isalpha( m_CurrStr[ i ] ) )
                {
                break;
                }
            ++i;
            }
        string tokenStr( m_CurrStr.substr( m_CurrIdx, tempIndex - m_CurrIdx ) );
        
        if( i < tempIndex )
            {
            m_CurrToken = Token( TokenTypes_Error, tokenStr );
            }
        else
            {
            int num = atoi( tokenStr.c_str() );
           // cout << "numeric-" << num << endl;
            m_CurrToken = Token( TokenTypes_NumericAtoms, tokenStr, num );
            }
        m_CurrIdx = tempIndex;
        }
    else if( m_CurrStr[ tempIndex ] == '(' )
        {
        ++m_CurrIdx;
        // cout << str[ tempIndex ] << endl;
        m_CurrToken = Token( TokenTypes_OpenParentheses );
        }
    else if( m_CurrStr[ tempIndex ] == ')' )
        {
        ++m_CurrIdx;
        //  cout << str[ tempIndex ] << endl;
        m_CurrToken = Token( TokenTypes_ClosingParentheses );
        }
    }
