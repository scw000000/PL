#pragma once
#include "Scanner.h"

#include <memory> 
#include <locale>
#include <vector>

enum NodeTypes
    {
    NodeTypes_LiteralAtoms,
    NodeTypes_NumericAtoms,
    //NodeTypes_Nil,
    NodeTypes_Empty
    };

typedef struct parserNode
    {
    public:
        parserNode( NodeTypes type, const std::string& str )
            : m_Type( type ), m_Str( str ) 
            {
            
            }   
        std::shared_ptr< parserNode > GetListNode( unsigned int index ) const;
        bool IsList( void );
        int GetListLength( void );
        bool HasNoChild( void ) const { return !m_pLeftChild && !m_pRightChild; }
        bool IsAtom( void ) const { return HasNoChild() && m_Type != NodeTypes_Empty; }
        bool IsLiteralAtom( void ) const {  return IsAtom() && m_Type == NodeTypes_LiteralAtoms; }
        bool IsT( void ) const { return IsLiteralAtom() && !m_Str.compare( "T" ); }
        bool IsNilAtom( void ) const { return IsLiteralAtom() && !m_Str.compare( "NIL" ); }
        // bool IsNumericAtom( void ) const {  std::locale loc; return IsAtom() && m_Type == NodeTypes_NumericAtoms && std::isdigit( m_Str[ 0 ], loc ); }
        bool IsNumericAtom( void ) const { return IsAtom() && m_Type == NodeTypes_NumericAtoms; }
        void LinkLeftChild( std::shared_ptr< parserNode > left );
        void LinkRightChild( std::shared_ptr< parserNode > right );
        void LinkChildren( std::shared_ptr< parserNode > left, std::shared_ptr< parserNode > right );
        
    public:
        NodeTypes                   m_Type;
        parserNode*                 m_pParent;
        std::shared_ptr< parserNode >    m_pLeftChild;
        std::shared_ptr< parserNode >    m_pRightChild;
        std::string                 m_Str;
    }ParserNode;

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
        std::shared_ptr< parserNode > GenNode( NodeTypes type, const std::string& str ) const;
        std::shared_ptr< parserNode > Car( std::shared_ptr< ParserNode > pNode ) const;
        std::shared_ptr< parserNode > Cdr( std::shared_ptr< ParserNode > pNode ) const;
        std::shared_ptr< parserNode > EvaluateCond( std::shared_ptr< ParserNode > pExp,
                                                std::shared_ptr< ParserNode > pActual,
                                                std::shared_ptr< ParserNode > pDef
                                                );
        std::shared_ptr< parserNode > EvaluateList( std::shared_ptr< ParserNode > pExp,
                                                std::shared_ptr< ParserNode > pActual,
                                                std::shared_ptr< ParserNode > pDef
                                                );
        std::shared_ptr< parserNode > Apply( std::shared_ptr< ParserNode > pFunct,
                                                std::shared_ptr< ParserNode > pExp,
                                                std::shared_ptr< ParserNode > pActual,
                                                std::shared_ptr< ParserNode > pDef
                                                );
        std::shared_ptr< parserNode > Evaluate( std::shared_ptr< ParserNode > pExp,
                                                std::shared_ptr< ParserNode > pActual,
                                                std::shared_ptr< ParserNode > pDef
                                                );
        
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