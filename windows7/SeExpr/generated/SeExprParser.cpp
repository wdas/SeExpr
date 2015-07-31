
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
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

/* All symbols defined below should begin with SeExpr or SeExprYY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define SeExprYYBISON 1

/* Bison version.  */
#define SeExprYYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define SeExprYYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define SeExprYYPURE 0

/* Push parsers.  */
#define SeExprYYPUSH 0

/* Pull parsers.  */
#define SeExprYYPULL 1

/* Using locations.  */
#define SeExprYYLSP_NEEDED 1

/* Substitute the variable and function names.  */
#define SeExprparse         SeExprparse
#define SeExprlex           SeExprlex
#define SeExprerror         SeExprerror
#define SeExprlval          SeExprlval
#define SeExprchar          SeExprchar
#define SeExprdebug         SeExprdebug
#define SeExprnerrs         SeExprnerrs
#define SeExprlloc          SeExprlloc

/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 18 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"

#ifndef MAKEDEPEND
#include <algorithm>
#include <vector>
#include <stdio.h>
#endif
#include "SeExprNode.h"
#include "SeExprParser.h"
#include "SeExpression.h"
#include "SeMutex.h"

/******************
 lexer declarations
 ******************/

// declarations of functions and data in SeExprParser.l
int SeExprlex();
int SeExprpos();
extern int SeExpr_start;
extern char* SeExprtext;
struct SeExpr_buffer_state;
SeExpr_buffer_state* SeExpr_scan_string(const char *str);
void SeExpr_delete_buffer(SeExpr_buffer_state*);

/*******************
 parser declarations
 *******************/

// forward declaration
static void SeExprerror(const char* msg);

// local data
static const char* ParseStr;    // string being parsed
static std::string ParseError;  // error (set from SeExprerror)
static SeExprNode* ParseResult; // must set result here since SeExprparse can't return it
static const SeExpression* Expr;// used for parenting created SeExprOp's

/* The list of nodes being built is remembered locally here.
   Eventually (if there are no syntax errors) ownership of the nodes
   will belong solely to the parse tree and the parent expression.
   However, if there is a syntax error, we must loop through this list
   and free any nodes that were allocated before the error to avoid a
   memory leak. */
static std::vector<SeExprNode*> ParseNodes;
inline SeExprNode* Remember(SeExprNode* n,const int startPos,const int endPos) 
    { ParseNodes.push_back(n); n->setPosition(startPos,endPos); return n; }
inline void Forget(SeExprNode* n) 
    { ParseNodes.erase(std::find(ParseNodes.begin(), ParseNodes.end(), n)); }

/* These are handy node constructors for 0-3 arguments */
#define NODE(startPos,endPos,name) Remember(new SeExpr##name(Expr),startPos,endPos)
#define NODE1(startPos,endPos,name,a) Remember(new SeExpr##name(Expr,a),startPos,endPos)
#define NODE2(startPos,endPos,name,a,b) Remember(new SeExpr##name(Expr,a,b),startPos,endPos)
#define NODE3(startPos,endPos,name,a,b,c) Remember(new SeExpr##name(Expr,a,b,c),startPos,endPos)


/* Line 189 of yacc.c  */
#line 138 "y.tab.c"

/* Enabling traces.  */
#ifndef SeExprYYDEBUG
# define SeExprYYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef SeExprYYERROR_VERBOSE
# undef SeExprYYERROR_VERBOSE
# define SeExprYYERROR_VERBOSE 1
#else
# define SeExprYYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef SeExprYYTOKEN_TABLE
# define SeExprYYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef SeExprYYTOKENTYPE
# define SeExprYYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum SeExprtokentype {
     IF = 258,
     ELSE = 259,
     NAME = 260,
     VAR = 261,
     STR = 262,
     NUMBER = 263,
     AddEq = 264,
     SubEq = 265,
     MultEq = 266,
     DivEq = 267,
     ExpEq = 268,
     ModEq = 269,
     ARROW = 270,
     OR = 271,
     AND = 272,
     NE = 273,
     EQ = 274,
     GE = 275,
     LE = 276,
     UNARY = 277
   };
#endif
/* Tokens.  */
#define IF 258
#define ELSE 259
#define NAME 260
#define VAR 261
#define STR 262
#define NUMBER 263
#define AddEq 264
#define SubEq 265
#define MultEq 266
#define DivEq 267
#define ExpEq 268
#define ModEq 269
#define ARROW 270
#define OR 271
#define AND 272
#define NE 273
#define EQ 274
#define GE 275
#define LE 276
#define UNARY 277




#if ! defined SeExprYYSTYPE && ! defined SeExprYYSTYPE_IS_DECLARED
typedef union SeExprYYSTYPE
{

/* Line 214 of yacc.c  */
#line 74 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"

    SeExprNode* n; /* a node is returned for all non-terminals to
		      build the parse tree from the leaves up. */
    double d;      // return value for number tokens
    char* s;       /* return value for name tokens.  Note: the string
		      is allocated with strdup() in the lexer and must
		      be freed with free() */



/* Line 214 of yacc.c  */
#line 229 "y.tab.c"
} SeExprYYSTYPE;
# define SeExprYYSTYPE_IS_TRIVIAL 1
# define SeExprstype SeExprYYSTYPE /* obsolescent; will be withdrawn */
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
# define SeExprltype SeExprYYLTYPE /* obsolescent; will be withdrawn */
# define SeExprYYLTYPE_IS_DECLARED 1
# define SeExprYYLTYPE_IS_TRIVIAL 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 254 "y.tab.c"

#ifdef short
# undef short
#endif

#ifdef SeExprYYTYPE_UINT8
typedef SeExprYYTYPE_UINT8 SeExprtype_uint8;
#else
typedef unsigned char SeExprtype_uint8;
#endif

#ifdef SeExprYYTYPE_INT8
typedef SeExprYYTYPE_INT8 SeExprtype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char SeExprtype_int8;
#else
typedef short int SeExprtype_int8;
#endif

#ifdef SeExprYYTYPE_UINT16
typedef SeExprYYTYPE_UINT16 SeExprtype_uint16;
#else
typedef unsigned short int SeExprtype_uint16;
#endif

#ifdef SeExprYYTYPE_INT16
typedef SeExprYYTYPE_INT16 SeExprtype_int16;
#else
typedef short int SeExprtype_int16;
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
# if SeExprYYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define SeExprYY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef SeExprYY_
#  define SeExprYY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define SeExprYYUSE(e) ((void) (e))
#else
# define SeExprYYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define SeExprYYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
SeExprYYID (int SeExpri)
#else
static int
SeExprYYID (SeExpri)
    int SeExpri;
#endif
{
  return SeExpri;
}
#endif

#if ! defined SeExproverflow || SeExprYYERROR_VERBOSE

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
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
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
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined SeExprYYMALLOC || defined malloc) \
	     && (defined SeExprYYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef SeExprYYMALLOC
#   define SeExprYYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (SeExprYYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef SeExprYYFREE
#   define SeExprYYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined SeExproverflow || SeExprYYERROR_VERBOSE */


#if (! defined SeExproverflow \
     && (! defined __cplusplus \
	 || (defined SeExprYYLTYPE_IS_TRIVIAL && SeExprYYLTYPE_IS_TRIVIAL \
	     && defined SeExprYYSTYPE_IS_TRIVIAL && SeExprYYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union SeExpralloc
{
  SeExprtype_int16 SeExprss_alloc;
  SeExprYYSTYPE SeExprvs_alloc;
  SeExprYYLTYPE SeExprls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define SeExprYYSTACK_GAP_MAXIMUM (sizeof (union SeExpralloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define SeExprYYSTACK_BYTES(N) \
     ((N) * (sizeof (SeExprtype_int16) + sizeof (SeExprYYSTYPE) + sizeof (SeExprYYLTYPE)) \
      + 2 * SeExprYYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef SeExprYYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define SeExprYYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define SeExprYYCOPY(To, From, Count)		\
      do					\
	{					\
	  SeExprYYSIZE_T SeExpri;				\
	  for (SeExpri = 0; SeExpri < (Count); SeExpri++)	\
	    (To)[SeExpri] = (From)[SeExpri];		\
	}					\
      while (SeExprYYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables SeExprYYSIZE and SeExprYYSTACKSIZE give the old and new number of
   elements in the stack, and SeExprYYPTR gives the new location of the
   stack.  Advance SeExprYYPTR to a properly aligned location for the next
   stack.  */
# define SeExprYYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	SeExprYYSIZE_T SeExprnewbytes;						\
	SeExprYYCOPY (&SeExprptr->Stack_alloc, Stack, SeExprsize);			\
	Stack = &SeExprptr->Stack_alloc;					\
	SeExprnewbytes = SeExprstacksize * sizeof (*Stack) + SeExprYYSTACK_GAP_MAXIMUM; \
	SeExprptr += SeExprnewbytes / sizeof (*SeExprptr);				\
      }									\
    while (SeExprYYID (0))

#endif

/* SeExprYYFINAL -- State number of the termination state.  */
#define SeExprYYFINAL  40
/* SeExprYYLAST -- Last index in SeExprYYTABLE.  */
#define SeExprYYLAST   693

/* SeExprYYNTOKENS -- Number of terminals.  */
#define SeExprYYNTOKENS  44
/* SeExprYYNNTS -- Number of nonterminals.  */
#define SeExprYYNNTS  11
/* SeExprYYNRULES -- Number of rules.  */
#define SeExprYYNRULES  59
/* SeExprYYNRULES -- Number of states.  */
#define SeExprYYNSTATES  139

/* SeExprYYTRANSLATE(SeExprYYLEX) -- Bison symbol number corresponding to SeExprYYLEX.  */
#define SeExprYYUNDEFTOK  2
#define SeExprYYMAXUTOK   277

#define SeExprYYTRANSLATE(SeExprYYX)						\
  ((unsigned int) (SeExprYYX) <= SeExprYYMAXUTOK ? SeExprtranslate[SeExprYYX] : SeExprYYUNDEFTOK)

/* SeExprYYTRANSLATE[SeExprYYLEX] -- Bison symbol number corresponding to SeExprYYLEX.  */
static const SeExprtype_uint8 SeExprtranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    33,     2,     2,     2,    32,     2,     2,
      15,    16,    30,    28,    42,    29,     2,    31,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    18,    39,
      24,    38,    25,    19,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    37,     2,    43,    36,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    40,     2,    41,    34,     2,     2,     2,
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
      17,    20,    21,    22,    23,    26,    27,    35
};

#if SeExprYYDEBUG
/* SeExprYYPRHS[SeExprYYN] -- Index of the first RHS symbol of rule number SeExprYYN in
   SeExprYYRHS.  */
static const SeExprtype_uint8 SeExprprhs[] =
{
       0,     0,     3,     6,     8,     9,    11,    13,    16,    18,
      23,    28,    33,    38,    43,    48,    53,    58,    63,    68,
      73,    78,    83,    88,    97,    98,   103,   106,   110,   118,
     123,   129,   133,   137,   141,   145,   149,   153,   157,   161,
     164,   167,   170,   173,   177,   181,   185,   189,   193,   197,
     202,   209,   211,   213,   215,   216,   218,   220,   224,   226
};

/* SeExprYYRHS -- A `-1'-separated list of the rules' RHS.  */
static const SeExprtype_int8 SeExprrhs[] =
{
      45,     0,    -1,    47,    51,    -1,    51,    -1,    -1,    47,
      -1,    48,    -1,    47,    48,    -1,    49,    -1,     6,    38,
      51,    39,    -1,     6,     9,    51,    39,    -1,     6,    10,
      51,    39,    -1,     6,    11,    51,    39,    -1,     6,    12,
      51,    39,    -1,     6,    13,    51,    39,    -1,     6,    14,
      51,    39,    -1,     5,    38,    51,    39,    -1,     5,     9,
      51,    39,    -1,     5,    10,    51,    39,    -1,     5,    11,
      51,    39,    -1,     5,    12,    51,    39,    -1,     5,    13,
      51,    39,    -1,     5,    14,    51,    39,    -1,     3,    15,
      51,    16,    40,    46,    41,    50,    -1,    -1,     4,    40,
      46,    41,    -1,     4,    49,    -1,    15,    51,    16,    -1,
      37,    51,    42,    51,    42,    51,    43,    -1,    51,    37,
      51,    43,    -1,    51,    19,    51,    18,    51,    -1,    51,
      20,    51,    -1,    51,    21,    51,    -1,    51,    23,    51,
      -1,    51,    22,    51,    -1,    51,    24,    51,    -1,    51,
      25,    51,    -1,    51,    27,    51,    -1,    51,    26,    51,
      -1,    28,    51,    -1,    29,    51,    -1,    33,    51,    -1,
      34,    51,    -1,    51,    28,    51,    -1,    51,    29,    51,
      -1,    51,    30,    51,    -1,    51,    31,    51,    -1,    51,
      32,    51,    -1,    51,    36,    51,    -1,     5,    15,    52,
      16,    -1,    51,    17,     5,    15,    52,    16,    -1,     6,
      -1,     5,    -1,     8,    -1,    -1,    53,    -1,    54,    -1,
      53,    42,    54,    -1,    51,    -1,     7,    -1
};

/* SeExprYYRLINE[SeExprYYN] -- source line where rule number SeExprYYN was defined.  */
static const SeExprtype_uint8 SeExprrline[] =
{
       0,   119,   119,   120,   125,   126,   130,   131,   136,   137,
     138,   141,   144,   147,   150,   153,   156,   157,   160,   163,
     166,   169,   172,   178,   183,   184,   185,   190,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     215,   220,   221,   222,   227,   228,   233,   234,   238,   239
};
#endif

#if SeExprYYDEBUG || SeExprYYERROR_VERBOSE || SeExprYYTOKEN_TABLE
/* SeExprYYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at SeExprYYNTOKENS, nonterminals.  */
static const char *const SeExprtname[] =
{
  "$end", "error", "$undefined", "IF", "ELSE", "NAME", "VAR", "STR",
  "NUMBER", "AddEq", "SubEq", "MultEq", "DivEq", "ExpEq", "ModEq", "'('",
  "')'", "ARROW", "':'", "'?'", "OR", "AND", "NE", "EQ", "'<'", "'>'",
  "GE", "LE", "'+'", "'-'", "'*'", "'/'", "'%'", "'!'", "'~'", "UNARY",
  "'^'", "'['", "'='", "';'", "'{'", "'}'", "','", "']'", "$accept",
  "expr", "optassigns", "assigns", "assign", "ifthenelse", "optelse", "e",
  "optargs", "args", "arg", 0
};
#endif

# ifdef SeExprYYPRINT
/* SeExprYYTOKNUM[SeExprYYLEX-NUM] -- Internal token number corresponding to
   token SeExprYYLEX-NUM.  */
static const SeExprtype_uint16 SeExprtoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,    40,    41,   270,    58,    63,
     271,   272,   273,   274,    60,    62,   275,   276,    43,    45,
      42,    47,    37,    33,   126,   277,    94,    91,    61,    59,
     123,   125,    44,    93
};
# endif

/* SeExprYYR1[SeExprYYN] -- Symbol number of symbol that rule SeExprYYN derives.  */
static const SeExprtype_uint8 SeExprr1[] =
{
       0,    44,    45,    45,    46,    46,    47,    47,    48,    48,
      48,    48,    48,    48,    48,    48,    48,    48,    48,    48,
      48,    48,    48,    49,    50,    50,    50,    51,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    51,
      51,    51,    51,    51,    52,    52,    53,    53,    54,    54
};

/* SeExprYYR2[SeExprYYN] -- Number of symbols composing right hand side of rule SeExprYYN.  */
static const SeExprtype_uint8 SeExprr2[] =
{
       0,     2,     2,     1,     0,     1,     1,     2,     1,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     8,     0,     4,     2,     3,     7,     4,
       5,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       2,     2,     2,     3,     3,     3,     3,     3,     3,     4,
       6,     1,     1,     1,     0,     1,     1,     3,     1,     1
};

/* SeExprYYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when SeExprYYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const SeExprtype_uint8 SeExprdefact[] =
{
       0,     0,    52,    51,    53,     0,     0,     0,     0,     0,
       0,     0,     0,     6,     8,     3,     0,     0,     0,     0,
       0,     0,     0,    54,     0,     0,     0,     0,     0,     0,
       0,     0,    52,    51,     0,    39,    40,    41,    42,     0,
       1,     7,     2,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    59,    58,     0,
      55,    56,     0,     0,     0,     0,     0,     0,     0,     0,
      27,     0,     0,     0,    31,    32,    34,    33,    35,    36,
      38,    37,    43,    44,    45,    46,    47,    48,     0,     0,
      17,    18,    19,    20,    21,    22,    49,     0,    16,    10,
      11,    12,    13,    14,    15,     9,     0,    54,     0,    29,
       4,    57,     0,     0,    30,     0,     0,     0,     5,     0,
      50,    24,    28,     0,    23,     4,    26,     0,    25
};

/* SeExprYYDEFGOTO[NTERM-NUM].  */
static const SeExprtype_int16 SeExprdefgoto[] =
{
      -1,    11,   127,   128,    13,    14,   134,    68,    69,    70,
      71
};

/* SeExprYYPACT[STATE-NUM] -- Index in SeExprYYTABLE of the portion describing
   STATE-NUM.  */
#define SeExprYYPACT_NINF -65
static const SeExprtype_int16 SeExprpact[] =
{
      57,    25,    23,   127,   -65,    98,    98,    98,    98,    98,
      98,    15,    57,   -65,   -65,   590,    98,    98,    98,    98,
      98,    98,    98,    68,    98,    98,    98,    98,    98,    98,
      98,    98,    26,   -65,   526,   -33,   -33,   -33,   -33,   181,
     -65,   -65,   590,    18,    98,    98,    98,    98,    98,    98,
      98,    98,    98,    98,    98,    98,    98,    98,    98,    98,
     548,   229,   250,   271,   292,   313,   334,   -65,   590,    27,
      22,   -65,   355,   376,   397,   418,   439,   460,   481,   502,
     -65,    98,    51,   569,   626,   642,   656,   656,    79,    79,
      79,    79,    93,    93,   -33,   -33,   -33,   -33,   131,     2,
     -65,   -65,   -65,   -65,   -65,   -65,   -65,    68,   -65,   -65,
     -65,   -65,   -65,   -65,   -65,   -65,   205,    68,    98,   -65,
       8,   -65,    98,    52,   609,   133,   127,    28,     8,   156,
     -65,    63,   -65,    -1,   -65,     8,   -65,    29,   -65
};

/* SeExprYYPGOTO[NTERM-NUM].  */
static const SeExprtype_int8 SeExprpgoto[] =
{
     -65,   -65,   -64,    77,   -11,   -55,   -65,     0,   -38,   -65,
     -27
};

/* SeExprYYTABLE[SeExprYYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what SeExprYYDEFACT says.
   If SeExprYYTABLE_NINF, syntax error.  */
#define SeExprYYTABLE_NINF -1
static const SeExprtype_uint8 SeExprtable[] =
{
      15,    41,     1,    58,    59,    34,    35,    36,    37,    38,
      39,     1,    42,   125,   126,    40,    60,    61,    62,    63,
      64,    65,    66,    82,    72,    73,    74,    75,    76,    77,
      78,    79,    17,    18,    19,    20,    21,    22,    23,   135,
      16,    23,   120,   106,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
       1,    24,     2,     3,   107,     4,   117,   133,   130,   131,
     138,   137,     5,    32,    33,    67,     4,    12,   136,   123,
     121,   116,     0,     5,     0,     6,     7,     0,     0,     0,
       8,     9,     0,     0,    10,     0,     6,     7,     0,     0,
       0,     8,     9,    32,    33,    10,     4,    53,    54,    55,
      56,    57,     0,     5,     0,    58,    59,    41,   124,     0,
       0,     0,   129,    55,    56,    57,     6,     7,     0,    58,
      59,     8,     9,     0,     0,    10,    25,    26,    27,    28,
      29,    30,    17,    18,    19,    20,    21,    22,    43,     0,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,     0,    31,     0,    58,    59,     0,
       0,    24,     0,    43,   119,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,     0,
       0,     0,    58,    59,     0,     0,     0,     0,    43,   132,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,     0,     0,     0,    58,    59,     0,
       0,     0,    43,    81,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,     0,     0,
       0,    58,    59,     0,     0,     0,    43,   122,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,     0,     0,     0,    58,    59,    43,   100,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,     0,     0,     0,    58,    59,    43,   101,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,     0,     0,     0,    58,    59,    43,
     102,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,     0,     0,     0,    58,    59,
      43,   103,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,     0,     0,     0,    58,
      59,    43,   104,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,     0,     0,     0,
      58,    59,    43,   105,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,     0,     0,
       0,    58,    59,    43,   108,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,     0,
       0,     0,    58,    59,    43,   109,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
       0,     0,     0,    58,    59,    43,   110,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,     0,     0,     0,    58,    59,    43,   111,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,     0,     0,     0,    58,    59,    43,   112,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,     0,     0,     0,    58,    59,    43,   113,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,     0,     0,     0,    58,    59,    43,
     114,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,     0,     0,     0,    58,    59,
       0,   115,    80,    43,     0,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,     0,
       0,     0,    58,    59,    99,    43,     0,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,     0,     0,     0,    58,    59,    43,   118,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,     0,     0,     0,    58,    59,    43,     0,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,     0,     0,     0,    58,    59,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,     0,     0,     0,    58,    59,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,     0,
       0,     0,    58,    59,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,     0,     0,     0,    58,    59,
      49,    50,    51,    52,    53,    54,    55,    56,    57,     0,
       0,     0,    58,    59
};

static const SeExprtype_int16 SeExprcheck[] =
{
       0,    12,     3,    36,    37,     5,     6,     7,     8,     9,
      10,     3,    12,     5,     6,     0,    16,    17,    18,    19,
      20,    21,    22,     5,    24,    25,    26,    27,    28,    29,
      30,    31,     9,    10,    11,    12,    13,    14,    15,    40,
      15,    15,    40,    16,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
       3,    38,     5,     6,    42,     8,    15,     4,    16,    41,
      41,   135,    15,     5,     6,     7,     8,     0,   133,   117,
     107,    81,    -1,    15,    -1,    28,    29,    -1,    -1,    -1,
      33,    34,    -1,    -1,    37,    -1,    28,    29,    -1,    -1,
      -1,    33,    34,     5,     6,    37,     8,    28,    29,    30,
      31,    32,    -1,    15,    -1,    36,    37,   128,   118,    -1,
      -1,    -1,   122,    30,    31,    32,    28,    29,    -1,    36,
      37,    33,    34,    -1,    -1,    37,     9,    10,    11,    12,
      13,    14,     9,    10,    11,    12,    13,    14,    17,    -1,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    -1,    38,    -1,    36,    37,    -1,
      -1,    38,    -1,    17,    43,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    -1,
      -1,    -1,    36,    37,    -1,    -1,    -1,    -1,    17,    43,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    -1,    -1,    -1,    36,    37,    -1,
      -1,    -1,    17,    42,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    -1,    -1,
      -1,    36,    37,    -1,    -1,    -1,    17,    42,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    -1,    -1,    -1,    36,    37,    17,    39,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    -1,    -1,    -1,    36,    37,    17,    39,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    -1,    -1,    -1,    36,    37,    17,
      39,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    -1,    -1,    -1,    36,    37,
      17,    39,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    -1,    -1,    -1,    36,
      37,    17,    39,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    -1,    -1,    -1,
      36,    37,    17,    39,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    -1,    -1,
      -1,    36,    37,    17,    39,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    -1,
      -1,    -1,    36,    37,    17,    39,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      -1,    -1,    -1,    36,    37,    17,    39,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    -1,    -1,    -1,    36,    37,    17,    39,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    -1,    -1,    -1,    36,    37,    17,    39,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    -1,    -1,    -1,    36,    37,    17,    39,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    -1,    -1,    -1,    36,    37,    17,
      39,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    -1,    -1,    -1,    36,    37,
      -1,    39,    16,    17,    -1,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    -1,
      -1,    -1,    36,    37,    16,    17,    -1,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    -1,    -1,    -1,    36,    37,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    -1,    -1,    -1,    36,    37,    17,    -1,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    -1,    -1,    -1,    36,    37,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    -1,    -1,    -1,    36,    37,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    -1,
      -1,    -1,    36,    37,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    -1,    -1,    -1,    36,    37,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    -1,
      -1,    -1,    36,    37
};

/* SeExprYYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const SeExprtype_uint8 SeExprstos[] =
{
       0,     3,     5,     6,     8,    15,    28,    29,    33,    34,
      37,    45,    47,    48,    49,    51,    15,     9,    10,    11,
      12,    13,    14,    15,    38,     9,    10,    11,    12,    13,
      14,    38,     5,     6,    51,    51,    51,    51,    51,    51,
       0,    48,    51,    17,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    36,    37,
      51,    51,    51,    51,    51,    51,    51,     7,    51,    52,
      53,    54,    51,    51,    51,    51,    51,    51,    51,    51,
      16,    42,     5,    51,    51,    51,    51,    51,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    16,
      39,    39,    39,    39,    39,    39,    16,    42,    39,    39,
      39,    39,    39,    39,    39,    39,    51,    15,    18,    43,
      40,    54,    42,    52,    51,     5,     6,    46,    47,    51,
      16,    41,    43,     4,    50,    40,    49,    46,    41
};

#define SeExprerrok		(SeExprerrstatus = 0)
#define SeExprclearin	(SeExprchar = SeExprYYEMPTY)
#define SeExprYYEMPTY		(-2)
#define SeExprYYEOF		0

#define SeExprYYACCEPT	goto SeExpracceptlab
#define SeExprYYABORT		goto SeExprabortlab
#define SeExprYYERROR		goto SeExprerrorlab


/* Like SeExprYYERROR except do call SeExprerror.  This remains here temporarily
   to ease the transition to the new meaning of SeExprYYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define SeExprYYFAIL		goto SeExprerrlab

#define SeExprYYRECOVERING()  (!!SeExprerrstatus)

#define SeExprYYBACKUP(Token, Value)					\
do								\
  if (SeExprchar == SeExprYYEMPTY && SeExprlen == 1)				\
    {								\
      SeExprchar = (Token);						\
      SeExprlval = (Value);						\
      SeExprtoken = SeExprYYTRANSLATE (SeExprchar);				\
      SeExprYYPOPSTACK (1);						\
      goto SeExprbackup;						\
    }								\
  else								\
    {								\
      SeExprerror (SeExprYY_("syntax error: cannot back up")); \
      SeExprYYERROR;							\
    }								\
while (SeExprYYID (0))


#define SeExprYYTERROR	1
#define SeExprYYERRCODE	256


/* SeExprYYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define SeExprYYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef SeExprYYLLOC_DEFAULT
# define SeExprYYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (SeExprYYID (N))                                                    \
	{								\
	  (Current).first_line   = SeExprYYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = SeExprYYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = SeExprYYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = SeExprYYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    SeExprYYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    SeExprYYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (SeExprYYID (0))
#endif


/* SeExprYY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef SeExprYY_LOCATION_PRINT
# if SeExprYYLTYPE_IS_TRIVIAL
#  define SeExprYY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define SeExprYY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* SeExprYYLEX -- calling `SeExprlex' with the right arguments.  */

#ifdef SeExprYYLEX_PARAM
# define SeExprYYLEX SeExprlex (SeExprYYLEX_PARAM)
#else
# define SeExprYYLEX SeExprlex ()
#endif

/* Enable debugging if requested.  */
#if SeExprYYDEBUG

# ifndef SeExprYYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define SeExprYYFPRINTF fprintf
# endif

# define SeExprYYDPRINTF(Args)			\
do {						\
  if (SeExprdebug)					\
    SeExprYYFPRINTF Args;				\
} while (SeExprYYID (0))

# define SeExprYY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (SeExprdebug)								  \
    {									  \
      SeExprYYFPRINTF (stderr, "%s ", Title);					  \
      SeExpr_symbol_print (stderr,						  \
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
SeExpr_symbol_value_print (FILE *SeExproutput, int SeExprtype, SeExprYYSTYPE const * const SeExprvaluep, SeExprYYLTYPE const * const SeExprlocationp)
#else
static void
SeExpr_symbol_value_print (SeExproutput, SeExprtype, SeExprvaluep, SeExprlocationp)
    FILE *SeExproutput;
    int SeExprtype;
    SeExprYYSTYPE const * const SeExprvaluep;
    SeExprYYLTYPE const * const SeExprlocationp;
#endif
{
  if (!SeExprvaluep)
    return;
  SeExprYYUSE (SeExprlocationp);
# ifdef SeExprYYPRINT
  if (SeExprtype < SeExprYYNTOKENS)
    SeExprYYPRINT (SeExproutput, SeExprtoknum[SeExprtype], *SeExprvaluep);
# else
  SeExprYYUSE (SeExproutput);
# endif
  switch (SeExprtype)
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
SeExpr_symbol_print (FILE *SeExproutput, int SeExprtype, SeExprYYSTYPE const * const SeExprvaluep, SeExprYYLTYPE const * const SeExprlocationp)
#else
static void
SeExpr_symbol_print (SeExproutput, SeExprtype, SeExprvaluep, SeExprlocationp)
    FILE *SeExproutput;
    int SeExprtype;
    SeExprYYSTYPE const * const SeExprvaluep;
    SeExprYYLTYPE const * const SeExprlocationp;
#endif
{
  if (SeExprtype < SeExprYYNTOKENS)
    SeExprYYFPRINTF (SeExproutput, "token %s (", SeExprtname[SeExprtype]);
  else
    SeExprYYFPRINTF (SeExproutput, "nterm %s (", SeExprtname[SeExprtype]);

  SeExprYY_LOCATION_PRINT (SeExproutput, *SeExprlocationp);
  SeExprYYFPRINTF (SeExproutput, ": ");
  SeExpr_symbol_value_print (SeExproutput, SeExprtype, SeExprvaluep, SeExprlocationp);
  SeExprYYFPRINTF (SeExproutput, ")");
}

/*------------------------------------------------------------------.
| SeExpr_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
SeExpr_stack_print (SeExprtype_int16 *SeExprbottom, SeExprtype_int16 *SeExprtop)
#else
static void
SeExpr_stack_print (SeExprbottom, SeExprtop)
    SeExprtype_int16 *SeExprbottom;
    SeExprtype_int16 *SeExprtop;
#endif
{
  SeExprYYFPRINTF (stderr, "Stack now");
  for (; SeExprbottom <= SeExprtop; SeExprbottom++)
    {
      int SeExprbot = *SeExprbottom;
      SeExprYYFPRINTF (stderr, " %d", SeExprbot);
    }
  SeExprYYFPRINTF (stderr, "\n");
}

# define SeExprYY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (SeExprdebug)							\
    SeExpr_stack_print ((Bottom), (Top));				\
} while (SeExprYYID (0))


/*------------------------------------------------.
| Report that the SeExprYYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
SeExpr_reduce_print (SeExprYYSTYPE *SeExprvsp, SeExprYYLTYPE *SeExprlsp, int SeExprrule)
#else
static void
SeExpr_reduce_print (SeExprvsp, SeExprlsp, SeExprrule)
    SeExprYYSTYPE *SeExprvsp;
    SeExprYYLTYPE *SeExprlsp;
    int SeExprrule;
#endif
{
  int SeExprnrhs = SeExprr2[SeExprrule];
  int SeExpri;
  unsigned long int SeExprlno = SeExprrline[SeExprrule];
  SeExprYYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     SeExprrule - 1, SeExprlno);
  /* The symbols being reduced.  */
  for (SeExpri = 0; SeExpri < SeExprnrhs; SeExpri++)
    {
      SeExprYYFPRINTF (stderr, "   $%d = ", SeExpri + 1);
      SeExpr_symbol_print (stderr, SeExprrhs[SeExprprhs[SeExprrule] + SeExpri],
		       &(SeExprvsp[(SeExpri + 1) - (SeExprnrhs)])
		       , &(SeExprlsp[(SeExpri + 1) - (SeExprnrhs)])		       );
      SeExprYYFPRINTF (stderr, "\n");
    }
}

# define SeExprYY_REDUCE_PRINT(Rule)		\
do {					\
  if (SeExprdebug)				\
    SeExpr_reduce_print (SeExprvsp, SeExprlsp, Rule); \
} while (SeExprYYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int SeExprdebug;
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

# ifndef SeExprstrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define SeExprstrlen strlen
#  else
/* Return the length of SeExprYYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static SeExprYYSIZE_T
SeExprstrlen (const char *SeExprstr)
#else
static SeExprYYSIZE_T
SeExprstrlen (SeExprstr)
    const char *SeExprstr;
#endif
{
  SeExprYYSIZE_T SeExprlen;
  for (SeExprlen = 0; SeExprstr[SeExprlen]; SeExprlen++)
    continue;
  return SeExprlen;
}
#  endif
# endif

# ifndef SeExprstpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define SeExprstpcpy stpcpy
#  else
/* Copy SeExprYYSRC to SeExprYYDEST, returning the address of the terminating '\0' in
   SeExprYYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
SeExprstpcpy (char *SeExprdest, const char *SeExprsrc)
#else
static char *
SeExprstpcpy (SeExprdest, SeExprsrc)
    char *SeExprdest;
    const char *SeExprsrc;
#endif
{
  char *SeExprd = SeExprdest;
  const char *SeExprs = SeExprsrc;

  while ((*SeExprd++ = *SeExprs++) != '\0')
    continue;

  return SeExprd - 1;
}
#  endif
# endif

# ifndef SeExprtnamerr
/* Copy to SeExprYYRES the contents of SeExprYYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for SeExprerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  SeExprYYSTR is taken from SeExprtname.  If SeExprYYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static SeExprYYSIZE_T
SeExprtnamerr (char *SeExprres, const char *SeExprstr)
{
  if (*SeExprstr == '"')
    {
      SeExprYYSIZE_T SeExprn = 0;
      char const *SeExprp = SeExprstr;

      for (;;)
	switch (*++SeExprp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++SeExprp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (SeExprres)
	      SeExprres[SeExprn] = *SeExprp;
	    SeExprn++;
	    break;

	  case '"':
	    if (SeExprres)
	      SeExprres[SeExprn] = '\0';
	    return SeExprn;
	  }
    do_not_strip_quotes: ;
    }

  if (! SeExprres)
    return SeExprstrlen (SeExprstr);

  return SeExprstpcpy (SeExprres, SeExprstr) - SeExprres;
}
# endif

/* Copy into SeExprYYRESULT an error message about the unexpected token
   SeExprYYCHAR while in state SeExprYYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If SeExprYYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return SeExprYYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static SeExprYYSIZE_T
SeExprsyntax_error (char *SeExprresult, int SeExprstate, int SeExprchar)
{
  int SeExprn = SeExprpact[SeExprstate];

  if (! (SeExprYYPACT_NINF < SeExprn && SeExprn <= SeExprYYLAST))
    return 0;
  else
    {
      int SeExprtype = SeExprYYTRANSLATE (SeExprchar);
      SeExprYYSIZE_T SeExprsize0 = SeExprtnamerr (0, SeExprtname[SeExprtype]);
      SeExprYYSIZE_T SeExprsize = SeExprsize0;
      SeExprYYSIZE_T SeExprsize1;
      int SeExprsize_overflow = 0;
      enum { SeExprYYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *SeExprarg[SeExprYYERROR_VERBOSE_ARGS_MAXIMUM];
      int SeExprx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      SeExprYY_("syntax error, unexpected %s");
      SeExprYY_("syntax error, unexpected %s, expecting %s");
      SeExprYY_("syntax error, unexpected %s, expecting %s or %s");
      SeExprYY_("syntax error, unexpected %s, expecting %s or %s or %s");
      SeExprYY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *SeExprfmt;
      char const *SeExprf;
      static char const SeExprunexpected[] = "syntax error, unexpected %s";
      static char const SeExprexpecting[] = ", expecting %s";
      static char const SeExpror[] = " or %s";
      char SeExprformat[sizeof SeExprunexpected
		    + sizeof SeExprexpecting - 1
		    + ((SeExprYYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof SeExpror - 1))];
      char const *SeExprprefix = SeExprexpecting;

      /* Start SeExprYYX at -SeExprYYN if negative to avoid negative indexes in
	 SeExprYYCHECK.  */
      int SeExprxbegin = SeExprn < 0 ? -SeExprn : 0;

      /* Stay within bounds of both SeExprcheck and SeExprtname.  */
      int SeExprchecklim = SeExprYYLAST - SeExprn + 1;
      int SeExprxend = SeExprchecklim < SeExprYYNTOKENS ? SeExprchecklim : SeExprYYNTOKENS;
      int SeExprcount = 1;

      SeExprarg[0] = SeExprtname[SeExprtype];
      SeExprfmt = SeExprstpcpy (SeExprformat, SeExprunexpected);

      for (SeExprx = SeExprxbegin; SeExprx < SeExprxend; ++SeExprx)
	if (SeExprcheck[SeExprx + SeExprn] == SeExprx && SeExprx != SeExprYYTERROR)
	  {
	    if (SeExprcount == SeExprYYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		SeExprcount = 1;
		SeExprsize = SeExprsize0;
		SeExprformat[sizeof SeExprunexpected - 1] = '\0';
		break;
	      }
	    SeExprarg[SeExprcount++] = SeExprtname[SeExprx];
	    SeExprsize1 = SeExprsize + SeExprtnamerr (0, SeExprtname[SeExprx]);
	    SeExprsize_overflow |= (SeExprsize1 < SeExprsize);
	    SeExprsize = SeExprsize1;
	    SeExprfmt = SeExprstpcpy (SeExprfmt, SeExprprefix);
	    SeExprprefix = SeExpror;
	  }

      SeExprf = SeExprYY_(SeExprformat);
      SeExprsize1 = SeExprsize + SeExprstrlen (SeExprf);
      SeExprsize_overflow |= (SeExprsize1 < SeExprsize);
      SeExprsize = SeExprsize1;

      if (SeExprsize_overflow)
	return SeExprYYSIZE_MAXIMUM;

      if (SeExprresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *SeExprp = SeExprresult;
	  int SeExpri = 0;
	  while ((*SeExprp = *SeExprf) != '\0')
	    {
	      if (*SeExprp == '%' && SeExprf[1] == 's' && SeExpri < SeExprcount)
		{
		  SeExprp += SeExprtnamerr (SeExprp, SeExprarg[SeExpri++]);
		  SeExprf += 2;
		}
	      else
		{
		  SeExprp++;
		  SeExprf++;
		}
	    }
	}
      return SeExprsize;
    }
}
#endif /* SeExprYYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
SeExprdestruct (const char *SeExprmsg, int SeExprtype, SeExprYYSTYPE *SeExprvaluep, SeExprYYLTYPE *SeExprlocationp)
#else
static void
SeExprdestruct (SeExprmsg, SeExprtype, SeExprvaluep, SeExprlocationp)
    const char *SeExprmsg;
    int SeExprtype;
    SeExprYYSTYPE *SeExprvaluep;
    SeExprYYLTYPE *SeExprlocationp;
#endif
{
  SeExprYYUSE (SeExprvaluep);
  SeExprYYUSE (SeExprlocationp);

  if (!SeExprmsg)
    SeExprmsg = "Deleting";
  SeExprYY_SYMBOL_PRINT (SeExprmsg, SeExprtype, SeExprvaluep, SeExprlocationp);

  switch (SeExprtype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef SeExprYYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int SeExprparse (void *SeExprYYPARSE_PARAM);
#else
int SeExprparse ();
#endif
#else /* ! SeExprYYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int SeExprparse (void);
#else
int SeExprparse ();
#endif
#endif /* ! SeExprYYPARSE_PARAM */


/* The lookahead symbol.  */
int SeExprchar;

/* The semantic value of the lookahead symbol.  */
SeExprYYSTYPE SeExprlval;

/* Location data for the lookahead symbol.  */
SeExprYYLTYPE SeExprlloc;

/* Number of syntax errors so far.  */
int SeExprnerrs;



/*-------------------------.
| SeExprparse or SeExprpush_parse.  |
`-------------------------*/

#ifdef SeExprYYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
SeExprparse (void *SeExprYYPARSE_PARAM)
#else
int
SeExprparse (SeExprYYPARSE_PARAM)
    void *SeExprYYPARSE_PARAM;
#endif
#else /* ! SeExprYYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
SeExprparse (void)
#else
int
SeExprparse ()

#endif
#endif
{


    int SeExprstate;
    /* Number of tokens to shift before error messages enabled.  */
    int SeExprerrstatus;

    /* The stacks and their tools:
       `SeExprss': related to states.
       `SeExprvs': related to semantic values.
       `SeExprls': related to locations.

       Refer to the stacks thru separate pointers, to allow SeExproverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    SeExprtype_int16 SeExprssa[SeExprYYINITDEPTH];
    SeExprtype_int16 *SeExprss;
    SeExprtype_int16 *SeExprssp;

    /* The semantic value stack.  */
    SeExprYYSTYPE SeExprvsa[SeExprYYINITDEPTH];
    SeExprYYSTYPE *SeExprvs;
    SeExprYYSTYPE *SeExprvsp;

    /* The location stack.  */
    SeExprYYLTYPE SeExprlsa[SeExprYYINITDEPTH];
    SeExprYYLTYPE *SeExprls;
    SeExprYYLTYPE *SeExprlsp;

    /* The locations where the error started and ended.  */
    SeExprYYLTYPE SeExprerror_range[2];

    SeExprYYSIZE_T SeExprstacksize;

  int SeExprn;
  int SeExprresult;
  /* Lookahead token as an internal (translated) token number.  */
  int SeExprtoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  SeExprYYSTYPE SeExprval;
  SeExprYYLTYPE SeExprloc;

#if SeExprYYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char SeExprmsgbuf[128];
  char *SeExprmsg = SeExprmsgbuf;
  SeExprYYSIZE_T SeExprmsg_alloc = sizeof SeExprmsgbuf;
#endif

#define SeExprYYPOPSTACK(N)   (SeExprvsp -= (N), SeExprssp -= (N), SeExprlsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int SeExprlen = 0;

  SeExprtoken = 0;
  SeExprss = SeExprssa;
  SeExprvs = SeExprvsa;
  SeExprls = SeExprlsa;
  SeExprstacksize = SeExprYYINITDEPTH;

  SeExprYYDPRINTF ((stderr, "Starting parse\n"));

  SeExprstate = 0;
  SeExprerrstatus = 0;
  SeExprnerrs = 0;
  SeExprchar = SeExprYYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  SeExprssp = SeExprss;
  SeExprvsp = SeExprvs;
  SeExprlsp = SeExprls;

#if SeExprYYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  SeExprlloc.first_line   = SeExprlloc.last_line   = 1;
  SeExprlloc.first_column = SeExprlloc.last_column = 1;
#endif

  goto SeExprsetstate;

/*------------------------------------------------------------.
| SeExprnewstate -- Push a new state, which is found in SeExprstate.  |
`------------------------------------------------------------*/
 SeExprnewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  SeExprssp++;

 SeExprsetstate:
  *SeExprssp = SeExprstate;

  if (SeExprss + SeExprstacksize - 1 <= SeExprssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      SeExprYYSIZE_T SeExprsize = SeExprssp - SeExprss + 1;

#ifdef SeExproverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	SeExprYYSTYPE *SeExprvs1 = SeExprvs;
	SeExprtype_int16 *SeExprss1 = SeExprss;
	SeExprYYLTYPE *SeExprls1 = SeExprls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if SeExproverflow is a macro.  */
	SeExproverflow (SeExprYY_("memory exhausted"),
		    &SeExprss1, SeExprsize * sizeof (*SeExprssp),
		    &SeExprvs1, SeExprsize * sizeof (*SeExprvsp),
		    &SeExprls1, SeExprsize * sizeof (*SeExprlsp),
		    &SeExprstacksize);

	SeExprls = SeExprls1;
	SeExprss = SeExprss1;
	SeExprvs = SeExprvs1;
      }
#else /* no SeExproverflow */
# ifndef SeExprYYSTACK_RELOCATE
      goto SeExprexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (SeExprYYMAXDEPTH <= SeExprstacksize)
	goto SeExprexhaustedlab;
      SeExprstacksize *= 2;
      if (SeExprYYMAXDEPTH < SeExprstacksize)
	SeExprstacksize = SeExprYYMAXDEPTH;

      {
	SeExprtype_int16 *SeExprss1 = SeExprss;
	union SeExpralloc *SeExprptr =
	  (union SeExpralloc *) SeExprYYSTACK_ALLOC (SeExprYYSTACK_BYTES (SeExprstacksize));
	if (! SeExprptr)
	  goto SeExprexhaustedlab;
	SeExprYYSTACK_RELOCATE (SeExprss_alloc, SeExprss);
	SeExprYYSTACK_RELOCATE (SeExprvs_alloc, SeExprvs);
	SeExprYYSTACK_RELOCATE (SeExprls_alloc, SeExprls);
#  undef SeExprYYSTACK_RELOCATE
	if (SeExprss1 != SeExprssa)
	  SeExprYYSTACK_FREE (SeExprss1);
      }
# endif
#endif /* no SeExproverflow */

      SeExprssp = SeExprss + SeExprsize - 1;
      SeExprvsp = SeExprvs + SeExprsize - 1;
      SeExprlsp = SeExprls + SeExprsize - 1;

      SeExprYYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) SeExprstacksize));

      if (SeExprss + SeExprstacksize - 1 <= SeExprssp)
	SeExprYYABORT;
    }

  SeExprYYDPRINTF ((stderr, "Entering state %d\n", SeExprstate));

  if (SeExprstate == SeExprYYFINAL)
    SeExprYYACCEPT;

  goto SeExprbackup;

/*-----------.
| SeExprbackup.  |
`-----------*/
SeExprbackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  SeExprn = SeExprpact[SeExprstate];
  if (SeExprn == SeExprYYPACT_NINF)
    goto SeExprdefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* SeExprYYCHAR is either SeExprYYEMPTY or SeExprYYEOF or a valid lookahead symbol.  */
  if (SeExprchar == SeExprYYEMPTY)
    {
      SeExprYYDPRINTF ((stderr, "Reading a token: "));
      SeExprchar = SeExprYYLEX;
    }

  if (SeExprchar <= SeExprYYEOF)
    {
      SeExprchar = SeExprtoken = SeExprYYEOF;
      SeExprYYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      SeExprtoken = SeExprYYTRANSLATE (SeExprchar);
      SeExprYY_SYMBOL_PRINT ("Next token is", SeExprtoken, &SeExprlval, &SeExprlloc);
    }

  /* If the proper action on seeing token SeExprYYTOKEN is to reduce or to
     detect an error, take that action.  */
  SeExprn += SeExprtoken;
  if (SeExprn < 0 || SeExprYYLAST < SeExprn || SeExprcheck[SeExprn] != SeExprtoken)
    goto SeExprdefault;
  SeExprn = SeExprtable[SeExprn];
  if (SeExprn <= 0)
    {
      if (SeExprn == 0 || SeExprn == SeExprYYTABLE_NINF)
	goto SeExprerrlab;
      SeExprn = -SeExprn;
      goto SeExprreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (SeExprerrstatus)
    SeExprerrstatus--;

  /* Shift the lookahead token.  */
  SeExprYY_SYMBOL_PRINT ("Shifting", SeExprtoken, &SeExprlval, &SeExprlloc);

  /* Discard the shifted token.  */
  SeExprchar = SeExprYYEMPTY;

  SeExprstate = SeExprn;
  *++SeExprvsp = SeExprlval;
  *++SeExprlsp = SeExprlloc;
  goto SeExprnewstate;


/*-----------------------------------------------------------.
| SeExprdefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
SeExprdefault:
  SeExprn = SeExprdefact[SeExprstate];
  if (SeExprn == 0)
    goto SeExprerrlab;
  goto SeExprreduce;


/*-----------------------------.
| SeExprreduce -- Do a reduction.  |
`-----------------------------*/
SeExprreduce:
  /* SeExprn is the number of a rule to reduce with.  */
  SeExprlen = SeExprr2[SeExprn];

  /* If SeExprYYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets SeExprYYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to SeExprYYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that SeExprYYVAL may be used uninitialized.  */
  SeExprval = SeExprvsp[1-SeExprlen];

  /* Default location.  */
  SeExprYYLLOC_DEFAULT (SeExprloc, (SeExprlsp - SeExprlen), SeExprlen);
  SeExprYY_REDUCE_PRINT (SeExprn);
  switch (SeExprn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 119 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { ParseResult = NODE2((SeExprloc).first_column,(SeExprloc).last_column,BlockNode, (SeExprvsp[(1) - (2)].n), (SeExprvsp[(2) - (2)].n)); }
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 120 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { ParseResult = (SeExprvsp[(1) - (1)].n); }
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 125 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE((SeExprloc).first_column,(SeExprloc).last_column,Node); /* create empty node */; }
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 126 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = (SeExprvsp[(1) - (1)].n); }
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 130 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE1((SeExprloc).first_column,(SeExprloc).last_column,Node, (SeExprvsp[(1) - (1)].n)); /* create var list */}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 131 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = (SeExprvsp[(1) - (2)].n); (SeExprvsp[(1) - (2)].n)->addChild((SeExprvsp[(2) - (2)].n)); /* add to list */}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 136 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = (SeExprvsp[(1) - (1)].n); }
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 137 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,AssignNode, (SeExprvsp[(1) - (4)].s), (SeExprvsp[(3) - (4)].n));  }
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 138 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    {SeExprNode* varNode=NODE1((SeExprlsp[(1) - (4)]).first_column,(SeExprlsp[(1) - (4)]).first_column,VarNode, (SeExprvsp[(1) - (4)].s));
                                SeExprNode* opNode=NODE2((SeExprlsp[(3) - (4)]).first_column,(SeExprlsp[(3) - (4)]).first_column,AddNode,varNode,(SeExprvsp[(3) - (4)].n));
                                (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,AssignNode, (SeExprvsp[(1) - (4)].s), opNode);}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 141 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    {SeExprNode* varNode=NODE1((SeExprlsp[(1) - (4)]).first_column,(SeExprlsp[(1) - (4)]).first_column,VarNode, (SeExprvsp[(1) - (4)].s));
                                SeExprNode* opNode=NODE2((SeExprlsp[(3) - (4)]).first_column,(SeExprlsp[(3) - (4)]).first_column,SubNode,varNode,(SeExprvsp[(3) - (4)].n));
                                (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,AssignNode, (SeExprvsp[(1) - (4)].s), opNode);}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 144 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    {SeExprNode* varNode=NODE1((SeExprlsp[(1) - (4)]).first_column,(SeExprlsp[(1) - (4)]).first_column,VarNode, (SeExprvsp[(1) - (4)].s));
                                SeExprNode* opNode=NODE2((SeExprlsp[(3) - (4)]).first_column,(SeExprlsp[(3) - (4)]).first_column,MulNode,varNode,(SeExprvsp[(3) - (4)].n));
                                (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,AssignNode, (SeExprvsp[(1) - (4)].s), opNode);}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 147 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    {SeExprNode* varNode=NODE1((SeExprlsp[(1) - (4)]).first_column,(SeExprlsp[(1) - (4)]).first_column,VarNode, (SeExprvsp[(1) - (4)].s));
                                SeExprNode* opNode=NODE2((SeExprlsp[(3) - (4)]).first_column,(SeExprlsp[(3) - (4)]).first_column,DivNode,varNode,(SeExprvsp[(3) - (4)].n));
                                (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,AssignNode, (SeExprvsp[(1) - (4)].s), opNode);}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 150 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    {SeExprNode* varNode=NODE1((SeExprlsp[(1) - (4)]).first_column,(SeExprlsp[(1) - (4)]).first_column,VarNode, (SeExprvsp[(1) - (4)].s));
                                SeExprNode* opNode=NODE2((SeExprlsp[(3) - (4)]).first_column,(SeExprlsp[(3) - (4)]).first_column,ExpNode,varNode,(SeExprvsp[(3) - (4)].n));
                                (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,AssignNode, (SeExprvsp[(1) - (4)].s), opNode);}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 153 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    {SeExprNode* varNode=NODE1((SeExprlsp[(1) - (4)]).first_column,(SeExprlsp[(1) - (4)]).first_column,VarNode, (SeExprvsp[(1) - (4)].s));
                                SeExprNode* opNode=NODE2((SeExprlsp[(3) - (4)]).first_column,(SeExprlsp[(3) - (4)]).first_column,ModNode,varNode,(SeExprvsp[(3) - (4)].n));
                                (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,AssignNode, (SeExprvsp[(1) - (4)].s), opNode);}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 156 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,AssignNode, (SeExprvsp[(1) - (4)].s), (SeExprvsp[(3) - (4)].n));  }
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 157 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    {SeExprNode* varNode=NODE1((SeExprlsp[(1) - (4)]).first_column,(SeExprlsp[(1) - (4)]).first_column,VarNode, (SeExprvsp[(1) - (4)].s));
                                SeExprNode* opNode=NODE2((SeExprlsp[(3) - (4)]).first_column,(SeExprlsp[(3) - (4)]).first_column,AddNode,varNode,(SeExprvsp[(3) - (4)].n));
                                (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,AssignNode, (SeExprvsp[(1) - (4)].s), opNode);}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 160 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    {SeExprNode* varNode=NODE1((SeExprlsp[(1) - (4)]).first_column,(SeExprlsp[(1) - (4)]).first_column,VarNode, (SeExprvsp[(1) - (4)].s));
                                SeExprNode* opNode=NODE2((SeExprlsp[(3) - (4)]).first_column,(SeExprlsp[(3) - (4)]).first_column,SubNode,varNode,(SeExprvsp[(3) - (4)].n));
                                (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,AssignNode, (SeExprvsp[(1) - (4)].s), opNode);}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 163 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    {SeExprNode* varNode=NODE1((SeExprlsp[(1) - (4)]).first_column,(SeExprlsp[(1) - (4)]).first_column,VarNode, (SeExprvsp[(1) - (4)].s));
                                SeExprNode* opNode=NODE2((SeExprlsp[(3) - (4)]).first_column,(SeExprlsp[(3) - (4)]).first_column,MulNode,varNode,(SeExprvsp[(3) - (4)].n));
                                (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,AssignNode, (SeExprvsp[(1) - (4)].s), opNode);}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 166 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    {SeExprNode* varNode=NODE1((SeExprlsp[(1) - (4)]).first_column,(SeExprlsp[(1) - (4)]).first_column,VarNode, (SeExprvsp[(1) - (4)].s));
                                SeExprNode* opNode=NODE2((SeExprlsp[(3) - (4)]).first_column,(SeExprlsp[(3) - (4)]).first_column,DivNode,varNode,(SeExprvsp[(3) - (4)].n));
                                (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,AssignNode, (SeExprvsp[(1) - (4)].s), opNode);}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 169 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    {SeExprNode* varNode=NODE1((SeExprlsp[(1) - (4)]).first_column,(SeExprlsp[(1) - (4)]).first_column,VarNode, (SeExprvsp[(1) - (4)].s));
                                SeExprNode* opNode=NODE2((SeExprlsp[(3) - (4)]).first_column,(SeExprlsp[(3) - (4)]).first_column,ExpNode,varNode,(SeExprvsp[(3) - (4)].n));
                                (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,AssignNode, (SeExprvsp[(1) - (4)].s), opNode);}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 172 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    {SeExprNode* varNode=NODE1((SeExprlsp[(1) - (4)]).first_column,(SeExprlsp[(1) - (4)]).first_column,VarNode, (SeExprvsp[(1) - (4)].s));
                                SeExprNode* opNode=NODE2((SeExprlsp[(3) - (4)]).first_column,(SeExprlsp[(3) - (4)]).first_column,ModNode,varNode,(SeExprvsp[(3) - (4)].n));
                                (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,AssignNode, (SeExprvsp[(1) - (4)].s), opNode);}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 179 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE3((SeExprloc).first_column,(SeExprloc).last_column,IfThenElseNode, (SeExprvsp[(3) - (8)].n), (SeExprvsp[(6) - (8)].n), (SeExprvsp[(8) - (8)].n)); }
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 183 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE((SeExprloc).first_column,(SeExprloc).last_column,Node); /* create empty node */ }
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 184 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = (SeExprvsp[(3) - (4)].n); }
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 185 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = (SeExprvsp[(2) - (2)].n); }
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 190 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = (SeExprvsp[(2) - (3)].n); }
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 191 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE3((SeExprloc).first_column,(SeExprloc).last_column,VecNode, (SeExprvsp[(2) - (7)].n), (SeExprvsp[(4) - (7)].n), (SeExprvsp[(6) - (7)].n)); }
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 192 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,SubscriptNode, (SeExprvsp[(1) - (4)].n), (SeExprvsp[(3) - (4)].n)); }
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 193 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE3((SeExprloc).first_column,(SeExprloc).last_column,CondNode, (SeExprvsp[(1) - (5)].n), (SeExprvsp[(3) - (5)].n), (SeExprvsp[(5) - (5)].n)); }
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 194 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,OrNode, (SeExprvsp[(1) - (3)].n), (SeExprvsp[(3) - (3)].n)); }
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 195 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,AndNode, (SeExprvsp[(1) - (3)].n), (SeExprvsp[(3) - (3)].n)); }
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 196 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,EqNode, (SeExprvsp[(1) - (3)].n), (SeExprvsp[(3) - (3)].n)); }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 197 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,NeNode, (SeExprvsp[(1) - (3)].n), (SeExprvsp[(3) - (3)].n)); }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 198 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,LtNode, (SeExprvsp[(1) - (3)].n), (SeExprvsp[(3) - (3)].n)); }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 199 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,GtNode, (SeExprvsp[(1) - (3)].n), (SeExprvsp[(3) - (3)].n)); }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 200 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,LeNode, (SeExprvsp[(1) - (3)].n), (SeExprvsp[(3) - (3)].n)); }
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 201 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,GeNode, (SeExprvsp[(1) - (3)].n), (SeExprvsp[(3) - (3)].n)); }
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 202 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = (SeExprvsp[(2) - (2)].n); }
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 203 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE1((SeExprloc).first_column,(SeExprloc).last_column,NegNode, (SeExprvsp[(2) - (2)].n)); }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 204 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE1((SeExprloc).first_column,(SeExprloc).last_column,NotNode, (SeExprvsp[(2) - (2)].n)); }
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 205 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE1((SeExprloc).first_column,(SeExprloc).last_column,InvertNode, (SeExprvsp[(2) - (2)].n)); }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 206 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,AddNode, (SeExprvsp[(1) - (3)].n), (SeExprvsp[(3) - (3)].n)); }
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 207 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,SubNode, (SeExprvsp[(1) - (3)].n), (SeExprvsp[(3) - (3)].n)); }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 208 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,MulNode, (SeExprvsp[(1) - (3)].n), (SeExprvsp[(3) - (3)].n)); }
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 209 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,DivNode, (SeExprvsp[(1) - (3)].n), (SeExprvsp[(3) - (3)].n)); }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 210 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,ModNode, (SeExprvsp[(1) - (3)].n), (SeExprvsp[(3) - (3)].n)); }
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 211 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE2((SeExprloc).first_column,(SeExprloc).last_column,ExpNode, (SeExprvsp[(1) - (3)].n), (SeExprvsp[(3) - (3)].n)); }
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 212 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE1((SeExprloc).first_column,(SeExprloc).last_column,FuncNode, (SeExprvsp[(1) - (4)].s)); 
				  // add args directly and discard arg list node
				  (SeExprval.n)->addChildren((SeExprvsp[(3) - (4)].n)); Forget((SeExprvsp[(3) - (4)].n)); }
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 216 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE1((SeExprloc).first_column,(SeExprloc).last_column,FuncNode, (SeExprvsp[(3) - (6)].s)); 
				  (SeExprval.n)->addChild((SeExprvsp[(1) - (6)].n));
				  // add args directly and discard arg list node
				  (SeExprval.n)->addChildren((SeExprvsp[(5) - (6)].n)); Forget((SeExprvsp[(5) - (6)].n)); }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 220 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE1((SeExprloc).first_column,(SeExprloc).last_column,VarNode, (SeExprvsp[(1) - (1)].s)); }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 221 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE1((SeExprloc).first_column,(SeExprloc).last_column,VarNode, (SeExprvsp[(1) - (1)].s)); }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 222 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE1((SeExprloc).first_column,(SeExprloc).last_column,NumNode, (SeExprvsp[(1) - (1)].d)); /*printf("line %d",@$.last_column);*/}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 227 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE((SeExprloc).first_column,(SeExprloc).last_column,Node); /* create empty node */}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 228 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = (SeExprvsp[(1) - (1)].n); }
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 233 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE1((SeExprloc).first_column,(SeExprloc).last_column,Node, (SeExprvsp[(1) - (1)].n)); /* create arg list */}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 234 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = (SeExprvsp[(1) - (3)].n); (SeExprvsp[(1) - (3)].n)->addChild((SeExprvsp[(3) - (3)].n)); /* add to list */}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 238 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = (SeExprvsp[(1) - (1)].n); }
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 239 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"
    { (SeExprval.n) = NODE1((SeExprloc).first_column,(SeExprloc).last_column,StrNode, (SeExprvsp[(1) - (1)].s));}
    break;



/* Line 1455 of yacc.c  */
#line 2135 "y.tab.c"
      default: break;
    }
  SeExprYY_SYMBOL_PRINT ("-> $$ =", SeExprr1[SeExprn], &SeExprval, &SeExprloc);

  SeExprYYPOPSTACK (SeExprlen);
  SeExprlen = 0;
  SeExprYY_STACK_PRINT (SeExprss, SeExprssp);

  *++SeExprvsp = SeExprval;
  *++SeExprlsp = SeExprloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  SeExprn = SeExprr1[SeExprn];

  SeExprstate = SeExprpgoto[SeExprn - SeExprYYNTOKENS] + *SeExprssp;
  if (0 <= SeExprstate && SeExprstate <= SeExprYYLAST && SeExprcheck[SeExprstate] == *SeExprssp)
    SeExprstate = SeExprtable[SeExprstate];
  else
    SeExprstate = SeExprdefgoto[SeExprn - SeExprYYNTOKENS];

  goto SeExprnewstate;


/*------------------------------------.
| SeExprerrlab -- here on detecting error |
`------------------------------------*/
SeExprerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!SeExprerrstatus)
    {
      ++SeExprnerrs;
#if ! SeExprYYERROR_VERBOSE
      SeExprerror (SeExprYY_("syntax error"));
#else
      {
	SeExprYYSIZE_T SeExprsize = SeExprsyntax_error (0, SeExprstate, SeExprchar);
	if (SeExprmsg_alloc < SeExprsize && SeExprmsg_alloc < SeExprYYSTACK_ALLOC_MAXIMUM)
	  {
	    SeExprYYSIZE_T SeExpralloc = 2 * SeExprsize;
	    if (! (SeExprsize <= SeExpralloc && SeExpralloc <= SeExprYYSTACK_ALLOC_MAXIMUM))
	      SeExpralloc = SeExprYYSTACK_ALLOC_MAXIMUM;
	    if (SeExprmsg != SeExprmsgbuf)
	      SeExprYYSTACK_FREE (SeExprmsg);
	    SeExprmsg = (char *) SeExprYYSTACK_ALLOC (SeExpralloc);
	    if (SeExprmsg)
	      SeExprmsg_alloc = SeExpralloc;
	    else
	      {
		SeExprmsg = SeExprmsgbuf;
		SeExprmsg_alloc = sizeof SeExprmsgbuf;
	      }
	  }

	if (0 < SeExprsize && SeExprsize <= SeExprmsg_alloc)
	  {
	    (void) SeExprsyntax_error (SeExprmsg, SeExprstate, SeExprchar);
	    SeExprerror (SeExprmsg);
	  }
	else
	  {
	    SeExprerror (SeExprYY_("syntax error"));
	    if (SeExprsize != 0)
	      goto SeExprexhaustedlab;
	  }
      }
#endif
    }

  SeExprerror_range[0] = SeExprlloc;

  if (SeExprerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (SeExprchar <= SeExprYYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (SeExprchar == SeExprYYEOF)
	    SeExprYYABORT;
	}
      else
	{
	  SeExprdestruct ("Error: discarding",
		      SeExprtoken, &SeExprlval, &SeExprlloc);
	  SeExprchar = SeExprYYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto SeExprerrlab1;


/*---------------------------------------------------.
| SeExprerrorlab -- error raised explicitly by SeExprYYERROR.  |
`---------------------------------------------------*/
SeExprerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     SeExprYYERROR and the label SeExprerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto SeExprerrorlab;

  SeExprerror_range[0] = SeExprlsp[1-SeExprlen];
  /* Do not reclaim the symbols of the rule which action triggered
     this SeExprYYERROR.  */
  SeExprYYPOPSTACK (SeExprlen);
  SeExprlen = 0;
  SeExprYY_STACK_PRINT (SeExprss, SeExprssp);
  SeExprstate = *SeExprssp;
  goto SeExprerrlab1;


/*-------------------------------------------------------------.
| SeExprerrlab1 -- common code for both syntax error and SeExprYYERROR.  |
`-------------------------------------------------------------*/
SeExprerrlab1:
  SeExprerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      SeExprn = SeExprpact[SeExprstate];
      if (SeExprn != SeExprYYPACT_NINF)
	{
	  SeExprn += SeExprYYTERROR;
	  if (0 <= SeExprn && SeExprn <= SeExprYYLAST && SeExprcheck[SeExprn] == SeExprYYTERROR)
	    {
	      SeExprn = SeExprtable[SeExprn];
	      if (0 < SeExprn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (SeExprssp == SeExprss)
	SeExprYYABORT;

      SeExprerror_range[0] = *SeExprlsp;
      SeExprdestruct ("Error: popping",
		  SeExprstos[SeExprstate], SeExprvsp, SeExprlsp);
      SeExprYYPOPSTACK (1);
      SeExprstate = *SeExprssp;
      SeExprYY_STACK_PRINT (SeExprss, SeExprssp);
    }

  *++SeExprvsp = SeExprlval;

  SeExprerror_range[1] = SeExprlloc;
  /* Using SeExprYYLLOC is tempting, but would change the location of
     the lookahead.  SeExprYYLOC is available though.  */
  SeExprYYLLOC_DEFAULT (SeExprloc, (SeExprerror_range - 1), 2);
  *++SeExprlsp = SeExprloc;

  /* Shift the error token.  */
  SeExprYY_SYMBOL_PRINT ("Shifting", SeExprstos[SeExprn], SeExprvsp, SeExprlsp);

  SeExprstate = SeExprn;
  goto SeExprnewstate;


/*-------------------------------------.
| SeExpracceptlab -- SeExprYYACCEPT comes here.  |
`-------------------------------------*/
SeExpracceptlab:
  SeExprresult = 0;
  goto SeExprreturn;

/*-----------------------------------.
| SeExprabortlab -- SeExprYYABORT comes here.  |
`-----------------------------------*/
SeExprabortlab:
  SeExprresult = 1;
  goto SeExprreturn;

#if !defined(SeExproverflow) || SeExprYYERROR_VERBOSE
/*-------------------------------------------------.
| SeExprexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
SeExprexhaustedlab:
  SeExprerror (SeExprYY_("memory exhausted"));
  SeExprresult = 2;
  /* Fall through.  */
#endif

SeExprreturn:
  if (SeExprchar != SeExprYYEMPTY)
     SeExprdestruct ("Cleanup: discarding lookahead",
		 SeExprtoken, &SeExprlval, &SeExprlloc);
  /* Do not reclaim the symbols of the rule which action triggered
     this SeExprYYABORT or SeExprYYACCEPT.  */
  SeExprYYPOPSTACK (SeExprlen);
  SeExprYY_STACK_PRINT (SeExprss, SeExprssp);
  while (SeExprssp != SeExprss)
    {
      SeExprdestruct ("Cleanup: popping",
		  SeExprstos[*SeExprssp], SeExprvsp, SeExprlsp);
      SeExprYYPOPSTACK (1);
    }
#ifndef SeExproverflow
  if (SeExprss != SeExprssa)
    SeExprYYSTACK_FREE (SeExprss);
#endif
#if SeExprYYERROR_VERBOSE
  if (SeExprmsg != SeExprmsgbuf)
    SeExprYYSTACK_FREE (SeExprmsg);
#endif
  /* Make sure SeExprYYID is used.  */
  return SeExprYYID (SeExprresult);
}



/* Line 1675 of yacc.c  */
#line 242 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExpr/SeExprParser.y"


      /* SeExprerror - Report an error.  This is called by the parser.
	 (Note: the "msg" param is useless as it is usually just "parse error".
	 so it's ignored.)
      */
static void SeExprerror(const char* /*msg*/)
{
    // find start of line containing error
    int pos = SeExprpos(), lineno = 1, start = 0, end = strlen(ParseStr);
    bool multiline = 0;
    for (int i = start; i < pos; i++)
	if (ParseStr[i] == '\n') { start = i + 1; lineno++; multiline=1; }

    // find end of line containing error
    for (int i = end; i > pos; i--)
	if (ParseStr[i] == '\n') { end = i - 1; multiline=1; }

    ParseError = SeExprtext[0] ? "Syntax error" : "Unexpected end of expression";
    if (multiline) {
	char buff[30];
	snprintf(buff, 30, " at line %d", lineno);
	ParseError += buff;
    }
    if (SeExprtext[0]) {
	ParseError += " near '";
	ParseError += SeExprtext;
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

extern void resetCounters(std::vector<char*>* stringTokens);

static SeExprInternal::Mutex mutex;
int SeExprlex_destroy  (void);
bool SeExprParse(SeExprNode*& parseTree, std::string& error, int& errorStart, int& errorEnd,
    const SeExpression* expr, const char* str, 
    std::vector<char*>* stringTokens)
{
    SeExprInternal::AutoMutex locker(mutex);

    // glue around crippled C interface - ugh!
    Expr = expr;
    ParseStr = str;
    resetCounters(stringTokens); // reset lineNumber and columnNumber in scanner
    SeExpr_buffer_state* buffer = SeExpr_scan_string(str);
    ParseResult = 0;
    int resultCode = SeExprparse();
    SeExpr_delete_buffer(buffer);
    SeExprlex_destroy ();
    if (resultCode == 0) {
	// success
	error = "";
	parseTree = ParseResult;
    }
    else {
	// failure
	error = ParseError;
        errorStart=SeExprlloc.first_column;
        errorEnd=SeExprlloc.last_column;
	parseTree = 0;
	// gather list of nodes with no parent
	std::vector<SeExprNode*> delnodes;
	std::vector<SeExprNode*>::iterator iter;
	for (iter = ParseNodes.begin(); iter != ParseNodes.end(); iter++)
	    if (!(*iter)->parent()) { delnodes.push_back(*iter); }
	// now delete them (they will delete their own children)
	for (iter = delnodes.begin(); iter != delnodes.end(); iter++)
	    delete *iter;
    }
    ParseNodes.clear();

    return parseTree != 0;
}

