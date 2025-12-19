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
    #include "transducer.h"
    #include "tools.h"

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
%token                 PARAM_EQ        "'='"
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


%token <bool>          BOOL_TRUE       "true"
%token <bool>          BOOL_FALSE      "false"
%token <double>        DOUBLE		   "double"
%token <std::string>   CONSTANT        "constant"
%token <std::string>   PARAM_ID        "param_id"
%token <std::string>   PHI_ID          "phi_id"
%token <std::string>   NEW_ID          "new_id"
%token <std::string>   SIGNAL_ID       "signal_id"
%token <std::string>   STL_TEST_ID     "stl_test_id"
%token <std::string>   STRING		   "string"

%left AND
%left OR
%left NOT
%left IMPLIES
%left LT GT
%left BOX DIAMOND UNTIL ASSIGN
%nonassoc LPAREN RPAREN



%type <STLRom::Transducer*> signal stl_atom
%type <STLRom::Transducer*> signal_expr signal_atom signal_addexpr signal_multexpr signal_unaryexpr
%type <STLRom::Transducer*> constant_signal
%type <STLRom::Transducer*> stl_formula
%type <STLRom::Transducer*>   interval
%type <std::string>            op
%type <std::string>            constant
%type <bool>           boolean
%type <std::map<string,double>*>  local_param_assignements local_param_assignement_list local_param_assignement

%start start_semicolon

%%

constant : CONSTANT
        {
            $$ = $1;
        }
        ;
        | PARAM_ID
        {
           $$ = $1;
        };


constant_signal : CONSTANT
        {
            $$ = new Transducer($1);
        }
        ;
        | PARAM_ID
        {
           $$ = new Transducer($1);
        };

signal: SIGNAL_ID LINT TIME RINT
        {
            $$ = new Transducer($1);
        }
        ;

signal_atom : signal
        {
            $$ = $1;
        }
        ;
        | constant_signal
        {
            $$ = $1;
        }
        ;
        | LPAREN signal_expr RPAREN
        {
	       $$ = $2;
	    }

signal_unaryexpr : signal_atom
        {
	      $$ = $1;
	    }
        | ABS LPAREN signal_expr RPAREN
        {
            $$ = new Transducer("abs", $3);
        }

signal_multexpr : signal_unaryexpr
        {
	      $$ = $1;
	    }
        | signal_addexpr MULT signal_atom
          {
	      $$ = new Transducer("mult", $1, $3);
          }

signal_addexpr : signal_multexpr
        {
	      $$ = $1;
	    }
        | signal_addexpr PLUS signal_atom
          {
	      $$ = new Transducer("+", $1, $3);

          }
        | signal_addexpr MINUS signal_atom
          {
	      $$ = new Transducer("-", $1, $3);
          }

signal_expr : signal_addexpr
        {
            $$ = $1;
        }


stl_atom : signal_expr op signal_expr
          {
              $$ = new Transducer($2, $1, $3);
          }
          ;

op        : LT { $$ = "<"; }
          | GT { $$ = ">"; }
          ;

interval : LINT constant COMMA constant RINT
         {
             $$ = new Transducer("interval", new Transducer($2), new Transducer($4));
         }
         ;
         | LINT constant constant RINT
         {
             $$ = new Transducer("interval", new Transducer($2), new Transducer($3));
         }

stl_formula :
             stl_atom
             {
                 $$ = $1;
             }
             | NOT stl_formula %prec NOT
             {
                 $$ = new Transducer("not", $2);
             }
             | stl_formula AND stl_formula %prec AND
             {
                 $$ = new Transducer("and", $1, $3);
             }
             | stl_formula OR stl_formula %prec AND
             {
                 $$ = new Transducer("or", $1, $3);

             }
             | stl_formula IMPLIES stl_formula %prec AND
             {
                 $$ = new Transducer("implies", $1, $3);

             }
             | DIAMOND interval stl_formula %prec DIAMOND
             {
                $$ = new Transducer("ev", $2, $3);

             }
             | BOX interval stl_formula %prec BOX
             {
                 $$ = new Transducer("alw", $2, $3);

             }
             | stl_formula UNTIL interval stl_formula %prec UNTIL
             {
                 $$ = new Transducer("until", $1, $4);

             }
             | LPAREN stl_formula RPAREN
             {
                 $$ = $2;
             }
             | PHI_ID
             {

                 Transducer * ref = driver.formula_map[$1];

                 if (ref==nullptr) {
                     cout << "Parsing error: unknown identifier " << $1 << endl;
                     $$ = nullptr;
                     YYERROR;
                 }
                 else {
                     $$ = ref->clone();
                 }
             }
;


assignement : NEW_ID ASSIGN stl_formula
            {
                driver.formula_map[$1] = $3;
            }

/* trace_env: TEST NEW_ID ':' STRING
         {
             double x;
             driver.add_trace_test(*$2, *$4, 0., false);
             delete $2;
         }
         | TEST NEW_ID ':' STRING COMMA CONSTANT COMMA boolean  //test id: environment, simulation time, visualization 
         {
             double x;
             s_to_d(*$6, x);
             driver.add_trace_test(*$2, *$4, x, $8);
             delete $2;
             delete $4;
             delete $6;
         } */

boolean: BOOL_TRUE
       {
           $$ = true;
       }
       | BOOL_FALSE
       {
           $$ = false;
       }

// Parameters

param_assignement: PARAM_ID PARAM_EQ CONSTANT
                 {
                    double val;
                    s_to_d( $3, val );
                    driver.param_map[$1] = val;
                 }
                 | NEW_ID PARAM_EQ CONSTANT
                 {
                    double val;
                    s_to_d( $3, val );
                    driver.param_map[$1] = val;
                 }

param_assignement_list: param_assignement
                      | param_assignement_list COMMA param_assignement

param_assignements: PARAM_DECL param_assignement_list



local_param_assignement: PARAM_ID '=' CONSTANT
                 {

                    double val;
                    s_to_d( $3, val );

                    $$ = new map<string,double>();
                    (*$$)[*$1] = val;
                 }
                 | NEW_ID '=' CONSTANT
                 {
                    double val;
                    s_to_d( $3, val );
                    $$ = new map<string,double>();
                    (*$$)[$1] = val;
                 }


local_param_assignement_list: local_param_assignement
                            | local_param_assignement_list COMMA local_param_assignement
{
    $$ = new map<string,double>(*$1);
    auto elem = $3->begin();
    (*$$)[elem->first] = elem->second;
}

local_param_assignements: /* empty */
                       {
                           $$ = new map<string,double>();
                       }
                       | LPAREN local_param_assignement_list RPAREN
                       {
                           $$ = $2;
                       }

// Signals

signal_new: NEW_ID
          {
             short idx =  driver.signal_map.size()+1;
             driver.signal_map[$1] = idx;
          }
          | SIGNAL_ID
          {
          }

signal_new_list: signal_new
                      | signal_new_list COMMA signal_new

signal_decl: SIGNAL_DECL signal_new_list



start : assignement
      | param_assignements
      | signal_decl
      | start assignement
      | start param_assignements
      | start_semicolon

start_semicolon : start SEMICOLON
                {
                    cout << "Parsing completed successfully." << endl;
                    for (const auto& [k, v] : driver.param_map)
                        std::cout << k << " = " << v << '\n';
                    for (const auto& [k, v] : driver.signal_map)
                        std::cout << k << " = " << v << '\n';
                    for (const auto& [k, v] : driver.formula_map)
                        std::cout << k << " = " << (v ? v->toString() : "<null>") << '\n';
                }
                ;


/* program :   {
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
     */
%%

// Bison expects us to provide implementation - otherwise linker complains
void STLRom::Parser::error(const location &loc , const std::string &message) {
        
        // Location should be initialized inside scanner action, but is not in this example.
        // Let's grab location directly from driver class.
	// cout << "Error: " << message << endl << "Location: " << loc << endl;
	
        cout << "Error: " << message << endl << "Error location: " << driver.location() << endl;
}
