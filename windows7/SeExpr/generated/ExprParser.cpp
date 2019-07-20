/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with SeExpr2 or SeExprYY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define SeExprYYBISON 1

/* Bison version.  */
#define SeExprYYBISON_VERSION "2.7"

/* Skeleton name.  */
#define SeExprYYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define SeExprYYPURE 0

/* Push parsers.  */
#define SeExprYYPUSH 0

/* Pull parsers.  */
#define SeExprYYPULL 1


/* Substitute the variable and function names.  */
#define SeExpr2parse         SeExpr2parse
#define SeExpr2lex           SeExpr2lex
#define SeExpr2error         SeExpr2error
#define SeExpr2lval          SeExpr2lval
#define SeExpr2char          SeExpr2char
#define SeExpr2debug         SeExpr2debug
#define SeExpr2nerrs         SeExpr2nerrs
#define SeExpr2lloc          SeExpr2lloc

/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 18 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"

#ifndef MAKEDEPEND
#include <algorithm>
#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>
#endif
#include "ExprType.h"
#include "ExprNode.h"
#include "ExprParser.h"
#include "Expression.h"
#include "Mutex.h"

/******************
 lexer declarations
 ******************/

// declarations of functions and data in ExprParser.l
int SeExpr2lex();
int SeExpr2pos();
extern int SeExpr2_start;
extern char* SeExpr2text;
struct SeExpr2_buffer_state;
SeExpr2_buffer_state* SeExpr2_scan_string(const char *str);
void SeExpr2_delete_buffer(SeExpr2_buffer_state*);

/*******************
 parser declarations
 *******************/

// forward declaration
static void SeExpr2error(const char* msg);

// local data
static const char* ParseStr;    // string being parsed
static std::string ParseError;  // error (set from SeExpr2error)
static SeExpr2::ExprNode* ParseResult; // must set result here since SeExpr2parse can't return it
static const SeExpr2::Expression* Expr;// used for parenting created SeExprOp's

/* The list of nodes being built is remembered locally here.
   Eventually (if there are no syntax errors) ownership of the nodes
   will belong solely to the parse tree and the parent expression.
   However, if there is a syntax error, we must loop through this list
   and free any nodes that were allocated before the error to avoid a
   memory leak. */
static std::vector<SeExpr2::ExprNode*> ParseNodes;
inline SeExpr2::ExprNode* Remember(SeExpr2::ExprNode* n,const int startPos,const int endPos)
    { ParseNodes.push_back(n); n->setPosition(startPos,endPos); return n; }
inline void Forget(SeExpr2::ExprNode* n)
    { ParseNodes.erase(std::find(ParseNodes.begin(), ParseNodes.end(), n)); }
/* These are handy node constructors for 0-3 arguments */
#define NODE(startPos,endPos,name) Remember(new SeExpr2::Expr##name(Expr),startPos,endPos)
#define NODE1(startPos,endPos,name,a) Remember(new SeExpr2::Expr##name(Expr,a),startPos,endPos)
#define NODE2(startPos,endPos,name,a,b) Remember(new SeExpr2::Expr##name(Expr,a,b),startPos,endPos)
#define NODE3(startPos,endPos,name,a,b,c) Remember(new SeExpr2::Expr##name(Expr,a,b,c),startPos,endPos)
#define NODE4(startPos,endPos,name,a,b,c,t) Remember(new SeExpr2::Expr##name(Expr,a,b,c,t),startPos,endPos)

/* Line 371 of yacc.c  */
#line 135 "y.tab.c"

# ifndef SeExprYY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define SeExprYY_NULL nullptr
#  else
#   define SeExprYY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef SeExprYYERROR_VERBOSE
# undef SeExprYYERROR_VERBOSE
# define SeExprYYERROR_VERBOSE 1
#else
# define SeExprYYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef SeExprYY_SEEXPR2_Y_TAB_H_INCLUDED
# define SeExprYY_SEEXPR2_Y_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef SeExprYYDEBUG
# define SeExprYYDEBUG 0
#endif
#if SeExprYYDEBUG
extern int SeExpr2debug;
#endif

/* Tokens.  */
#ifndef SeExprYYTOKENTYPE
# define SeExprYYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum SeExpr2tokentype {
     IF = 258,
     ELSE = 259,
     EXTERN = 260,
     DEF = 261,
     FLOATPOINT = 262,
     STRING = 263,
     NAME = 264,
     VAR = 265,
     STR = 266,
     NUMBER = 267,
     LIFETIME_CONSTANT = 268,
     LIFETIME_UNIFORM = 269,
     LIFETIME_VARYING = 270,
     LIFETIME_ERROR = 271,
     AddEq = 272,
     SubEq = 273,
     MultEq = 274,
     DivEq = 275,
     ExpEq = 276,
     ModEq = 277,
     ARROW = 278,
     OR = 279,
     AND = 280,
     NE = 281,
     EQ = 282,
     SEEXPR_GE = 283,
     SEEXPR_LE = 284,
     UNARY = 285
   };
#endif
/* Tokens.  */
#define IF 258
#define ELSE 259
#define EXTERN 260
#define DEF 261
#define FLOATPOINT 262
#define STRING 263
#define NAME 264
#define VAR 265
#define STR 266
#define NUMBER 267
#define LIFETIME_CONSTANT 268
#define LIFETIME_UNIFORM 269
#define LIFETIME_VARYING 270
#define LIFETIME_ERROR 271
#define AddEq 272
#define SubEq 273
#define MultEq 274
#define DivEq 275
#define ExpEq 276
#define ModEq 277
#define ARROW 278
#define OR 279
#define AND 280
#define NE 281
#define EQ 282
#define SEEXPR_GE 283
#define SEEXPR_LE 284
#define UNARY 285



#if ! defined SeExprYYSTYPE && ! defined SeExprYYSTYPE_IS_DECLARED
typedef union SeExprYYSTYPE
{
/* Line 387 of yacc.c  */
#line 77 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"

    SeExpr2::ExprNode* n; /* a node is returned for all non-terminals to
		      build the parse tree from the leaves up. */
    double d;      // return value for number tokens
    char* s;       /* return value for name tokens.  Note: the string
		      is allocated with strdup() in the lexer and must
		      be freed with free() */
    struct {
        SeExpr2::ExprType::Type     type;
        int                  dim;
        SeExpr2::ExprType::Lifetime lifetime;
    } t;  // return value for types
    SeExpr2::ExprType::Lifetime l; // return value for lifetime qualifiers


/* Line 387 of yacc.c  */
#line 254 "y.tab.c"
} SeExprYYSTYPE;
# define SeExprYYSTYPE_IS_TRIVIAL 1
# define SeExpr2stype SeExprYYSTYPE /* obsolescent; will be withdrawn */
# define SeExprYYSTYPE_IS_DECLARED 1
#endif

#if ! defined SeExprYYLTYPE && ! defined SeExprYYLTYPE_IS_DECLARED
typedef struct SeExprYYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} SeExprYYLTYPE;
# define SeExpr2ltype SeExprYYLTYPE /* obsolescent; will be withdrawn */
# define SeExprYYLTYPE_IS_DECLARED 1
# define SeExprYYLTYPE_IS_TRIVIAL 1
#endif

extern SeExprYYSTYPE SeExpr2lval;
extern SeExprYYLTYPE SeExpr2lloc;
#ifdef SeExprYYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int SeExpr2parse (void *SeExprYYPARSE_PARAM);
#else
int SeExpr2parse ();
#endif
#else /* ! SeExprYYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int SeExpr2parse (void);
#else
int SeExpr2parse ();
#endif
#endif /* ! SeExprYYPARSE_PARAM */

#endif /* !SeExprYY_SEEXPR2_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 295 "y.tab.c"

#ifdef short
# undef short
#endif

#ifdef SeExprYYTYPE_UINT8
typedef SeExprYYTYPE_UINT8 SeExpr2type_uint8;
#else
typedef unsigned char SeExpr2type_uint8;
#endif

#ifdef SeExprYYTYPE_INT8
typedef SeExprYYTYPE_INT8 SeExpr2type_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char SeExpr2type_int8;
#else
typedef short int SeExpr2type_int8;
#endif

#ifdef SeExprYYTYPE_UINT16
typedef SeExprYYTYPE_UINT16 SeExpr2type_uint16;
#else
typedef unsigned short int SeExpr2type_uint16;
#endif

#ifdef SeExprYYTYPE_INT16
typedef SeExprYYTYPE_INT16 SeExpr2type_int16;
#else
typedef short int SeExpr2type_int16;
#endif

#ifndef SeExprYYSIZE_T
# ifdef __SIZE_TYPE__
#  define SeExprYYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define SeExprYYSIZE_T size_t
# elif ! defined SeExprYYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define SeExprYYSIZE_T size_t
# else
#  define SeExprYYSIZE_T unsigned int
# endif
#endif

#define SeExprYYSIZE_MAXIMUM ((SeExprYYSIZE_T) -1)

#ifndef SeExprYY_
# if defined SeExprYYENABLE_NLS && SeExprYYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define SeExprYY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef SeExprYY_
#  define SeExprYY_(Msgid) Msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define SeExprYYUSE(E) ((void) (E))
#else
# define SeExprYYUSE(E) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define SeExprYYID(N) (N)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
SeExprYYID (int SeExpr2i)
#else
static int
SeExprYYID (SeExpr2i)
    int SeExpr2i;
#endif
{
  return SeExpr2i;
}
#endif

#if ! defined SeExpr2overflow || SeExprYYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef SeExprYYSTACK_USE_ALLOCA
#  if SeExprYYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define SeExprYYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define SeExprYYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define SeExprYYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef SeExprYYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define SeExprYYSTACK_FREE(Ptr) do { /* empty */; } while (SeExprYYID (0))
#  ifndef SeExprYYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define SeExprYYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define SeExprYYSTACK_ALLOC SeExprYYMALLOC
#  define SeExprYYSTACK_FREE SeExprYYFREE
#  ifndef SeExprYYSTACK_ALLOC_MAXIMUM
#   define SeExprYYSTACK_ALLOC_MAXIMUM SeExprYYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined SeExprYYMALLOC || defined malloc) \
	     && (defined SeExprYYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef SeExprYYMALLOC
#   define SeExprYYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (SeExprYYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef SeExprYYFREE
#   define SeExprYYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined SeExpr2overflow || SeExprYYERROR_VERBOSE */


#if (! defined SeExpr2overflow \
     && (! defined __cplusplus \
	 || (defined SeExprYYLTYPE_IS_TRIVIAL && SeExprYYLTYPE_IS_TRIVIAL \
	     && defined SeExprYYSTYPE_IS_TRIVIAL && SeExprYYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union SeExpr2alloc
{
  SeExpr2type_int16 SeExpr2ss_alloc;
  SeExprYYSTYPE SeExpr2vs_alloc;
  SeExprYYLTYPE SeExpr2ls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define SeExprYYSTACK_GAP_MAXIMUM (sizeof (union SeExpr2alloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define SeExprYYSTACK_BYTES(N) \
     ((N) * (sizeof (SeExpr2type_int16) + sizeof (SeExprYYSTYPE) + sizeof (SeExprYYLTYPE)) \
      + 2 * SeExprYYSTACK_GAP_MAXIMUM)

# define SeExprYYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables SeExprYYSIZE and SeExprYYSTACKSIZE give the old and new number of
   elements in the stack, and SeExprYYPTR gives the new location of the
   stack.  Advance SeExprYYPTR to a properly aligned location for the next
   stack.  */
# define SeExprYYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	SeExprYYSIZE_T SeExpr2newbytes;						\
	SeExprYYCOPY (&SeExpr2ptr->Stack_alloc, Stack, SeExpr2size);			\
	Stack = &SeExpr2ptr->Stack_alloc;					\
	SeExpr2newbytes = SeExpr2stacksize * sizeof (*Stack) + SeExprYYSTACK_GAP_MAXIMUM; \
	SeExpr2ptr += SeExpr2newbytes / sizeof (*SeExpr2ptr);				\
      }									\
    while (SeExprYYID (0))

#endif

#if defined SeExprYYCOPY_NEEDED && SeExprYYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef SeExprYYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define SeExprYYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define SeExprYYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          SeExprYYSIZE_T SeExpr2i;                         \
          for (SeExpr2i = 0; SeExpr2i < (Count); SeExpr2i++)   \
            (Dst)[SeExpr2i] = (Src)[SeExpr2i];            \
        }                                       \
      while (SeExprYYID (0))
#  endif
# endif
#endif /* !SeExprYYCOPY_NEEDED */

/* SeExprYYFINAL -- State number of the termination state.  */
#define SeExprYYFINAL  52
/* SeExprYYLAST -- Last index in SeExprYYTABLE.  */
#define SeExprYYLAST   691

/* SeExprYYNTOKENS -- Number of terminals.  */
#define SeExprYYNTOKENS  52
/* SeExprYYNNTS -- Number of nonterminals.  */
#define SeExprYYNNTS  21
/* SeExprYYNRULES -- Number of rules.  */
#define SeExprYYNRULES  84
/* SeExprYYNRULES -- Number of states.  */
#define SeExprYYNSTATES  187

/* SeExprYYTRANSLATE(SeExprYYLEX) -- Bison symbol number corresponding to SeExprYYLEX.  */
#define SeExprYYUNDEFTOK  2
#define SeExprYYMAXUTOK   285

#define SeExprYYTRANSLATE(SeExprYYX)						\
  ((unsigned int) (SeExprYYX) <= SeExprYYMAXUTOK ? SeExpr2translate[SeExprYYX] : SeExprYYUNDEFTOK)

/* SeExprYYTRANSLATE[SeExprYYLEX] -- Bison symbol number corresponding to SeExprYYLEX.  */
static const SeExpr2type_uint8 SeExpr2translate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    41,     2,     2,     2,    40,     2,     2,
      23,    24,    38,    36,    49,    37,     2,    39,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    26,    51,
      32,    50,    33,    27,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    45,     2,    48,    44,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    46,     2,    47,    42,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    25,    28,
      29,    30,    31,    34,    35,    43
};

#if SeExprYYDEBUG
/* SeExprYYPRHS[SeExprYYN] -- Index of the first RHS symbol of rule number SeExprYYN in
   SeExprYYRHS.  */
static const SeExpr2type_uint16 SeExpr2prhs[] =
{
       0,     0,     3,     6,     8,    10,    13,    20,    30,    39,
      40,    42,    44,    46,    48,    51,    57,    60,    61,    63,
      65,    69,    70,    72,    75,    80,    83,    85,    86,    88,
      90,    93,    95,   100,   105,   110,   115,   120,   125,   130,
     135,   140,   145,   150,   155,   160,   165,   174,   175,   180,
     183,   187,   191,   196,   202,   206,   210,   214,   218,   222,
     226,   230,   234,   237,   240,   243,   246,   250,   254,   258,
     262,   266,   270,   275,   282,   284,   286,   288,   290,   292,
     296,   297,   299,   301,   305
};

/* SeExprYYRHS -- A `-1'-separated list of the rules' RHS.  */
static const SeExpr2type_int8 SeExpr2rhs[] =
{
      53,     0,    -1,    54,    62,    -1,    62,    -1,    55,    -1,
      54,    55,    -1,     5,    57,     9,    23,    58,    24,    -1,
       6,    57,     9,    23,    60,    24,    46,    62,    47,    -1,
       6,     9,    23,    60,    24,    46,    62,    47,    -1,    -1,
      13,    -1,    14,    -1,    15,    -1,    16,    -1,     7,    56,
      -1,     7,    45,    12,    48,    56,    -1,     8,    56,    -1,
      -1,    59,    -1,    57,    -1,    59,    49,    57,    -1,    -1,
      61,    -1,    57,     9,    -1,    61,    49,    57,     9,    -1,
      64,    68,    -1,    68,    -1,    -1,    64,    -1,    65,    -1,
      64,    65,    -1,    66,    -1,    10,    50,    68,    51,    -1,
      10,    17,    68,    51,    -1,    10,    18,    68,    51,    -1,
      10,    19,    68,    51,    -1,    10,    20,    68,    51,    -1,
      10,    21,    68,    51,    -1,    10,    22,    68,    51,    -1,
       9,    50,    68,    51,    -1,     9,    17,    68,    51,    -1,
       9,    18,    68,    51,    -1,     9,    19,    68,    51,    -1,
       9,    20,    68,    51,    -1,     9,    21,    68,    51,    -1,
       9,    22,    68,    51,    -1,     3,    23,    68,    24,    46,
      63,    47,    67,    -1,    -1,     4,    46,    63,    47,    -1,
       4,    66,    -1,    23,    68,    24,    -1,    45,    69,    48,
      -1,    68,    45,    68,    48,    -1,    68,    27,    68,    26,
      68,    -1,    68,    28,    68,    -1,    68,    29,    68,    -1,
      68,    31,    68,    -1,    68,    30,    68,    -1,    68,    32,
      68,    -1,    68,    33,    68,    -1,    68,    35,    68,    -1,
      68,    34,    68,    -1,    36,    68,    -1,    37,    68,    -1,
      41,    68,    -1,    42,    68,    -1,    68,    36,    68,    -1,
      68,    37,    68,    -1,    68,    38,    68,    -1,    68,    39,
      68,    -1,    68,    40,    68,    -1,    68,    44,    68,    -1,
       9,    23,    70,    24,    -1,    68,    25,     9,    23,    70,
      24,    -1,    10,    -1,     9,    -1,    12,    -1,    11,    -1,
      68,    -1,    69,    49,    68,    -1,    -1,    71,    -1,    72,
      -1,    71,    49,    72,    -1,    68,    -1
};

/* SeExprYYRLINE[SeExprYYN] -- source line where rule number SeExprYYN was defined.  */
static const SeExpr2type_uint16 SeExpr2rline[] =
{
       0,   132,   132,   134,   139,   140,   146,   154,   162,   172,
     173,   174,   175,   176,   180,   183,   188,   194,   195,   199,
     203,   210,   211,   215,   220,   229,   230,   235,   236,   240,
     241,   245,   246,   247,   250,   253,   256,   259,   262,   265,
     266,   269,   272,   275,   278,   281,   287,   292,   293,   294,
     299,   300,   301,   302,   303,   304,   305,   306,   307,   308,
     309,   310,   311,   312,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   325,   331,   332,   333,   334,   338,   339,
     345,   346,   351,   352,   356
};
#endif

#if SeExprYYDEBUG || SeExprYYERROR_VERBOSE || 0
/* SeExprYYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at SeExprYYNTOKENS, nonterminals.  */
static const char *const SeExpr2tname[] =
{
  "$end", "error", "$undefined", "IF", "ELSE", "EXTERN", "DEF",
  "FLOATPOINT", "STRING", "NAME", "VAR", "STR", "NUMBER",
  "LIFETIME_CONSTANT", "LIFETIME_UNIFORM", "LIFETIME_VARYING",
  "LIFETIME_ERROR", "AddEq", "SubEq", "MultEq", "DivEq", "ExpEq", "ModEq",
  "'('", "')'", "ARROW", "':'", "'?'", "OR", "AND", "NE", "EQ", "'<'",
  "'>'", "SEEXPR_GE", "SEEXPR_LE", "'+'", "'-'", "'*'", "'/'", "'%'",
  "'!'", "'~'", "UNARY", "'^'", "'['", "'{'", "'}'", "']'", "','", "'='",
  "';'", "$accept", "module", "declarationList", "declaration",
  "lifetimeOptional", "typeDeclare", "typeListOptional", "typeList",
  "formalTypeListOptional", "formalTypeList", "block", "optassigns",
  "assigns", "assign", "ifthenelse", "optelse", "e", "exprlist", "optargs",
  "args", "arg", SeExprYY_NULL
};
#endif

# ifdef SeExprYYPRINT
/* SeExprYYTOKNUM[SeExprYYLEX-NUM] -- Internal token number corresponding to
   token SeExprYYLEX-NUM.  */
static const SeExpr2type_uint16 SeExpr2toknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,    40,    41,   278,    58,    63,   279,   280,
     281,   282,    60,    62,   283,   284,    43,    45,    42,    47,
      37,    33,   126,   285,    94,    91,   123,   125,    93,    44,
      61,    59
};
# endif

/* SeExprYYR1[SeExprYYN] -- Symbol number of symbol that rule SeExprYYN derives.  */
static const SeExpr2type_uint8 SeExpr2r1[] =
{
       0,    52,    53,    53,    54,    54,    55,    55,    55,    56,
      56,    56,    56,    56,    57,    57,    57,    58,    58,    59,
      59,    60,    60,    61,    61,    62,    62,    63,    63,    64,
      64,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    66,    67,    67,    67,
      68,    68,    68,    68,    68,    68,    68,    68,    68,    68,
      68,    68,    68,    68,    68,    68,    68,    68,    68,    68,
      68,    68,    68,    68,    68,    68,    68,    68,    69,    69,
      70,    70,    71,    71,    72
};

/* SeExprYYR2[SeExprYYN] -- Number of symbols composing right hand side of rule SeExprYYN.  */
static const SeExpr2type_uint8 SeExpr2r2[] =
{
       0,     2,     2,     1,     1,     2,     6,     9,     8,     0,
       1,     1,     1,     1,     2,     5,     2,     0,     1,     1,
       3,     0,     1,     2,     4,     2,     1,     0,     1,     1,
       2,     1,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     8,     0,     4,     2,
       3,     3,     4,     5,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     2,     2,     2,     3,     3,     3,     3,
       3,     3,     4,     6,     1,     1,     1,     1,     1,     3,
       0,     1,     1,     3,     1
};

/* SeExprYYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when SeExprYYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const SeExpr2type_uint8 SeExpr2defact[] =
{
       0,     0,     0,     0,    75,    74,    77,    76,     0,     0,
       0,     0,     0,     0,     0,     0,     4,     3,     0,    29,
      31,    26,     0,     9,     9,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    80,     0,     0,     0,     0,     0,
       0,     0,     0,    75,    74,     0,    62,    63,    64,    65,
      78,     0,     1,     5,     2,    30,    25,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    10,    11,    12,    13,     0,
      14,    16,     0,    21,     0,     0,     0,     0,     0,     0,
       0,    84,     0,    81,    82,     0,     0,     0,     0,     0,
       0,     0,     0,    50,    51,     0,     0,     0,    54,    55,
      57,    56,    58,    59,    61,    60,    66,    67,    68,    69,
      70,    71,     0,     0,     0,    17,     0,     0,    22,    21,
      40,    41,    42,    43,    44,    45,    72,     0,    39,    33,
      34,    35,    36,    37,    38,    32,    79,    80,     0,    52,
      27,     9,    19,     0,    18,    23,     0,     0,     0,    83,
       0,    53,     0,     0,     0,    28,    15,     6,     0,     0,
       0,     0,    73,    47,    20,     0,    24,     0,     0,    46,
       8,     0,    27,    49,     7,     0,    48
};

/* SeExprYYDEFGOTO[NTERM-NUM].  */
static const SeExpr2type_int16 SeExpr2defgoto[] =
{
      -1,    14,    15,    16,    80,   126,   153,   154,   127,   128,
      17,   164,    18,    19,    20,   179,    21,    51,    92,    93,
      94
};

/* SeExprYYPACT[STATE-NUM] -- Index in SeExprYYTABLE of the portion describing
   STATE-NUM.  */
#define SeExprYYPACT_NINF -68
static const SeExpr2type_int16 SeExpr2pact[] =
{
      67,     0,     4,    90,    24,    31,   -68,   -68,    84,    84,
      84,    84,    84,    84,    71,    67,   -68,   -68,    77,   -68,
     -68,   580,    84,   125,   202,    66,    17,    93,    84,    84,
      84,    84,    84,    84,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    82,   -68,   516,    40,    40,    40,    40,
     580,    43,   -68,   -68,   -68,   -68,   580,    97,    84,    84,
      84,    84,    84,    84,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    84,   538,   -68,   -68,   -68,   -68,    98,
     -68,   -68,    88,     4,    94,   144,   169,   194,   219,   244,
     269,   580,    92,    75,   -68,   294,   319,   344,   369,   394,
     419,   444,   469,   -68,   -68,    84,   104,   559,   616,   632,
     646,   646,   109,   109,   109,   109,   -23,   -23,    40,    40,
      40,    40,   494,    85,    80,     4,   121,   108,    86,     4,
     -68,   -68,   -68,   -68,   -68,   -68,   -68,    84,   -68,   -68,
     -68,   -68,   -68,   -68,   -68,   -68,   580,    84,    84,   -68,
      10,   202,   -68,   110,    87,   -68,    96,     4,   127,   -68,
     128,   599,   140,    31,   116,    10,   -68,   -68,     4,    77,
     156,   122,   -68,   163,   -68,   138,   -68,    77,    36,   -68,
     -68,   139,    10,   -68,   -68,   145,   -68
};

/* SeExprYYPGOTO[NTERM-NUM].  */
static const SeExpr2type_int16 SeExpr2pgoto[] =
{
     -68,   -68,   -68,   172,   -14,    -2,   -68,   -68,    62,   -68,
     -13,    11,   -67,   -15,    32,   -68,    -4,   -68,    64,   -68,
      99
};

/* SeExprYYTABLE[SeExprYYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If SeExprYYTABLE_NINF, syntax error.  */
#define SeExprYYTABLE_NINF -1
static const SeExpr2type_uint8 SeExpr2table[] =
{
      25,    27,    54,    55,    45,    46,    47,    48,    49,    50,
      81,    23,    24,     1,    56,    69,    70,    71,    74,   162,
     163,    72,    73,    22,    85,    86,    87,    88,    89,    90,
      91,    95,    96,    97,    98,    99,   100,   101,   102,     1,
      83,    28,    29,    30,    31,    32,    33,    34,    36,    37,
      38,    39,    40,    41,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
       1,    52,     2,     3,    35,    82,     4,     5,     6,     7,
       1,    42,   182,   165,    72,    73,     4,     5,     6,     7,
       8,   104,   105,    43,    44,     6,     7,    23,    24,    26,
       8,   146,    84,     9,    10,    34,   106,     8,    11,    12,
     124,   125,    13,     9,    10,   165,   136,   129,    11,    12,
       9,    10,    13,   152,   137,    11,    12,   147,   151,    13,
     155,   150,   156,    91,   167,   157,   168,   166,    75,    76,
      77,    78,   169,    91,   161,    67,    68,    69,    70,    71,
      55,   171,   172,    72,    73,   170,   175,    28,    29,    30,
      31,    32,    33,   173,   181,   176,   174,   178,   177,    57,
      79,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,   180,   184,    53,    72,    73,
      35,   158,   186,   185,    57,   130,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
     183,   160,     0,    72,    73,    75,    76,    77,    78,    57,
     131,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,     0,   159,     0,    72,    73,
       0,     0,     0,     0,    57,   132,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
       0,     0,     0,    72,    73,     0,     0,     0,     0,    57,
     133,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,     0,     0,     0,    72,    73,
       0,     0,     0,     0,    57,   134,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
       0,     0,     0,    72,    73,     0,     0,     0,     0,    57,
     135,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,     0,     0,     0,    72,    73,
       0,     0,     0,     0,    57,   138,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
       0,     0,     0,    72,    73,     0,     0,     0,     0,    57,
     139,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,     0,     0,     0,    72,    73,
       0,     0,     0,     0,    57,   140,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
       0,     0,     0,    72,    73,     0,     0,     0,     0,    57,
     141,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,     0,     0,     0,    72,    73,
       0,     0,     0,     0,    57,   142,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
       0,     0,     0,    72,    73,     0,     0,     0,     0,    57,
     143,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,     0,     0,     0,    72,    73,
       0,     0,     0,     0,    57,   144,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
       0,     0,     0,    72,    73,     0,     0,     0,     0,    57,
     145,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,     0,     0,     0,    72,    73,
     103,    57,   149,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,     0,     0,     0,
      72,    73,   123,    57,     0,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,     0,
       0,     0,    72,    73,    57,   148,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
       0,     0,     0,    72,    73,    57,     0,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,     0,     0,     0,    72,    73,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
       0,     0,     0,    72,    73,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,     0,     0,     0,
      72,    73,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,     0,     0,     0,    72,    73,    63,    64,
      65,    66,    67,    68,    69,    70,    71,     0,     0,     0,
      72,    73
};

#define SeExpr2pact_value_is_default(Yystate) \
  (!!((Yystate) == (-68)))

#define SeExpr2table_value_is_error(Yytable_value) \
  SeExprYYID (0)

static const SeExpr2type_int16 SeExpr2check[] =
{
       2,     3,    15,    18,     8,     9,    10,    11,    12,    13,
      24,     7,     8,     3,    18,    38,    39,    40,    22,     9,
      10,    44,    45,    23,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     3,
      23,    17,    18,    19,    20,    21,    22,    23,    17,    18,
      19,    20,    21,    22,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
       3,     0,     5,     6,    50,     9,     9,    10,    11,    12,
       3,    50,    46,   150,    44,    45,     9,    10,    11,    12,
      23,    48,    49,     9,    10,    11,    12,     7,     8,     9,
      23,   105,     9,    36,    37,    23,     9,    23,    41,    42,
      12,    23,    45,    36,    37,   182,    24,    23,    41,    42,
      36,    37,    45,   125,    49,    41,    42,    23,    48,    45,
       9,    46,    24,   137,    24,    49,    49,   151,    13,    14,
      15,    16,    46,   147,   148,    36,    37,    38,    39,    40,
     165,    24,    24,    44,    45,   157,   169,    17,    18,    19,
      20,    21,    22,    47,   177,     9,   168,     4,    46,    25,
      45,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    47,    47,    15,    44,    45,
      50,   129,    47,   182,    25,    51,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
     178,   147,    -1,    44,    45,    13,    14,    15,    16,    25,
      51,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    -1,   137,    -1,    44,    45,
      -1,    -1,    -1,    -1,    25,    51,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      -1,    -1,    -1,    44,    45,    -1,    -1,    -1,    -1,    25,
      51,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    -1,    -1,    -1,    44,    45,
      -1,    -1,    -1,    -1,    25,    51,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      -1,    -1,    -1,    44,    45,    -1,    -1,    -1,    -1,    25,
      51,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    -1,    -1,    -1,    44,    45,
      -1,    -1,    -1,    -1,    25,    51,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      -1,    -1,    -1,    44,    45,    -1,    -1,    -1,    -1,    25,
      51,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    -1,    -1,    -1,    44,    45,
      -1,    -1,    -1,    -1,    25,    51,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      -1,    -1,    -1,    44,    45,    -1,    -1,    -1,    -1,    25,
      51,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    -1,    -1,    -1,    44,    45,
      -1,    -1,    -1,    -1,    25,    51,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      -1,    -1,    -1,    44,    45,    -1,    -1,    -1,    -1,    25,
      51,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    -1,    -1,    -1,    44,    45,
      -1,    -1,    -1,    -1,    25,    51,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      -1,    -1,    -1,    44,    45,    -1,    -1,    -1,    -1,    25,
      51,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    -1,    -1,    -1,    44,    45,
      24,    25,    48,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    -1,    -1,    -1,
      44,    45,    24,    25,    -1,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    -1,
      -1,    -1,    44,    45,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      -1,    -1,    -1,    44,    45,    25,    -1,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    -1,    -1,    -1,    44,    45,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      -1,    -1,    -1,    44,    45,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    -1,    -1,    -1,
      44,    45,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    -1,    -1,    -1,    44,    45,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    -1,    -1,    -1,
      44,    45
};

/* SeExprYYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const SeExpr2type_uint8 SeExpr2stos[] =
{
       0,     3,     5,     6,     9,    10,    11,    12,    23,    36,
      37,    41,    42,    45,    53,    54,    55,    62,    64,    65,
      66,    68,    23,     7,     8,    57,     9,    57,    17,    18,
      19,    20,    21,    22,    23,    50,    17,    18,    19,    20,
      21,    22,    50,     9,    10,    68,    68,    68,    68,    68,
      68,    69,     0,    55,    62,    65,    68,    25,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    44,    45,    68,    13,    14,    15,    16,    45,
      56,    56,     9,    23,     9,    68,    68,    68,    68,    68,
      68,    68,    70,    71,    72,    68,    68,    68,    68,    68,
      68,    68,    68,    24,    48,    49,     9,    68,    68,    68,
      68,    68,    68,    68,    68,    68,    68,    68,    68,    68,
      68,    68,    68,    24,    12,    23,    57,    60,    61,    23,
      51,    51,    51,    51,    51,    51,    24,    49,    51,    51,
      51,    51,    51,    51,    51,    51,    68,    23,    26,    48,
      46,    48,    57,    58,    59,     9,    24,    49,    60,    72,
      70,    68,     9,    10,    63,    64,    56,    24,    49,    46,
      57,    24,    24,    47,    57,    62,     9,    46,     4,    67,
      47,    62,    46,    66,    47,    63,    47
};

#define SeExpr2errok		(SeExpr2errstatus = 0)
#define SeExpr2clearin	(SeExpr2char = SeExprYYEMPTY)
#define SeExprYYEMPTY		(-2)
#define SeExprYYEOF		0

#define SeExprYYACCEPT	goto SeExpr2acceptlab
#define SeExprYYABORT		goto SeExpr2abortlab
#define SeExprYYERROR		goto SeExpr2errorlab


/* Like SeExprYYERROR except do call SeExpr2error.  This remains here temporarily
   to ease the transition to the new meaning of SeExprYYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   SeExprYYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define SeExprYYFAIL		goto SeExpr2errlab
#if defined SeExprYYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     SeExprYYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define SeExprYYRECOVERING()  (!!SeExpr2errstatus)

#define SeExprYYBACKUP(Token, Value)                                  \
do                                                              \
  if (SeExpr2char == SeExprYYEMPTY)                                        \
    {                                                           \
      SeExpr2char = (Token);                                         \
      SeExpr2lval = (Value);                                         \
      SeExprYYPOPSTACK (SeExpr2len);                                       \
      SeExpr2state = *SeExpr2ssp;                                         \
      goto SeExpr2backup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      SeExpr2error (SeExprYY_("syntax error: cannot back up")); \
      SeExprYYERROR;							\
    }								\
while (SeExprYYID (0))

/* Error token number */
#define SeExprYYTERROR	1
#define SeExprYYERRCODE	256


/* SeExprYYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef SeExprYYLLOC_DEFAULT
# define SeExprYYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (SeExprYYID (N))                                                     \
        {                                                               \
          (Current).first_line   = SeExprYYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = SeExprYYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = SeExprYYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = SeExprYYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            SeExprYYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            SeExprYYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (SeExprYYID (0))
#endif

#define SeExprYYRHSLOC(Rhs, K) ((Rhs)[K])


/* SeExprYY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef SeExprYY_LOCATION_PRINT
# if defined SeExprYYLTYPE_IS_TRIVIAL && SeExprYYLTYPE_IS_TRIVIAL

/* Print *SeExprYYLOCP on SeExprYYO.  Private, do not rely on its existence. */

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static unsigned
SeExpr2_location_print_ (FILE *SeExpr2o, SeExprYYLTYPE const * const SeExpr2locp)
#else
static unsigned
SeExpr2_location_print_ (SeExpr2o, SeExpr2locp)
    FILE *SeExpr2o;
    SeExprYYLTYPE const * const SeExpr2locp;
#endif
{
  unsigned res = 0;
  int end_col = 0 != SeExpr2locp->last_column ? SeExpr2locp->last_column - 1 : 0;
  if (0 <= SeExpr2locp->first_line)
    {
      res += fprintf (SeExpr2o, "%d", SeExpr2locp->first_line);
      if (0 <= SeExpr2locp->first_column)
        res += fprintf (SeExpr2o, ".%d", SeExpr2locp->first_column);
    }
  if (0 <= SeExpr2locp->last_line)
    {
      if (SeExpr2locp->first_line < SeExpr2locp->last_line)
        {
          res += fprintf (SeExpr2o, "-%d", SeExpr2locp->last_line);
          if (0 <= end_col)
            res += fprintf (SeExpr2o, ".%d", end_col);
        }
      else if (0 <= end_col && SeExpr2locp->first_column < end_col)
        res += fprintf (SeExpr2o, "-%d", end_col);
    }
  return res;
 }

#  define SeExprYY_LOCATION_PRINT(File, Loc)          \
  SeExpr2_location_print_ (File, &(Loc))

# else
#  define SeExprYY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* SeExprYYLEX -- calling `SeExpr2lex' with the right arguments.  */
#ifdef SeExprYYLEX_PARAM
# define SeExprYYLEX SeExpr2lex (SeExprYYLEX_PARAM)
#else
# define SeExprYYLEX SeExpr2lex ()
#endif

/* Enable debugging if requested.  */
#if SeExprYYDEBUG

# ifndef SeExprYYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define SeExprYYFPRINTF fprintf
# endif

# define SeExprYYDPRINTF(Args)			\
do {						\
  if (SeExpr2debug)					\
    SeExprYYFPRINTF Args;				\
} while (SeExprYYID (0))

# define SeExprYY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (SeExpr2debug)								  \
    {									  \
      SeExprYYFPRINTF (stderr, "%s ", Title);					  \
      SeExpr2_symbol_print (stderr,						  \
		  Type, Value, Location); \
      SeExprYYFPRINTF (stderr, "\n");						  \
    }									  \
} while (SeExprYYID (0))


/*--------------------------------.
| Print this symbol on SeExprYYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
SeExpr2_symbol_value_print (FILE *SeExpr2output, int SeExpr2type, SeExprYYSTYPE const * const SeExpr2valuep, SeExprYYLTYPE const * const SeExpr2locationp)
#else
static void
SeExpr2_symbol_value_print (SeExpr2output, SeExpr2type, SeExpr2valuep, SeExpr2locationp)
    FILE *SeExpr2output;
    int SeExpr2type;
    SeExprYYSTYPE const * const SeExpr2valuep;
    SeExprYYLTYPE const * const SeExpr2locationp;
#endif
{
  FILE *SeExpr2o = SeExpr2output;
  SeExprYYUSE (SeExpr2o);
  if (!SeExpr2valuep)
    return;
  SeExprYYUSE (SeExpr2locationp);
# ifdef SeExprYYPRINT
  if (SeExpr2type < SeExprYYNTOKENS)
    SeExprYYPRINT (SeExpr2output, SeExpr2toknum[SeExpr2type], *SeExpr2valuep);
# else
  SeExprYYUSE (SeExpr2output);
# endif
  switch (SeExpr2type)
    {
      default:
        break;
    }
}


/*--------------------------------.
| Print this symbol on SeExprYYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
SeExpr2_symbol_print (FILE *SeExpr2output, int SeExpr2type, SeExprYYSTYPE const * const SeExpr2valuep, SeExprYYLTYPE const * const SeExpr2locationp)
#else
static void
SeExpr2_symbol_print (SeExpr2output, SeExpr2type, SeExpr2valuep, SeExpr2locationp)
    FILE *SeExpr2output;
    int SeExpr2type;
    SeExprYYSTYPE const * const SeExpr2valuep;
    SeExprYYLTYPE const * const SeExpr2locationp;
#endif
{
  if (SeExpr2type < SeExprYYNTOKENS)
    SeExprYYFPRINTF (SeExpr2output, "token %s (", SeExpr2tname[SeExpr2type]);
  else
    SeExprYYFPRINTF (SeExpr2output, "nterm %s (", SeExpr2tname[SeExpr2type]);

  SeExprYY_LOCATION_PRINT (SeExpr2output, *SeExpr2locationp);
  SeExprYYFPRINTF (SeExpr2output, ": ");
  SeExpr2_symbol_value_print (SeExpr2output, SeExpr2type, SeExpr2valuep, SeExpr2locationp);
  SeExprYYFPRINTF (SeExpr2output, ")");
}

/*------------------------------------------------------------------.
| SeExpr2_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
SeExpr2_stack_print (SeExpr2type_int16 *SeExpr2bottom, SeExpr2type_int16 *SeExpr2top)
#else
static void
SeExpr2_stack_print (SeExpr2bottom, SeExpr2top)
    SeExpr2type_int16 *SeExpr2bottom;
    SeExpr2type_int16 *SeExpr2top;
#endif
{
  SeExprYYFPRINTF (stderr, "Stack now");
  for (; SeExpr2bottom <= SeExpr2top; SeExpr2bottom++)
    {
      int SeExpr2bot = *SeExpr2bottom;
      SeExprYYFPRINTF (stderr, " %d", SeExpr2bot);
    }
  SeExprYYFPRINTF (stderr, "\n");
}

# define SeExprYY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (SeExpr2debug)							\
    SeExpr2_stack_print ((Bottom), (Top));				\
} while (SeExprYYID (0))


/*------------------------------------------------.
| Report that the SeExprYYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
SeExpr2_reduce_print (SeExprYYSTYPE *SeExpr2vsp, SeExprYYLTYPE *SeExpr2lsp, int SeExpr2rule)
#else
static void
SeExpr2_reduce_print (SeExpr2vsp, SeExpr2lsp, SeExpr2rule)
    SeExprYYSTYPE *SeExpr2vsp;
    SeExprYYLTYPE *SeExpr2lsp;
    int SeExpr2rule;
#endif
{
  int SeExpr2nrhs = SeExpr2r2[SeExpr2rule];
  int SeExpr2i;
  unsigned long int SeExpr2lno = SeExpr2rline[SeExpr2rule];
  SeExprYYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     SeExpr2rule - 1, SeExpr2lno);
  /* The symbols being reduced.  */
  for (SeExpr2i = 0; SeExpr2i < SeExpr2nrhs; SeExpr2i++)
    {
      SeExprYYFPRINTF (stderr, "   $%d = ", SeExpr2i + 1);
      SeExpr2_symbol_print (stderr, SeExpr2rhs[SeExpr2prhs[SeExpr2rule] + SeExpr2i],
		       &(SeExpr2vsp[(SeExpr2i + 1) - (SeExpr2nrhs)])
		       , &(SeExpr2lsp[(SeExpr2i + 1) - (SeExpr2nrhs)])		       );
      SeExprYYFPRINTF (stderr, "\n");
    }
}

# define SeExprYY_REDUCE_PRINT(Rule)		\
do {					\
  if (SeExpr2debug)				\
    SeExpr2_reduce_print (SeExpr2vsp, SeExpr2lsp, Rule); \
} while (SeExprYYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int SeExpr2debug;
#else /* !SeExprYYDEBUG */
# define SeExprYYDPRINTF(Args)
# define SeExprYY_SYMBOL_PRINT(Title, Type, Value, Location)
# define SeExprYY_STACK_PRINT(Bottom, Top)
# define SeExprYY_REDUCE_PRINT(Rule)
#endif /* !SeExprYYDEBUG */


/* SeExprYYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	SeExprYYINITDEPTH
# define SeExprYYINITDEPTH 200
#endif

/* SeExprYYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SeExprYYSTACK_ALLOC_MAXIMUM < SeExprYYSTACK_BYTES (SeExprYYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef SeExprYYMAXDEPTH
# define SeExprYYMAXDEPTH 10000
#endif


#if SeExprYYERROR_VERBOSE

# ifndef SeExpr2strlen
#  if defined __GLIBC__ && defined _STRING_H
#   define SeExpr2strlen strlen
#  else
/* Return the length of SeExprYYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static SeExprYYSIZE_T
SeExpr2strlen (const char *SeExpr2str)
#else
static SeExprYYSIZE_T
SeExpr2strlen (SeExpr2str)
    const char *SeExpr2str;
#endif
{
  SeExprYYSIZE_T SeExpr2len;
  for (SeExpr2len = 0; SeExpr2str[SeExpr2len]; SeExpr2len++)
    continue;
  return SeExpr2len;
}
#  endif
# endif

# ifndef SeExpr2stpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define SeExpr2stpcpy stpcpy
#  else
/* Copy SeExprYYSRC to SeExprYYDEST, returning the address of the terminating '\0' in
   SeExprYYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
SeExpr2stpcpy (char *SeExpr2dest, const char *SeExpr2src)
#else
static char *
SeExpr2stpcpy (SeExpr2dest, SeExpr2src)
    char *SeExpr2dest;
    const char *SeExpr2src;
#endif
{
  char *SeExpr2d = SeExpr2dest;
  const char *SeExpr2s = SeExpr2src;

  while ((*SeExpr2d++ = *SeExpr2s++) != '\0')
    continue;

  return SeExpr2d - 1;
}
#  endif
# endif

# ifndef SeExpr2tnamerr
/* Copy to SeExprYYRES the contents of SeExprYYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for SeExpr2error.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  SeExprYYSTR is taken from SeExpr2tname.  If SeExprYYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static SeExprYYSIZE_T
SeExpr2tnamerr (char *SeExpr2res, const char *SeExpr2str)
{
  if (*SeExpr2str == '"')
    {
      SeExprYYSIZE_T SeExpr2n = 0;
      char const *SeExpr2p = SeExpr2str;

      for (;;)
	switch (*++SeExpr2p)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++SeExpr2p != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (SeExpr2res)
	      SeExpr2res[SeExpr2n] = *SeExpr2p;
	    SeExpr2n++;
	    break;

	  case '"':
	    if (SeExpr2res)
	      SeExpr2res[SeExpr2n] = '\0';
	    return SeExpr2n;
	  }
    do_not_strip_quotes: ;
    }

  if (! SeExpr2res)
    return SeExpr2strlen (SeExpr2str);

  return SeExpr2stpcpy (SeExpr2res, SeExpr2str) - SeExpr2res;
}
# endif

/* Copy into *SeExprYYMSG, which is of size *SeExprYYMSG_ALLOC, an error message
   about the unexpected token SeExprYYTOKEN for the state stack whose top is
   SeExprYYSSP.

   Return 0 if *SeExprYYMSG was successfully written.  Return 1 if *SeExprYYMSG is
   not large enough to hold the message.  In that case, also set
   *SeExprYYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
SeExpr2syntax_error (SeExprYYSIZE_T *SeExpr2msg_alloc, char **SeExpr2msg,
                SeExpr2type_int16 *SeExpr2ssp, int SeExpr2token)
{
  SeExprYYSIZE_T SeExpr2size0 = SeExpr2tnamerr (SeExprYY_NULL, SeExpr2tname[SeExpr2token]);
  SeExprYYSIZE_T SeExpr2size = SeExpr2size0;
  enum { SeExprYYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *SeExpr2format = SeExprYY_NULL;
  /* Arguments of SeExpr2format. */
  char const *SeExpr2arg[SeExprYYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int SeExpr2count = 0;

  /* There are many possibilities here to consider:
     - Assume SeExprYYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  SeExprYYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in SeExpr2char) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated SeExpr2char.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (SeExpr2token != SeExprYYEMPTY)
    {
      int SeExpr2n = SeExpr2pact[*SeExpr2ssp];
      SeExpr2arg[SeExpr2count++] = SeExpr2tname[SeExpr2token];
      if (!SeExpr2pact_value_is_default (SeExpr2n))
        {
          /* Start SeExprYYX at -SeExprYYN if negative to avoid negative indexes in
             SeExprYYCHECK.  In other words, skip the first -SeExprYYN actions for
             this state because they are default actions.  */
          int SeExpr2xbegin = SeExpr2n < 0 ? -SeExpr2n : 0;
          /* Stay within bounds of both SeExpr2check and SeExpr2tname.  */
          int SeExpr2checklim = SeExprYYLAST - SeExpr2n + 1;
          int SeExpr2xend = SeExpr2checklim < SeExprYYNTOKENS ? SeExpr2checklim : SeExprYYNTOKENS;
          int SeExpr2x;

          for (SeExpr2x = SeExpr2xbegin; SeExpr2x < SeExpr2xend; ++SeExpr2x)
            if (SeExpr2check[SeExpr2x + SeExpr2n] == SeExpr2x && SeExpr2x != SeExprYYTERROR
                && !SeExpr2table_value_is_error (SeExpr2table[SeExpr2x + SeExpr2n]))
              {
                if (SeExpr2count == SeExprYYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    SeExpr2count = 1;
                    SeExpr2size = SeExpr2size0;
                    break;
                  }
                SeExpr2arg[SeExpr2count++] = SeExpr2tname[SeExpr2x];
                {
                  SeExprYYSIZE_T SeExpr2size1 = SeExpr2size + SeExpr2tnamerr (SeExprYY_NULL, SeExpr2tname[SeExpr2x]);
                  if (! (SeExpr2size <= SeExpr2size1
                         && SeExpr2size1 <= SeExprYYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  SeExpr2size = SeExpr2size1;
                }
              }
        }
    }

  switch (SeExpr2count)
    {
# define SeExprYYCASE_(N, S)                      \
      case N:                               \
        SeExpr2format = S;                       \
      break
      SeExprYYCASE_(0, SeExprYY_("syntax error"));
      SeExprYYCASE_(1, SeExprYY_("syntax error, unexpected %s"));
      SeExprYYCASE_(2, SeExprYY_("syntax error, unexpected %s, expecting %s"));
      SeExprYYCASE_(3, SeExprYY_("syntax error, unexpected %s, expecting %s or %s"));
      SeExprYYCASE_(4, SeExprYY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      SeExprYYCASE_(5, SeExprYY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef SeExprYYCASE_
    }

  {
    SeExprYYSIZE_T SeExpr2size1 = SeExpr2size + SeExpr2strlen (SeExpr2format);
    if (! (SeExpr2size <= SeExpr2size1 && SeExpr2size1 <= SeExprYYSTACK_ALLOC_MAXIMUM))
      return 2;
    SeExpr2size = SeExpr2size1;
  }

  if (*SeExpr2msg_alloc < SeExpr2size)
    {
      *SeExpr2msg_alloc = 2 * SeExpr2size;
      if (! (SeExpr2size <= *SeExpr2msg_alloc
             && *SeExpr2msg_alloc <= SeExprYYSTACK_ALLOC_MAXIMUM))
        *SeExpr2msg_alloc = SeExprYYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *SeExpr2p = *SeExpr2msg;
    int SeExpr2i = 0;
    while ((*SeExpr2p = *SeExpr2format) != '\0')
      if (*SeExpr2p == '%' && SeExpr2format[1] == 's' && SeExpr2i < SeExpr2count)
        {
          SeExpr2p += SeExpr2tnamerr (SeExpr2p, SeExpr2arg[SeExpr2i++]);
          SeExpr2format += 2;
        }
      else
        {
          SeExpr2p++;
          SeExpr2format++;
        }
  }
  return 0;
}
#endif /* SeExprYYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
SeExpr2destruct (const char *SeExpr2msg, int SeExpr2type, SeExprYYSTYPE *SeExpr2valuep, SeExprYYLTYPE *SeExpr2locationp)
#else
static void
SeExpr2destruct (SeExpr2msg, SeExpr2type, SeExpr2valuep, SeExpr2locationp)
    const char *SeExpr2msg;
    int SeExpr2type;
    SeExprYYSTYPE *SeExpr2valuep;
    SeExprYYLTYPE *SeExpr2locationp;
#endif
{
  SeExprYYUSE (SeExpr2valuep);
  SeExprYYUSE (SeExpr2locationp);

  if (!SeExpr2msg)
    SeExpr2msg = "Deleting";
  SeExprYY_SYMBOL_PRINT (SeExpr2msg, SeExpr2type, SeExpr2valuep, SeExpr2locationp);

  switch (SeExpr2type)
    {

      default:
        break;
    }
}




/* The lookahead symbol.  */
int SeExpr2char;


#ifndef SeExprYY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define SeExprYY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define SeExprYY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef SeExprYY_INITIAL_VALUE
# define SeExprYY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
SeExprYYSTYPE SeExpr2lval SeExprYY_INITIAL_VALUE(SeExpr2val_default);

/* Location data for the lookahead symbol.  */
SeExprYYLTYPE SeExpr2lloc
# if defined SeExprYYLTYPE_IS_TRIVIAL && SeExprYYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;


/* Number of syntax errors so far.  */
int SeExpr2nerrs;


/*----------.
| SeExpr2parse.  |
`----------*/

#ifdef SeExprYYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
SeExpr2parse (void *SeExprYYPARSE_PARAM)
#else
int
SeExpr2parse (SeExprYYPARSE_PARAM)
    void *SeExprYYPARSE_PARAM;
#endif
#else /* ! SeExprYYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
SeExpr2parse (void)
#else
int
SeExpr2parse ()

#endif
#endif
{
    int SeExpr2state;
    /* Number of tokens to shift before error messages enabled.  */
    int SeExpr2errstatus;

    /* The stacks and their tools:
       `SeExpr2ss': related to states.
       `SeExpr2vs': related to semantic values.
       `SeExpr2ls': related to locations.

       Refer to the stacks through separate pointers, to allow SeExpr2overflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    SeExpr2type_int16 SeExpr2ssa[SeExprYYINITDEPTH];
    SeExpr2type_int16 *SeExpr2ss;
    SeExpr2type_int16 *SeExpr2ssp;

    /* The semantic value stack.  */
    SeExprYYSTYPE SeExpr2vsa[SeExprYYINITDEPTH];
    SeExprYYSTYPE *SeExpr2vs;
    SeExprYYSTYPE *SeExpr2vsp;

    /* The location stack.  */
    SeExprYYLTYPE SeExpr2lsa[SeExprYYINITDEPTH];
    SeExprYYLTYPE *SeExpr2ls;
    SeExprYYLTYPE *SeExpr2lsp;

    /* The locations where the error started and ended.  */
    SeExprYYLTYPE SeExpr2error_range[3];

    SeExprYYSIZE_T SeExpr2stacksize;

  int SeExpr2n;
  int SeExpr2result;
  /* Lookahead token as an internal (translated) token number.  */
  int SeExpr2token = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  SeExprYYSTYPE SeExpr2val;
  SeExprYYLTYPE SeExpr2loc;

#if SeExprYYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char SeExpr2msgbuf[128];
  char *SeExpr2msg = SeExpr2msgbuf;
  SeExprYYSIZE_T SeExpr2msg_alloc = sizeof SeExpr2msgbuf;
#endif

#define SeExprYYPOPSTACK(N)   (SeExpr2vsp -= (N), SeExpr2ssp -= (N), SeExpr2lsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int SeExpr2len = 0;

  SeExpr2ssp = SeExpr2ss = SeExpr2ssa;
  SeExpr2vsp = SeExpr2vs = SeExpr2vsa;
  SeExpr2lsp = SeExpr2ls = SeExpr2lsa;
  SeExpr2stacksize = SeExprYYINITDEPTH;

  SeExprYYDPRINTF ((stderr, "Starting parse\n"));

  SeExpr2state = 0;
  SeExpr2errstatus = 0;
  SeExpr2nerrs = 0;
  SeExpr2char = SeExprYYEMPTY; /* Cause a token to be read.  */
  SeExpr2lsp[0] = SeExpr2lloc;
  goto SeExpr2setstate;

/*------------------------------------------------------------.
| SeExpr2newstate -- Push a new state, which is found in SeExpr2state.  |
`------------------------------------------------------------*/
 SeExpr2newstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  SeExpr2ssp++;

 SeExpr2setstate:
  *SeExpr2ssp = SeExpr2state;

  if (SeExpr2ss + SeExpr2stacksize - 1 <= SeExpr2ssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      SeExprYYSIZE_T SeExpr2size = SeExpr2ssp - SeExpr2ss + 1;

#ifdef SeExpr2overflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	SeExprYYSTYPE *SeExpr2vs1 = SeExpr2vs;
	SeExpr2type_int16 *SeExpr2ss1 = SeExpr2ss;
	SeExprYYLTYPE *SeExpr2ls1 = SeExpr2ls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if SeExpr2overflow is a macro.  */
	SeExpr2overflow (SeExprYY_("memory exhausted"),
		    &SeExpr2ss1, SeExpr2size * sizeof (*SeExpr2ssp),
		    &SeExpr2vs1, SeExpr2size * sizeof (*SeExpr2vsp),
		    &SeExpr2ls1, SeExpr2size * sizeof (*SeExpr2lsp),
		    &SeExpr2stacksize);

	SeExpr2ls = SeExpr2ls1;
	SeExpr2ss = SeExpr2ss1;
	SeExpr2vs = SeExpr2vs1;
      }
#else /* no SeExpr2overflow */
# ifndef SeExprYYSTACK_RELOCATE
      goto SeExpr2exhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (SeExprYYMAXDEPTH <= SeExpr2stacksize)
	goto SeExpr2exhaustedlab;
      SeExpr2stacksize *= 2;
      if (SeExprYYMAXDEPTH < SeExpr2stacksize)
	SeExpr2stacksize = SeExprYYMAXDEPTH;

      {
	SeExpr2type_int16 *SeExpr2ss1 = SeExpr2ss;
	union SeExpr2alloc *SeExpr2ptr =
	  (union SeExpr2alloc *) SeExprYYSTACK_ALLOC (SeExprYYSTACK_BYTES (SeExpr2stacksize));
	if (! SeExpr2ptr)
	  goto SeExpr2exhaustedlab;
	SeExprYYSTACK_RELOCATE (SeExpr2ss_alloc, SeExpr2ss);
	SeExprYYSTACK_RELOCATE (SeExpr2vs_alloc, SeExpr2vs);
	SeExprYYSTACK_RELOCATE (SeExpr2ls_alloc, SeExpr2ls);
#  undef SeExprYYSTACK_RELOCATE
	if (SeExpr2ss1 != SeExpr2ssa)
	  SeExprYYSTACK_FREE (SeExpr2ss1);
      }
# endif
#endif /* no SeExpr2overflow */

      SeExpr2ssp = SeExpr2ss + SeExpr2size - 1;
      SeExpr2vsp = SeExpr2vs + SeExpr2size - 1;
      SeExpr2lsp = SeExpr2ls + SeExpr2size - 1;

      SeExprYYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) SeExpr2stacksize));

      if (SeExpr2ss + SeExpr2stacksize - 1 <= SeExpr2ssp)
	SeExprYYABORT;
    }

  SeExprYYDPRINTF ((stderr, "Entering state %d\n", SeExpr2state));

  if (SeExpr2state == SeExprYYFINAL)
    SeExprYYACCEPT;

  goto SeExpr2backup;

/*-----------.
| SeExpr2backup.  |
`-----------*/
SeExpr2backup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  SeExpr2n = SeExpr2pact[SeExpr2state];
  if (SeExpr2pact_value_is_default (SeExpr2n))
    goto SeExpr2default;

  /* Not known => get a lookahead token if don't already have one.  */

  /* SeExprYYCHAR is either SeExprYYEMPTY or SeExprYYEOF or a valid lookahead symbol.  */
  if (SeExpr2char == SeExprYYEMPTY)
    {
      SeExprYYDPRINTF ((stderr, "Reading a token: "));
      SeExpr2char = SeExprYYLEX;
    }

  if (SeExpr2char <= SeExprYYEOF)
    {
      SeExpr2char = SeExpr2token = SeExprYYEOF;
      SeExprYYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      SeExpr2token = SeExprYYTRANSLATE (SeExpr2char);
      SeExprYY_SYMBOL_PRINT ("Next token is", SeExpr2token, &SeExpr2lval, &SeExpr2lloc);
    }

  /* If the proper action on seeing token SeExprYYTOKEN is to reduce or to
     detect an error, take that action.  */
  SeExpr2n += SeExpr2token;
  if (SeExpr2n < 0 || SeExprYYLAST < SeExpr2n || SeExpr2check[SeExpr2n] != SeExpr2token)
    goto SeExpr2default;
  SeExpr2n = SeExpr2table[SeExpr2n];
  if (SeExpr2n <= 0)
    {
      if (SeExpr2table_value_is_error (SeExpr2n))
        goto SeExpr2errlab;
      SeExpr2n = -SeExpr2n;
      goto SeExpr2reduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (SeExpr2errstatus)
    SeExpr2errstatus--;

  /* Shift the lookahead token.  */
  SeExprYY_SYMBOL_PRINT ("Shifting", SeExpr2token, &SeExpr2lval, &SeExpr2lloc);

  /* Discard the shifted token.  */
  SeExpr2char = SeExprYYEMPTY;

  SeExpr2state = SeExpr2n;
  SeExprYY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++SeExpr2vsp = SeExpr2lval;
  SeExprYY_IGNORE_MAYBE_UNINITIALIZED_END
  *++SeExpr2lsp = SeExpr2lloc;
  goto SeExpr2newstate;


/*-----------------------------------------------------------.
| SeExpr2default -- do the default action for the current state.  |
`-----------------------------------------------------------*/
SeExpr2default:
  SeExpr2n = SeExpr2defact[SeExpr2state];
  if (SeExpr2n == 0)
    goto SeExpr2errlab;
  goto SeExpr2reduce;


/*-----------------------------.
| SeExpr2reduce -- Do a reduction.  |
`-----------------------------*/
SeExpr2reduce:
  /* SeExpr2n is the number of a rule to reduce with.  */
  SeExpr2len = SeExpr2r2[SeExpr2n];

  /* If SeExprYYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets SeExprYYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to SeExprYYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that SeExprYYVAL may be used uninitialized.  */
  SeExpr2val = SeExpr2vsp[1-SeExpr2len];

  /* Default location.  */
  SeExprYYLLOC_DEFAULT (SeExpr2loc, (SeExpr2lsp - SeExpr2len), SeExpr2len);
  SeExprYY_REDUCE_PRINT (SeExpr2n);
  switch (SeExpr2n)
    {
        case 2:
/* Line 1792 of yacc.c  */
#line 132 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { ParseResult = (SeExpr2vsp[(1) - (2)].n); ParseResult->setPosition((SeExpr2loc).first_column, (SeExpr2loc).last_column);
                                  ParseResult->addChild((SeExpr2vsp[(2) - (2)].n)); }
    break;

  case 3:
/* Line 1792 of yacc.c  */
#line 134 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { ParseResult = NODE((SeExpr2loc).first_column, (SeExpr2loc).last_column, ModuleNode);
                                  ParseResult->addChild((SeExpr2vsp[(1) - (1)].n)); }
    break;

  case 4:
/* Line 1792 of yacc.c  */
#line 139 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE((SeExpr2loc).first_column, (SeExpr2loc).last_column, ModuleNode); (SeExpr2val.n)->addChild((SeExpr2vsp[(1) - (1)].n)); }
    break;

  case 5:
/* Line 1792 of yacc.c  */
#line 141 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = (SeExpr2vsp[(1) - (2)].n); (SeExpr2val.n)->setPosition((SeExpr2loc).first_column, (SeExpr2loc).last_column);
                                  (SeExpr2val.n)->addChild((SeExpr2vsp[(2) - (2)].n)); }
    break;

  case 6:
/* Line 1792 of yacc.c  */
#line 147 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { SeExpr2::ExprType type = SeExpr2::ExprType((SeExpr2vsp[(2) - (6)].t).type, (SeExpr2vsp[(2) - (6)].t).dim, (SeExpr2vsp[(2) - (6)].t).lifetime);
                                    SeExpr2::ExprPrototypeNode * prototype =
                                        (SeExpr2::ExprPrototypeNode*)NODE2((SeExpr2loc).first_column, (SeExpr2loc).last_column, PrototypeNode, (SeExpr2vsp[(3) - (6)].s), type);
                                  prototype->addArgTypes((SeExpr2vsp[(5) - (6)].n));
                                  Forget((SeExpr2vsp[(5) - (6)].n));
                                  (SeExpr2val.n) = prototype;
                                  free((SeExpr2vsp[(3) - (6)].s)); }
    break;

  case 7:
/* Line 1792 of yacc.c  */
#line 155 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { SeExpr2::ExprType type = SeExpr2::ExprType((SeExpr2vsp[(2) - (9)].t).type, (SeExpr2vsp[(2) - (9)].t).dim, (SeExpr2vsp[(2) - (9)].t).lifetime);
                                  SeExpr2::ExprPrototypeNode * prototype =
                                      (SeExpr2::ExprPrototypeNode*)NODE2((SeExpr2loc).first_column, (SeExpr2lsp[(6) - (9)]).last_column, PrototypeNode, (SeExpr2vsp[(3) - (9)].s), type);
                                  prototype->addArgs((SeExpr2vsp[(5) - (9)].n));
                                  Forget((SeExpr2vsp[(5) - (9)].n));
                                  (SeExpr2val.n) = NODE2((SeExpr2loc).first_column, (SeExpr2loc).last_column, LocalFunctionNode, prototype, (SeExpr2vsp[(8) - (9)].n));
                                  free((SeExpr2vsp[(3) - (9)].s)); }
    break;

  case 8:
/* Line 1792 of yacc.c  */
#line 163 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { SeExpr2::ExprPrototypeNode * prototype =
                                        (SeExpr2::ExprPrototypeNode*)NODE1((SeExpr2loc).first_column, (SeExpr2lsp[(5) - (8)]).last_column, PrototypeNode, (SeExpr2vsp[(2) - (8)].s));
                                  prototype->addArgs((SeExpr2vsp[(4) - (8)].n));
                                  Forget((SeExpr2vsp[(4) - (8)].n));
                                  (SeExpr2val.n) = NODE2((SeExpr2loc).first_column, (SeExpr2loc).last_column, LocalFunctionNode, prototype, (SeExpr2vsp[(7) - (8)].n));
                                  free((SeExpr2vsp[(2) - (8)].s)); }
    break;

  case 9:
/* Line 1792 of yacc.c  */
#line 172 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.l) = SeExpr2::ExprType::ltVARYING; }
    break;

  case 10:
/* Line 1792 of yacc.c  */
#line 173 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.l) = SeExpr2::ExprType::ltCONSTANT; }
    break;

  case 11:
/* Line 1792 of yacc.c  */
#line 174 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.l) = SeExpr2::ExprType::ltUNIFORM; }
    break;

  case 12:
/* Line 1792 of yacc.c  */
#line 175 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.l) = SeExpr2::ExprType::ltVARYING; }
    break;

  case 13:
/* Line 1792 of yacc.c  */
#line 176 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.l) = SeExpr2::ExprType::ltERROR; }
    break;

  case 14:
/* Line 1792 of yacc.c  */
#line 180 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    {(SeExpr2val.t).type     = SeExpr2::ExprType::tFP;
                                  (SeExpr2val.t).dim      = 1;
                                  (SeExpr2val.t).lifetime = (SeExpr2vsp[(2) - (2)].l); }
    break;

  case 15:
/* Line 1792 of yacc.c  */
#line 184 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.t).type = ((SeExpr2vsp[(3) - (5)].d) > 0 ? SeExpr2::ExprType::tFP : SeExpr2::ExprType::tERROR);
                                  //TODO: This causes an error but does not report it to user. Change this.
                                  (SeExpr2val.t).dim  = ((SeExpr2vsp[(3) - (5)].d) > 0 ? (SeExpr2vsp[(3) - (5)].d) : 0);
                                  (SeExpr2val.t).lifetime = (SeExpr2vsp[(5) - (5)].l); }
    break;

  case 16:
/* Line 1792 of yacc.c  */
#line 188 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.t).type = SeExpr2::ExprType::tSTRING;
                                  (SeExpr2val.t).dim  = 1;
                                  (SeExpr2val.t).lifetime = (SeExpr2vsp[(2) - (2)].l); }
    break;

  case 17:
/* Line 1792 of yacc.c  */
#line 194 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE((SeExpr2loc).first_column, (SeExpr2loc).last_column, Node); }
    break;

  case 18:
/* Line 1792 of yacc.c  */
#line 195 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = (SeExpr2vsp[(1) - (1)].n); }
    break;

  case 19:
/* Line 1792 of yacc.c  */
#line 199 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE((SeExpr2loc).first_column, (SeExpr2loc).last_column, Node);
                                  SeExpr2::ExprType type = SeExpr2::ExprType((SeExpr2vsp[(1) - (1)].t).type, (SeExpr2vsp[(1) - (1)].t).dim, (SeExpr2vsp[(1) - (1)].t).lifetime);
                                  SeExpr2::ExprNode* varNode = NODE2((SeExpr2loc).first_column, (SeExpr2loc).last_column, VarNode, 0, type);
                                  (SeExpr2val.n)->addChild(varNode); }
    break;

  case 20:
/* Line 1792 of yacc.c  */
#line 203 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = (SeExpr2vsp[(1) - (3)].n);
                                  SeExpr2::ExprType type = SeExpr2::ExprType((SeExpr2vsp[(3) - (3)].t).type, (SeExpr2vsp[(3) - (3)].t).dim, (SeExpr2vsp[(3) - (3)].t).lifetime);
                                  SeExpr2::ExprNode* varNode = NODE2((SeExpr2lsp[(3) - (3)]).first_column, (SeExpr2lsp[(3) - (3)]).last_column, VarNode, 0, type);
                                  (SeExpr2val.n)->addChild(varNode); }
    break;

  case 21:
/* Line 1792 of yacc.c  */
#line 210 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE((SeExpr2loc).first_column, (SeExpr2loc).last_column, Node); }
    break;

  case 22:
/* Line 1792 of yacc.c  */
#line 211 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = (SeExpr2vsp[(1) - (1)].n); }
    break;

  case 23:
/* Line 1792 of yacc.c  */
#line 215 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE((SeExpr2loc).first_column, (SeExpr2loc).last_column, Node);
                                  SeExpr2::ExprType type = SeExpr2::ExprType((SeExpr2vsp[(1) - (2)].t).type, (SeExpr2vsp[(1) - (2)].t).dim, (SeExpr2vsp[(1) - (2)].t).lifetime);
                                  SeExpr2::ExprNode* varNode = NODE2((SeExpr2loc).first_column, (SeExpr2loc).last_column, VarNode, (SeExpr2vsp[(2) - (2)].s), type);
                                  (SeExpr2val.n)->addChild(varNode);
                                  free((SeExpr2vsp[(2) - (2)].s)); }
    break;

  case 24:
/* Line 1792 of yacc.c  */
#line 221 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = (SeExpr2vsp[(1) - (4)].n);
                                  SeExpr2::ExprType type = SeExpr2::ExprType((SeExpr2vsp[(3) - (4)].t).type, (SeExpr2vsp[(3) - (4)].t).dim, (SeExpr2vsp[(3) - (4)].t).lifetime);
                                  SeExpr2::ExprNode* varNode = NODE2((SeExpr2lsp[(3) - (4)]).first_column, (SeExpr2lsp[(4) - (4)]).last_column, VarNode, (SeExpr2vsp[(4) - (4)].s), type);
                                  (SeExpr2val.n)->addChild(varNode);
                                  free((SeExpr2vsp[(4) - (4)].s)); }
    break;

  case 25:
/* Line 1792 of yacc.c  */
#line 229 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE2((SeExpr2loc).first_column,(SeExpr2loc).last_column,BlockNode, (SeExpr2vsp[(1) - (2)].n), (SeExpr2vsp[(2) - (2)].n)); }
    break;

  case 26:
/* Line 1792 of yacc.c  */
#line 230 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = (SeExpr2vsp[(1) - (1)].n); }
    break;

  case 27:
/* Line 1792 of yacc.c  */
#line 235 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE((SeExpr2loc).first_column,(SeExpr2loc).last_column,Node); /* create empty node */; }
    break;

  case 28:
/* Line 1792 of yacc.c  */
#line 236 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = (SeExpr2vsp[(1) - (1)].n); }
    break;

  case 29:
/* Line 1792 of yacc.c  */
#line 240 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE1((SeExpr2loc).first_column,(SeExpr2loc).last_column,Node, (SeExpr2vsp[(1) - (1)].n)); /* create var list */}
    break;

  case 30:
/* Line 1792 of yacc.c  */
#line 241 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = (SeExpr2vsp[(1) - (2)].n); (SeExpr2vsp[(1) - (2)].n)->addChild((SeExpr2vsp[(2) - (2)].n)); /* add to list */}
    break;

  case 31:
/* Line 1792 of yacc.c  */
#line 245 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = (SeExpr2vsp[(1) - (1)].n); }
    break;

  case 32:
/* Line 1792 of yacc.c  */
#line 246 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE2((SeExpr2loc).first_column,(SeExpr2loc).last_column,AssignNode, (SeExpr2vsp[(1) - (4)].s), (SeExpr2vsp[(3) - (4)].n)); free((SeExpr2vsp[(1) - (4)].s)); }
    break;

  case 33:
/* Line 1792 of yacc.c  */
#line 247 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    {SeExpr2::ExprNode* varNode=NODE1((SeExpr2lsp[(1) - (4)]).first_column,(SeExpr2lsp[(1) - (4)]).first_column,VarNode, (SeExpr2vsp[(1) - (4)].s));
                               SeExpr2::ExprNode* opNode=NODE3((SeExpr2lsp[(3) - (4)]).first_column,(SeExpr2lsp[(3) - (4)]).first_column,BinaryOpNode,varNode,(SeExpr2vsp[(3) - (4)].n),'+');
                                (SeExpr2val.n) = NODE2((SeExpr2loc).first_column,(SeExpr2loc).last_column,AssignNode, (SeExpr2vsp[(1) - (4)].s), opNode);free((SeExpr2vsp[(1) - (4)].s));}
    break;

  case 34:
/* Line 1792 of yacc.c  */
#line 250 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    {SeExpr2::ExprNode* varNode=NODE1((SeExpr2lsp[(1) - (4)]).first_column,(SeExpr2lsp[(1) - (4)]).first_column,VarNode, (SeExpr2vsp[(1) - (4)].s));
                               SeExpr2::ExprNode* opNode=NODE3((SeExpr2lsp[(3) - (4)]).first_column,(SeExpr2lsp[(3) - (4)]).first_column,BinaryOpNode,varNode,(SeExpr2vsp[(3) - (4)].n),'-');
                                (SeExpr2val.n) = NODE2((SeExpr2loc).first_column,(SeExpr2loc).last_column,AssignNode, (SeExpr2vsp[(1) - (4)].s), opNode);free((SeExpr2vsp[(1) - (4)].s));}
    break;

  case 35:
/* Line 1792 of yacc.c  */
#line 253 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    {SeExpr2::ExprNode* varNode=NODE1((SeExpr2lsp[(1) - (4)]).first_column,(SeExpr2lsp[(1) - (4)]).first_column,VarNode, (SeExpr2vsp[(1) - (4)].s));
                               SeExpr2::ExprNode* opNode=NODE3((SeExpr2lsp[(3) - (4)]).first_column,(SeExpr2lsp[(3) - (4)]).first_column,BinaryOpNode,varNode,(SeExpr2vsp[(3) - (4)].n),'*');
                                (SeExpr2val.n) = NODE2((SeExpr2loc).first_column,(SeExpr2loc).last_column,AssignNode, (SeExpr2vsp[(1) - (4)].s), opNode);free((SeExpr2vsp[(1) - (4)].s));}
    break;

  case 36:
/* Line 1792 of yacc.c  */
#line 256 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    {SeExpr2::ExprNode* varNode=NODE1((SeExpr2lsp[(1) - (4)]).first_column,(SeExpr2lsp[(1) - (4)]).first_column,VarNode, (SeExpr2vsp[(1) - (4)].s));
                               SeExpr2::ExprNode* opNode=NODE3((SeExpr2lsp[(3) - (4)]).first_column,(SeExpr2lsp[(3) - (4)]).first_column,BinaryOpNode,varNode,(SeExpr2vsp[(3) - (4)].n),'/');
                                (SeExpr2val.n) = NODE2((SeExpr2loc).first_column,(SeExpr2loc).last_column,AssignNode, (SeExpr2vsp[(1) - (4)].s), opNode);free((SeExpr2vsp[(1) - (4)].s));}
    break;

  case 37:
/* Line 1792 of yacc.c  */
#line 259 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    {SeExpr2::ExprNode* varNode=NODE1((SeExpr2lsp[(1) - (4)]).first_column,(SeExpr2lsp[(1) - (4)]).first_column,VarNode, (SeExpr2vsp[(1) - (4)].s));
                               SeExpr2::ExprNode* opNode=NODE3((SeExpr2lsp[(3) - (4)]).first_column,(SeExpr2lsp[(3) - (4)]).first_column,BinaryOpNode,varNode,(SeExpr2vsp[(3) - (4)].n),'^');
                                (SeExpr2val.n) = NODE2((SeExpr2loc).first_column,(SeExpr2loc).last_column,AssignNode, (SeExpr2vsp[(1) - (4)].s), opNode);free((SeExpr2vsp[(1) - (4)].s));}
    break;

  case 38:
/* Line 1792 of yacc.c  */
#line 262 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    {SeExpr2::ExprNode* varNode=NODE1((SeExpr2lsp[(1) - (4)]).first_column,(SeExpr2lsp[(1) - (4)]).first_column,VarNode, (SeExpr2vsp[(1) - (4)].s));
                               SeExpr2::ExprNode* opNode=NODE3((SeExpr2lsp[(3) - (4)]).first_column,(SeExpr2lsp[(3) - (4)]).first_column,BinaryOpNode,varNode,(SeExpr2vsp[(3) - (4)].n),'%');
                                (SeExpr2val.n) = NODE2((SeExpr2loc).first_column,(SeExpr2loc).last_column,AssignNode, (SeExpr2vsp[(1) - (4)].s), opNode);free((SeExpr2vsp[(1) - (4)].s));}
    break;

  case 39:
/* Line 1792 of yacc.c  */
#line 265 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE2((SeExpr2loc).first_column,(SeExpr2loc).last_column,AssignNode, (SeExpr2vsp[(1) - (4)].s), (SeExpr2vsp[(3) - (4)].n)); free((SeExpr2vsp[(1) - (4)].s)); }
    break;

  case 40:
/* Line 1792 of yacc.c  */
#line 266 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    {SeExpr2::ExprNode* varNode=NODE1((SeExpr2lsp[(1) - (4)]).first_column,(SeExpr2lsp[(1) - (4)]).first_column,VarNode, (SeExpr2vsp[(1) - (4)].s));
                               SeExpr2::ExprNode* opNode=NODE3((SeExpr2lsp[(3) - (4)]).first_column,(SeExpr2lsp[(3) - (4)]).first_column,BinaryOpNode,varNode,(SeExpr2vsp[(3) - (4)].n),'+');
                                (SeExpr2val.n) = NODE2((SeExpr2loc).first_column,(SeExpr2loc).last_column,AssignNode, (SeExpr2vsp[(1) - (4)].s), opNode);free((SeExpr2vsp[(1) - (4)].s));}
    break;

  case 41:
/* Line 1792 of yacc.c  */
#line 269 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    {SeExpr2::ExprNode* varNode=NODE1((SeExpr2lsp[(1) - (4)]).first_column,(SeExpr2lsp[(1) - (4)]).first_column,VarNode, (SeExpr2vsp[(1) - (4)].s));
                               SeExpr2::ExprNode* opNode=NODE3((SeExpr2lsp[(3) - (4)]).first_column,(SeExpr2lsp[(3) - (4)]).first_column,BinaryOpNode,varNode,(SeExpr2vsp[(3) - (4)].n),'-');
                                (SeExpr2val.n) = NODE2((SeExpr2loc).first_column,(SeExpr2loc).last_column,AssignNode, (SeExpr2vsp[(1) - (4)].s), opNode);free((SeExpr2vsp[(1) - (4)].s));}
    break;

  case 42:
/* Line 1792 of yacc.c  */
#line 272 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    {SeExpr2::ExprNode* varNode=NODE1((SeExpr2lsp[(1) - (4)]).first_column,(SeExpr2lsp[(1) - (4)]).first_column,VarNode, (SeExpr2vsp[(1) - (4)].s));
                               SeExpr2::ExprNode* opNode=NODE3((SeExpr2lsp[(3) - (4)]).first_column,(SeExpr2lsp[(3) - (4)]).first_column,BinaryOpNode,varNode,(SeExpr2vsp[(3) - (4)].n),'*');
                                (SeExpr2val.n) = NODE2((SeExpr2loc).first_column,(SeExpr2loc).last_column,AssignNode, (SeExpr2vsp[(1) - (4)].s), opNode);free((SeExpr2vsp[(1) - (4)].s));}
    break;

  case 43:
/* Line 1792 of yacc.c  */
#line 275 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    {SeExpr2::ExprNode* varNode=NODE1((SeExpr2lsp[(1) - (4)]).first_column,(SeExpr2lsp[(1) - (4)]).first_column,VarNode, (SeExpr2vsp[(1) - (4)].s));
                               SeExpr2::ExprNode* opNode=NODE3((SeExpr2lsp[(3) - (4)]).first_column,(SeExpr2lsp[(3) - (4)]).first_column,BinaryOpNode,varNode,(SeExpr2vsp[(3) - (4)].n),'/');
                                (SeExpr2val.n) = NODE2((SeExpr2loc).first_column,(SeExpr2loc).last_column,AssignNode, (SeExpr2vsp[(1) - (4)].s), opNode);free((SeExpr2vsp[(1) - (4)].s));}
    break;

  case 44:
/* Line 1792 of yacc.c  */
#line 278 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    {SeExpr2::ExprNode* varNode=NODE1((SeExpr2lsp[(1) - (4)]).first_column,(SeExpr2lsp[(1) - (4)]).first_column,VarNode, (SeExpr2vsp[(1) - (4)].s));
                               SeExpr2::ExprNode* opNode=NODE3((SeExpr2lsp[(3) - (4)]).first_column,(SeExpr2lsp[(3) - (4)]).first_column,BinaryOpNode,varNode,(SeExpr2vsp[(3) - (4)].n),'^');
                                (SeExpr2val.n) = NODE2((SeExpr2loc).first_column,(SeExpr2loc).last_column,AssignNode, (SeExpr2vsp[(1) - (4)].s), opNode);free((SeExpr2vsp[(1) - (4)].s));}
    break;

  case 45:
/* Line 1792 of yacc.c  */
#line 281 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    {SeExpr2::ExprNode* varNode=NODE1((SeExpr2lsp[(1) - (4)]).first_column,(SeExpr2lsp[(1) - (4)]).first_column,VarNode, (SeExpr2vsp[(1) - (4)].s));
                               SeExpr2::ExprNode* opNode=NODE3((SeExpr2lsp[(3) - (4)]).first_column,(SeExpr2lsp[(3) - (4)]).first_column,BinaryOpNode,varNode,(SeExpr2vsp[(3) - (4)].n),'%');
                                (SeExpr2val.n) = NODE2((SeExpr2loc).first_column,(SeExpr2loc).last_column,AssignNode, (SeExpr2vsp[(1) - (4)].s), opNode);free((SeExpr2vsp[(1) - (4)].s));}
    break;

  case 46:
/* Line 1792 of yacc.c  */
#line 288 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE3((SeExpr2loc).first_column,(SeExpr2loc).last_column,IfThenElseNode, (SeExpr2vsp[(3) - (8)].n), (SeExpr2vsp[(6) - (8)].n), (SeExpr2vsp[(8) - (8)].n)); }
    break;

  case 47:
/* Line 1792 of yacc.c  */
#line 292 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE((SeExpr2loc).first_column,(SeExpr2loc).last_column,Node); /* create empty node */ }
    break;

  case 48:
/* Line 1792 of yacc.c  */
#line 293 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = (SeExpr2vsp[(3) - (4)].n); }
    break;

  case 49:
/* Line 1792 of yacc.c  */
#line 294 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = (SeExpr2vsp[(2) - (2)].n); }
    break;

  case 50:
/* Line 1792 of yacc.c  */
#line 299 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = (SeExpr2vsp[(2) - (3)].n); }
    break;

  case 51:
/* Line 1792 of yacc.c  */
#line 300 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { SeExpr2::ExprNode* newNode = NODE((SeExpr2loc).first_column,(SeExpr2loc).last_column,VecNode); newNode->addChildren((SeExpr2vsp[(2) - (3)].n)); Forget((SeExpr2vsp[(2) - (3)].n)); (SeExpr2val.n)=newNode;}
    break;

  case 52:
/* Line 1792 of yacc.c  */
#line 301 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE2((SeExpr2loc).first_column,(SeExpr2loc).last_column,SubscriptNode, (SeExpr2vsp[(1) - (4)].n), (SeExpr2vsp[(3) - (4)].n)); }
    break;

  case 53:
/* Line 1792 of yacc.c  */
#line 302 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE3((SeExpr2loc).first_column,(SeExpr2loc).last_column,CondNode, (SeExpr2vsp[(1) - (5)].n), (SeExpr2vsp[(3) - (5)].n), (SeExpr2vsp[(5) - (5)].n)); }
    break;

  case 54:
/* Line 1792 of yacc.c  */
#line 303 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE3((SeExpr2loc).first_column,(SeExpr2loc).last_column,CompareNode, (SeExpr2vsp[(1) - (3)].n), (SeExpr2vsp[(3) - (3)].n), '|'); }
    break;

  case 55:
/* Line 1792 of yacc.c  */
#line 304 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE3((SeExpr2loc).first_column,(SeExpr2loc).last_column,CompareNode, (SeExpr2vsp[(1) - (3)].n), (SeExpr2vsp[(3) - (3)].n), '&'); }
    break;

  case 56:
/* Line 1792 of yacc.c  */
#line 305 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE3((SeExpr2loc).first_column,(SeExpr2loc).last_column,CompareEqNode, (SeExpr2vsp[(1) - (3)].n), (SeExpr2vsp[(3) - (3)].n),'='); }
    break;

  case 57:
/* Line 1792 of yacc.c  */
#line 306 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE3((SeExpr2loc).first_column,(SeExpr2loc).last_column,CompareEqNode, (SeExpr2vsp[(1) - (3)].n), (SeExpr2vsp[(3) - (3)].n),'!'); }
    break;

  case 58:
/* Line 1792 of yacc.c  */
#line 307 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE3((SeExpr2loc).first_column,(SeExpr2loc).last_column,CompareNode, (SeExpr2vsp[(1) - (3)].n), (SeExpr2vsp[(3) - (3)].n),'<'); }
    break;

  case 59:
/* Line 1792 of yacc.c  */
#line 308 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE3((SeExpr2loc).first_column,(SeExpr2loc).last_column,CompareNode, (SeExpr2vsp[(1) - (3)].n), (SeExpr2vsp[(3) - (3)].n),'>'); }
    break;

  case 60:
/* Line 1792 of yacc.c  */
#line 309 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE3((SeExpr2loc).first_column,(SeExpr2loc).last_column,CompareNode, (SeExpr2vsp[(1) - (3)].n), (SeExpr2vsp[(3) - (3)].n),'l'); }
    break;

  case 61:
/* Line 1792 of yacc.c  */
#line 310 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE3((SeExpr2loc).first_column,(SeExpr2loc).last_column,CompareNode, (SeExpr2vsp[(1) - (3)].n), (SeExpr2vsp[(3) - (3)].n),'g'); }
    break;

  case 62:
/* Line 1792 of yacc.c  */
#line 311 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = (SeExpr2vsp[(2) - (2)].n); }
    break;

  case 63:
/* Line 1792 of yacc.c  */
#line 312 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE2((SeExpr2loc).first_column,(SeExpr2loc).last_column,UnaryOpNode, (SeExpr2vsp[(2) - (2)].n), '-'); }
    break;

  case 64:
/* Line 1792 of yacc.c  */
#line 313 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE2((SeExpr2loc).first_column,(SeExpr2loc).last_column,UnaryOpNode, (SeExpr2vsp[(2) - (2)].n), '!'); }
    break;

  case 65:
/* Line 1792 of yacc.c  */
#line 314 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE2((SeExpr2loc).first_column,(SeExpr2loc).last_column,UnaryOpNode, (SeExpr2vsp[(2) - (2)].n), '~'); }
    break;

  case 66:
/* Line 1792 of yacc.c  */
#line 315 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE3((SeExpr2loc).first_column,(SeExpr2loc).last_column,BinaryOpNode, (SeExpr2vsp[(1) - (3)].n), (SeExpr2vsp[(3) - (3)].n), '+'); }
    break;

  case 67:
/* Line 1792 of yacc.c  */
#line 316 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE3((SeExpr2loc).first_column,(SeExpr2loc).last_column,BinaryOpNode, (SeExpr2vsp[(1) - (3)].n), (SeExpr2vsp[(3) - (3)].n), '-'); }
    break;

  case 68:
/* Line 1792 of yacc.c  */
#line 317 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE3((SeExpr2loc).first_column,(SeExpr2loc).last_column,BinaryOpNode, (SeExpr2vsp[(1) - (3)].n), (SeExpr2vsp[(3) - (3)].n), '*'); }
    break;

  case 69:
/* Line 1792 of yacc.c  */
#line 318 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE3((SeExpr2loc).first_column,(SeExpr2loc).last_column,BinaryOpNode, (SeExpr2vsp[(1) - (3)].n), (SeExpr2vsp[(3) - (3)].n), '/'); }
    break;

  case 70:
/* Line 1792 of yacc.c  */
#line 319 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE3((SeExpr2loc).first_column,(SeExpr2loc).last_column,BinaryOpNode, (SeExpr2vsp[(1) - (3)].n), (SeExpr2vsp[(3) - (3)].n), '%'); }
    break;

  case 71:
/* Line 1792 of yacc.c  */
#line 320 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE3((SeExpr2loc).first_column,(SeExpr2loc).last_column,BinaryOpNode, (SeExpr2vsp[(1) - (3)].n), (SeExpr2vsp[(3) - (3)].n), '^'); }
    break;

  case 72:
/* Line 1792 of yacc.c  */
#line 321 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE1((SeExpr2loc).first_column,(SeExpr2loc).last_column,FuncNode, (SeExpr2vsp[(1) - (4)].s));
				  free((SeExpr2vsp[(1) - (4)].s)); // free name string
				  // add args directly and discard arg list node
				  (SeExpr2val.n)->addChildren((SeExpr2vsp[(3) - (4)].n)); Forget((SeExpr2vsp[(3) - (4)].n)); }
    break;

  case 73:
/* Line 1792 of yacc.c  */
#line 326 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE1((SeExpr2loc).first_column,(SeExpr2loc).last_column,FuncNode, (SeExpr2vsp[(3) - (6)].s));
				  free((SeExpr2vsp[(3) - (6)].s)); // free name string
				  (SeExpr2val.n)->addChild((SeExpr2vsp[(1) - (6)].n));
				  // add args directly and discard arg list node
				  (SeExpr2val.n)->addChildren((SeExpr2vsp[(5) - (6)].n)); Forget((SeExpr2vsp[(5) - (6)].n)); }
    break;

  case 74:
/* Line 1792 of yacc.c  */
#line 331 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE1((SeExpr2loc).first_column,(SeExpr2loc).last_column,VarNode, (SeExpr2vsp[(1) - (1)].s)); free((SeExpr2vsp[(1) - (1)].s)); /* free name string */ }
    break;

  case 75:
/* Line 1792 of yacc.c  */
#line 332 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE1((SeExpr2loc).first_column,(SeExpr2loc).last_column,VarNode, (SeExpr2vsp[(1) - (1)].s)); free((SeExpr2vsp[(1) - (1)].s)); /* free name string */ }
    break;

  case 76:
/* Line 1792 of yacc.c  */
#line 333 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE1((SeExpr2loc).first_column,(SeExpr2loc).last_column,NumNode, (SeExpr2vsp[(1) - (1)].d)); /*printf("line %d",@$.last_column);*/}
    break;

  case 77:
/* Line 1792 of yacc.c  */
#line 334 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE1((SeExpr2loc).first_column,(SeExpr2loc).last_column,StrNode, (SeExpr2vsp[(1) - (1)].s)); free((SeExpr2vsp[(1) - (1)].s)); /* free string */}
    break;

  case 78:
/* Line 1792 of yacc.c  */
#line 338 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE1((SeExpr2loc).first_column,(SeExpr2loc).last_column,Node,(SeExpr2vsp[(1) - (1)].n)); }
    break;

  case 79:
/* Line 1792 of yacc.c  */
#line 339 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = (SeExpr2vsp[(1) - (3)].n);
                                  (SeExpr2vsp[(1) - (3)].n)->addChild((SeExpr2vsp[(3) - (3)].n)); }
    break;

  case 80:
/* Line 1792 of yacc.c  */
#line 345 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE((SeExpr2loc).first_column,(SeExpr2loc).last_column,Node); /* create empty node */}
    break;

  case 81:
/* Line 1792 of yacc.c  */
#line 346 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = (SeExpr2vsp[(1) - (1)].n); }
    break;

  case 82:
/* Line 1792 of yacc.c  */
#line 351 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = NODE1((SeExpr2loc).first_column,(SeExpr2loc).last_column,Node, (SeExpr2vsp[(1) - (1)].n)); /* create arg list */}
    break;

  case 83:
/* Line 1792 of yacc.c  */
#line 352 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = (SeExpr2vsp[(1) - (3)].n); (SeExpr2vsp[(1) - (3)].n)->addChild((SeExpr2vsp[(3) - (3)].n)); /* add to list */}
    break;

  case 84:
/* Line 1792 of yacc.c  */
#line 356 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"
    { (SeExpr2val.n) = (SeExpr2vsp[(1) - (1)].n); }
    break;


/* Line 1792 of yacc.c  */
#line 2426 "y.tab.c"
      default: break;
    }
  /* User semantic actions sometimes alter SeExpr2char, and that requires
     that SeExpr2token be updated with the new translation.  We take the
     approach of translating immediately before every use of SeExpr2token.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     SeExprYYABORT, SeExprYYACCEPT, or SeExprYYERROR immediately after altering SeExpr2char or
     if it invokes SeExprYYBACKUP.  In the case of SeExprYYABORT or SeExprYYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of SeExprYYERROR or SeExprYYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  SeExprYY_SYMBOL_PRINT ("-> $$ =", SeExpr2r1[SeExpr2n], &SeExpr2val, &SeExpr2loc);

  SeExprYYPOPSTACK (SeExpr2len);
  SeExpr2len = 0;
  SeExprYY_STACK_PRINT (SeExpr2ss, SeExpr2ssp);

  *++SeExpr2vsp = SeExpr2val;
  *++SeExpr2lsp = SeExpr2loc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  SeExpr2n = SeExpr2r1[SeExpr2n];

  SeExpr2state = SeExpr2pgoto[SeExpr2n - SeExprYYNTOKENS] + *SeExpr2ssp;
  if (0 <= SeExpr2state && SeExpr2state <= SeExprYYLAST && SeExpr2check[SeExpr2state] == *SeExpr2ssp)
    SeExpr2state = SeExpr2table[SeExpr2state];
  else
    SeExpr2state = SeExpr2defgoto[SeExpr2n - SeExprYYNTOKENS];

  goto SeExpr2newstate;


/*------------------------------------.
| SeExpr2errlab -- here on detecting error |
`------------------------------------*/
SeExpr2errlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  SeExpr2token = SeExpr2char == SeExprYYEMPTY ? SeExprYYEMPTY : SeExprYYTRANSLATE (SeExpr2char);

  /* If not already recovering from an error, report this error.  */
  if (!SeExpr2errstatus)
    {
      ++SeExpr2nerrs;
#if ! SeExprYYERROR_VERBOSE
      SeExpr2error (SeExprYY_("syntax error"));
#else
# define SeExprYYSYNTAX_ERROR SeExpr2syntax_error (&SeExpr2msg_alloc, &SeExpr2msg, \
                                        SeExpr2ssp, SeExpr2token)
      {
        char const *SeExpr2msgp = SeExprYY_("syntax error");
        int SeExpr2syntax_error_status;
        SeExpr2syntax_error_status = SeExprYYSYNTAX_ERROR;
        if (SeExpr2syntax_error_status == 0)
          SeExpr2msgp = SeExpr2msg;
        else if (SeExpr2syntax_error_status == 1)
          {
            if (SeExpr2msg != SeExpr2msgbuf)
              SeExprYYSTACK_FREE (SeExpr2msg);
            SeExpr2msg = (char *) SeExprYYSTACK_ALLOC (SeExpr2msg_alloc);
            if (!SeExpr2msg)
              {
                SeExpr2msg = SeExpr2msgbuf;
                SeExpr2msg_alloc = sizeof SeExpr2msgbuf;
                SeExpr2syntax_error_status = 2;
              }
            else
              {
                SeExpr2syntax_error_status = SeExprYYSYNTAX_ERROR;
                SeExpr2msgp = SeExpr2msg;
              }
          }
        SeExpr2error (SeExpr2msgp);
        if (SeExpr2syntax_error_status == 2)
          goto SeExpr2exhaustedlab;
      }
# undef SeExprYYSYNTAX_ERROR
#endif
    }

  SeExpr2error_range[1] = SeExpr2lloc;

  if (SeExpr2errstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (SeExpr2char <= SeExprYYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (SeExpr2char == SeExprYYEOF)
	    SeExprYYABORT;
	}
      else
	{
	  SeExpr2destruct ("Error: discarding",
		      SeExpr2token, &SeExpr2lval, &SeExpr2lloc);
	  SeExpr2char = SeExprYYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto SeExpr2errlab1;


/*---------------------------------------------------.
| SeExpr2errorlab -- error raised explicitly by SeExprYYERROR.  |
`---------------------------------------------------*/
SeExpr2errorlab:

  /* Pacify compilers like GCC when the user code never invokes
     SeExprYYERROR and the label SeExpr2errorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto SeExpr2errorlab;

  SeExpr2error_range[1] = SeExpr2lsp[1-SeExpr2len];
  /* Do not reclaim the symbols of the rule which action triggered
     this SeExprYYERROR.  */
  SeExprYYPOPSTACK (SeExpr2len);
  SeExpr2len = 0;
  SeExprYY_STACK_PRINT (SeExpr2ss, SeExpr2ssp);
  SeExpr2state = *SeExpr2ssp;
  goto SeExpr2errlab1;


/*-------------------------------------------------------------.
| SeExpr2errlab1 -- common code for both syntax error and SeExprYYERROR.  |
`-------------------------------------------------------------*/
SeExpr2errlab1:
  SeExpr2errstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      SeExpr2n = SeExpr2pact[SeExpr2state];
      if (!SeExpr2pact_value_is_default (SeExpr2n))
	{
	  SeExpr2n += SeExprYYTERROR;
	  if (0 <= SeExpr2n && SeExpr2n <= SeExprYYLAST && SeExpr2check[SeExpr2n] == SeExprYYTERROR)
	    {
	      SeExpr2n = SeExpr2table[SeExpr2n];
	      if (0 < SeExpr2n)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (SeExpr2ssp == SeExpr2ss)
	SeExprYYABORT;

      SeExpr2error_range[1] = *SeExpr2lsp;
      SeExpr2destruct ("Error: popping",
		  SeExpr2stos[SeExpr2state], SeExpr2vsp, SeExpr2lsp);
      SeExprYYPOPSTACK (1);
      SeExpr2state = *SeExpr2ssp;
      SeExprYY_STACK_PRINT (SeExpr2ss, SeExpr2ssp);
    }

  SeExprYY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++SeExpr2vsp = SeExpr2lval;
  SeExprYY_IGNORE_MAYBE_UNINITIALIZED_END

  SeExpr2error_range[2] = SeExpr2lloc;
  /* Using SeExprYYLLOC is tempting, but would change the location of
     the lookahead.  SeExprYYLOC is available though.  */
  SeExprYYLLOC_DEFAULT (SeExpr2loc, SeExpr2error_range, 2);
  *++SeExpr2lsp = SeExpr2loc;

  /* Shift the error token.  */
  SeExprYY_SYMBOL_PRINT ("Shifting", SeExpr2stos[SeExpr2n], SeExpr2vsp, SeExpr2lsp);

  SeExpr2state = SeExpr2n;
  goto SeExpr2newstate;


/*-------------------------------------.
| SeExpr2acceptlab -- SeExprYYACCEPT comes here.  |
`-------------------------------------*/
SeExpr2acceptlab:
  SeExpr2result = 0;
  goto SeExpr2return;

/*-----------------------------------.
| SeExpr2abortlab -- SeExprYYABORT comes here.  |
`-----------------------------------*/
SeExpr2abortlab:
  SeExpr2result = 1;
  goto SeExpr2return;

#if !defined SeExpr2overflow || SeExprYYERROR_VERBOSE
/*-------------------------------------------------.
| SeExpr2exhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
SeExpr2exhaustedlab:
  SeExpr2error (SeExprYY_("memory exhausted"));
  SeExpr2result = 2;
  /* Fall through.  */
#endif

SeExpr2return:
  if (SeExpr2char != SeExprYYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      SeExpr2token = SeExprYYTRANSLATE (SeExpr2char);
      SeExpr2destruct ("Cleanup: discarding lookahead",
                  SeExpr2token, &SeExpr2lval, &SeExpr2lloc);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this SeExprYYABORT or SeExprYYACCEPT.  */
  SeExprYYPOPSTACK (SeExpr2len);
  SeExprYY_STACK_PRINT (SeExpr2ss, SeExpr2ssp);
  while (SeExpr2ssp != SeExpr2ss)
    {
      SeExpr2destruct ("Cleanup: popping",
		  SeExpr2stos[*SeExpr2ssp], SeExpr2vsp, SeExpr2lsp);
      SeExprYYPOPSTACK (1);
    }
#ifndef SeExpr2overflow
  if (SeExpr2ss != SeExpr2ssa)
    SeExprYYSTACK_FREE (SeExpr2ss);
#endif
#if SeExprYYERROR_VERBOSE
  if (SeExpr2msg != SeExpr2msgbuf)
    SeExprYYSTACK_FREE (SeExpr2msg);
#endif
  /* Make sure SeExprYYID is used.  */
  return SeExprYYID (SeExpr2result);
}


/* Line 2055 of yacc.c  */
#line 359 "/disney/users/jberlin/projects/seexpr2/src/SeExpr/ExprParser.y"


      /* SeExpr2error - Report an error.  This is called by the parser.
	 (Note: the "msg" param is useless as it is usually just "parse error".
	 so it's ignored.)
      */
static void SeExpr2error(const char* /*msg*/)
{
    // find start of line containing error
    int pos = SeExpr2pos(), lineno = 1, start = 0, end = strlen(ParseStr);
    bool multiline = 0;
    for (int i = start; i < pos; i++)
	if (ParseStr[i] == '\n') { start = i + 1; lineno++; multiline=1; }

    // find end of line containing error
    for (int i = end; i > pos; i--)
	if (ParseStr[i] == '\n') { end = i - 1; multiline=1; }

    ParseError = SeExpr2text[0] ? "Syntax error" : "Unexpected end of expression";
    if (multiline) {
	char buff[30];
	snprintf(buff, 30, " at line %d", lineno);
	ParseError += buff;
    }
    if (SeExpr2text[0]) {
	ParseError += " near '";
	ParseError += SeExpr2text;
    }
    ParseError += "':\n    ";

    int s = std::max(start, pos-30);
    int e = std::min(end, pos+30);

    if (s != start) ParseError += "...";
    ParseError += std::string(ParseStr, s, e-s+1);
    if (e != end) ParseError += "...";
}


/* CallParser - This is our entrypoint from the rest of the expr library.
   A string is passed in and a parse tree is returned.	If the tree is null,
   an error string is returned.  Any flags set during parsing are passed
   along.
 */

extern void SeExprLexerResetState(std::vector<std::pair<int,int> >& comments);

static SeExprInternal2::Mutex mutex;

namespace SeExpr2 {
bool ExprParse(SeExpr2::ExprNode*& parseTree,
    std::string& error, int& errorStart, int& errorEnd,
    std::vector<std::pair<int,int> >& comments,
    const SeExpr2::Expression* expr, const char* str, bool wantVec)
{
    SeExprInternal2::AutoMutex locker(mutex);

    // glue around crippled C interface - ugh!
    Expr = expr;
    ParseStr = str;
    SeExprLexerResetState(comments);
    SeExpr2_buffer_state* buffer = SeExpr2_scan_string(str);
    ParseResult = 0;
    int resultCode = SeExpr2parse();
    SeExpr2_delete_buffer(buffer);

    if (resultCode == 0) {
	// success
	error = "";
	parseTree = ParseResult;
    }
    else {
	// failure
	error = ParseError;
        errorStart=SeExpr2lloc.first_column;
        errorEnd=SeExpr2lloc.last_column;
	parseTree = 0;
	// gather list of nodes with no parent
	std::vector<SeExpr2::ExprNode*> delnodes;
	std::vector<SeExpr2::ExprNode*>::iterator iter;
	for (iter = ParseNodes.begin(); iter != ParseNodes.end(); iter++)
	    if (!(*iter)->parent()) { delnodes.push_back(*iter); }
	// now delete them (they will delete their own children)
	for (iter = delnodes.begin(); iter != delnodes.end(); iter++)
	    delete *iter;
    }
    ParseNodes.clear();

    return parseTree != 0;
}
}
