#include "Parser.h"

#include <iostream>
#include <string>
using namespace std;
    
shared_ptr< parserNode > GenNode( NodeTypes type, const std::string& str, shared_ptr< abstractValData > pData = shared_ptr< abstractValData >() )
    {
    return shared_ptr< parserNode >( new ParserNode( type, str, pData ) );
    }
        
bool Parser::ParseExpr( void )
    {
    bool success = ParseExprRecursive( shared_ptr< ParserNode >() );
    if( !success )
        {
        return false;
        }
    if( !TypeCheck( m_pRoot ) )
        {
        cout << "Type error" << endl;
        return false;
        }
    m_pRoot = Evaluate( m_pRoot );
    if( !m_pRoot )
        {
        return false;   
        }
    
    return true;
    }

void Parser::Print( void )
    {
    PrintRecursive( m_pRoot );
    }

bool Parser::ParseExprRecursive( shared_ptr< ParserNode > pParent )
    {
    const Token& token = m_Scanner.GetCurrToken();

    if( token.m_Type == TokenTypes_LiteralAtoms || token.m_Type == TokenTypes_NumericAtoms )
        {
        string tokenStr( token.m_Str );
        NodeTypes nodeType;
        if( token.m_Type == TokenTypes_LiteralAtoms )
            {
            nodeType = NodeTypes_LiteralAtoms; 
            }
        else
            {
            nodeType = NodeTypes_NumericAtoms;
            }
        
        auto pNewNode = GenNode( nodeType, tokenStr );
        if( !pParent )
            {
            m_pRoot = pNewNode; 
            }
        else
            {
            pParent->LinkLeftChild( pNewNode );
            }
        
        m_Scanner.MoveToNext();
        return true;
        }
    
    if( token.m_Type == TokenTypes_OpenParentheses )
        {
        m_Scanner.MoveToNext();
        // an empty pair, add an NIL node and leave
        if( m_Scanner.GetCurrToken().m_Type == TokenTypes_ClosingParentheses )
            {
            m_Scanner.MoveToNext(); // remove closign parentheses
            // shared_ptr< ParserNode > pCurrNode( new ParserNode( NodeTypes_LiteralAtoms, "NIL" ) );
            auto pCurrNode = GenNode( NodeTypes_LiteralAtoms, "NIL" );
            if( pParent )
                {
                pParent->LinkLeftChild( pCurrNode );
                }
            else
                {
                m_pRoot = pCurrNode;
                }
            return true;
            }
        else // generate an right node for it
            {
            // shared_ptr< ParserNode > pCurrNode( shared_ptr< ParserNode >( new ParserNode( NodeTypes_Empty, "", AbstractVals_Unkown ) ) );
            auto pCurrNode = GenNode( NodeTypes_Empty, "" );
            if( !pParent ) // set the root node
                {
                m_pRoot = pCurrNode; 
                }
            else // generate a node for left child
                {
                pParent->LinkLeftChild( pCurrNode );
                }

            // generate an empty node
            while( m_Scanner.GetCurrToken().m_Type != TokenTypes_ClosingParentheses )
                {
                if( !ParseExprRecursive( pCurrNode ) ) 
                    {
                    return false;   
                    }
                // not the end of parentheses, add an right child and keep going
                if( m_Scanner.GetCurrToken().m_Type != TokenTypes_ClosingParentheses )
                    {
                    // generate an empty node
                    // move to right child
                    // pCurrNode->LinkRightChild( shared_ptr< ParserNode >( new ParserNode( NodeTypes_Empty, "", AbstractVals_Unkown ) ) );
                    pCurrNode->LinkRightChild( shared_ptr< ParserNode >( GenNode( NodeTypes_Empty, "" ) ) );
                    pCurrNode = pCurrNode->m_pRightChild;   
                    }
                }
            pCurrNode->LinkRightChild( GenNode( NodeTypes_LiteralAtoms, "NIL" ) );
                    
            m_Scanner.MoveToNext();
            return true;
            }
        }
    
    // Error handling
    if( token.m_Type == TokenTypes_ClosingParentheses ) // no matching parentheses
        {
        m_ErrorStr = "ERROR: Cannot find matching open parentheses";
        }
    else if( token.m_Type == TokenTypes_EOF ) // no matching parentheses
        {
        m_ErrorStr = "ERROR: Cannot find matching closing parentheses";  
        }
    else // invalid token
        {
        m_ErrorStr = "ERROR: Invalid token ";
        m_ErrorStr.append( token.m_Str );
        }
    return false;
    }
    
void Parser::PrintRecursive( std::shared_ptr< ParserNode > pCurrent ) const 
    {
    if( !pCurrent )
        {
        std::cout << "printing null node, return" << std::endl;
        return;
        }
    if( pCurrent->IsAtom() )
        {
        cout << pCurrent->m_Str;
        return;
        }
    
    unsigned int listIdx = 0u;
    while( pCurrent )
        {
        if( pCurrent->IsAtom() ) // Last node
            {
            if( pCurrent->IsNilAtom() )
                {
                cout << ")";
                }
            else
                {
                cout << " . " << pCurrent->m_Str << ")";
                }
            }
        else if( pCurrent->m_pLeftChild )
            {
            if( listIdx )
                {
                cout << " ";  
                }
            else
                {
                cout << "("; 
                }
            PrintRecursive( pCurrent->m_pLeftChild );
            // cout << " ";
            }
        pCurrent = pCurrent->m_pRightChild;
        ++listIdx;
        }
    
    return;
    }
    
bool Parser::BelongToGroup( const std::string& myOp, const std::vector< std::string >& opers ) const
    {
    if( !opers.size() )
        {
        return false;   
        }
    for( unsigned int i = 0; i < opers.size(); ++i )
        {
        if( !myOp.compare( opers[ i ] ) )
            {
            return true;
            }
        }
    return false;
    }

std::shared_ptr< parserNode > Parser::Arithmetic( std::shared_ptr< ParserNode > pS1, std::shared_ptr< ParserNode > pS2, const std::function< int ( int, int ) >& func ) 
    {
    if( !pS1->IsNumericAtom() || !pS2->IsNumericAtom() )
        {
        m_ErrorStr = "ERROR: ps1 or ps2 is not numeric atom";
        return shared_ptr< parserNode >();  
        }
    int num1 = stoi( pS1->m_Str );
    int num2 = stoi( pS2->m_Str );
    return GenNode( NodeTypes_NumericAtoms, std::to_string( func( num1, num2 ) ) );
    }
    
std::shared_ptr< parserNode > Parser::Compare( std::shared_ptr< ParserNode > pS1, std::shared_ptr< ParserNode > pS2, const std::function< bool ( int, int ) >& func ) const
    {
    int num1 = stoi( pS1->m_Str );
    int num2 = stoi( pS2->m_Str );
    if( func( num1, num2 ) )
        {
        return GenNode( NodeTypes_LiteralAtoms, "T" );
        }
    return  GenNode( NodeTypes_LiteralAtoms, "NIL" );
    }
    
std::shared_ptr< parserNode > Parser::Equal( std::shared_ptr< ParserNode > pS1, std::shared_ptr< ParserNode > pS2 )
    {
    if( !pS1->IsAtom() || !pS2->IsAtom() )
        {
        m_ErrorStr = "ERROR: eval( s1 ) or eval( s2 ) is not atom";
        return shared_ptr< parserNode >();  
        }
    if( ( !pS1->IsNumericAtom() && !pS1->IsLiteralAtom() ) || ( !pS2->IsNumericAtom() && !pS2->IsLiteralAtom() ) )
        {
        return shared_ptr< parserNode >(); 
        }
    if( !pS1->m_Str.compare( pS2->m_Str ) )
        {
        return GenNode( NodeTypes_LiteralAtoms, "T" );
        }
    return  GenNode( NodeTypes_LiteralAtoms, "NIL" );
    }
    
shared_ptr< parserNode > Parser::Atom( shared_ptr< ParserNode > pS1 ) const
    {
    if( pS1->IsAtom() )
        {
        return GenNode( NodeTypes_LiteralAtoms, "T" );
        }
    return  GenNode( NodeTypes_LiteralAtoms, "NIL" );
    }
    
shared_ptr< parserNode > Parser::Int( shared_ptr< ParserNode > pS1 ) const
    {
    if( pS1->IsNumericAtom() )
        {
        return GenNode( NodeTypes_LiteralAtoms, "T" );
        }
    return  GenNode( NodeTypes_LiteralAtoms, "NIL" );
    }
    
shared_ptr< parserNode > Parser::Nul( shared_ptr< ParserNode > pS1 ) const
    {
    if( pS1->IsNilAtom() )
        {
        return GenNode( NodeTypes_LiteralAtoms, "T" );
        }
    return  GenNode( NodeTypes_LiteralAtoms, "NIL" );
    }    
    
std::shared_ptr< parserNode > Parser::Cons( std::shared_ptr< ParserNode > pS1, std::shared_ptr< ParserNode > pS2 ) const
    {
    auto root = GenNode( NodeTypes_Empty, "" );
    root->LinkChildren( pS1, pS2 );
    return root;
    }
    
shared_ptr< parserNode > Parser::Car( shared_ptr< ParserNode > pNode ) const
    {
    if( !pNode->m_pLeftChild )
        {
        return shared_ptr< parserNode >();
        }
    return  pNode->m_pLeftChild;
    }
    
shared_ptr< parserNode > Parser::Cdr( shared_ptr< ParserNode > pNode ) const
    {
    if( !pNode->m_pRightChild )
        {
        return shared_ptr< parserNode >();
        }
    return  pNode->m_pRightChild;
    }

shared_ptr< parserNode > Parser::EvaluateCond( shared_ptr< ParserNode > pExp )
    {
    int listLen = pExp->GetListLength();
    if( listLen <= 0 )
        {
        m_ErrorStr = "ERROR: All bk are NIL or it's not valid list";
        return shared_ptr< parserNode >();   
        }
    // Test if each member in list is a list
    for( int i = 0; i < listLen; ++i )
        {
        auto s( pExp->GetListNode( i ) );
        int sLength = s->GetListLength();
        if( sLength != 2 )
            {
            m_ErrorStr = "ERROR: Sk is not a list or it's length is not 2";
            return shared_ptr< parserNode >(); 
            }
        }
    for( int i = 0; i < listLen; ++i )
        {
        auto s( pExp->GetListNode( i ) );
        shared_ptr< parserNode > evalB( Evaluate( s->GetListNode( 0u ) ) );
        if( !evalB )
            {
            return shared_ptr< parserNode >(); 
            }
        else if( !evalB->IsNilAtom() )
            {
            return Evaluate( s->GetListNode( 1u ) );
            }
        }
            
    m_ErrorStr = "ERROR: All bk are NIL";
    return shared_ptr< parserNode >();    
    }

bool Parser::Bound( shared_ptr< parserNode > pKey, shared_ptr< parserNode > pList ) const
    {
    if( !pKey || !pList )
        {
        return false;
        }
    int listLen = pList->GetListLength();
    if( listLen <= 0 )
        {
        return false;
        }
    for( int i = 0; i < listLen; ++i )
        {
        auto pCurrNode = pList->GetListNode( i );
        if( Car( pCurrNode )->Equal( *( pKey.get() ) )  )
            {
            return true;
            }
        }
    return false;
    }
 
 shared_ptr< parserNode > Parser::GetVal( shared_ptr< parserNode > pKey, shared_ptr< parserNode > pList ) const
    {
    if( !pKey || !pList )
        {
        return shared_ptr< parserNode >();
        }
    int listLen = pList->GetListLength();
    // std::cout << "finding " << pKey->m_Str << std::endl;
    if( listLen <= 0 )
        {
        return shared_ptr< parserNode >();
        }
    // std::cout << "Length = " << listLen << std::endl;
    for( int i = 0; i < listLen; ++i )
        {
        auto pCurrNode = pList->GetListNode( i );
        /* std::cout << "testing ";
        PrintRecursive( pCurrNode );
        std::cout << std::endl; */
        if( Car( pCurrNode )->Equal( *( pKey.get() ) )  )
            {
            return Cdr( pCurrNode );
            }
        }
    return shared_ptr< parserNode >();
    }
    
shared_ptr< parserNode > Parser::EvaluateList( shared_ptr< ParserNode > pExp )
    {
    // std::cout << "Evaluating list";
    // std::cout << "{" << std::endl;
    // PrintRecursive( pExp );
    // std::cout << std::endl;
    if( pExp->IsNilAtom() )
        {
        return GenNode( NodeTypes_LiteralAtoms, "NIL" );
        }
    auto pLeft = Evaluate( Car( pExp ) );
    if( !pLeft )
        {
        return shared_ptr< parserNode >();
        }
    auto pRight = EvaluateList( Cdr( pExp ) );
    if( !pRight )
        {
        return shared_ptr< parserNode >();
        }
    return Cons( pLeft, pRight );
    }
    
shared_ptr< parserNode > Parser::Apply( shared_ptr< ParserNode > pFunct,
                                    shared_ptr< ParserNode > pExp )
    {
    int listLen = pExp->GetListLength();
    if( listLen < 0 )
        {
        m_ErrorStr = "ERROR: exp is not a list";
        return shared_ptr< parserNode >();
        }
        
    if( BelongToGroup( pFunct->m_Str, { "PLUS", "MINUS", "TIMES", "LESS", "GREATER" } ) )
        {
        if( listLen != 2 )
            {
            m_ErrorStr = "ERROR: Invalid parameter list length, it must be 2";
            return shared_ptr< parserNode >(); 
            }
        auto evalLeft = pExp->GetListNode( 0u );
        auto evalRight = pExp->GetListNode( 1u );

        if( !evalLeft || !evalRight )
            {
            return shared_ptr< parserNode >(); 
            }
        if( !evalLeft->IsNumericAtom() || !evalRight->IsNumericAtom() )
            {
            m_ErrorStr = "ERROR: eval( s1 ) or eval( s2 ) is not numeric atom";
            
            return shared_ptr< parserNode >();  
            }
        if( !pFunct->m_Str.compare( "PLUS" ) )
            {
            return Arithmetic( evalLeft, evalRight, [ & ] ( int a, int b )->int{ return a + b; } );
            } 
        else if( !pFunct->m_Str.compare( "MINUS" ) )
            {
            return Arithmetic( evalLeft, evalRight, [ & ] ( int a, int b )->int{ return a - b; } ); 
            } 
        else if( !pFunct->m_Str.compare( "TIMES" ) )
            {
            return Arithmetic( evalLeft, evalRight, [ & ] ( int a, int b )->int{ return a * b; } );
            } 
        else if( !pFunct->m_Str.compare( "LESS" ) )
            {
            return Compare( evalLeft, evalRight, [ & ] ( int a, int b ) ->bool { return a < b; } );
            } 
        else if( !pFunct->m_Str.compare( "GREATER" ) )
            {
            return Compare( evalLeft, evalRight, [ & ] ( int a, int b ) ->bool { return a > b; } );
            } 
        else 
            {
            m_ErrorStr = "ERROR: Not supported operator ";
            m_ErrorStr.append( pFunct->m_Str );
            return shared_ptr< parserNode >();
            }

        }
    else if( BelongToGroup( pFunct->m_Str, { "EQ" } ) )
        {
        if( listLen != 2 )
            {
            m_ErrorStr = "ERROR: Invalid parameter list length, it must be 2";
            return shared_ptr< parserNode >(); 
            }
        auto evalLeft = pExp->GetListNode( 0u );
        auto evalRight = pExp->GetListNode( 1u );
        if( !evalLeft || !evalRight )
            {
            return shared_ptr< parserNode >();   
            } 
        if( !evalLeft->IsAtom() || !evalRight->IsAtom() )
            {
            m_ErrorStr = "ERROR: eval( s1 ) or eval( s2 ) is not atom";
            return shared_ptr< parserNode >();  
            }
        return Equal( evalLeft, evalRight );
        }
    else if( BelongToGroup( pFunct->m_Str, { "ATOM", "INT", "NULL" } ) )
        {
        if( listLen != 1 )
            {
            m_ErrorStr = "ERROR: Invalid parameter list length, it must be 1";
            return shared_ptr< parserNode >(); 
            }
        auto evalLeft = pExp->GetListNode( 0u );
        if( !evalLeft )
            {
            return shared_ptr< parserNode >();  
            }
        if( !pFunct->m_Str.compare( "ATOM" ) )
            {
            return Atom( evalLeft );
            }
        else if( !pFunct->m_Str.compare( "INT" ) )
            {
            return Int( evalLeft );
            }
        else if( !pFunct->m_Str.compare( "NULL" ) )
            {
            return Nul( evalLeft );
            }
        else 
            {
            m_ErrorStr = "ERROR: Not supported operator ";
            m_ErrorStr.append( pFunct->m_Str );
            return shared_ptr< parserNode >();
            }
        }
    else if( BelongToGroup( pFunct->m_Str, { "CAR", "CDR" } ) )
        {
        if( listLen != 1 )
            {
            m_ErrorStr = "ERROR: Invalid parameter list length, it must be 1";
            return shared_ptr< parserNode >(); 
            }
        auto evalLeft = pExp->GetListNode( 0u );
        if( !evalLeft )
            {
            return shared_ptr< parserNode >();  
            }
        if( evalLeft->IsAtom() )
            {
            m_ErrorStr = "ERROR: s1 is atom";
            return shared_ptr< parserNode >();  
            }
        if( !pFunct->m_Str.compare( "CAR" ) )
            {
            return Car( evalLeft );
            }
        else if( !pFunct->m_Str.compare( "CDR" ) )
            {
            return Cdr( evalLeft );
            }
        else 
            {
            m_ErrorStr = "ERROR: Not supported operator ";
            m_ErrorStr.append( pFunct->m_Str );
            return shared_ptr< parserNode >();
            }
        }
    else if( BelongToGroup( pFunct->m_Str, { "CONS" } ) )
        {
        if( listLen != 2 )
            {
            m_ErrorStr = "ERROR: Invalid paremeter list length, it must be 2";
            return shared_ptr< parserNode >(); 
            }
        auto evalLeft = pExp->GetListNode( 0u );
        auto evalRight = pExp->GetListNode( 1u );
        if( !evalLeft || !evalRight )
            {
            return shared_ptr< parserNode >();  
            }
        if( !pFunct->m_Str.compare( "CONS" ) )
            {
            return Cons( evalLeft, evalRight );
            }
        else 
            {
            m_ErrorStr = "ERROR: Not supported operator ";
            m_ErrorStr.append( pFunct->m_Str );
            return shared_ptr< parserNode >();
            }
        }
    
    m_ErrorStr = "ERROR: Undefined operator ";
    m_ErrorStr.append( pFunct->m_Str );
    return shared_ptr< parserNode >();
    }
    
shared_ptr< parserNode > Parser::Evaluate( shared_ptr< ParserNode > pExp )
    {
    /* std::cout << "Evaluating ";
    PrintRecursive( pExp );
    std::cout << std::endl; */
   // std::cout << "{" << std::endl;
    if( pExp->HasNoChild() )
        {
        switch ( pExp->m_Type )
            {
            case NodeTypes_LiteralAtoms:
                if( !pExp->IsNilAtom() && !pExp->IsT() && !pExp->IsF() )
                    {
                    m_ErrorStr = "ERROR: Cannot eval literal atom other than boolean var or empty list";
                    return shared_ptr< parserNode >();
                    }
                return pExp;
                break;
            case NodeTypes_NumericAtoms:
                if( Int( pExp )->IsT() )
                    {
                    // PrintRecursive( pExp );
                    return pExp;
                    }
                m_ErrorStr = "ERROR: Invalid numeric atom";
                return shared_ptr< parserNode >();                
                break;
            case NodeTypes_Empty: 
                m_ErrorStr = "ERROR: Invalid empty atom";
                return shared_ptr< parserNode >();
                break;
            };
        }
    int listLen = pExp->GetListLength();
    if( listLen < 1 )
        {
        m_ErrorStr = "ERROR: Invalid list length < 1 or it's not a list";
        return shared_ptr< parserNode >();
        }
    shared_ptr< parserNode > operatorName( Car( pExp ) );
    if( !operatorName )
        {
        m_ErrorStr = "ERROR: Illiegal operator or cannot find it";
        return shared_ptr< parserNode >();    
        }    
        
    if( BelongToGroup( operatorName->m_Str, { "COND" } ) )
        {
        return EvaluateCond( Cdr( pExp ) ); 
        }
    else // predefined function
        {
        if( !operatorName->IsAtom() )
            {
            m_ErrorStr = "ERROR: Invalid operator, which is not an atom";
            return shared_ptr< parserNode >();  
            }

        switch ( operatorName->m_Type )
            {
            case NodeTypes_LiteralAtoms:
                if( operatorName->IsNilAtom() || operatorName->IsT() )
                    {
                    m_ErrorStr = "ERROR: Invalid operator name which is boolean";
                    return shared_ptr< parserNode >();  
                    }
                break;
            case NodeTypes_NumericAtoms:
                m_ErrorStr = "ERROR: Invalid operator name which is numeric atom";
                return shared_ptr< parserNode >();                
                break;
            case NodeTypes_Empty: 
                m_ErrorStr = "ERROR: Invalid operator name which is empty";
                return shared_ptr< parserNode >();
                break;
            };
            
        std::vector< std::string > predefinedFuncs( { "CAR", "CDR", "CONS", "ATOM", "EQ", "NULL", "INT", "PLUS", "MINUS", 
                "TIMES", "LESS", "GREATER", "COND" } );
                
        if( !BelongToGroup( operatorName->m_Str, predefinedFuncs ) )
            {
            m_ErrorStr = "ERROR: Not supported operator";
            m_ErrorStr.append( operatorName->m_Str );
            return shared_ptr< parserNode >();       
            }
        
        auto pEvalList = EvaluateList( Cdr( pExp ) );
        if( !pEvalList )
            {
            return pEvalList;
            }
            
        return Apply( operatorName, pEvalList );
        }
    
    m_ErrorStr = "ERROR: Undefined operator ";
    m_ErrorStr.append( operatorName->m_Str );
    return shared_ptr< parserNode >();
    }
    
std::shared_ptr< parserNode > Parser::TypeCheckList( std::shared_ptr< ParserNode > pExp )
    {
    if( pExp->IsNilAtom() )
        {
        return GenNode( NodeTypes_LiteralAtoms, "NIL" );
        }
    auto pLeft = TypeCheck( Car( pExp ) );
    if( !pLeft )
        {
        return shared_ptr< parserNode >();
        }
    auto pRight = TypeCheckList( Cdr( pExp ) );
    if( !pRight )
        {
        return shared_ptr< parserNode >();
        }
    return Cons( pLeft, pRight );    
    }
    
std::shared_ptr< parserNode > Parser::TypeCheck( std::shared_ptr< ParserNode > pExp )
    {
    if( pExp->HasNoChild() )
        {
        switch ( pExp->m_Type )
            {
            case NodeTypes_LiteralAtoms:
                if( pExp->IsNilAtom() )
                    {
                    return GenNode( NodeTypes_LiteralAtoms, "NIL", make_shared< abstractValData >( AbstractVals_List, 0 ) );
                    }
                else if( pExp->IsT() )
                    {
                    return GenNode( NodeTypes_LiteralAtoms, "T", make_shared< abstractValData >( AbstractVals_True ) );
                    //pExp->m_pAbstractVal = make_shared< abstractValData >( AbstractVals_True );    
                    }
                else if( pExp->IsF() )
                    {
                    return GenNode( NodeTypes_LiteralAtoms, "F", make_shared< abstractValData >( AbstractVals_False) );
                    // pExp->m_pAbstractVal = make_shared< abstractValData >( AbstractVals_False );   
                    }
                else
                    {
                    m_ErrorStr = "ERROR: Cannot eval literal atom other than boolean or NIL";
                    return shared_ptr< parserNode >();    
                    }
                return pExp;
                break;
            case NodeTypes_NumericAtoms:
                if( Int( pExp )->IsT() )
                    {
                    return GenNode( NodeTypes_NumericAtoms, "0", make_shared< abstractValData >( AbstractVals_AnyNat ) );
                    // pExp->m_pAbstractVal = make_shared< abstractValData >( AbstractVals_AnyNat );   
                    // return pExp;
                    }
                m_ErrorStr = "ERROR: Invalid numeric atom";
                return shared_ptr< parserNode >();                
                break;
            case NodeTypes_Empty: 
                m_ErrorStr = "ERROR: Invalid empty atom";
                return shared_ptr< parserNode >();
                break;
            };
        }
    int listLen = pExp->GetListLength();
    if( listLen < 2 )
        {
        m_ErrorStr = "ERROR: Invalid list length < 2 or it's not a list";
        return shared_ptr< parserNode >();
        }
        
    shared_ptr< parserNode > pFunct( Car( pExp ) );
    if( !pFunct )
        {
        m_ErrorStr = "ERROR: Illiegal operator or cannot find it";
        return shared_ptr< parserNode >();    
        }    
    
    if( !pFunct->IsAtom() )
            {
            m_ErrorStr = "ERROR: Invalid operator, which is not an atom";
            return shared_ptr< parserNode >();  
            }

    switch ( pFunct->m_Type )
        {
        case NodeTypes_LiteralAtoms:
            if( pFunct->IsNilAtom() || pFunct->IsT() )
                {
                m_ErrorStr = "ERROR: Invalid operator name which is boolean";
                return shared_ptr< parserNode >();  
                }
            break;
        case NodeTypes_NumericAtoms:
            m_ErrorStr = "ERROR: Invalid operator name which is numeric atom";
            return shared_ptr< parserNode >();                
            break;
        case NodeTypes_Empty: 
            m_ErrorStr = "ERROR: Invalid operator name which is empty";
            return shared_ptr< parserNode >();
            break;
        };
            
    std::vector< std::string > predefinedFuncs( { "CAR", "CDR", "CONS", "ATOM", "EQ", "NULL", "INT", "PLUS", "MINUS", 
            "TIMES", "LESS", "GREATER", "COND" } );
                
    if( !BelongToGroup( pFunct->m_Str, predefinedFuncs ) )
        {
        m_ErrorStr = "ERROR: Not supported operator";
        m_ErrorStr.append( pFunct->m_Str );
        return shared_ptr< parserNode >();       
        }
    
    if( BelongToGroup( pFunct->m_Str, { "COND" } ) )
        {
         auto pCondList = pExp->GetListNode( 1 );
        // Test if each member in list is a list
        for( int i = 1; i < listLen; ++i )
            {
            auto s( pExp->GetListNode( i ) );
            int sLength = s->GetListLength();
            //PrintRecursive( s );
            //cout << "------------ " << i << endl;
            if( sLength != 2 )
                {
                m_ErrorStr = "ERROR: Sk is not a list or it's length is not 2";
                return shared_ptr< parserNode >(); 
                }
            }
            
        auto eAbsVal = AbstractVals_Unkown;
        int  eAbsListLen = -1;
        for( int i = 1; i < listLen; ++i )
            {
            auto s( pExp->GetListNode( i ) );
            
            shared_ptr< parserNode > evalB( TypeCheck( s->GetListNode( 0u ) ) );
            if( !evalB )
                {
                return shared_ptr< parserNode >(); 
                }
            auto evalBAbsVal = evalB->GetAbstractVal();
            if( evalBAbsVal != AbstractVals_AnyBool && evalBAbsVal != AbstractVals_True && evalBAbsVal != AbstractVals_False)
                {
                m_ErrorStr = "ERROR: Bk is not a boolean type";
                return shared_ptr< parserNode >(); 
                }    
                
            shared_ptr< parserNode > evalE( TypeCheck( s->GetListNode( 1u ) ) );
            if( !evalE )
                {
                return shared_ptr< parserNode >(); 
                }
            auto singleEVal = evalE->GetAbstractVal();
            if( singleEVal == AbstractVals_Unkown )
                {
                m_ErrorStr = "ERROR: Ek is invalid";
                return shared_ptr< parserNode >(); 
                }  
                
            if( i == 1 )
                {
                eAbsVal = singleEVal;
                eAbsListLen = evalE->GetAbstractListLen();
                }
            else
                {
                if( eAbsVal == AbstractVals_AnyNat || eAbsVal == AbstractVals_List )
                    {
                    if( eAbsVal != singleEVal )
                        {
                        m_ErrorStr = "ERROR: Ei does not have the same abstract value";
                        return shared_ptr< parserNode >(); 
                        }
                    eAbsListLen = min( eAbsListLen, evalE->GetAbstractListLen() );      
                    }   
                else // eAbsVal == bool group
                    {
                    if( singleEVal != AbstractVals_AnyBool && singleEVal != AbstractVals_True && singleEVal != AbstractVals_False )
                        {
                        m_ErrorStr = "ERROR: Ei does not have the same abstract value as bool";
                        return shared_ptr< parserNode >();     
                        } 
                    eAbsVal = AbstractVals_AnyBool;
                    }
                }
            }
        switch( eAbsVal )
            {
            case AbstractVals_AnyNat:
                return GenNode( NodeTypes_NumericAtoms, "1", make_shared< abstractValData >( eAbsVal ) );
                break;
            case AbstractVals_List:
                return GenNode( NodeTypes_Empty, "", make_shared< abstractValData >( eAbsVal, eAbsListLen ) );
                break;
            case AbstractVals_Unkown:
                return shared_ptr< parserNode >(); 
                break;
            default:
                return GenNode( NodeTypes_LiteralAtoms, "T", make_shared< abstractValData >( eAbsVal ) );
                break;
            };
        }
        
    auto pEvalList = TypeCheckList( Cdr( pExp ) );
    if( !pEvalList )
        {
        return pEvalList;
        }
            
    if( BelongToGroup( pFunct->m_Str, { "PLUS", "MINUS", "TIMES", "LESS", "GREATER" } ) )
        {
        if( listLen != 3 )
            {
            m_ErrorStr = "ERROR: Invalid parameter list length, it must be 3";
            return shared_ptr< parserNode >(); 
            }
        auto evalLeft = pEvalList->GetListNode( 0u );
        auto evalRight = pEvalList->GetListNode( 1u );

        if( !evalLeft || !evalRight )
            {
            return shared_ptr< parserNode >(); 
            }
                
        if( evalLeft->GetAbstractVal() != AbstractVals_AnyNat || evalRight->GetAbstractVal() != AbstractVals_AnyNat )
            {
            m_ErrorStr = "ERROR: eval( s1 ) or eval( s2 ) is not numeric atom";
            
            return shared_ptr< parserNode >();  
            }
        if( BelongToGroup( pFunct->m_Str, { "PLUS", "MINUS", "TIMES" } ) )
            {
            return GenNode( NodeTypes_NumericAtoms, "0", make_shared< abstractValData >( AbstractVals_AnyNat ) ); 
            }
        else // less or greater
            {
            return GenNode( NodeTypes_LiteralAtoms, "T", make_shared< abstractValData >( AbstractVals_AnyBool ) );    
            }
        }
    else if( BelongToGroup( pFunct->m_Str, { "EQ" } ) )
        {
        if( listLen != 3 )
            {
            m_ErrorStr = "ERROR: Invalid list length, it must be 3";
            return shared_ptr< parserNode >(); 
            }
        auto evalLeft = pEvalList->GetListNode( 0u );
        auto evalRight = pEvalList->GetListNode( 1u );
        if( !evalLeft || !evalRight )
            {
            return shared_ptr< parserNode >();   
            } 
        if( evalLeft->GetAbstractVal() != AbstractVals_AnyNat || evalRight->GetAbstractVal() != AbstractVals_AnyNat )
            {
            m_ErrorStr = "ERROR: eval( s1 ) or eval( s2 ) is not numeric atom";
            return shared_ptr< parserNode >();  
            }
        return GenNode( NodeTypes_LiteralAtoms, "T", make_shared< abstractValData >( AbstractVals_AnyBool ) );
        }
    else if( BelongToGroup( pFunct->m_Str, { "ATOM", "INT", "NULL" } ) )
        {
        if( listLen != 2 )
           {
            m_ErrorStr = "ERROR: Invalid list length, it must be 2";
            return shared_ptr< parserNode >(); 
            }
        auto evalLeft = pEvalList->GetListNode( 0u );
        if( !evalLeft )
            {
            return shared_ptr< parserNode >();  
            }
                
        auto abstractVal = evalLeft->GetAbstractVal();
        if( abstractVal == AbstractVals_Unkown )
            {
            m_ErrorStr = "ERROR: Non well typed parameter";
            return shared_ptr< parserNode >(); 
            }
        if( !pFunct->m_Str.compare( "ATOM" ) )
            {
            if( abstractVal != AbstractVals_List )
                {
                return GenNode( NodeTypes_LiteralAtoms, "T", make_shared< abstractValData >( AbstractVals_True ) );   
                }
            else
                {
                return GenNode( NodeTypes_LiteralAtoms, "F", make_shared< abstractValData >( AbstractVals_False ) ); 
                }
            }
        else if( !pFunct->m_Str.compare( "INT" ) )
            {
            if( abstractVal == AbstractVals_AnyNat )
                {
                return GenNode( NodeTypes_LiteralAtoms, "T", make_shared< abstractValData >( AbstractVals_True ) );   
                }
            else
                {
                return GenNode( NodeTypes_LiteralAtoms, "F", make_shared< abstractValData >( AbstractVals_False ) ); 
                }
            }
        else if( !pFunct->m_Str.compare( "NULL" ) )
            {
            if( abstractVal != AbstractVals_List )
                {
                m_ErrorStr = "ERROR: Input is not a list for NULL operator";
                return shared_ptr< parserNode >(); 
                }
                
            int abstractListLen = evalLeft->GetAbstractListLen(); 
            if( abstractListLen < 0 )
                {
                m_ErrorStr = "ERROR: Input is not a list for NULL operator";
                return shared_ptr< parserNode >();    
                }
                    
            if( abstractListLen == 0 ) // it should be anybool instead of True
                {
                return GenNode( NodeTypes_LiteralAtoms, "T", make_shared< abstractValData >( AbstractVals_AnyBool ) );       
                }  
            else
                {
                return GenNode( NodeTypes_LiteralAtoms, "F", make_shared< abstractValData >( AbstractVals_False ) ); 
                }
            }
        else 
            {
            m_ErrorStr = "ERROR: Not supported operator ";
            m_ErrorStr.append( pFunct->m_Str );
            return shared_ptr< parserNode >();
            }
        }
    else if( BelongToGroup( pFunct->m_Str, { "CAR", "CDR" } ) )
        {
        if( listLen != 2 )
            {
            m_ErrorStr = "ERROR: Invalid list length, it must be 2";
            return shared_ptr< parserNode >(); 
            }
                
        auto evalLeft = pEvalList->GetListNode( 0u );
        if( !evalLeft )
            {
            return shared_ptr< parserNode >();  
            }
                
        auto abstractVal = evalLeft->GetAbstractVal();
        if( abstractVal == AbstractVals_Unkown )
            {
            m_ErrorStr = "ERROR: Non well typed parameter";
            return shared_ptr< parserNode >(); 
            }
            
        if( abstractVal != AbstractVals_List )
            {
            m_ErrorStr = "ERROR: Parameter is not a list";
            return shared_ptr< parserNode >(); 
            }
            
        int abstractListLen = evalLeft->GetAbstractListLen();
        if( abstractListLen <= 0 )
            {
            m_ErrorStr = "ERROR: List length >= 0";
            return shared_ptr< parserNode >();         
            }
            
        if( !pFunct->m_Str.compare( "CAR" ) )
            {
            return GenNode( NodeTypes_NumericAtoms, "0", make_shared< abstractValData >( AbstractVals_AnyNat ) );
            }
        else if( !pFunct->m_Str.compare( "CDR" ) )
            {
            return GenNode( NodeTypes_Empty, "LIST", make_shared< abstractValData >( AbstractVals_List, abstractListLen - 1 ) );
            }
        else 
            {
            m_ErrorStr = "ERROR: Not supported operator ";
            m_ErrorStr.append( pFunct->m_Str );
            return shared_ptr< parserNode >();
            }
        }
    else if( BelongToGroup( pFunct->m_Str, { "CONS" } ) )
        {
        if( listLen != 3 )
            {
            m_ErrorStr = "ERROR: Invalid list length, it must be 3";
            return shared_ptr< parserNode >(); 
            }
        auto evalLeft = pEvalList->GetListNode( 0u );
        auto evalRight = pEvalList->GetListNode( 1u );
        if( !evalLeft || !evalRight )
            {
            return shared_ptr< parserNode >();  
            }
                
       if( evalLeft->GetAbstractVal() != AbstractVals_AnyNat || evalRight->GetAbstractVal() != AbstractVals_List )
            {
            m_ErrorStr = "ERROR: invalid parameter type for cons, left is not Nat or right is not Nat list";
            PrintRecursive( pExp );
            return shared_ptr< parserNode >();  
            }
                
        return GenNode( NodeTypes_Empty, "", make_shared< abstractValData >( AbstractVals_List, evalRight->GetAbstractListLen() + 1 ) );
        }

    m_ErrorStr = "ERROR: Undefined operator ";
    m_ErrorStr.append( pFunct->m_Str );
    return shared_ptr< parserNode >();    
    }
    
std::shared_ptr< ParserNode > Parser::AddPairs( std::shared_ptr< ParserNode > pXList, 
                                                std::shared_ptr< ParserNode > pYList,
                                                std::shared_ptr< ParserNode > pZList
                                                )
    {
    int xListLen = pXList->GetListLength();
    int yListLen = pYList->GetListLength();
    // std::cout << "xLen " << xListLen << " yLen " << yListLen << std::endl; 
    if( xListLen == -1 || yListLen == -1 || xListLen != yListLen )
        {
        m_ErrorStr = "ERROR: parameter list length not equal or either one of them is not a list";
        // std::cout << xListLen << " " << yListLen << std::endl;
        return std::shared_ptr< ParserNode >();
        }
    for( int i = xListLen - 1; i >= 0; --i )
        {
        auto pNewMember = Cons( pXList->GetListNode( i ), pYList->GetListNode( i ) );
        pZList = Cons( pNewMember, pZList );
        }
    // PrintRecursive( pZList );
    return pZList;
    }
