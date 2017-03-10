#include "Parser.h"

#include <iostream>
#include <string>
using namespace std;
//using namespace std::tr1;

bool ParserNode::IsList( void )
    {
    ParserNode* pNode = this;    
    while( pNode )
        {
        if( pNode->HasNoChild() )
            {
            if( !pNode->IsNilAtom() )
                {
                return false;   
                }
            return true;  
            }
        else
            {
            if( !pNode->m_pLeftChild )
                {
                return false;   
                }
            }
        pNode = pNode->m_pRightChild.get();  
        }
    
    return true;
    }

int ParserNode::GetListLength( void )
    {
    int ans = 0;
    ParserNode* pNode = this;    
    while( pNode )
        {
        if( pNode->HasNoChild() )
            {
            if( !pNode->IsNilAtom() )
                {
                return -1;   
                }
            return ans;  
            }
        else
            {
            if( !pNode->m_pLeftChild )
                {
                return -1;   
                }
            }
        pNode = pNode->m_pRightChild.get();
        ++ans;
        }
    
    return true;
    }
    
shared_ptr< ParserNode > ParserNode::GetListNode( unsigned int index ) const
    {
    if( HasNoChild() )
        {
        return shared_ptr< parserNode >();
        }
    if( !index )
        { 
        return m_pLeftChild;   
        }
    return m_pRightChild->GetListNode( --index );
    }
        
void ParserNode::LinkLeftChild( std::shared_ptr< ParserNode > left )
    {
    m_pLeftChild = left;
    if( m_pLeftChild )
        {
        m_pLeftChild->m_pParent = this;
        }
    }

void ParserNode::LinkRightChild( std::shared_ptr< ParserNode > right )
    {
    m_pRightChild = right;
    if( m_pRightChild )
        {
        m_pRightChild->m_pParent = this;
        }    
    }
    
void ParserNode::LinkChildren( std::shared_ptr< ParserNode > left, std::shared_ptr< ParserNode > right )
    {
    LinkLeftChild( left );
    LinkRightChild( right );
    }

bool Parser::ParseExpr( void )
    {
    bool success = ParseExprRecursive( shared_ptr< ParserNode >() );
    if( !success )
        {
        return false;
        }
    m_pRoot = Evaluate( m_pRoot, NULL, NULL );
    if( !m_pRoot )
        {
        return false;   
        }
    
    return true;
   // return Evaluate();
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
        // cout << "add token: " << tokenStr << endl;
        shared_ptr< ParserNode > pNewNode( new ParserNode( nodeType, tokenStr ) );
        if( !pParent )
            {
            m_pRoot = pNewNode; 
            }
        else
            {
            pParent->LinkLeftChild( pNewNode );
          //  pParent->m_pLeftChild = pNewNode;
         //   pNewNode->m_pParent = pParent.get();
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
            shared_ptr< ParserNode > pCurrNode( new ParserNode( NodeTypes_LiteralAtoms, "NIL" ) );
            
            if( pParent )
                {
               // pParent->m_pLeftChild = pCurrNode;
               // pCurrNode->m_pParent = pParent.get();
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
            shared_ptr< ParserNode > pCurrNode( shared_ptr< ParserNode >( new ParserNode( NodeTypes_Empty, "" ) ) );
            if( !pParent ) // set the root node
                {
                m_pRoot = pCurrNode; 
                }
            else // generate a node for left child
                {
                //pParent->m_pLeftChild = pCurrNode;
               // pParent->m_pLeftChild->m_pParent = pParent.get();
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
                    //pCurrNode->m_pRightChild = shared_ptr< ParserNode >( new ParserNode( NodeTypes_Empty, "" ) );
                    //pCurrNode->m_pRightChild->m_pParent = pCurrNode.get();
                    pCurrNode->LinkRightChild( shared_ptr< ParserNode >( new ParserNode( NodeTypes_Empty, "" ) ) );
                    pCurrNode = pCurrNode->m_pRightChild;   
                    }
                }
          //  pCurrNode->m_pRightChild = shared_ptr< ParserNode >( new ParserNode( NodeTypes_Nil, "NIL" ) );
          //  pCurrNode->m_pRightChild->m_pParent = pCurrNode.get();
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
    
shared_ptr< parserNode > Parser::GenNode( NodeTypes type, const std::string& str ) const
    {
    return shared_ptr< parserNode >( new ParserNode( type, str ) );
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

shared_ptr< parserNode > Parser::EvaluateCond(  shared_ptr< ParserNode > pExp,
                                                shared_ptr< ParserNode > pActual,
                                                shared_ptr< ParserNode > pDef
                                            )
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
        shared_ptr< parserNode > evalB( Evaluate( s->GetListNode( 0u ), pActual, pDef ) );
        if( !evalB )
            {
            return shared_ptr< parserNode >(); 
            }
        else if( !evalB->IsNilAtom() )
            {
            return Evaluate( s->GetListNode( 1u ), pActual, pDef );
            }
        }
            
    m_ErrorStr = "ERROR: All bk are NIL";
    return shared_ptr< parserNode >();    
    }

shared_ptr< parserNode > Parser::EvaluateList( shared_ptr< ParserNode > pExp,
                                       shared_ptr< ParserNode > pActual,
                                       shared_ptr< ParserNode > pDef
                                     )
    {
    std::cout << "Evaluating list";
   // std::cout << "{" << std::endl;
    PrintRecursive( pExp );
    std::cout << std::endl;
    if( pExp->IsNilAtom() )
        {
        auto p = GenNode( NodeTypes_LiteralAtoms, "NIL" );
      //  std::cout << "}" << std::endl;
        return p;
        /* return GenNode( NodeTypes_LiteralAtoms, "NIL" ); */
        }
    auto pLeft = Evaluate( Car( pExp ), pActual, pDef );
    if( !pLeft )
        {
        return shared_ptr< parserNode >();
        }
    auto pRight = EvaluateList( Cdr( pExp ),pActual, pDef );
    if( !pRight )
        {
        return shared_ptr< parserNode >();
        }
    auto p = Cons( pLeft, pRight );
   // std::cout << "}" << std::endl;
    return p;
   /*  return Cons( pLeft, pRight ); */
    }
    
shared_ptr< parserNode > Parser::Apply( shared_ptr< ParserNode > pFunct,
                                    shared_ptr< ParserNode > pExp,
                                    shared_ptr< ParserNode > pActual,
                                    shared_ptr< ParserNode > pDef
                                                )
    {
    std::cout << "Applying ";
    PrintRecursive( pExp );
    std::cout << std::endl;
    if( !pFunct->IsAtom() )
        {
        m_ErrorStr = "ERROR: Invalid function, which is not an atom";
        return shared_ptr< parserNode >();  
        }

    switch ( pFunct->m_Type )
        {
        case NodeTypes_LiteralAtoms:
            if( pFunct->IsNilAtom() || pFunct->IsT() )
                {
                m_ErrorStr = "ERROR: Invalid function name which is boolean";
                return shared_ptr< parserNode >();  
                }
            break;
        case NodeTypes_NumericAtoms:
            m_ErrorStr = "ERROR: Invalid function name which is numeric atom";
            return shared_ptr< parserNode >();                
            break;
        case NodeTypes_Empty: 
            m_ErrorStr = "ERROR: Invalid function name which is empty";
            return shared_ptr< parserNode >();
            break;
        };
 
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
            std::cout << "ddddasd" << std::endl;
            //PrintRecursive( pExp );
            return shared_ptr< parserNode >(); 
            }
        shared_ptr< parserNode > evalLeft( pExp->GetListNode( 0u ) );
        shared_ptr< parserNode > evalRight( pExp->GetListNode( 1u ) );

        if( !evalLeft || !evalRight )
            {
            return shared_ptr< parserNode >(); 
            }
        if( !evalLeft->IsNumericAtom() || !evalRight->IsNumericAtom() )
            {
            m_ErrorStr = "ERROR: eval( s1 ) or eval( s2 ) is not numeric atom";
            std::cout << "They are:::::: ";
            PrintRecursive( evalLeft );
            PrintRecursive( evalRight );
            std::cout << std::endl;
            
            return shared_ptr< parserNode >();  
            }
        std::function< int ( int, int )> func;
        if( !pFunct->m_Str.compare( "PLUS" ) )
            {
            return Arithmetic( evalLeft, evalRight, [ & ] ( int a, int b )->int{ return a + b; } );
            } 
        else if( !pFunct->m_Str.compare( "MINUS" ) )
            {
            //func = [ & ] ( int a, int b )->int{ return a - b; };
            auto p = Arithmetic( evalLeft, evalRight, [ & ] ( int a, int b )->int{ return a - b; } );
            //std::cout << "}" << std::endl;
            return p;
            /* return Arithmetic( evalLeft, evalRight, [ & ] ( int a, int b )->int{ return a - b; } );
             */
            } 
        else if( !pFunct->m_Str.compare( "TIMES" ) )
            {
            // func = [ & ] ( int a, int b )->int{ return a * b; };
            return Arithmetic( evalLeft, evalRight, [ & ] ( int a, int b )->int{ return a * b; } );
            } 
        else if( !pFunct->m_Str.compare( "LESS" ) )
            {
            // func = [ & ] ( int a, int b )->int{ return a + b; };
            return Compare( evalLeft, evalRight, [ & ] ( int a, int b ) ->bool { return a < b; } );
            } 
        else if( !pFunct->m_Str.compare( "GREATER" ) )
            {
            // func = [ & ] ( int a, int b )->int{ return a + b; };
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
        shared_ptr< parserNode > evalLeft( pExp->GetListNode( 0u ) );
        shared_ptr< parserNode > evalRight( pExp->GetListNode( 1u ) );
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
        shared_ptr< parserNode > evalLeft( pExp->GetListNode( 0u ) );
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
        shared_ptr< parserNode > evalLeft( pExp->GetListNode( 0u ) );
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
        shared_ptr< parserNode > evalLeft( pExp->GetListNode( 0u ) );
        shared_ptr< parserNode > evalRight( pExp->GetListNode( 1u ) );
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
    else // user-defined function
        {
        return shared_ptr< parserNode >();    
        }
    
    m_ErrorStr = "ERROR: Undefined operator ";
    m_ErrorStr.append( pFunct->m_Str );
    return shared_ptr< parserNode >();
    }
    
shared_ptr< parserNode > Parser::Evaluate( shared_ptr< ParserNode > pExp,
                                           shared_ptr< ParserNode > pActual,
                                           shared_ptr< ParserNode > pDef
                                         )
    {
    std::cout << "Evaluating ";
    PrintRecursive( pExp );
    std::cout << std::endl;
   // std::cout << "{" << std::endl;
    if( pExp->HasNoChild() )
        {
        switch ( pExp->m_Type )
            {
            case NodeTypes_LiteralAtoms:
                if( !pExp->IsNilAtom() && !pExp->IsT() )
                    {
                    std::cout << "!!!!!!!!!!!!!!!!!ddddaweeee" << std::endl;
                    m_ErrorStr = "ERROR: Cannot evaluate literal atom";
                    return shared_ptr< parserNode >();  
                    }
                return pExp;
                break;
            case NodeTypes_NumericAtoms:
                if( Int( pExp )->IsT() )
                    {
                  //  PrintRecursive( pExp );
                   // std::cout << "}" << std::endl;
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
    if( listLen < 2 )
        {
        m_ErrorStr = "ERROR: Invalid list length < 2 or it's not a list";
        std::cout << listLen << std::endl;
        return shared_ptr< parserNode >();
        }
    shared_ptr< parserNode > operatorName( Car( pExp ) );
    if( !operatorName )
        {
        m_ErrorStr = "ERROR: Cannot find operator";
        return shared_ptr< parserNode >();    
        }
        
    if( BelongToGroup( operatorName->m_Str, { "QUOTE" } ) )
        {
        if( listLen != 2 )
            {
            m_ErrorStr = "ERROR: Invalid list length, it must be 2";
            return shared_ptr< parserNode >(); 
            }
        return pExp->GetListNode( 1u );
        }
    else if( BelongToGroup( operatorName->m_Str, { "COND" } ) )
        {
        return EvaluateCond( Cdr( pExp ), pActual, pDef );
        /* for( int i = 1; i < listLen; ++i )
            {
            auto s( pExp->GetListNode( i ) );
            int sLength = s->GetListLength();
            if( sLength < 0 )
                {
                m_ErrorStr = "ERROR: Sk is not a list";
                return shared_ptr< parserNode >(); 
                }
            else if( sLength != 2 )
                {
                m_ErrorStr = "ERROR: Invalid list length of Sk, it must be 2";
                return shared_ptr< parserNode >(); 
                }
            }
        for( int i = 1; i < listLen; ++i )
            {
            auto s( pExp->GetListNode( i ) );
            shared_ptr< parserNode > evalB( Evaluate( s->GetListNode( 0u ), pActual, pDef ) );
            if( !evalB )
                {
                return shared_ptr< parserNode >(); 
                }
            else if( !evalB->IsNilAtom() )
                {
                return Evaluate( s->GetListNode( 1u ), pActual, pDef );
                }
            }
            
        m_ErrorStr = "ERROR: All bk are NIL";
        return shared_ptr< parserNode >();   */  
        }
    else if( BelongToGroup( operatorName->m_Str, { "DEFUN" } ) )
        {
        return shared_ptr< parserNode >();
        }
    else // Maybe user defined function
        {
        std::cout << "calling apply from ";
        PrintRecursive( pExp );
        std::cout << "calee: "<<std::endl;
        PrintRecursive( Cdr( pExp ) );
        std::cout << std::endl;
        auto pEvalList = EvaluateList( Cdr( pExp ), pActual, pDef );
        if( !pEvalList )
            {
            return pEvalList;
            }
        auto p = Apply( operatorName, pEvalList, pActual, pDef );
      //  std::cout << m_ErrorStr << std::endl;
        return p;
        /* return Apply( operatorName, EvaluateList( Cdr( pExp ), pActual, pDef ), pActual, pDef ); */
        }
    
    m_ErrorStr = "ERROR: Undefined operator ";
    m_ErrorStr.append( operatorName->m_Str );
    return shared_ptr< parserNode >();
    }
    
std::shared_ptr< ParserNode > Parser::AddPairs(  std::shared_ptr< ParserNode > pXList, 
                        std::shared_ptr< ParserNode > pYList,
                        std::shared_ptr< ParserNode > pZList
                        )
    {
    int xListLen = pXList->GetListLength();
    int yListLen = pYList->GetListLength();
    
    if( xListLen == -1 || yListLen == -1 || xListLen != yListLen )
        {
        m_ErrorStr = "parameter list length not equal or either one of them is not a list";
        return std::shared_ptr< ParserNode >();
        }
    for( unsigned int i = xListLen - 1; i >= 0; --i )
        {
        auto pNewMember = Cons( pXList->GetListNode( i ), pYList->GetListNode( i ) );
        pZList = Cons( pNewMember, pZList );
        }
    return pZList;
    }
