#pragma once

#include <memory> 
// #include <locale>
#include <string>

enum NodeTypes
    {
    NodeTypes_LiteralAtoms,
    NodeTypes_NumericAtoms,
    NodeTypes_Empty
    };

enum AbstractVals
    {
    AbstractVals_True,
    AbstractVals_False,
    AbstractVals_AnyBool,
    AbstractVals_AnyNat,
    AbstractVals_List,
    AbstractVals_Unkown
    };

typedef struct abstractValData
    {
    public:
        abstractValData( AbstractVals absVal, int listLen = 0 ) 
         : m_AbstractVal( absVal ), m_ListLenth( listLen ) 
            {
               
            }
    public:
        AbstractVals                m_AbstractVal;
        int                         m_ListLenth;
    }AbstractValData;
    
typedef struct parserNode
    {
    public:
        parserNode( NodeTypes type, const std::string& str, std::shared_ptr< abstractValData > pData )
            : m_Type( type ), m_Str( str ), m_pAbstractVal( pData )
            {
            } 
        std::shared_ptr< parserNode > GetListNode( unsigned int index ) const;
        bool Equal( const parserNode& other ) const;
        bool IsList( void );
        int GetListLength( void );
        bool HasNoChild( void ) const { return !m_pLeftChild && !m_pRightChild; }
        bool IsAtom( void ) const { return HasNoChild() && m_Type != NodeTypes_Empty; }
        bool IsLiteralAtom( void ) const {  return IsAtom() && m_Type == NodeTypes_LiteralAtoms; }
        bool IsT( void ) const { return IsLiteralAtom() && !m_Str.compare( "T" ); }
        bool IsF( void ) const { return IsLiteralAtom() && !m_Str.compare( "F" ); }
        bool IsNilAtom( void ) const { return IsLiteralAtom() && !m_Str.compare( "NIL" ); }
        // bool IsNumericAtom( void ) const {  std::locale loc; return IsAtom() && m_Type == NodeTypes_NumericAtoms && std::isdigit( m_Str[ 0 ], loc ); }
        bool IsNumericAtom( void ) const { return IsAtom() && m_Type == NodeTypes_NumericAtoms; }
        void LinkLeftChild( std::shared_ptr< parserNode > left );
        void LinkRightChild( std::shared_ptr< parserNode > right );
        void LinkChildren( std::shared_ptr< parserNode > left, std::shared_ptr< parserNode > right );
        AbstractVals GetAbstractVal( void ) const { if( m_pAbstractVal ) { return m_pAbstractVal->m_AbstractVal; } return AbstractVals_Unkown; }
        int GetAbstractListLen( void ) const { if( m_pAbstractVal ) { return m_pAbstractVal->m_ListLenth; } return -1; }
        
    public:
        parserNode*                 m_pParent;
        std::shared_ptr< parserNode >    m_pLeftChild;
        std::shared_ptr< parserNode >    m_pRightChild;
        NodeTypes                   m_Type;
        std::string                 m_Str;
        std::shared_ptr< abstractValData >    m_pAbstractVal;
        
    }ParserNode;