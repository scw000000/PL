#pragma once
#include "Scanner.h"
#include "ParserNode.h"

#include <memory> 
#include <locale>
#include <vector>

// std::shared_ptr< parserNode > GenNode( NodeTypes type, const std::string& str, AbstractVals absVal = AbstractVals_Unkown, int listLen = 0 );

class Parser
    {
    public:
        Parser( Scanner& scanner ) : m_Scanner( scanner ){}
        bool ParseExpr( void );
        void Print( void );
        const std::string& GetErrorString( void ) const { return m_ErrorStr; };
        
    private:
        bool ParseExprRecursive( std::shared_ptr< ParserNode > pParent );
        void PrintRecursive( std::shared_ptr< ParserNode > pCurrent ) const;
        
        bool BelongToGroup( const std::string& myOp, const std::vector< std::string >& opers ) const;
        std::shared_ptr< parserNode > Arithmetic( std::shared_ptr< ParserNode > pS1, std::shared_ptr< ParserNode > pS2, const std::function< int ( int, int ) >& func );
        std::shared_ptr< parserNode > Compare( std::shared_ptr< ParserNode > pS1, std::shared_ptr< ParserNode > pS2, const std::function< bool ( int, int ) >& func ) const;
        std::shared_ptr< parserNode > Equal( std::shared_ptr< ParserNode > pS1, std::shared_ptr< ParserNode > pS2 );
        std::shared_ptr< parserNode > Atom( std::shared_ptr< ParserNode > pS1 ) const;
        std::shared_ptr< parserNode > Int( std::shared_ptr< ParserNode > pS1 ) const;
        std::shared_ptr< parserNode > Nul( std::shared_ptr< ParserNode > pS1 ) const;
        std::shared_ptr< parserNode > Cons( std::shared_ptr< ParserNode > pS1, std::shared_ptr< ParserNode > pS2 ) const;   
        std::shared_ptr< parserNode > Car( std::shared_ptr< ParserNode > pNode ) const;
        std::shared_ptr< parserNode > Cdr( std::shared_ptr< ParserNode > pNode ) const;
        std::shared_ptr< parserNode > EvaluateCond( std::shared_ptr< ParserNode > pExp );
        bool Bound( std::shared_ptr< parserNode > pKey, std::shared_ptr< parserNode > pList ) const;
        std::shared_ptr< parserNode > GetVal( std::shared_ptr< parserNode > pKey, std::shared_ptr< parserNode > pList ) const;                                               
        std::shared_ptr< parserNode > EvaluateList( std::shared_ptr< ParserNode > pExp );
        std::shared_ptr< parserNode > Apply( std::shared_ptr< ParserNode > pFunct,
                                                std::shared_ptr< ParserNode > pExp );
        std::shared_ptr< parserNode > Evaluate( std::shared_ptr< ParserNode > pExp );
        std::shared_ptr< parserNode > TypeCheckList( std::shared_ptr< ParserNode > pExp );
        std::shared_ptr< parserNode > TypeCheck( std::shared_ptr< ParserNode > pExp );
        
        std::shared_ptr< ParserNode > AddPairs(  std::shared_ptr< ParserNode > pXList, 
                        std::shared_ptr< ParserNode > pYList,
                        std::shared_ptr< ParserNode > pZList
                        );
        
    private:
        Scanner& m_Scanner;
        std::shared_ptr< ParserNode > m_pRoot;
        std::string m_ErrorStr;
        
        std::shared_ptr< ParserNode > m_pEvalRoot;
    };