#include "ParserNode.h"

#include <iostream>

using namespace std;

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
    
    return -1;
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
    
bool ParserNode::Equal( const ParserNode& other ) const
    {
    if( m_Type != other.m_Type || m_Str.compare( other.m_Str ) )
        {
        return false;
        }
    if( m_pLeftChild )
        {
        if( !other.m_pLeftChild )
            {
            return false;    
            }
        if( !m_pLeftChild->Equal( *other.m_pLeftChild ) )
            {
            return false;   
            }
        }
    else
        {
        if( other.m_pLeftChild )
            {
            return false;    
            }
        }
    
    if( m_pRightChild )
        {
        if( !other.m_pRightChild )
            {
            return false;    
            }
        if( !m_pRightChild->Equal( *other.m_pRightChild ) )
            {
            return false;   
            }
        }
    else
        {
        if( other.m_pRightChild )
            {
            return false;    
            }
        }
    return true;    
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