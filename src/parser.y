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

%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0"
%defines
%define api.parser.class { Parser }

%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define api.namespace { STLRom }
%code requires
{
    #include <iostream>
    #include <string>
    #include <vector>
    #include <stdint.h>
    #include "command.h"

    using namespace std;

    namespace STLRom {
        class Scanner;
        class STLDriver;
    }
}

// Bison calls yylex() function that must be provided by us to suck tokens
// from the scanner. This block will be placed at the beginning of IMPLEMENTATION file (cpp).
// We define this function here (function! not method).
// This function is called only inside Bison, so we make it static to limit symbol visibility for the linker
// to avoid potential linking conflicts.
%code top
{
    #include <iostream>
    #include "scanner.h"
    #include "parser.hpp"
    #include "stl_driver.h"
    #include "location.hh"
    
    // yylex() arguments are defined in parser.y
    static STLRom::Parser::symbol_type yylex(STLRom::Scanner &scanner, STLRom::STLDriver &driver) {
        return scanner.get_next_token();
    }
    
    // you can accomplish the same thing by inlining the code using preprocessor
    // x and y are same as in above static function
    // #define yylex(x, y) scanner.get_next_token()
    
    using namespace STLRom;
}

%lex-param { STLRom::Scanner &scanner }
%lex-param { STLRom::STLDriver &driver }
%parse-param { STLRom::Scanner &scanner }
%parse-param { STLRom::STLDriver &driver }
%locations
%define parse.trace
%define parse.error verbose

%define api.token.prefix {TOKEN_}

//%token END 0 "end of file"
//%token <std::string> STRING  "string";
%token <uint64_t> NUMBER "number";
%token SEMICOLON "semicolon";
//%token COMMA "comma";

%token                 LINT            "["
%token                 RINT            "]"
%token                 LPAREN          "("
%token                 RPAREN          ")"
%token                 LT              "<"
%token                 NOT             "not"
%token                 GT              ">"
%token                 AND             "and"
%token                 OR              "or"
%token                 IMPLIES         "=>"
%token                 BOX             "alw"
%token                 DIAMOND         "ev"
%token                 UNTIL           "until"
%token                 TIME            "time"
%token                 COMMA           ","
%token                 END	       0   "end of file"
%token                 EOL             "end of line"
%token                 ASSIGN          ":="
%token                 PLUS            "+"
%token                 MINUS           "-"
%token                 MULT            "*"
%token                 ABS             "abs"
%token                 PARAM_DECL      "param_decl"
%token                 SIGNAL_DECL     "signal_decl"
%token                 TEST            "test"

%token                 CONSTANT_IDENTIFIER "constant_identifier"


%token <bool>       BOOL_TRUE       "true"
%token <bool>       BOOL_FALSE      "false"
%token <double>     DOUBLE		   "double"
%token <std::string>     CONSTANT        "constant"
%token <std::string>     PARAM_ID        "param_id"
%token <std::string>     PHI_ID          "phi_id"
%token <std::string>     NEW_ID          "new_id"
%token <std::string>     SIGNAL_ID       "signal_id"
%token <std::string>     STL_TEST_ID     "stl_test_id"
%token <std::string> 	   STRING		   "string"

%left AND
%left OR
%left NOT
%left IMPLIES
%left LT GT
%left BOX DIAMOND UNTIL ASSIGN
%nonassoc LPAREN RPAREN



%type< STLRom::Command > command;
%type< std::vector<uint64_t> > arguments;

%start program

%%

program :   {
                cout << "*** RUN ***" << endl;
                cout << "Type function with list of parmeters. Parameter list can be empty" << endl
                     << "or contain positive integers only. Examples: " << endl
                     << " * function()" << endl
                     << " * function(1,2,3)" << endl
                     << "Terminate listing with ; to see parsed AST" << endl
                     << "Terminate parser with Ctrl-D" << endl;
                
                cout << endl << "prompt> ";
                
                driver.clear();
            }
        | program command
            {
                const Command &cmd = $2;
                cout << "command parsed, updating AST" << endl;
                driver.addCommand(cmd);
                cout << endl << "prompt> ";
            }
        | program SEMICOLON
            {
                cout << "*** STOP RUN ***" << endl;
                cout << driver.str() << endl;
            }
        ;


command : STRING LPAREN RPAREN
        {
            string &id = $1;
            cout << "ID: " << id << endl;
            $$ = Command(id);
        }
    | STRING LPAREN arguments RPAREN
        {
            string &id = $1;
            const std::vector<uint64_t> &args = $3;
            cout << "function: " << id << ", " << args.size() << endl;
            $$ = Command(id, args);
        }
    ;

arguments : NUMBER
        {
            uint64_t number = $1;
            $$ = std::vector<uint64_t>();
            $$.push_back(number);
            cout << "first argument: " << number << endl;
        }
    | arguments COMMA NUMBER
        {
            uint64_t number = $3;
            std::vector<uint64_t> &args = $1;
            args.push_back(number);
            $$ = args;
            cout << "next argument: " << number << ", arg list size = " << args.size() << endl;
        }
    ;
    
%%

// Bison expects us to provide implementation - otherwise linker complains
void STLRom::Parser::error(const location &loc , const std::string &message) {
        
        // Location should be initialized inside scanner action, but is not in this example.
        // Let's grab location directly from driver class.
	// cout << "Error: " << message << endl << "Location: " << loc << endl;
	
        cout << "Error: " << message << endl << "Error location: " << driver.location() << endl;
}
