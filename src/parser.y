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

%locations

%debug


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
    #include "transducer.h"
    #include "interval.h"
    #include "tools.h"
    #include "signal_expr.h"
    #include "robustness.h"

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

    constexpr const char* CYAN  = "\033[36m";
    constexpr const char* RED  = "\033[31m";
    constexpr const char* RESET = "\033[0m";
    
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
%token                 PARAM_EQ        "="
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
%left PLUS MINUS
%left MULT
%left UNARY_OPERATOR
%nonassoc LPAREN RPAREN



%type <STLRom::transducer*> signal stl_atom
%type <STLRom::transducer*> signal_expr signal_atom signal_addexpr signal_multexpr signal_unaryexpr
%type <STLRom::transducer*> constant_signal
%type <STLRom::transducer*> stl_formula
%type <STLRom::interval*>   interval
%type <std::string>            op
%type <std::string>            constant
/* %type <bool>           boolean */
/* %type <std::map<string,double>*>  local_param_assignements local_param_assignement_list local_param_assignement */

%start start

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
            $$ = new constant_transducer($1);
            $$->trace_data_ptr = &driver.data;
            $$->param_map = driver.param_map;
            $$->signal_map = driver.signal_map;
        }
        ;
        | PARAM_ID
        {
           $$ = new constant_transducer($1);
           $$->trace_data_ptr = &driver.data;
           $$->param_map = driver.param_map;
           $$->signal_map = driver.signal_map;
        };

signal: SIGNAL_ID LINT TIME RINT
        {
            $$ = new signal_transducer($1);

            $$->trace_data_ptr = &driver.data;
            $$->param_map = driver.param_map;
            $$->signal_map = driver.signal_map;


            // WARNING TODO:
            // this will never be called because if signal is not defined,
            // scanner will not return SIGNAL_ID token but rather a NEW_ID token
            // need to fix this properly later
            int i = driver.signal_map[$1];

            if (i==0) {
                cout << "Parsing error: unknown signal " << $1 << endl;
                delete $$;
                $$ = nullptr;
                YYERROR;
            }
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
            $$ = new abs_transducer($3);
            $$->trace_data_ptr = &driver.data;
            $$->param_map = driver.param_map;
            $$->signal_map = driver.signal_map;
        }
        | MINUS signal_unaryexpr %prec UNARY_OPERATOR /* unary operators only work with abs, constants, and parenthesized stuff, so not signal_expr */
        {
            $$ = new unary_minus_transducer($2);
            $$->trace_data_ptr = &driver.data;
            $$->param_map = driver.param_map;
            $$->signal_map = driver.signal_map;
        }
        | PLUS signal_unaryexpr %prec UNARY_OPERATOR
        {
            $$ = $2;
        }

signal_multexpr : signal_unaryexpr
        {
	      $$ = $1;
	    }
        | signal_multexpr MULT signal_unaryexpr /* changed for precedence */
          {
	      $$ = new mult_transducer($1, $3);
          $$->trace_data_ptr = &driver.data;
          $$->param_map = driver.param_map;
          $$->signal_map = driver.signal_map;
          }

signal_addexpr : signal_multexpr
        {
	      $$ = $1;
	    }
        | signal_addexpr PLUS signal_multexpr
          {
	      $$ = new plus_transducer($1, $3);
          $$->trace_data_ptr = &driver.data;
          $$->param_map = driver.param_map;
          $$->signal_map = driver.signal_map;
          }
        | signal_addexpr MINUS signal_multexpr
          {
	        $$ = new minus_transducer($1, $3);
            $$->trace_data_ptr = &driver.data;
            $$->param_map = driver.param_map;
            $$->signal_map = driver.signal_map;
          }

signal_expr : signal_addexpr
        {
            $$ = $1;
        }


stl_atom : signal_expr op signal_expr
          {
              $$ = new stl_atom($1, $2, $3);
              $$->trace_data_ptr = &driver.data;
              $$->param_map = driver.param_map;
              $$->signal_map = driver.signal_map;
          }
          ;

op        : LT { $$ = "<"; }
          | GT { $$ = ">"; }
          ;

interval : LINT constant COMMA constant RINT
         {
             $$ = new interval($2, $4);
         }
         ;
         | LINT constant constant RINT
         {
             $$ = new interval($2, $3);
         }

stl_formula :
             stl_atom
             {
                 $$ = $1;
             }
             | NOT stl_formula %prec NOT
             {
                 $$ = new not_transducer($2);
                 $$->trace_data_ptr = &driver.data;
                 $$->param_map = driver.param_map;
                 $$->signal_map = driver.signal_map;
             }
             | stl_formula AND stl_formula %prec AND
             {
                 $$ = new and_transducer($1, $3);
                 $$->trace_data_ptr = &driver.data;
                 $$->param_map = driver.param_map;
                 $$->signal_map = driver.signal_map;
             }
             | stl_formula OR stl_formula %prec AND
             {
                 $$ = new or_transducer($1, $3);
                 $$->trace_data_ptr = &driver.data;
                 $$->param_map = driver.param_map;
                 $$->signal_map = driver.signal_map;

             }
             | stl_formula IMPLIES stl_formula %prec AND
             {
                 $$ = new implies_transducer($1, $3);
                 $$->trace_data_ptr = &driver.data;
                 $$->param_map = driver.param_map;
                 $$->signal_map = driver.signal_map;

             }
             | DIAMOND interval stl_formula %prec DIAMOND
             {
                $$ = new ev_transducer($2, $3);
                $$->trace_data_ptr = &driver.data;
                 $$->param_map = driver.param_map;
                 $$->signal_map = driver.signal_map;

             }
             | BOX interval stl_formula %prec BOX
             {
                 $$ = new alw_transducer($2, $3);
                 $$->trace_data_ptr = &driver.data;
                 $$->param_map = driver.param_map;
                 $$->signal_map = driver.signal_map;

             }
             | stl_formula UNTIL interval stl_formula %prec UNTIL
             {
                $$ = new until_transducer($1, $3, $4);
                $$->trace_data_ptr = &driver.data;
                $$->param_map = driver.param_map;
                $$->signal_map = driver.signal_map;

             }
             | LPAREN stl_formula RPAREN
             {
                 $$ = $2;
             }
             | PHI_ID
             {

                 transducer * ref = driver.formula_map[$1];

                 if (ref==nullptr) {
                     cout << "Parsing error: unknown identifier " << $1 << endl;
                     $$ = nullptr;
                     YYERROR;
                 }
                 else {
                     $$ = ref->clone();
                     // TODO: copy variables (should be done in clone() no?)
                 }
             }
;


assignement : NEW_ID ASSIGN stl_formula
            {
                driver.formula_map[$1] = $3;
                if (driver.verbose_parser)
                    cout << CYAN << "Defined formula " << $1 << " = " << *$3 << RESET << endl;
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

/* boolean: BOOL_TRUE
       {
           $$ = true;
       }
       | BOOL_FALSE
       {
           $$ = false;
       } */

// Parameters

param_assignement: PARAM_ID PARAM_EQ CONSTANT
                 {
                    double val;
                    s_to_d( $3, val );
                    driver.param_map[$1] = val;
                    if (driver.verbose_parser)
                        cout << CYAN << "Parameter " << $1 << " re-assigned value " << val << RESET << endl;
                 }
                 | PARAM_ID PARAM_EQ MINUS CONSTANT
                 {
                    double val;
                    s_to_d( $4, val );
                    val = -val;
                    driver.param_map[$1] = val;
                    if (driver.verbose_parser)
                        cout << CYAN << "Parameter " << $1 << " re-assigned value " << val << RESET << endl;
                 }
                 | NEW_ID PARAM_EQ CONSTANT
                 {
                    double val;
                    s_to_d( $3, val );
                    driver.param_map[$1] = val;
                    if (driver.verbose_parser)
                        cout << CYAN << "New parameter " << $1 << " assigned value " << val << RESET << endl;
                 }
                 | NEW_ID PARAM_EQ MINUS CONSTANT
                 {
                    double val;
                    s_to_d( $4, val );
                    val = -val;
                    driver.param_map[$1] = val;
                    if (driver.verbose_parser)
                        cout << CYAN << "New parameter " << $1 << " assigned value " << val << RESET << endl;
                 }

param_assignement_list: param_assignement
                      | param_assignement_list COMMA param_assignement

param_assignements: PARAM_DECL param_assignement_list



/* local_param_assignement: PARAM_ID PARAM_EQ CONSTANT
*                 {
*
*                    double val;
*                    s_to_d( $3, val );
*
*                    $$ = new map<string,double>();
*                    (*$$)[$1] = val;
*                 }
*                 | NEW_ID PARAM_EQ CONSTANT
*                 {
*                    double val;
*                    s_to_d( $3, val );
*                    $$ = new map<string,double>();
*                    (*$$)[$1] = val;
*                 }
*
*
*local_param_assignement_list: local_param_assignement
*                            | local_param_assignement_list COMMA local_param_assignement
*{
*    $$ = new map<string,double>(*$1);
*    auto elem = $3->begin();
*    (*$$)[elem->first] = elem->second;
*}
*
*local_param_assignements: /* empty */
/*                       {
*                          $$ = new map<string,double>();
*                       }
*                       | LPAREN local_param_assignement_list RPAREN
*                       {
*                           $$ = $2;
*                       } */

// Signals

signal_new: NEW_ID
          {
                short idx =  driver.signal_map.size()+1;
                driver.signal_map[$1] = idx;
                if (driver.verbose_parser)
                    cout << CYAN << "Defined signal " << $1 << " with index " << idx << RESET << endl;
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
      | start END


%%

// Bison expects us to provide implementation - otherwise linker complains
void STLRom::Parser::error(const location &loc , const std::string &message) {	
        std::cerr << RED << "Error: " << message << endl << "At line " << loc.begin.line;
        if (loc.begin.column == loc.end.column - 1)
            std::cerr << ", column " << loc.begin.column << std::endl << RESET;
        else
            std::cerr << ", columns " << loc.begin.column << "-" << (loc.end.column - 1) << std::endl << RESET;
}
