// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

// "%code top" blocks.
#line 69 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"

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

#line 62 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"




#include "parser.hpp"




#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 42 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
namespace  STLRom  {
#line 162 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"

  /// Build a parser object.
   Parser :: Parser  (STLRom::Scanner &scanner_yyarg, STLRom::STLDriver &driver_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      scanner (scanner_yyarg),
      driver (driver_yyarg)
  {}

   Parser ::~ Parser  ()
  {}

   Parser ::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/



  // by_state.
   Parser ::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

   Parser ::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
   Parser ::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
   Parser ::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

   Parser ::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

   Parser ::symbol_kind_type
   Parser ::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

   Parser ::stack_symbol_type::stack_symbol_type ()
  {}

   Parser ::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_interval: // interval
        value.YY_MOVE_OR_COPY< STLRom::interval* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_constant_signal: // constant_signal
      case symbol_kind::S_signal: // signal
      case symbol_kind::S_signal_atom: // signal_atom
      case symbol_kind::S_signal_unaryexpr: // signal_unaryexpr
      case symbol_kind::S_signal_multexpr: // signal_multexpr
      case symbol_kind::S_signal_addexpr: // signal_addexpr
      case symbol_kind::S_signal_expr: // signal_expr
      case symbol_kind::S_stl_atom: // stl_atom
      case symbol_kind::S_stl_formula: // stl_formula
        value.YY_MOVE_OR_COPY< STLRom::transducer* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_BOOL_TRUE: // "true"
      case symbol_kind::S_BOOL_FALSE: // "false"
        value.YY_MOVE_OR_COPY< bool > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_DOUBLE: // "double"
        value.YY_MOVE_OR_COPY< double > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_CONSTANT: // "constant"
      case symbol_kind::S_PARAM_ID: // "param_id"
      case symbol_kind::S_PHI_ID: // "phi_id"
      case symbol_kind::S_NEW_ID: // "new_id"
      case symbol_kind::S_SIGNAL_ID: // "signal_id"
      case symbol_kind::S_STL_TEST_ID: // "stl_test_id"
      case symbol_kind::S_STRING: // "string"
      case symbol_kind::S_constant: // constant
      case symbol_kind::S_op: // op
      case symbol_kind::S_op_eq: // op_eq
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_NUMBER: // "number"
        value.YY_MOVE_OR_COPY< uint64_t > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

   Parser ::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_interval: // interval
        value.move< STLRom::interval* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_constant_signal: // constant_signal
      case symbol_kind::S_signal: // signal
      case symbol_kind::S_signal_atom: // signal_atom
      case symbol_kind::S_signal_unaryexpr: // signal_unaryexpr
      case symbol_kind::S_signal_multexpr: // signal_multexpr
      case symbol_kind::S_signal_addexpr: // signal_addexpr
      case symbol_kind::S_signal_expr: // signal_expr
      case symbol_kind::S_stl_atom: // stl_atom
      case symbol_kind::S_stl_formula: // stl_formula
        value.move< STLRom::transducer* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_BOOL_TRUE: // "true"
      case symbol_kind::S_BOOL_FALSE: // "false"
        value.move< bool > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_DOUBLE: // "double"
        value.move< double > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_CONSTANT: // "constant"
      case symbol_kind::S_PARAM_ID: // "param_id"
      case symbol_kind::S_PHI_ID: // "phi_id"
      case symbol_kind::S_NEW_ID: // "new_id"
      case symbol_kind::S_SIGNAL_ID: // "signal_id"
      case symbol_kind::S_STL_TEST_ID: // "stl_test_id"
      case symbol_kind::S_STRING: // "string"
      case symbol_kind::S_constant: // constant
      case symbol_kind::S_op: // op
      case symbol_kind::S_op_eq: // op_eq
        value.move< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_NUMBER: // "number"
        value.move< uint64_t > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
   Parser ::stack_symbol_type&
   Parser ::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_interval: // interval
        value.copy< STLRom::interval* > (that.value);
        break;

      case symbol_kind::S_constant_signal: // constant_signal
      case symbol_kind::S_signal: // signal
      case symbol_kind::S_signal_atom: // signal_atom
      case symbol_kind::S_signal_unaryexpr: // signal_unaryexpr
      case symbol_kind::S_signal_multexpr: // signal_multexpr
      case symbol_kind::S_signal_addexpr: // signal_addexpr
      case symbol_kind::S_signal_expr: // signal_expr
      case symbol_kind::S_stl_atom: // stl_atom
      case symbol_kind::S_stl_formula: // stl_formula
        value.copy< STLRom::transducer* > (that.value);
        break;

      case symbol_kind::S_BOOL_TRUE: // "true"
      case symbol_kind::S_BOOL_FALSE: // "false"
        value.copy< bool > (that.value);
        break;

      case symbol_kind::S_DOUBLE: // "double"
        value.copy< double > (that.value);
        break;

      case symbol_kind::S_CONSTANT: // "constant"
      case symbol_kind::S_PARAM_ID: // "param_id"
      case symbol_kind::S_PHI_ID: // "phi_id"
      case symbol_kind::S_NEW_ID: // "new_id"
      case symbol_kind::S_SIGNAL_ID: // "signal_id"
      case symbol_kind::S_STL_TEST_ID: // "stl_test_id"
      case symbol_kind::S_STRING: // "string"
      case symbol_kind::S_constant: // constant
      case symbol_kind::S_op: // op
      case symbol_kind::S_op_eq: // op_eq
        value.copy< std::string > (that.value);
        break;

      case symbol_kind::S_NUMBER: // "number"
        value.copy< uint64_t > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }

   Parser ::stack_symbol_type&
   Parser ::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_interval: // interval
        value.move< STLRom::interval* > (that.value);
        break;

      case symbol_kind::S_constant_signal: // constant_signal
      case symbol_kind::S_signal: // signal
      case symbol_kind::S_signal_atom: // signal_atom
      case symbol_kind::S_signal_unaryexpr: // signal_unaryexpr
      case symbol_kind::S_signal_multexpr: // signal_multexpr
      case symbol_kind::S_signal_addexpr: // signal_addexpr
      case symbol_kind::S_signal_expr: // signal_expr
      case symbol_kind::S_stl_atom: // stl_atom
      case symbol_kind::S_stl_formula: // stl_formula
        value.move< STLRom::transducer* > (that.value);
        break;

      case symbol_kind::S_BOOL_TRUE: // "true"
      case symbol_kind::S_BOOL_FALSE: // "false"
        value.move< bool > (that.value);
        break;

      case symbol_kind::S_DOUBLE: // "double"
        value.move< double > (that.value);
        break;

      case symbol_kind::S_CONSTANT: // "constant"
      case symbol_kind::S_PARAM_ID: // "param_id"
      case symbol_kind::S_PHI_ID: // "phi_id"
      case symbol_kind::S_NEW_ID: // "new_id"
      case symbol_kind::S_SIGNAL_ID: // "signal_id"
      case symbol_kind::S_STL_TEST_ID: // "stl_test_id"
      case symbol_kind::S_STRING: // "string"
      case symbol_kind::S_constant: // constant
      case symbol_kind::S_op: // op
      case symbol_kind::S_op_eq: // op_eq
        value.move< std::string > (that.value);
        break;

      case symbol_kind::S_NUMBER: // "number"
        value.move< uint64_t > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
   Parser ::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
   Parser ::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " ("
            << yysym.location << ": ";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
   Parser ::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
   Parser ::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
   Parser ::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
   Parser ::debug_stream () const
  {
    return *yycdebug_;
  }

  void
   Parser ::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


   Parser ::debug_level_type
   Parser ::debug_level () const
  {
    return yydebug_;
  }

  void
   Parser ::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

   Parser ::state_type
   Parser ::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
   Parser ::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
   Parser ::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
   Parser ::operator() ()
  {
    return parse ();
  }

  int
   Parser ::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            symbol_type yylookahead (yylex (scanner, driver));
            yyla.move (yylookahead);
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case symbol_kind::S_interval: // interval
        yylhs.value.emplace< STLRom::interval* > ();
        break;

      case symbol_kind::S_constant_signal: // constant_signal
      case symbol_kind::S_signal: // signal
      case symbol_kind::S_signal_atom: // signal_atom
      case symbol_kind::S_signal_unaryexpr: // signal_unaryexpr
      case symbol_kind::S_signal_multexpr: // signal_multexpr
      case symbol_kind::S_signal_addexpr: // signal_addexpr
      case symbol_kind::S_signal_expr: // signal_expr
      case symbol_kind::S_stl_atom: // stl_atom
      case symbol_kind::S_stl_formula: // stl_formula
        yylhs.value.emplace< STLRom::transducer* > ();
        break;

      case symbol_kind::S_BOOL_TRUE: // "true"
      case symbol_kind::S_BOOL_FALSE: // "false"
        yylhs.value.emplace< bool > ();
        break;

      case symbol_kind::S_DOUBLE: // "double"
        yylhs.value.emplace< double > ();
        break;

      case symbol_kind::S_CONSTANT: // "constant"
      case symbol_kind::S_PARAM_ID: // "param_id"
      case symbol_kind::S_PHI_ID: // "phi_id"
      case symbol_kind::S_NEW_ID: // "new_id"
      case symbol_kind::S_SIGNAL_ID: // "signal_id"
      case symbol_kind::S_STL_TEST_ID: // "stl_test_id"
      case symbol_kind::S_STRING: // "string"
      case symbol_kind::S_constant: // constant
      case symbol_kind::S_op: // op
      case symbol_kind::S_op_eq: // op_eq
        yylhs.value.emplace< std::string > ();
        break;

      case symbol_kind::S_NUMBER: // "number"
        yylhs.value.emplace< uint64_t > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // constant: "constant"
#line 178 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
        {
            yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > ();
        }
#line 769 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 3: // constant: "param_id"
#line 183 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
        {
           yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > ();
        }
#line 777 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 4: // constant_signal: "constant"
#line 189 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
        {
            yylhs.value.as < STLRom::transducer* > () = new constant_transducer(yystack_[0].value.as < std::string > ());
            yylhs.value.as < STLRom::transducer* > ()->trace_data_ptr = &driver.data;
            yylhs.value.as < STLRom::transducer* > ()->param_map = driver.param_map;
            yylhs.value.as < STLRom::transducer* > ()->signal_map = driver.signal_map;
        }
#line 788 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 5: // constant_signal: "param_id"
#line 197 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
        {
           yylhs.value.as < STLRom::transducer* > () = new constant_transducer(yystack_[0].value.as < std::string > ());
           yylhs.value.as < STLRom::transducer* > ()->trace_data_ptr = &driver.data;
           yylhs.value.as < STLRom::transducer* > ()->param_map = driver.param_map;
           yylhs.value.as < STLRom::transducer* > ()->signal_map = driver.signal_map;
        }
#line 799 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 6: // signal: "signal_id" "[" "time" "]"
#line 205 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
        {
            yylhs.value.as < STLRom::transducer* > () = new signal_transducer(yystack_[3].value.as < std::string > ());

            yylhs.value.as < STLRom::transducer* > ()->trace_data_ptr = &driver.data;
            yylhs.value.as < STLRom::transducer* > ()->param_map = driver.param_map;
            yylhs.value.as < STLRom::transducer* > ()->signal_map = driver.signal_map;


            // WARNING TODO:
            // this will never be called because if signal is not defined,
            // scanner will not return SIGNAL_ID token but rather a NEW_ID token
            // need to fix this properly later
            int i = driver.signal_map[yystack_[3].value.as < std::string > ()];

            if (i==0) {
                cout << "Parsing error: unknown signal " << yystack_[3].value.as < std::string > () << endl;
                delete yylhs.value.as < STLRom::transducer* > ();
                yylhs.value.as < STLRom::transducer* > () = nullptr;
                YYERROR;
            }
        }
#line 825 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 7: // signal_atom: signal
#line 229 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
        {
            yylhs.value.as < STLRom::transducer* > () = yystack_[0].value.as < STLRom::transducer* > ();
        }
#line 833 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 8: // signal_atom: constant_signal
#line 234 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
        {
            yylhs.value.as < STLRom::transducer* > () = yystack_[0].value.as < STLRom::transducer* > ();
        }
#line 841 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 9: // signal_atom: "(" signal_expr ")"
#line 239 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
        {
	       yylhs.value.as < STLRom::transducer* > () = yystack_[1].value.as < STLRom::transducer* > ();
	    }
#line 849 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 10: // signal_unaryexpr: signal_atom
#line 244 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
        {
	      yylhs.value.as < STLRom::transducer* > () = yystack_[0].value.as < STLRom::transducer* > ();
	    }
#line 857 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 11: // signal_unaryexpr: "abs" "(" signal_expr ")"
#line 248 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
        {
            yylhs.value.as < STLRom::transducer* > () = new abs_transducer(yystack_[1].value.as < STLRom::transducer* > ());
            yylhs.value.as < STLRom::transducer* > ()->trace_data_ptr = &driver.data;
            yylhs.value.as < STLRom::transducer* > ()->param_map = driver.param_map;
            yylhs.value.as < STLRom::transducer* > ()->signal_map = driver.signal_map;
        }
#line 868 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 12: // signal_unaryexpr: "-" signal_unaryexpr
#line 255 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
        {
            yylhs.value.as < STLRom::transducer* > () = new unary_minus_transducer(yystack_[0].value.as < STLRom::transducer* > ());
            yylhs.value.as < STLRom::transducer* > ()->trace_data_ptr = &driver.data;
            yylhs.value.as < STLRom::transducer* > ()->param_map = driver.param_map;
            yylhs.value.as < STLRom::transducer* > ()->signal_map = driver.signal_map;
        }
#line 879 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 13: // signal_unaryexpr: "+" signal_unaryexpr
#line 262 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
        {
            yylhs.value.as < STLRom::transducer* > () = yystack_[0].value.as < STLRom::transducer* > ();
        }
#line 887 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 14: // signal_multexpr: signal_unaryexpr
#line 267 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
        {
	      yylhs.value.as < STLRom::transducer* > () = yystack_[0].value.as < STLRom::transducer* > ();
	    }
#line 895 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 15: // signal_multexpr: signal_multexpr "*" signal_unaryexpr
#line 271 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
          {
	      yylhs.value.as < STLRom::transducer* > () = new mult_transducer(yystack_[2].value.as < STLRom::transducer* > (), yystack_[0].value.as < STLRom::transducer* > ());
          yylhs.value.as < STLRom::transducer* > ()->trace_data_ptr = &driver.data;
          yylhs.value.as < STLRom::transducer* > ()->param_map = driver.param_map;
          yylhs.value.as < STLRom::transducer* > ()->signal_map = driver.signal_map;
          }
#line 906 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 16: // signal_addexpr: signal_multexpr
#line 279 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
        {
	      yylhs.value.as < STLRom::transducer* > () = yystack_[0].value.as < STLRom::transducer* > ();
	    }
#line 914 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 17: // signal_addexpr: signal_addexpr "+" signal_multexpr
#line 283 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
          {
	      yylhs.value.as < STLRom::transducer* > () = new plus_transducer(yystack_[2].value.as < STLRom::transducer* > (), yystack_[0].value.as < STLRom::transducer* > ());
          yylhs.value.as < STLRom::transducer* > ()->trace_data_ptr = &driver.data;
          yylhs.value.as < STLRom::transducer* > ()->param_map = driver.param_map;
          yylhs.value.as < STLRom::transducer* > ()->signal_map = driver.signal_map;
          }
#line 925 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 18: // signal_addexpr: signal_addexpr "-" signal_multexpr
#line 290 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
          {
	        yylhs.value.as < STLRom::transducer* > () = new minus_transducer(yystack_[2].value.as < STLRom::transducer* > (), yystack_[0].value.as < STLRom::transducer* > ());
            yylhs.value.as < STLRom::transducer* > ()->trace_data_ptr = &driver.data;
            yylhs.value.as < STLRom::transducer* > ()->param_map = driver.param_map;
            yylhs.value.as < STLRom::transducer* > ()->signal_map = driver.signal_map;
          }
#line 936 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 19: // signal_expr: signal_addexpr
#line 298 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
        {
            yylhs.value.as < STLRom::transducer* > () = yystack_[0].value.as < STLRom::transducer* > ();
        }
#line 944 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 20: // stl_atom: signal_expr op signal_expr
#line 304 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
          {
              yylhs.value.as < STLRom::transducer* > () = new stl_atom(yystack_[2].value.as < STLRom::transducer* > (), yystack_[1].value.as < std::string > (), yystack_[0].value.as < STLRom::transducer* > ());
              yylhs.value.as < STLRom::transducer* > ()->trace_data_ptr = &driver.data;
              yylhs.value.as < STLRom::transducer* > ()->param_map = driver.param_map;
              yylhs.value.as < STLRom::transducer* > ()->signal_map = driver.signal_map;
          }
#line 955 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 21: // op: "<"
#line 312 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
               { yylhs.value.as < std::string > () = "<"; }
#line 961 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 22: // op: ">"
#line 313 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
               { yylhs.value.as < std::string > () = ">"; }
#line 967 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 23: // op: "=" "="
#line 314 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
                              { yylhs.value.as < std::string > () = "="; }
#line 973 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 24: // op_eq: "<="
#line 318 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
               { yylhs.value.as < std::string > () = ">"; }
#line 979 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 25: // op_eq: ">="
#line 319 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
               { yylhs.value.as < std::string > () = "<"; }
#line 985 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 26: // interval: "[" constant "," constant "]"
#line 323 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
         {
             yylhs.value.as < STLRom::interval* > () = new interval(yystack_[3].value.as < std::string > (), yystack_[1].value.as < std::string > ());
         }
#line 993 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 27: // interval: "[" constant constant "]"
#line 328 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
         {
             yylhs.value.as < STLRom::interval* > () = new interval(yystack_[2].value.as < std::string > (), yystack_[1].value.as < std::string > ());
         }
#line 1001 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 28: // stl_formula: signal_expr op_eq signal_expr
#line 334 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
             {
                auto atom = new stl_atom(yystack_[2].value.as < STLRom::transducer* > (), yystack_[1].value.as < std::string > (), yystack_[0].value.as < STLRom::transducer* > ());
                atom->trace_data_ptr = &driver.data;
                atom->param_map = driver.param_map;
                atom->signal_map = driver.signal_map;

                yylhs.value.as < STLRom::transducer* > () = new not_transducer(atom);
                yylhs.value.as < STLRom::transducer* > ()->trace_data_ptr = &driver.data;
                yylhs.value.as < STLRom::transducer* > ()->param_map = driver.param_map;
                yylhs.value.as < STLRom::transducer* > ()->signal_map = driver.signal_map;
             }
#line 1017 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 29: // stl_formula: stl_atom
#line 346 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
             {
                 yylhs.value.as < STLRom::transducer* > () = yystack_[0].value.as < STLRom::transducer* > ();
             }
#line 1025 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 30: // stl_formula: "not" stl_formula
#line 350 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
             {
                 yylhs.value.as < STLRom::transducer* > () = new not_transducer(yystack_[0].value.as < STLRom::transducer* > ());
                 yylhs.value.as < STLRom::transducer* > ()->trace_data_ptr = &driver.data;
                 yylhs.value.as < STLRom::transducer* > ()->param_map = driver.param_map;
                 yylhs.value.as < STLRom::transducer* > ()->signal_map = driver.signal_map;
             }
#line 1036 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 31: // stl_formula: stl_formula "and" stl_formula
#line 357 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
             {
                 yylhs.value.as < STLRom::transducer* > () = new and_transducer(yystack_[2].value.as < STLRom::transducer* > (), yystack_[0].value.as < STLRom::transducer* > ());
                 yylhs.value.as < STLRom::transducer* > ()->trace_data_ptr = &driver.data;
                 yylhs.value.as < STLRom::transducer* > ()->param_map = driver.param_map;
                 yylhs.value.as < STLRom::transducer* > ()->signal_map = driver.signal_map;
             }
#line 1047 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 32: // stl_formula: stl_formula "or" stl_formula
#line 364 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
             {
                 yylhs.value.as < STLRom::transducer* > () = new or_transducer(yystack_[2].value.as < STLRom::transducer* > (), yystack_[0].value.as < STLRom::transducer* > ());
                 yylhs.value.as < STLRom::transducer* > ()->trace_data_ptr = &driver.data;
                 yylhs.value.as < STLRom::transducer* > ()->param_map = driver.param_map;
                 yylhs.value.as < STLRom::transducer* > ()->signal_map = driver.signal_map;

             }
#line 1059 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 33: // stl_formula: stl_formula "=>" stl_formula
#line 372 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
             {
                 yylhs.value.as < STLRom::transducer* > () = new implies_transducer(yystack_[2].value.as < STLRom::transducer* > (), yystack_[0].value.as < STLRom::transducer* > ());
                 yylhs.value.as < STLRom::transducer* > ()->trace_data_ptr = &driver.data;
                 yylhs.value.as < STLRom::transducer* > ()->param_map = driver.param_map;
                 yylhs.value.as < STLRom::transducer* > ()->signal_map = driver.signal_map;

             }
#line 1071 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 34: // stl_formula: "ev" interval stl_formula
#line 380 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
             {
                yylhs.value.as < STLRom::transducer* > () = new ev_transducer(yystack_[1].value.as < STLRom::interval* > (), yystack_[0].value.as < STLRom::transducer* > ());
                yylhs.value.as < STLRom::transducer* > ()->trace_data_ptr = &driver.data;
                 yylhs.value.as < STLRom::transducer* > ()->param_map = driver.param_map;
                 yylhs.value.as < STLRom::transducer* > ()->signal_map = driver.signal_map;

             }
#line 1083 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 35: // stl_formula: "alw" interval stl_formula
#line 388 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
             {
                 yylhs.value.as < STLRom::transducer* > () = new alw_transducer(yystack_[1].value.as < STLRom::interval* > (), yystack_[0].value.as < STLRom::transducer* > ());
                 yylhs.value.as < STLRom::transducer* > ()->trace_data_ptr = &driver.data;
                 yylhs.value.as < STLRom::transducer* > ()->param_map = driver.param_map;
                 yylhs.value.as < STLRom::transducer* > ()->signal_map = driver.signal_map;

             }
#line 1095 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 36: // stl_formula: stl_formula "until" interval stl_formula
#line 396 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
             {
                yylhs.value.as < STLRom::transducer* > () = new until_transducer(yystack_[3].value.as < STLRom::transducer* > (), yystack_[1].value.as < STLRom::interval* > (), yystack_[0].value.as < STLRom::transducer* > ());
                yylhs.value.as < STLRom::transducer* > ()->trace_data_ptr = &driver.data;
                yylhs.value.as < STLRom::transducer* > ()->param_map = driver.param_map;
                yylhs.value.as < STLRom::transducer* > ()->signal_map = driver.signal_map;

             }
#line 1107 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 37: // stl_formula: "(" stl_formula ")"
#line 404 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
             {
                 yylhs.value.as < STLRom::transducer* > () = yystack_[1].value.as < STLRom::transducer* > ();
             }
#line 1115 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 38: // stl_formula: "phi_id"
#line 408 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
             {

                 transducer * ref = driver.formula_map[yystack_[0].value.as < std::string > ()];

                 if (ref==nullptr) {
                     cout << "Parsing error: unknown identifier " << yystack_[0].value.as < std::string > () << endl;
                     yylhs.value.as < STLRom::transducer* > () = nullptr;
                     YYERROR;
                 }
                 else {
                     yylhs.value.as < STLRom::transducer* > () = ref->clone();
                     // TODO: copy variables (should be done in clone() no?)
                 }
             }
#line 1134 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 39: // assignement: "new_id" ":=" stl_formula
#line 426 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
            {
                driver.formula_map[yystack_[2].value.as < std::string > ()] = yystack_[0].value.as < STLRom::transducer* > ();
                if (driver.verbose_parser)
                    cout << CYAN << "Defined formula " << yystack_[2].value.as < std::string > () << " = " << *yystack_[0].value.as < STLRom::transducer* > () << RESET << endl;
            }
#line 1144 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 40: // param_assignement: "param_id" "=" "constant"
#line 460 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
                 {
                    double val;
                    s_to_d( yystack_[0].value.as < std::string > (), val );
                    driver.param_map[yystack_[2].value.as < std::string > ()] = val;
                    if (driver.verbose_parser)
                        cout << CYAN << "Parameter " << yystack_[2].value.as < std::string > () << " re-assigned value " << val << RESET << endl;
                 }
#line 1156 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 41: // param_assignement: "param_id" "=" "-" "constant"
#line 468 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
                 {
                    double val;
                    s_to_d( yystack_[0].value.as < std::string > (), val );
                    val = -val;
                    driver.param_map[yystack_[3].value.as < std::string > ()] = val;
                    if (driver.verbose_parser)
                        cout << CYAN << "Parameter " << yystack_[3].value.as < std::string > () << " re-assigned value " << val << RESET << endl;
                 }
#line 1169 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 42: // param_assignement: "new_id" "=" "constant"
#line 477 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
                 {
                    double val;
                    s_to_d( yystack_[0].value.as < std::string > (), val );
                    driver.param_map[yystack_[2].value.as < std::string > ()] = val;
                    if (driver.verbose_parser)
                        cout << CYAN << "New parameter " << yystack_[2].value.as < std::string > () << " assigned value " << val << RESET << endl;
                 }
#line 1181 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 43: // param_assignement: "new_id" "=" "-" "constant"
#line 485 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
                 {
                    double val;
                    s_to_d( yystack_[0].value.as < std::string > (), val );
                    val = -val;
                    driver.param_map[yystack_[3].value.as < std::string > ()] = val;
                    if (driver.verbose_parser)
                        cout << CYAN << "New parameter " << yystack_[3].value.as < std::string > () << " assigned value " << val << RESET << endl;
                 }
#line 1194 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 47: // signal_new: "new_id"
#line 539 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
          {
                short idx =  driver.signal_map.size()+1;
                driver.signal_map[yystack_[0].value.as < std::string > ()] = idx;
                if (driver.verbose_parser)
                    cout << CYAN << "Defined signal " << yystack_[0].value.as < std::string > () << " with index " << idx << RESET << endl;
          }
#line 1205 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;

  case 48: // signal_new: "signal_id"
#line 546 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
          {
          }
#line 1212 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"
    break;


#line 1216 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
        error (yyla.location, YY_MOVE (msg));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yyerror_range[1].location = yystack_[0].location;
        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
   Parser ::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
   Parser ::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              else
                goto append;

            append:
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

  std::string
   Parser ::symbol_name (symbol_kind_type yysymbol)
  {
    return yytnamerr_ (yytname_[yysymbol]);
  }



  //  Parser ::context.
   Parser ::context::context (const  Parser & yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
   Parser ::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

    const int yyn = yypact_[+yyparser_.yystack_[0].state];
    if (!yy_pact_value_is_default_ (yyn))
      {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
        const int yyxbegin = yyn < 0 ? -yyn : 0;
        // Stay within bounds of both yycheck and yytname.
        const int yychecklim = yylast_ - yyn + 1;
        const int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
          if (yycheck_[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror
              && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
            {
              if (!yyarg)
                ++yycount;
              else if (yycount == yyargn)
                return 0;
              else
                yyarg[yycount++] = YY_CAST (symbol_kind_type, yyx);
            }
      }

    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }






  int
   Parser ::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
   Parser ::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char  Parser ::yypact_ninf_ = -77;

  const signed char  Parser ::yytable_ninf_ = -1;

  const signed char
   Parser ::yypact_[] =
  {
     -13,   -28,    -2,    18,   -77,   -77,   -77,     2,    -5,    30,
     -77,    46,   -77,   -77,   -77,    69,    -4,   -77,   -77,   -77,
      -8,    -1,   -28,    -2,    -4,    -4,    86,    86,    20,    20,
      85,   -77,   -77,   -77,    88,   -77,   -77,   -77,   -77,    70,
      31,    53,   -77,    64,    58,   -77,    60,   -77,   -77,   -77,
      50,    61,    49,    34,    -4,    -4,    20,   -77,   -77,    20,
      78,    20,    20,    20,   -77,   -77,    83,   -77,   -77,    20,
      20,    -4,    -4,    -4,    86,   -77,   -77,   -77,   -77,   -77,
     -77,   -12,   -77,   -77,    91,    92,    95,   -77,    70,    70,
     -77,   -77,   -77,    72,    72,    72,    -4,    34,    96,   -77,
     -77,   -77,    97,   -77,   -77
  };

  const signed char
   Parser ::yydefact_[] =
  {
       0,     0,     0,     0,    52,    53,    54,     0,     0,     0,
      44,    46,    47,    48,    49,    51,     0,    57,    55,    56,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     4,     5,    38,     0,     8,     7,    10,    14,    16,
      19,     0,    29,    39,     0,    40,     0,    42,    45,    50,
       0,     0,    30,     0,     0,     0,     0,    13,    12,     0,
       0,     0,     0,     0,    21,    22,     0,    24,    25,     0,
       0,     0,     0,     0,     0,    41,    43,     9,    37,     2,
       3,     0,    35,    34,     0,     0,     0,    15,    17,    18,
      23,    20,    28,    31,    32,    33,     0,     0,     0,    11,
       6,    36,     0,    27,    26
  };

  const signed char
   Parser ::yypgoto_[] =
  {
     -77,   -76,   -77,   -77,   -77,   -11,    25,   -77,   -17,   -77,
     -77,   -77,   -23,   -24,    98,    82,   -77,    99,    84,   -77,
     -77,   -77
  };

  const signed char
   Parser ::yydefgoto_[] =
  {
       0,    81,    35,    36,    37,    38,    39,    40,    41,    42,
      69,    70,    54,    43,     4,    10,    11,     5,    14,    15,
       6,     7
  };

  const signed char
   Parser ::yytable_[] =
  {
      51,    52,    17,    24,    55,    98,    25,    50,    97,     8,
      20,     9,    26,    27,     1,     2,    44,    57,    58,    28,
      29,   102,    30,    46,    79,    80,     3,    56,    45,     1,
      82,    83,    31,    32,    33,    47,    34,    12,    13,    84,
      16,     3,    85,    28,    29,    21,    30,    93,    94,    95,
      87,    96,    91,    92,    62,    63,    31,    32,    77,    64,
      34,    65,    64,    73,    65,    66,    22,    74,    66,    78,
      79,    80,   101,    71,    72,    73,    71,    72,    73,    74,
      67,    68,    74,    67,    68,    72,    73,    88,    89,    23,
      74,    53,    59,    60,    75,    61,    76,    86,    90,    77,
      99,   100,   103,   104,    48,    18,    19,    49
  };

  const signed char
   Parser ::yycheck_[] =
  {
      24,    25,     0,     7,    27,    81,    10,    24,    20,    37,
      15,    39,    16,    17,    27,    28,    24,    28,    29,    23,
      24,    97,    26,    24,    36,    37,    39,     7,    36,    27,
      54,    55,    36,    37,    38,    36,    40,    39,    40,    56,
      22,    39,    59,    23,    24,    15,    26,    71,    72,    73,
      61,    74,    69,    70,    23,    24,    36,    37,     8,     9,
      40,    11,     9,    14,    11,    15,    20,    18,    15,     8,
      36,    37,    96,    12,    13,    14,    12,    13,    14,    18,
      30,    31,    18,    30,    31,    13,    14,    62,    63,    20,
      18,     5,     7,     5,    36,    25,    36,    19,    15,     8,
       8,     6,     6,     6,    22,     7,     7,    23
  };

  const signed char
   Parser ::yystos_[] =
  {
       0,    27,    28,    39,    58,    61,    64,    65,    37,    39,
      59,    60,    39,    40,    62,    63,    22,     0,    58,    61,
      15,    15,    20,    20,     7,    10,    16,    17,    23,    24,
      26,    36,    37,    38,    40,    46,    47,    48,    49,    50,
      51,    52,    53,    57,    24,    36,    24,    36,    59,    62,
      52,    57,    57,     5,    56,    56,     7,    49,    49,     7,
       5,    25,    23,    24,     9,    11,    15,    30,    31,    54,
      55,    12,    13,    14,    18,    36,    36,     8,     8,    36,
      37,    45,    57,    57,    52,    52,    19,    49,    50,    50,
      15,    52,    52,    57,    57,    57,    56,    20,    45,     8,
       6,    57,    45,     6,     6
  };

  const signed char
   Parser ::yyr1_[] =
  {
       0,    44,    45,    45,    46,    46,    47,    48,    48,    48,
      49,    49,    49,    49,    50,    50,    51,    51,    51,    52,
      53,    54,    54,    54,    55,    55,    56,    56,    57,    57,
      57,    57,    57,    57,    57,    57,    57,    57,    57,    58,
      59,    59,    59,    59,    60,    60,    61,    62,    62,    63,
      63,    64,    65,    65,    65,    65,    65,    65
  };

  const signed char
   Parser ::yyr2_[] =
  {
       0,     2,     1,     1,     1,     1,     4,     1,     1,     3,
       1,     4,     2,     2,     1,     3,     1,     3,     3,     1,
       3,     1,     1,     2,     1,     1,     5,     4,     3,     1,
       2,     3,     3,     3,     3,     3,     4,     3,     1,     3,
       3,     4,     3,     4,     1,     3,     2,     1,     1,     1,
       3,     2,     1,     1,     1,     2,     2,     2
  };


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const  Parser ::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "\"number\"",
  "\"semicolon\"", "\"[\"", "\"]\"", "\"(\"", "\")\"", "\"<\"", "\"not\"",
  "\">\"", "\"and\"", "\"or\"", "\"=>\"", "\"=\"", "\"alw\"", "\"ev\"",
  "\"until\"", "\"time\"", "\",\"", "\"end of line\"", "\":=\"", "\"+\"",
  "\"-\"", "\"*\"", "\"abs\"", "\"param_decl\"", "\"signal_decl\"",
  "\"test\"", "\"<=\"", "\">=\"", "\"constant_identifier\"", "\"true\"",
  "\"false\"", "\"double\"", "\"constant\"", "\"param_id\"", "\"phi_id\"",
  "\"new_id\"", "\"signal_id\"", "\"stl_test_id\"", "\"string\"",
  "UNARY_OPERATOR", "$accept", "constant", "constant_signal", "signal",
  "signal_atom", "signal_unaryexpr", "signal_multexpr", "signal_addexpr",
  "signal_expr", "stl_atom", "op", "op_eq", "interval", "stl_formula",
  "assignement", "param_assignement", "param_assignement_list",
  "param_assignements", "signal_new", "signal_new_list", "signal_decl",
  "start", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const short
   Parser ::yyrline_[] =
  {
       0,   177,   177,   182,   188,   196,   204,   228,   233,   238,
     243,   247,   254,   261,   266,   270,   278,   282,   289,   297,
     303,   312,   313,   314,   318,   319,   322,   327,   333,   345,
     349,   356,   363,   371,   379,   387,   395,   403,   407,   425,
     459,   467,   476,   484,   494,   495,   497,   538,   545,   549,
     550,   552,   556,   557,   558,   559,   560,   561
  };

  void
   Parser ::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
   Parser ::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG


#line 42 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"
} //  STLRom 
#line 1757 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.cpp"

#line 564 "/home/dayekhh/Documents/git-repos/stlrom2/src/parser.y"


// Bison expects us to provide implementation - otherwise linker complains
void STLRom::Parser::error(const location &loc , const std::string &message) {	
        std::cerr << RED << "Error: " << message << endl << "At line " << loc.begin.line;
        if (loc.begin.column == loc.end.column - 1)
            std::cerr << ", column " << loc.begin.column << std::endl << RESET;
        else
            std::cerr << ", columns " << loc.begin.column << "-" << (loc.end.column - 1) << std::endl << RESET;
}
