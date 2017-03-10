#include "Scanner.h"
#include "Parser.h"

#include <iostream>
#include <cstdio>
#include <vector>
#include <sstream> 
#include <stdlib.h>
 
//#include "common.h"

using namespace std;


int main( ) { 

    Scanner myScanner;
    myScanner.MoveToNext();
    
    Parser myParser( myScanner );
    if( myScanner.GetCurrToken().m_Type == TokenTypes_EOF )
        {
        cout << "ERROR: Empty file";
        return 1;
        }
    while( myScanner.GetCurrToken().m_Type != TokenTypes_EOF )
        {
        if( myParser.ParseExpr() )
            {
            myParser.Print();
            cout << endl;
            }
        else
            {
            cout << myParser.GetErrorString();
            return 1;
            }
        }
    
  return 0;
}
