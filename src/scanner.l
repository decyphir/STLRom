/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2014 Krzysztof Narkiewicz <krzysztof.narkiewicz@STLRom.com>
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * 
 */

%{
	#include <iostream>
	#include <cstdlib>
	#include "scanner.h"
	#include "stl_driver.h"
	#include "parser.hpp"
	#include "location.hh"

	using namespace std;

	// Original yyterminate() macro returns int. Since we're using Bison 3 variants
	// as tokens, we must redefine it to change type from `int` to `Parser::semantic_type`
	#define yyterminate() STLRom::Parser::make_END(yylloc);

	// This will track current scanner location.
	// Action is called when length of the token is known.
	#define YY_USER_ACTION \
        yylloc.step(); \
        yylloc.columns(yyleng);
        


%}

%option nodefault
%option noyywrap
%option c++
%option yyclass="Scanner"
%option prefix="STLRom_"

%option debug

%%

"<>"      { return STLRom::Parser::make_DIAMOND(yylloc); }
"<>_"     { return STLRom::Parser::make_DIAMOND(yylloc); }
"[]"      { return STLRom::Parser::make_BOX(yylloc); }
"[]_"     { return STLRom::Parser::make_BOX(yylloc); }
"["       { return STLRom::Parser::make_LINT(yylloc); }
"]"       { return STLRom::Parser::make_RINT(yylloc); }
"("       { return STLRom::Parser::make_LPAREN(yylloc); }
")"       { return STLRom::Parser::make_RPAREN(yylloc); }
"=>"      { return STLRom::Parser::make_IMPLIES(yylloc); }
"->"      { return STLRom::Parser::make_IMPLIES(yylloc); }
"<"       { return STLRom::Parser::make_LT(yylloc); }
">"       { return STLRom::Parser::make_GT(yylloc); }
"<="      { return STLRom::Parser::make_LT(yylloc); }
">="      { return STLRom::Parser::make_GT(yylloc); }
"!"       { return STLRom::Parser::make_NOT(yylloc); }
","       { return STLRom::Parser::make_COMMA(yylloc); }
"&"       { return STLRom::Parser::make_AND(yylloc); }
"|"       { return STLRom::Parser::make_OR(yylloc); }
":="      { return STLRom::Parser::make_ASSIGN(yylloc); }
"+"       { return STLRom::Parser::make_PLUS(yylloc); }
"-"       { return STLRom::Parser::make_MINUS(yylloc); }
"*"       { return STLRom::Parser::make_MULT(yylloc); }
"="     { return STLRom::Parser::make_PARAM_EQ(yylloc); }

"F"       { return STLRom::Parser::make_DIAMOND(yylloc); }
"F_"      { return STLRom::Parser::make_DIAMOND(yylloc); }
"ev"      { return STLRom::Parser::make_DIAMOND(yylloc); }
"ev_"     { return STLRom::Parser::make_DIAMOND(yylloc); }

"G"       { return STLRom::Parser::make_BOX(yylloc); }
"G_"      { return STLRom::Parser::make_BOX(yylloc); }
"alw"     { return STLRom::Parser::make_BOX(yylloc); }
"alw_"    { return STLRom::Parser::make_BOX(yylloc); }

"U"       { return STLRom::Parser::make_UNTIL(yylloc); }
"U_"      { return STLRom::Parser::make_UNTIL(yylloc); }
"until"   { return STLRom::Parser::make_UNTIL(yylloc); }
"until_"  { return STLRom::Parser::make_UNTIL(yylloc); }

"and"     { return STLRom::Parser::make_AND(yylloc); }
"or"      { return STLRom::Parser::make_OR(yylloc); }
"not"     { return STLRom::Parser::make_NOT(yylloc); }

"true"    { return STLRom::Parser::make_BOOL_TRUE(true, yylloc); }
"false"   { return STLRom::Parser::make_BOOL_FALSE(false, yylloc); }

"t"       { return STLRom::Parser::make_TIME(yylloc); }
"abs"     { return STLRom::Parser::make_ABS(yylloc); }
"param"   { return STLRom::Parser::make_PARAM_DECL(yylloc); }
"test"    { return STLRom::Parser::make_TEST(yylloc); }
"signal"  { return STLRom::Parser::make_SIGNAL_DECL(yylloc); }

[a-zA-Z][A-Za-z0-9_]*     { 

    if (m_driver.formula_map.find(yytext) != m_driver.formula_map.end()) {
        return STLRom::Parser::make_PHI_ID(yytext, yylloc);
    }
    else        
    if (m_driver.param_map.find(yytext) != m_driver.param_map.end()) {
        return STLRom::Parser::make_PARAM_ID(yytext, yylloc);
    }
    else 
    if (m_driver.signal_map.find(yytext) != m_driver.signal_map.end()) {
        return STLRom::Parser::make_SIGNAL_ID(yytext, yylloc);
    }
    else 
        return STLRom::Parser::make_NEW_ID(yytext, yylloc);;
       
}

[0-9]*\.[0-9]+     { 
    //yylval->stringVal = new std::string(yytext, yyleng);
    //cout <<  *yylval->stringVal  << " is a CONSTANT "<< endl; 
 
  //return token::CONSTANT;
  return STLRom::Parser::make_CONSTANT(yytext, yylloc);

}

[0-9]*\. {
    return STLRom::Parser::make_CONSTANT(yytext, yylloc);
}

[0-9]+             { 
    return STLRom::Parser::make_CONSTANT(yytext, yylloc);
}

\"(\\.|[^\\"])*\" { // "
    // TODO: check if stringVal can be constructed differently
    std::string stringVal(yytext+1, yyleng-2);
    return STLRom::Parser::make_STRING(stringVal, yylloc);
}


\n {
    yylloc.lines(yyleng);
    yylloc.step();
}

;   { return STLRom::Parser::make_SEMICOLON(yylloc); }

[ \t\r]+    { /* skip whitespace */ }

"#".*\n { 
    /* Comments; TODO check locations */
}

.           { 
                cout << "Scanner: unknown character [" << yytext << "]" << endl; 
            }
            
<<EOF>>     { return yyterminate(); }


%%
