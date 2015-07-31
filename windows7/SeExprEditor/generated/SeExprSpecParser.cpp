
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

/* All symbols defined below should begin with SeExprSpec or SeExprSpecYY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define SeExprSpecYYBISON 1

/* Bison version.  */
#define SeExprSpecYYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define SeExprSpecYYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define SeExprSpecYYPURE 0

/* Push parsers.  */
#define SeExprSpecYYPUSH 0

/* Pull parsers.  */
#define SeExprSpecYYPULL 1

/* Using locations.  */
#define SeExprSpecYYLSP_NEEDED 1

/* Substitute the variable and function names.  */
#define SeExprSpecparse         SeExprSpecparse
#define SeExprSpeclex           SeExprSpeclex
#define SeExprSpecerror         SeExprSpecerror
#define SeExprSpeclval          SeExprSpeclval
#define SeExprSpecchar          SeExprSpecchar
#define SeExprSpecdebug         SeExprSpecdebug
#define SeExprSpecnerrs         SeExprSpecnerrs
#define SeExprSpeclloc          SeExprSpeclloc

/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 18 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"

#include <algorithm>
#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>
#include <typeinfo>
#ifdef SEEXPR_USE_ANIMLIB
#include <animlib/AnimCurve.h>
#include <animlib/AnimKeyframe.h>
#else
#define UNUSED(x) (void)(x)
#endif
#include "SePlatform.h"
#include "SeExprSpecType.h"
#include "SeExprEdEditable.h"


/******************
 lexer declarations
 ******************/


#define SPEC_IS_NUMBER(x) \
    (dynamic_cast<SeExprSpecScalarNode*>(x) != 0)
#define SPEC_IS_VECTOR(x) \
    (dynamic_cast<SeExprSpecVectorNode*>(x) != 0)
#define SPEC_IS_STR(x) \
    (dynamic_cast<SeExprSpecStringNode*>(x) != 0)

// declarations of functions and data in SeExprParser.l
int SeExprSpeclex();
int SeExprSpecpos();
extern int SeExprSpec_start;
extern char* SeExprSpectext;
struct SeExprSpec_buffer_state;
SeExprSpec_buffer_state* SeExprSpec_scan_string(const char *str);
void SeExprSpec_delete_buffer(SeExprSpec_buffer_state*);

//#####################################
// Keep track of mini parse tree nodes

// temporary to the parse... all pointers deleted at end of parse
static std::vector<SeExprSpecNode*> specNodes; 
/// Remember the spec node, so we can delete it later
SeExprSpecNode* remember(SeExprSpecNode* node)
{specNodes.push_back(node);return node;}


/// list of strings duplicated by lexer to avoid error mem leak
static std::vector<char*> tokens; 

char* specRegisterToken(char* rawString)
{
    char* tok=strdup(rawString);
    tokens.push_back(tok);
    return tok;
}

//######################################################################
// Expose parser API inputs/outputs to yacc as statics 

// these are pointers to the arguments send into parse API
// made static here so the parser can see them in yacc actions
static std::vector<SeExprEdEditable*>* editables;
static std::vector<std::string>* variables;

static const char* ParseStr;    // string being parsed
static std::string ParseError;  // error (set from SeExprSpecerror)
static SeExprSpecNode* ParseResult; // must set result here since SeExprSpecparse can't return it


//######################################################################
// Helpers used by actions to register data


/// Remember that there is an assignment to this variable (For autocomplete)
void registerVariable(const char* var)
{
    variables->push_back(var);
}

/// Variable Assignment/String literal should be turned into an editable
/// an editable is the data part of a control (it's model essentially)
void registerEditable(const char* var,SeExprSpecNode* node)
{
    //std::cerr<<"we have editable var "<<var<<std::endl;
    if(!node){
        //std::cerr<<"   null ptr "<<var<<std::endl;
    }else if(SeExprSpecScalarNode* n=dynamic_cast<SeExprSpecScalarNode*>(node)){
        editables->push_back(new SeExprEdNumberEditable(var,node->startPos,node->endPos,n->v));
    }else if(SeExprSpecVectorNode* n=dynamic_cast<SeExprSpecVectorNode*>(node)){
        editables->push_back(new SeExprEdVectorEditable(var,node->startPos,node->endPos,n->v));
    }else if(SeExprSpecStringNode* n=dynamic_cast<SeExprSpecStringNode*>(node)){
        editables->push_back(new SeExprEdStringEditable(node->startPos,node->endPos,n->v));
    }else if(SeExprSpecCCurveNode* n=dynamic_cast<SeExprSpecCCurveNode*>(node)){
        if(SeExprSpecListNode* args=dynamic_cast<SeExprSpecListNode*>(n->args)){
            if((args->nodes.size())%3==0){
                SeExprEdColorCurveEditable* ccurve=new SeExprEdColorCurveEditable(var,node->startPos,node->endPos);
                bool valid=true;
                for(size_t i=0;i<args->nodes.size();i+=3){
                    SeExprSpecScalarNode* xnode=dynamic_cast<SeExprSpecScalarNode*>(args->nodes[i]);
                    SeExprSpecVectorNode* ynode=dynamic_cast<SeExprSpecVectorNode*>(args->nodes[i+1]);
                    SeExprSpecScalarNode* interpnode=dynamic_cast<SeExprSpecScalarNode*>(args->nodes[i+2]);
                    if(xnode && ynode && interpnode){
                        ccurve->add(xnode->v,ynode->v,interpnode->v);
                    }else{
                        valid=false;
                    }                
                }
                if(valid) editables->push_back(ccurve);
                else delete ccurve;
            }else{
                //std::cerr<<"Curve has wrong # of args"<<args->nodes.size()<<std::endl;
            }
        }
    }else if(SeExprSpecCurveNode* n=dynamic_cast<SeExprSpecCurveNode*>(node)){
        if(SeExprSpecListNode* args=dynamic_cast<SeExprSpecListNode*>(n->args)){
            if((args->nodes.size())%3==0){
                SeExprEdCurveEditable* ccurve=new SeExprEdCurveEditable(var,node->startPos,node->endPos);
                bool valid=true;
                for(size_t i=0;i<args->nodes.size();i+=3){
                    SeExprSpecScalarNode* xnode=dynamic_cast<SeExprSpecScalarNode*>(args->nodes[i]);
                    SeExprSpecScalarNode* ynode=dynamic_cast<SeExprSpecScalarNode*>(args->nodes[i+1]);
                    SeExprSpecScalarNode* interpnode=dynamic_cast<SeExprSpecScalarNode*>(args->nodes[i+2]);
                    if(xnode && ynode && interpnode){
                        ccurve->add(xnode->v,ynode->v,interpnode->v);
                    }else{
                        valid=false;
                    }                
                }
                if(valid) editables->push_back(ccurve);
                else{
                    delete ccurve;
                }
            }
        }
    }else if(SeExprSpecAnimCurveNode* n=dynamic_cast<SeExprSpecAnimCurveNode*>(node)){
        if(SeExprSpecListNode* args=dynamic_cast<SeExprSpecListNode*>(n->args)){
            // need 3 items for pre inf and post inf and weighting, plus 9 items per key
            if((args->nodes.size()-4)%9==0){
                SeExprEdAnimCurveEditable* animCurve=new SeExprEdAnimCurveEditable(var,node->startPos,node->endPos);
                bool valid=true;
                

#ifdef SEEXPR_USE_ANIMLIB
                if(SeExprSpecStringNode* s=dynamic_cast<SeExprSpecStringNode*>(args->nodes[0])){
                    animCurve->curve.setPreInfinity(animlib::AnimCurve::stringToInfinityType(s->v));
                }else valid=false;
                if(SeExprSpecStringNode* s=dynamic_cast<SeExprSpecStringNode*>(args->nodes[1])){
                    animCurve->curve.setPostInfinity(animlib::AnimCurve::stringToInfinityType(s->v));
                }else valid=false;
                if(SeExprSpecScalarNode* v=dynamic_cast<SeExprSpecScalarNode*>(args->nodes[2])){
                    animCurve->curve.setWeighted(bool(v->v));
                }
                if(SeExprSpecStringNode* v=dynamic_cast<SeExprSpecStringNode*>(args->nodes[3])){
                    animCurve->link=v->v;
                }

                for(size_t i=4;i<args->nodes.size();i+=9){
                    SeExprSpecScalarNode* xnode=dynamic_cast<SeExprSpecScalarNode*>(args->nodes[i]);
                    SeExprSpecScalarNode* ynode=dynamic_cast<SeExprSpecScalarNode*>(args->nodes[i+1]);
                    SeExprSpecScalarNode* inWeight=dynamic_cast<SeExprSpecScalarNode*>(args->nodes[i+2]);
                    SeExprSpecScalarNode* outWeight=dynamic_cast<SeExprSpecScalarNode*>(args->nodes[i+3]);
                    SeExprSpecScalarNode* inAngle=dynamic_cast<SeExprSpecScalarNode*>(args->nodes[i+4]);
                    SeExprSpecScalarNode* outAngle=dynamic_cast<SeExprSpecScalarNode*>(args->nodes[i+5]);
                    SeExprSpecStringNode* inTangType=dynamic_cast<SeExprSpecStringNode*>(args->nodes[i+6]);
                    SeExprSpecStringNode* outTangType=dynamic_cast<SeExprSpecStringNode*>(args->nodes[i+7]);
                    SeExprSpecScalarNode* weighted=dynamic_cast<SeExprSpecScalarNode*>(args->nodes[i+8]);
                    if(xnode && ynode && inWeight && outWeight && inAngle && outAngle && inTangType && outTangType ){
                        animlib::AnimKeyframe key(xnode->v,ynode->v);
                        key.setInWeight(inWeight->v);
                        key.setOutWeight(outWeight->v);
                        key.setInAngle(inAngle->v);
                        key.setOutAngle(outAngle->v);
                        key.setInTangentType(animlib::AnimKeyframe::stringToTangentType(inTangType->v));
                        key.setOutTangentType(animlib::AnimKeyframe::stringToTangentType(outTangType->v));
                        key.setWeightsLocked(weighted->v);
                        animCurve->curve.addKey(key);
                    }else{
                        valid=false;
                    }                
                }
                if(valid) editables->push_back(animCurve);
                else delete animCurve;
#else
                UNUSED(animCurve);
                UNUSED(valid);
#endif
            }
        }
    }else{
        std::cerr<<"SEEXPREDITOR LOGIC ERROR: We didn't recognize the Spec"<<std::endl;
    }
}


/*******************
 parser declarations
 *******************/

// forward declaration
static void SeExprSpecerror(const char* msg);



/* Line 189 of yacc.c  */
#line 288 "y.tab.c"

/* Enabling traces.  */
#ifndef SeExprSpecYYDEBUG
# define SeExprSpecYYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef SeExprSpecYYERROR_VERBOSE
# undef SeExprSpecYYERROR_VERBOSE
# define SeExprSpecYYERROR_VERBOSE 1
#else
# define SeExprSpecYYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef SeExprSpecYYTOKEN_TABLE
# define SeExprSpecYYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef SeExprSpecYYTOKENTYPE
# define SeExprSpecYYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum SeExprSpectokentype {
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




#if ! defined SeExprSpecYYSTYPE && ! defined SeExprSpecYYSTYPE_IS_DECLARED
typedef union SeExprSpecYYSTYPE
{

/* Line 214 of yacc.c  */
#line 224 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"

    SeExprSpecNode* n;
    double d;      // return value for number tokens
    char* s;       /* return value for name tokens.  Note: UNLIKE the regular parser, this is not strdup()'dthe string */



/* Line 214 of yacc.c  */
#line 376 "y.tab.c"
} SeExprSpecYYSTYPE;
# define SeExprSpecYYSTYPE_IS_TRIVIAL 1
# define SeExprSpecstype SeExprSpecYYSTYPE /* obsolescent; will be withdrawn */
# define SeExprSpecYYSTYPE_IS_DECLARED 1
#endif

#if ! defined SeExprSpecYYLTYPE && ! defined SeExprSpecYYLTYPE_IS_DECLARED
typedef struct SeExprSpecYYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} SeExprSpecYYLTYPE;
# define SeExprSpecltype SeExprSpecYYLTYPE /* obsolescent; will be withdrawn */
# define SeExprSpecYYLTYPE_IS_DECLARED 1
# define SeExprSpecYYLTYPE_IS_TRIVIAL 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 401 "y.tab.c"

#ifdef short
# undef short
#endif

#ifdef SeExprSpecYYTYPE_UINT8
typedef SeExprSpecYYTYPE_UINT8 SeExprSpectype_uint8;
#else
typedef unsigned char SeExprSpectype_uint8;
#endif

#ifdef SeExprSpecYYTYPE_INT8
typedef SeExprSpecYYTYPE_INT8 SeExprSpectype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char SeExprSpectype_int8;
#else
typedef short int SeExprSpectype_int8;
#endif

#ifdef SeExprSpecYYTYPE_UINT16
typedef SeExprSpecYYTYPE_UINT16 SeExprSpectype_uint16;
#else
typedef unsigned short int SeExprSpectype_uint16;
#endif

#ifdef SeExprSpecYYTYPE_INT16
typedef SeExprSpecYYTYPE_INT16 SeExprSpectype_int16;
#else
typedef short int SeExprSpectype_int16;
#endif

#ifndef SeExprSpecYYSIZE_T
# ifdef __SIZE_TYPE__
#  define SeExprSpecYYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define SeExprSpecYYSIZE_T size_t
# elif ! defined SeExprSpecYYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define SeExprSpecYYSIZE_T size_t
# else
#  define SeExprSpecYYSIZE_T unsigned int
# endif
#endif

#define SeExprSpecYYSIZE_MAXIMUM ((SeExprSpecYYSIZE_T) -1)

#ifndef SeExprSpecYY_
# if SeExprSpecYYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define SeExprSpecYY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef SeExprSpecYY_
#  define SeExprSpecYY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define SeExprSpecYYUSE(e) ((void) (e))
#else
# define SeExprSpecYYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define SeExprSpecYYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
SeExprSpecYYID (int SeExprSpeci)
#else
static int
SeExprSpecYYID (SeExprSpeci)
    int SeExprSpeci;
#endif
{
  return SeExprSpeci;
}
#endif

#if ! defined SeExprSpecoverflow || SeExprSpecYYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef SeExprSpecYYSTACK_USE_ALLOCA
#  if SeExprSpecYYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define SeExprSpecYYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define SeExprSpecYYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define SeExprSpecYYSTACK_ALLOC alloca
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

# ifdef SeExprSpecYYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define SeExprSpecYYSTACK_FREE(Ptr) do { /* empty */; } while (SeExprSpecYYID (0))
#  ifndef SeExprSpecYYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define SeExprSpecYYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define SeExprSpecYYSTACK_ALLOC SeExprSpecYYMALLOC
#  define SeExprSpecYYSTACK_FREE SeExprSpecYYFREE
#  ifndef SeExprSpecYYSTACK_ALLOC_MAXIMUM
#   define SeExprSpecYYSTACK_ALLOC_MAXIMUM SeExprSpecYYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined SeExprSpecYYMALLOC || defined malloc) \
	     && (defined SeExprSpecYYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef SeExprSpecYYMALLOC
#   define SeExprSpecYYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (SeExprSpecYYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef SeExprSpecYYFREE
#   define SeExprSpecYYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined SeExprSpecoverflow || SeExprSpecYYERROR_VERBOSE */


#if (! defined SeExprSpecoverflow \
     && (! defined __cplusplus \
	 || (defined SeExprSpecYYLTYPE_IS_TRIVIAL && SeExprSpecYYLTYPE_IS_TRIVIAL \
	     && defined SeExprSpecYYSTYPE_IS_TRIVIAL && SeExprSpecYYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union SeExprSpecalloc
{
  SeExprSpectype_int16 SeExprSpecss_alloc;
  SeExprSpecYYSTYPE SeExprSpecvs_alloc;
  SeExprSpecYYLTYPE SeExprSpecls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define SeExprSpecYYSTACK_GAP_MAXIMUM (sizeof (union SeExprSpecalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define SeExprSpecYYSTACK_BYTES(N) \
     ((N) * (sizeof (SeExprSpectype_int16) + sizeof (SeExprSpecYYSTYPE) + sizeof (SeExprSpecYYLTYPE)) \
      + 2 * SeExprSpecYYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef SeExprSpecYYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define SeExprSpecYYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define SeExprSpecYYCOPY(To, From, Count)		\
      do					\
	{					\
	  SeExprSpecYYSIZE_T SeExprSpeci;				\
	  for (SeExprSpeci = 0; SeExprSpeci < (Count); SeExprSpeci++)	\
	    (To)[SeExprSpeci] = (From)[SeExprSpeci];		\
	}					\
      while (SeExprSpecYYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables SeExprSpecYYSIZE and SeExprSpecYYSTACKSIZE give the old and new number of
   elements in the stack, and SeExprSpecYYPTR gives the new location of the
   stack.  Advance SeExprSpecYYPTR to a properly aligned location for the next
   stack.  */
# define SeExprSpecYYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	SeExprSpecYYSIZE_T SeExprSpecnewbytes;						\
	SeExprSpecYYCOPY (&SeExprSpecptr->Stack_alloc, Stack, SeExprSpecsize);			\
	Stack = &SeExprSpecptr->Stack_alloc;					\
	SeExprSpecnewbytes = SeExprSpecstacksize * sizeof (*Stack) + SeExprSpecYYSTACK_GAP_MAXIMUM; \
	SeExprSpecptr += SeExprSpecnewbytes / sizeof (*SeExprSpecptr);				\
      }									\
    while (SeExprSpecYYID (0))

#endif

/* SeExprSpecYYFINAL -- State number of the termination state.  */
#define SeExprSpecYYFINAL  40
/* SeExprSpecYYLAST -- Last index in SeExprSpecYYTABLE.  */
#define SeExprSpecYYLAST   693

/* SeExprSpecYYNTOKENS -- Number of terminals.  */
#define SeExprSpecYYNTOKENS  44
/* SeExprSpecYYNNTS -- Number of nonterminals.  */
#define SeExprSpecYYNNTS  11
/* SeExprSpecYYNRULES -- Number of rules.  */
#define SeExprSpecYYNRULES  59
/* SeExprSpecYYNRULES -- Number of states.  */
#define SeExprSpecYYNSTATES  139

/* SeExprSpecYYTRANSLATE(SeExprSpecYYLEX) -- Bison symbol number corresponding to SeExprSpecYYLEX.  */
#define SeExprSpecYYUNDEFTOK  2
#define SeExprSpecYYMAXUTOK   277

#define SeExprSpecYYTRANSLATE(SeExprSpecYYX)						\
  ((unsigned int) (SeExprSpecYYX) <= SeExprSpecYYMAXUTOK ? SeExprSpectranslate[SeExprSpecYYX] : SeExprSpecYYUNDEFTOK)

/* SeExprSpecYYTRANSLATE[SeExprSpecYYLEX] -- Bison symbol number corresponding to SeExprSpecYYLEX.  */
static const SeExprSpectype_uint8 SeExprSpectranslate[] =
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

#if SeExprSpecYYDEBUG
/* SeExprSpecYYPRHS[SeExprSpecYYN] -- Index of the first RHS symbol of rule number SeExprSpecYYN in
   SeExprSpecYYRHS.  */
static const SeExprSpectype_uint8 SeExprSpecprhs[] =
{
       0,     0,     3,     6,     8,     9,    11,    13,    16,    18,
      23,    28,    33,    38,    43,    48,    53,    58,    63,    68,
      73,    78,    83,    88,    97,    98,   103,   106,   110,   118,
     123,   129,   133,   137,   141,   145,   149,   153,   157,   161,
     164,   167,   170,   173,   177,   181,   185,   189,   193,   197,
     202,   209,   211,   213,   215,   216,   218,   220,   224,   226
};

/* SeExprSpecYYRHS -- A `-1'-separated list of the rules' RHS.  */
static const SeExprSpectype_int8 SeExprSpecrhs[] =
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

/* SeExprSpecYYRLINE[SeExprSpecYYN] -- source line where rule number SeExprSpecYYN was defined.  */
static const SeExprSpectype_uint16 SeExprSpecrline[] =
{
       0,   266,   266,   267,   272,   273,   277,   278,   283,   284,
     288,   289,   290,   291,   292,   293,   294,   298,   299,   300,
     301,   302,   303,   307,   312,   313,   314,   319,   320,   325,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   335,
     336,   345,   346,   347,   348,   349,   350,   351,   352,   353,
     374,   375,   376,   377,   382,   383,   388,   397,   409,   410
};
#endif

#if SeExprSpecYYDEBUG || SeExprSpecYYERROR_VERBOSE || SeExprSpecYYTOKEN_TABLE
/* SeExprSpecYYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at SeExprSpecYYNTOKENS, nonterminals.  */
static const char *const SeExprSpectname[] =
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

# ifdef SeExprSpecYYPRINT
/* SeExprSpecYYTOKNUM[SeExprSpecYYLEX-NUM] -- Internal token number corresponding to
   token SeExprSpecYYLEX-NUM.  */
static const SeExprSpectype_uint16 SeExprSpectoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,    40,    41,   270,    58,    63,
     271,   272,   273,   274,    60,    62,   275,   276,    43,    45,
      42,    47,    37,    33,   126,   277,    94,    91,    61,    59,
     123,   125,    44,    93
};
# endif

/* SeExprSpecYYR1[SeExprSpecYYN] -- Symbol number of symbol that rule SeExprSpecYYN derives.  */
static const SeExprSpectype_uint8 SeExprSpecr1[] =
{
       0,    44,    45,    45,    46,    46,    47,    47,    48,    48,
      48,    48,    48,    48,    48,    48,    48,    48,    48,    48,
      48,    48,    48,    49,    50,    50,    50,    51,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    51,
      51,    51,    51,    51,    52,    52,    53,    53,    54,    54
};

/* SeExprSpecYYR2[SeExprSpecYYN] -- Number of symbols composing right hand side of rule SeExprSpecYYN.  */
static const SeExprSpectype_uint8 SeExprSpecr2[] =
{
       0,     2,     2,     1,     0,     1,     1,     2,     1,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     8,     0,     4,     2,     3,     7,     4,
       5,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       2,     2,     2,     3,     3,     3,     3,     3,     3,     4,
       6,     1,     1,     1,     0,     1,     1,     3,     1,     1
};

/* SeExprSpecYYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when SeExprSpecYYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const SeExprSpectype_uint8 SeExprSpecdefact[] =
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

/* SeExprSpecYYDEFGOTO[NTERM-NUM].  */
static const SeExprSpectype_int16 SeExprSpecdefgoto[] =
{
      -1,    11,   127,   128,    13,    14,   134,    68,    69,    70,
      71
};

/* SeExprSpecYYPACT[STATE-NUM] -- Index in SeExprSpecYYTABLE of the portion describing
   STATE-NUM.  */
#define SeExprSpecYYPACT_NINF -65
static const SeExprSpectype_int16 SeExprSpecpact[] =
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

/* SeExprSpecYYPGOTO[NTERM-NUM].  */
static const SeExprSpectype_int8 SeExprSpecpgoto[] =
{
     -65,   -65,   -64,    77,   -11,   -55,   -65,     0,   -38,   -65,
     -27
};

/* SeExprSpecYYTABLE[SeExprSpecYYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what SeExprSpecYYDEFACT says.
   If SeExprSpecYYTABLE_NINF, syntax error.  */
#define SeExprSpecYYTABLE_NINF -1
static const SeExprSpectype_uint8 SeExprSpectable[] =
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

static const SeExprSpectype_int16 SeExprSpeccheck[] =
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

/* SeExprSpecYYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const SeExprSpectype_uint8 SeExprSpecstos[] =
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

#define SeExprSpecerrok		(SeExprSpecerrstatus = 0)
#define SeExprSpecclearin	(SeExprSpecchar = SeExprSpecYYEMPTY)
#define SeExprSpecYYEMPTY		(-2)
#define SeExprSpecYYEOF		0

#define SeExprSpecYYACCEPT	goto SeExprSpecacceptlab
#define SeExprSpecYYABORT		goto SeExprSpecabortlab
#define SeExprSpecYYERROR		goto SeExprSpecerrorlab


/* Like SeExprSpecYYERROR except do call SeExprSpecerror.  This remains here temporarily
   to ease the transition to the new meaning of SeExprSpecYYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define SeExprSpecYYFAIL		goto SeExprSpecerrlab

#define SeExprSpecYYRECOVERING()  (!!SeExprSpecerrstatus)

#define SeExprSpecYYBACKUP(Token, Value)					\
do								\
  if (SeExprSpecchar == SeExprSpecYYEMPTY && SeExprSpeclen == 1)				\
    {								\
      SeExprSpecchar = (Token);						\
      SeExprSpeclval = (Value);						\
      SeExprSpectoken = SeExprSpecYYTRANSLATE (SeExprSpecchar);				\
      SeExprSpecYYPOPSTACK (1);						\
      goto SeExprSpecbackup;						\
    }								\
  else								\
    {								\
      SeExprSpecerror (SeExprSpecYY_("syntax error: cannot back up")); \
      SeExprSpecYYERROR;							\
    }								\
while (SeExprSpecYYID (0))


#define SeExprSpecYYTERROR	1
#define SeExprSpecYYERRCODE	256


/* SeExprSpecYYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define SeExprSpecYYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef SeExprSpecYYLLOC_DEFAULT
# define SeExprSpecYYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (SeExprSpecYYID (N))                                                    \
	{								\
	  (Current).first_line   = SeExprSpecYYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = SeExprSpecYYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = SeExprSpecYYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = SeExprSpecYYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    SeExprSpecYYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    SeExprSpecYYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (SeExprSpecYYID (0))
#endif


/* SeExprSpecYY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef SeExprSpecYY_LOCATION_PRINT
# if SeExprSpecYYLTYPE_IS_TRIVIAL
#  define SeExprSpecYY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define SeExprSpecYY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* SeExprSpecYYLEX -- calling `SeExprSpeclex' with the right arguments.  */

#ifdef SeExprSpecYYLEX_PARAM
# define SeExprSpecYYLEX SeExprSpeclex (SeExprSpecYYLEX_PARAM)
#else
# define SeExprSpecYYLEX SeExprSpeclex ()
#endif

/* Enable debugging if requested.  */
#if SeExprSpecYYDEBUG

# ifndef SeExprSpecYYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define SeExprSpecYYFPRINTF fprintf
# endif

# define SeExprSpecYYDPRINTF(Args)			\
do {						\
  if (SeExprSpecdebug)					\
    SeExprSpecYYFPRINTF Args;				\
} while (SeExprSpecYYID (0))

# define SeExprSpecYY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (SeExprSpecdebug)								  \
    {									  \
      SeExprSpecYYFPRINTF (stderr, "%s ", Title);					  \
      SeExprSpec_symbol_print (stderr,						  \
		  Type, Value, Location); \
      SeExprSpecYYFPRINTF (stderr, "\n");						  \
    }									  \
} while (SeExprSpecYYID (0))


/*--------------------------------.
| Print this symbol on SeExprSpecYYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
SeExprSpec_symbol_value_print (FILE *SeExprSpecoutput, int SeExprSpectype, SeExprSpecYYSTYPE const * const SeExprSpecvaluep, SeExprSpecYYLTYPE const * const SeExprSpeclocationp)
#else
static void
SeExprSpec_symbol_value_print (SeExprSpecoutput, SeExprSpectype, SeExprSpecvaluep, SeExprSpeclocationp)
    FILE *SeExprSpecoutput;
    int SeExprSpectype;
    SeExprSpecYYSTYPE const * const SeExprSpecvaluep;
    SeExprSpecYYLTYPE const * const SeExprSpeclocationp;
#endif
{
  if (!SeExprSpecvaluep)
    return;
  SeExprSpecYYUSE (SeExprSpeclocationp);
# ifdef SeExprSpecYYPRINT
  if (SeExprSpectype < SeExprSpecYYNTOKENS)
    SeExprSpecYYPRINT (SeExprSpecoutput, SeExprSpectoknum[SeExprSpectype], *SeExprSpecvaluep);
# else
  SeExprSpecYYUSE (SeExprSpecoutput);
# endif
  switch (SeExprSpectype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on SeExprSpecYYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
SeExprSpec_symbol_print (FILE *SeExprSpecoutput, int SeExprSpectype, SeExprSpecYYSTYPE const * const SeExprSpecvaluep, SeExprSpecYYLTYPE const * const SeExprSpeclocationp)
#else
static void
SeExprSpec_symbol_print (SeExprSpecoutput, SeExprSpectype, SeExprSpecvaluep, SeExprSpeclocationp)
    FILE *SeExprSpecoutput;
    int SeExprSpectype;
    SeExprSpecYYSTYPE const * const SeExprSpecvaluep;
    SeExprSpecYYLTYPE const * const SeExprSpeclocationp;
#endif
{
  if (SeExprSpectype < SeExprSpecYYNTOKENS)
    SeExprSpecYYFPRINTF (SeExprSpecoutput, "token %s (", SeExprSpectname[SeExprSpectype]);
  else
    SeExprSpecYYFPRINTF (SeExprSpecoutput, "nterm %s (", SeExprSpectname[SeExprSpectype]);

  SeExprSpecYY_LOCATION_PRINT (SeExprSpecoutput, *SeExprSpeclocationp);
  SeExprSpecYYFPRINTF (SeExprSpecoutput, ": ");
  SeExprSpec_symbol_value_print (SeExprSpecoutput, SeExprSpectype, SeExprSpecvaluep, SeExprSpeclocationp);
  SeExprSpecYYFPRINTF (SeExprSpecoutput, ")");
}

/*------------------------------------------------------------------.
| SeExprSpec_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
SeExprSpec_stack_print (SeExprSpectype_int16 *SeExprSpecbottom, SeExprSpectype_int16 *SeExprSpectop)
#else
static void
SeExprSpec_stack_print (SeExprSpecbottom, SeExprSpectop)
    SeExprSpectype_int16 *SeExprSpecbottom;
    SeExprSpectype_int16 *SeExprSpectop;
#endif
{
  SeExprSpecYYFPRINTF (stderr, "Stack now");
  for (; SeExprSpecbottom <= SeExprSpectop; SeExprSpecbottom++)
    {
      int SeExprSpecbot = *SeExprSpecbottom;
      SeExprSpecYYFPRINTF (stderr, " %d", SeExprSpecbot);
    }
  SeExprSpecYYFPRINTF (stderr, "\n");
}

# define SeExprSpecYY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (SeExprSpecdebug)							\
    SeExprSpec_stack_print ((Bottom), (Top));				\
} while (SeExprSpecYYID (0))


/*------------------------------------------------.
| Report that the SeExprSpecYYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
SeExprSpec_reduce_print (SeExprSpecYYSTYPE *SeExprSpecvsp, SeExprSpecYYLTYPE *SeExprSpeclsp, int SeExprSpecrule)
#else
static void
SeExprSpec_reduce_print (SeExprSpecvsp, SeExprSpeclsp, SeExprSpecrule)
    SeExprSpecYYSTYPE *SeExprSpecvsp;
    SeExprSpecYYLTYPE *SeExprSpeclsp;
    int SeExprSpecrule;
#endif
{
  int SeExprSpecnrhs = SeExprSpecr2[SeExprSpecrule];
  int SeExprSpeci;
  unsigned long int SeExprSpeclno = SeExprSpecrline[SeExprSpecrule];
  SeExprSpecYYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     SeExprSpecrule - 1, SeExprSpeclno);
  /* The symbols being reduced.  */
  for (SeExprSpeci = 0; SeExprSpeci < SeExprSpecnrhs; SeExprSpeci++)
    {
      SeExprSpecYYFPRINTF (stderr, "   $%d = ", SeExprSpeci + 1);
      SeExprSpec_symbol_print (stderr, SeExprSpecrhs[SeExprSpecprhs[SeExprSpecrule] + SeExprSpeci],
		       &(SeExprSpecvsp[(SeExprSpeci + 1) - (SeExprSpecnrhs)])
		       , &(SeExprSpeclsp[(SeExprSpeci + 1) - (SeExprSpecnrhs)])		       );
      SeExprSpecYYFPRINTF (stderr, "\n");
    }
}

# define SeExprSpecYY_REDUCE_PRINT(Rule)		\
do {					\
  if (SeExprSpecdebug)				\
    SeExprSpec_reduce_print (SeExprSpecvsp, SeExprSpeclsp, Rule); \
} while (SeExprSpecYYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int SeExprSpecdebug;
#else /* !SeExprSpecYYDEBUG */
# define SeExprSpecYYDPRINTF(Args)
# define SeExprSpecYY_SYMBOL_PRINT(Title, Type, Value, Location)
# define SeExprSpecYY_STACK_PRINT(Bottom, Top)
# define SeExprSpecYY_REDUCE_PRINT(Rule)
#endif /* !SeExprSpecYYDEBUG */


/* SeExprSpecYYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	SeExprSpecYYINITDEPTH
# define SeExprSpecYYINITDEPTH 200
#endif

/* SeExprSpecYYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SeExprSpecYYSTACK_ALLOC_MAXIMUM < SeExprSpecYYSTACK_BYTES (SeExprSpecYYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef SeExprSpecYYMAXDEPTH
# define SeExprSpecYYMAXDEPTH 10000
#endif



#if SeExprSpecYYERROR_VERBOSE

# ifndef SeExprSpecstrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define SeExprSpecstrlen strlen
#  else
/* Return the length of SeExprSpecYYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static SeExprSpecYYSIZE_T
SeExprSpecstrlen (const char *SeExprSpecstr)
#else
static SeExprSpecYYSIZE_T
SeExprSpecstrlen (SeExprSpecstr)
    const char *SeExprSpecstr;
#endif
{
  SeExprSpecYYSIZE_T SeExprSpeclen;
  for (SeExprSpeclen = 0; SeExprSpecstr[SeExprSpeclen]; SeExprSpeclen++)
    continue;
  return SeExprSpeclen;
}
#  endif
# endif

# ifndef SeExprSpecstpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define SeExprSpecstpcpy stpcpy
#  else
/* Copy SeExprSpecYYSRC to SeExprSpecYYDEST, returning the address of the terminating '\0' in
   SeExprSpecYYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
SeExprSpecstpcpy (char *SeExprSpecdest, const char *SeExprSpecsrc)
#else
static char *
SeExprSpecstpcpy (SeExprSpecdest, SeExprSpecsrc)
    char *SeExprSpecdest;
    const char *SeExprSpecsrc;
#endif
{
  char *SeExprSpecd = SeExprSpecdest;
  const char *SeExprSpecs = SeExprSpecsrc;

  while ((*SeExprSpecd++ = *SeExprSpecs++) != '\0')
    continue;

  return SeExprSpecd - 1;
}
#  endif
# endif

# ifndef SeExprSpectnamerr
/* Copy to SeExprSpecYYRES the contents of SeExprSpecYYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for SeExprSpecerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  SeExprSpecYYSTR is taken from SeExprSpectname.  If SeExprSpecYYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static SeExprSpecYYSIZE_T
SeExprSpectnamerr (char *SeExprSpecres, const char *SeExprSpecstr)
{
  if (*SeExprSpecstr == '"')
    {
      SeExprSpecYYSIZE_T SeExprSpecn = 0;
      char const *SeExprSpecp = SeExprSpecstr;

      for (;;)
	switch (*++SeExprSpecp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++SeExprSpecp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (SeExprSpecres)
	      SeExprSpecres[SeExprSpecn] = *SeExprSpecp;
	    SeExprSpecn++;
	    break;

	  case '"':
	    if (SeExprSpecres)
	      SeExprSpecres[SeExprSpecn] = '\0';
	    return SeExprSpecn;
	  }
    do_not_strip_quotes: ;
    }

  if (! SeExprSpecres)
    return SeExprSpecstrlen (SeExprSpecstr);

  return SeExprSpecstpcpy (SeExprSpecres, SeExprSpecstr) - SeExprSpecres;
}
# endif

/* Copy into SeExprSpecYYRESULT an error message about the unexpected token
   SeExprSpecYYCHAR while in state SeExprSpecYYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If SeExprSpecYYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return SeExprSpecYYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static SeExprSpecYYSIZE_T
SeExprSpecsyntax_error (char *SeExprSpecresult, int SeExprSpecstate, int SeExprSpecchar)
{
  int SeExprSpecn = SeExprSpecpact[SeExprSpecstate];

  if (! (SeExprSpecYYPACT_NINF < SeExprSpecn && SeExprSpecn <= SeExprSpecYYLAST))
    return 0;
  else
    {
      int SeExprSpectype = SeExprSpecYYTRANSLATE (SeExprSpecchar);
      SeExprSpecYYSIZE_T SeExprSpecsize0 = SeExprSpectnamerr (0, SeExprSpectname[SeExprSpectype]);
      SeExprSpecYYSIZE_T SeExprSpecsize = SeExprSpecsize0;
      SeExprSpecYYSIZE_T SeExprSpecsize1;
      int SeExprSpecsize_overflow = 0;
      enum { SeExprSpecYYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *SeExprSpecarg[SeExprSpecYYERROR_VERBOSE_ARGS_MAXIMUM];
      int SeExprSpecx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      SeExprSpecYY_("syntax error, unexpected %s");
      SeExprSpecYY_("syntax error, unexpected %s, expecting %s");
      SeExprSpecYY_("syntax error, unexpected %s, expecting %s or %s");
      SeExprSpecYY_("syntax error, unexpected %s, expecting %s or %s or %s");
      SeExprSpecYY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *SeExprSpecfmt;
      char const *SeExprSpecf;
      static char const SeExprSpecunexpected[] = "syntax error, unexpected %s";
      static char const SeExprSpecexpecting[] = ", expecting %s";
      static char const SeExprSpecor[] = " or %s";
      char SeExprSpecformat[sizeof SeExprSpecunexpected
		    + sizeof SeExprSpecexpecting - 1
		    + ((SeExprSpecYYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof SeExprSpecor - 1))];
      char const *SeExprSpecprefix = SeExprSpecexpecting;

      /* Start SeExprSpecYYX at -SeExprSpecYYN if negative to avoid negative indexes in
	 SeExprSpecYYCHECK.  */
      int SeExprSpecxbegin = SeExprSpecn < 0 ? -SeExprSpecn : 0;

      /* Stay within bounds of both SeExprSpeccheck and SeExprSpectname.  */
      int SeExprSpecchecklim = SeExprSpecYYLAST - SeExprSpecn + 1;
      int SeExprSpecxend = SeExprSpecchecklim < SeExprSpecYYNTOKENS ? SeExprSpecchecklim : SeExprSpecYYNTOKENS;
      int SeExprSpeccount = 1;

      SeExprSpecarg[0] = SeExprSpectname[SeExprSpectype];
      SeExprSpecfmt = SeExprSpecstpcpy (SeExprSpecformat, SeExprSpecunexpected);

      for (SeExprSpecx = SeExprSpecxbegin; SeExprSpecx < SeExprSpecxend; ++SeExprSpecx)
	if (SeExprSpeccheck[SeExprSpecx + SeExprSpecn] == SeExprSpecx && SeExprSpecx != SeExprSpecYYTERROR)
	  {
	    if (SeExprSpeccount == SeExprSpecYYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		SeExprSpeccount = 1;
		SeExprSpecsize = SeExprSpecsize0;
		SeExprSpecformat[sizeof SeExprSpecunexpected - 1] = '\0';
		break;
	      }
	    SeExprSpecarg[SeExprSpeccount++] = SeExprSpectname[SeExprSpecx];
	    SeExprSpecsize1 = SeExprSpecsize + SeExprSpectnamerr (0, SeExprSpectname[SeExprSpecx]);
	    SeExprSpecsize_overflow |= (SeExprSpecsize1 < SeExprSpecsize);
	    SeExprSpecsize = SeExprSpecsize1;
	    SeExprSpecfmt = SeExprSpecstpcpy (SeExprSpecfmt, SeExprSpecprefix);
	    SeExprSpecprefix = SeExprSpecor;
	  }

      SeExprSpecf = SeExprSpecYY_(SeExprSpecformat);
      SeExprSpecsize1 = SeExprSpecsize + SeExprSpecstrlen (SeExprSpecf);
      SeExprSpecsize_overflow |= (SeExprSpecsize1 < SeExprSpecsize);
      SeExprSpecsize = SeExprSpecsize1;

      if (SeExprSpecsize_overflow)
	return SeExprSpecYYSIZE_MAXIMUM;

      if (SeExprSpecresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *SeExprSpecp = SeExprSpecresult;
	  int SeExprSpeci = 0;
	  while ((*SeExprSpecp = *SeExprSpecf) != '\0')
	    {
	      if (*SeExprSpecp == '%' && SeExprSpecf[1] == 's' && SeExprSpeci < SeExprSpeccount)
		{
		  SeExprSpecp += SeExprSpectnamerr (SeExprSpecp, SeExprSpecarg[SeExprSpeci++]);
		  SeExprSpecf += 2;
		}
	      else
		{
		  SeExprSpecp++;
		  SeExprSpecf++;
		}
	    }
	}
      return SeExprSpecsize;
    }
}
#endif /* SeExprSpecYYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
SeExprSpecdestruct (const char *SeExprSpecmsg, int SeExprSpectype, SeExprSpecYYSTYPE *SeExprSpecvaluep, SeExprSpecYYLTYPE *SeExprSpeclocationp)
#else
static void
SeExprSpecdestruct (SeExprSpecmsg, SeExprSpectype, SeExprSpecvaluep, SeExprSpeclocationp)
    const char *SeExprSpecmsg;
    int SeExprSpectype;
    SeExprSpecYYSTYPE *SeExprSpecvaluep;
    SeExprSpecYYLTYPE *SeExprSpeclocationp;
#endif
{
  SeExprSpecYYUSE (SeExprSpecvaluep);
  SeExprSpecYYUSE (SeExprSpeclocationp);

  if (!SeExprSpecmsg)
    SeExprSpecmsg = "Deleting";
  SeExprSpecYY_SYMBOL_PRINT (SeExprSpecmsg, SeExprSpectype, SeExprSpecvaluep, SeExprSpeclocationp);

  switch (SeExprSpectype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef SeExprSpecYYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int SeExprSpecparse (void *SeExprSpecYYPARSE_PARAM);
#else
int SeExprSpecparse ();
#endif
#else /* ! SeExprSpecYYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int SeExprSpecparse (void);
#else
int SeExprSpecparse ();
#endif
#endif /* ! SeExprSpecYYPARSE_PARAM */


/* The lookahead symbol.  */
int SeExprSpecchar;

/* The semantic value of the lookahead symbol.  */
SeExprSpecYYSTYPE SeExprSpeclval;

/* Location data for the lookahead symbol.  */
SeExprSpecYYLTYPE SeExprSpeclloc;

/* Number of syntax errors so far.  */
int SeExprSpecnerrs;



/*-------------------------.
| SeExprSpecparse or SeExprSpecpush_parse.  |
`-------------------------*/

#ifdef SeExprSpecYYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
SeExprSpecparse (void *SeExprSpecYYPARSE_PARAM)
#else
int
SeExprSpecparse (SeExprSpecYYPARSE_PARAM)
    void *SeExprSpecYYPARSE_PARAM;
#endif
#else /* ! SeExprSpecYYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
SeExprSpecparse (void)
#else
int
SeExprSpecparse ()

#endif
#endif
{


    int SeExprSpecstate;
    /* Number of tokens to shift before error messages enabled.  */
    int SeExprSpecerrstatus;

    /* The stacks and their tools:
       `SeExprSpecss': related to states.
       `SeExprSpecvs': related to semantic values.
       `SeExprSpecls': related to locations.

       Refer to the stacks thru separate pointers, to allow SeExprSpecoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    SeExprSpectype_int16 SeExprSpecssa[SeExprSpecYYINITDEPTH];
    SeExprSpectype_int16 *SeExprSpecss;
    SeExprSpectype_int16 *SeExprSpecssp;

    /* The semantic value stack.  */
    SeExprSpecYYSTYPE SeExprSpecvsa[SeExprSpecYYINITDEPTH];
    SeExprSpecYYSTYPE *SeExprSpecvs;
    SeExprSpecYYSTYPE *SeExprSpecvsp;

    /* The location stack.  */
    SeExprSpecYYLTYPE SeExprSpeclsa[SeExprSpecYYINITDEPTH];
    SeExprSpecYYLTYPE *SeExprSpecls;
    SeExprSpecYYLTYPE *SeExprSpeclsp;

    /* The locations where the error started and ended.  */
    SeExprSpecYYLTYPE SeExprSpecerror_range[2];

    SeExprSpecYYSIZE_T SeExprSpecstacksize;

  int SeExprSpecn;
  int SeExprSpecresult;
  /* Lookahead token as an internal (translated) token number.  */
  int SeExprSpectoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  SeExprSpecYYSTYPE SeExprSpecval;
  SeExprSpecYYLTYPE SeExprSpecloc;

#if SeExprSpecYYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char SeExprSpecmsgbuf[128];
  char *SeExprSpecmsg = SeExprSpecmsgbuf;
  SeExprSpecYYSIZE_T SeExprSpecmsg_alloc = sizeof SeExprSpecmsgbuf;
#endif

#define SeExprSpecYYPOPSTACK(N)   (SeExprSpecvsp -= (N), SeExprSpecssp -= (N), SeExprSpeclsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int SeExprSpeclen = 0;

  SeExprSpectoken = 0;
  SeExprSpecss = SeExprSpecssa;
  SeExprSpecvs = SeExprSpecvsa;
  SeExprSpecls = SeExprSpeclsa;
  SeExprSpecstacksize = SeExprSpecYYINITDEPTH;

  SeExprSpecYYDPRINTF ((stderr, "Starting parse\n"));

  SeExprSpecstate = 0;
  SeExprSpecerrstatus = 0;
  SeExprSpecnerrs = 0;
  SeExprSpecchar = SeExprSpecYYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  SeExprSpecssp = SeExprSpecss;
  SeExprSpecvsp = SeExprSpecvs;
  SeExprSpeclsp = SeExprSpecls;

#if SeExprSpecYYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  SeExprSpeclloc.first_line   = SeExprSpeclloc.last_line   = 1;
  SeExprSpeclloc.first_column = SeExprSpeclloc.last_column = 1;
#endif

  goto SeExprSpecsetstate;

/*------------------------------------------------------------.
| SeExprSpecnewstate -- Push a new state, which is found in SeExprSpecstate.  |
`------------------------------------------------------------*/
 SeExprSpecnewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  SeExprSpecssp++;

 SeExprSpecsetstate:
  *SeExprSpecssp = SeExprSpecstate;

  if (SeExprSpecss + SeExprSpecstacksize - 1 <= SeExprSpecssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      SeExprSpecYYSIZE_T SeExprSpecsize = SeExprSpecssp - SeExprSpecss + 1;

#ifdef SeExprSpecoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	SeExprSpecYYSTYPE *SeExprSpecvs1 = SeExprSpecvs;
	SeExprSpectype_int16 *SeExprSpecss1 = SeExprSpecss;
	SeExprSpecYYLTYPE *SeExprSpecls1 = SeExprSpecls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if SeExprSpecoverflow is a macro.  */
	SeExprSpecoverflow (SeExprSpecYY_("memory exhausted"),
		    &SeExprSpecss1, SeExprSpecsize * sizeof (*SeExprSpecssp),
		    &SeExprSpecvs1, SeExprSpecsize * sizeof (*SeExprSpecvsp),
		    &SeExprSpecls1, SeExprSpecsize * sizeof (*SeExprSpeclsp),
		    &SeExprSpecstacksize);

	SeExprSpecls = SeExprSpecls1;
	SeExprSpecss = SeExprSpecss1;
	SeExprSpecvs = SeExprSpecvs1;
      }
#else /* no SeExprSpecoverflow */
# ifndef SeExprSpecYYSTACK_RELOCATE
      goto SeExprSpecexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (SeExprSpecYYMAXDEPTH <= SeExprSpecstacksize)
	goto SeExprSpecexhaustedlab;
      SeExprSpecstacksize *= 2;
      if (SeExprSpecYYMAXDEPTH < SeExprSpecstacksize)
	SeExprSpecstacksize = SeExprSpecYYMAXDEPTH;

      {
	SeExprSpectype_int16 *SeExprSpecss1 = SeExprSpecss;
	union SeExprSpecalloc *SeExprSpecptr =
	  (union SeExprSpecalloc *) SeExprSpecYYSTACK_ALLOC (SeExprSpecYYSTACK_BYTES (SeExprSpecstacksize));
	if (! SeExprSpecptr)
	  goto SeExprSpecexhaustedlab;
	SeExprSpecYYSTACK_RELOCATE (SeExprSpecss_alloc, SeExprSpecss);
	SeExprSpecYYSTACK_RELOCATE (SeExprSpecvs_alloc, SeExprSpecvs);
	SeExprSpecYYSTACK_RELOCATE (SeExprSpecls_alloc, SeExprSpecls);
#  undef SeExprSpecYYSTACK_RELOCATE
	if (SeExprSpecss1 != SeExprSpecssa)
	  SeExprSpecYYSTACK_FREE (SeExprSpecss1);
      }
# endif
#endif /* no SeExprSpecoverflow */

      SeExprSpecssp = SeExprSpecss + SeExprSpecsize - 1;
      SeExprSpecvsp = SeExprSpecvs + SeExprSpecsize - 1;
      SeExprSpeclsp = SeExprSpecls + SeExprSpecsize - 1;

      SeExprSpecYYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) SeExprSpecstacksize));

      if (SeExprSpecss + SeExprSpecstacksize - 1 <= SeExprSpecssp)
	SeExprSpecYYABORT;
    }

  SeExprSpecYYDPRINTF ((stderr, "Entering state %d\n", SeExprSpecstate));

  if (SeExprSpecstate == SeExprSpecYYFINAL)
    SeExprSpecYYACCEPT;

  goto SeExprSpecbackup;

/*-----------.
| SeExprSpecbackup.  |
`-----------*/
SeExprSpecbackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  SeExprSpecn = SeExprSpecpact[SeExprSpecstate];
  if (SeExprSpecn == SeExprSpecYYPACT_NINF)
    goto SeExprSpecdefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* SeExprSpecYYCHAR is either SeExprSpecYYEMPTY or SeExprSpecYYEOF or a valid lookahead symbol.  */
  if (SeExprSpecchar == SeExprSpecYYEMPTY)
    {
      SeExprSpecYYDPRINTF ((stderr, "Reading a token: "));
      SeExprSpecchar = SeExprSpecYYLEX;
    }

  if (SeExprSpecchar <= SeExprSpecYYEOF)
    {
      SeExprSpecchar = SeExprSpectoken = SeExprSpecYYEOF;
      SeExprSpecYYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      SeExprSpectoken = SeExprSpecYYTRANSLATE (SeExprSpecchar);
      SeExprSpecYY_SYMBOL_PRINT ("Next token is", SeExprSpectoken, &SeExprSpeclval, &SeExprSpeclloc);
    }

  /* If the proper action on seeing token SeExprSpecYYTOKEN is to reduce or to
     detect an error, take that action.  */
  SeExprSpecn += SeExprSpectoken;
  if (SeExprSpecn < 0 || SeExprSpecYYLAST < SeExprSpecn || SeExprSpeccheck[SeExprSpecn] != SeExprSpectoken)
    goto SeExprSpecdefault;
  SeExprSpecn = SeExprSpectable[SeExprSpecn];
  if (SeExprSpecn <= 0)
    {
      if (SeExprSpecn == 0 || SeExprSpecn == SeExprSpecYYTABLE_NINF)
	goto SeExprSpecerrlab;
      SeExprSpecn = -SeExprSpecn;
      goto SeExprSpecreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (SeExprSpecerrstatus)
    SeExprSpecerrstatus--;

  /* Shift the lookahead token.  */
  SeExprSpecYY_SYMBOL_PRINT ("Shifting", SeExprSpectoken, &SeExprSpeclval, &SeExprSpeclloc);

  /* Discard the shifted token.  */
  SeExprSpecchar = SeExprSpecYYEMPTY;

  SeExprSpecstate = SeExprSpecn;
  *++SeExprSpecvsp = SeExprSpeclval;
  *++SeExprSpeclsp = SeExprSpeclloc;
  goto SeExprSpecnewstate;


/*-----------------------------------------------------------.
| SeExprSpecdefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
SeExprSpecdefault:
  SeExprSpecn = SeExprSpecdefact[SeExprSpecstate];
  if (SeExprSpecn == 0)
    goto SeExprSpecerrlab;
  goto SeExprSpecreduce;


/*-----------------------------.
| SeExprSpecreduce -- Do a reduction.  |
`-----------------------------*/
SeExprSpecreduce:
  /* SeExprSpecn is the number of a rule to reduce with.  */
  SeExprSpeclen = SeExprSpecr2[SeExprSpecn];

  /* If SeExprSpecYYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets SeExprSpecYYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to SeExprSpecYYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that SeExprSpecYYVAL may be used uninitialized.  */
  SeExprSpecval = SeExprSpecvsp[1-SeExprSpeclen];

  /* Default location.  */
  SeExprSpecYYLLOC_DEFAULT (SeExprSpecloc, (SeExprSpeclsp - SeExprSpeclen), SeExprSpeclen);
  SeExprSpecYY_REDUCE_PRINT (SeExprSpecn);
  switch (SeExprSpecn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 266 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { ParseResult = 0; }
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 267 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { ParseResult = 0; }
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 272 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 273 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 277 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 278 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 283 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 284 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { 
        registerVariable((SeExprSpecvsp[(1) - (4)].s));
        registerEditable((SeExprSpecvsp[(1) - (4)].s),(SeExprSpecvsp[(3) - (4)].n));
      }
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 288 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 289 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 290 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 291 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 292 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 293 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 294 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { 
        registerVariable((SeExprSpecvsp[(1) - (4)].s));
        registerEditable((SeExprSpecvsp[(1) - (4)].s),(SeExprSpecvsp[(3) - (4)].n));
      }
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 298 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    {  (SeExprSpecval.n) = 0; }
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 299 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    {  (SeExprSpecval.n) = 0; }
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 300 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    {  (SeExprSpecval.n) = 0; }
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 301 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    {  (SeExprSpecval.n) = 0; }
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 302 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    {  (SeExprSpecval.n) = 0; }
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 303 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    {  (SeExprSpecval.n) = 0; }
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 308 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 312 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 313 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 314 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 319 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 320 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { 
        if(SPEC_IS_NUMBER((SeExprSpecvsp[(2) - (7)].n)) && SPEC_IS_NUMBER((SeExprSpecvsp[(4) - (7)].n)) && SPEC_IS_NUMBER((SeExprSpecvsp[(6) - (7)].n))){
            (SeExprSpecval.n)=remember(new SeExprSpecVectorNode((SeExprSpecloc).first_column,(SeExprSpecloc).last_column,(SeExprSpecvsp[(2) - (7)].n),(SeExprSpecvsp[(4) - (7)].n),(SeExprSpecvsp[(6) - (7)].n)));
        }else (SeExprSpecval.n)=0;
      }
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 325 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 326 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 327 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 328 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 329 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 330 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 331 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 332 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 333 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 334 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 335 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = (SeExprSpecvsp[(2) - (2)].n); }
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 336 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { 
        if(SPEC_IS_NUMBER((SeExprSpecvsp[(2) - (2)].n))){
            SeExprSpecScalarNode* node=(SeExprSpecScalarNode*)(SeExprSpecvsp[(2) - (2)].n);
            node->v*=-1;
            node->startPos=(SeExprSpecloc).first_column;
            node->endPos=(SeExprSpecloc).last_column;
            (SeExprSpecval.n)=(SeExprSpecvsp[(2) - (2)].n);
        }else (SeExprSpecval.n)=0;
      }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 345 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 346 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 347 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 348 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 349 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 350 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 351 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 352 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0; }
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 353 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { 
        if((SeExprSpecvsp[(3) - (4)].n) && strcmp((SeExprSpecvsp[(1) - (4)].s),"curve")==0){
            (SeExprSpecval.n)=remember(new SeExprSpecCurveNode((SeExprSpecvsp[(3) - (4)].n)));
        }else if((SeExprSpecvsp[(3) - (4)].n) && strcmp((SeExprSpecvsp[(1) - (4)].s),"ccurve")==0){
            (SeExprSpecval.n)=remember(new SeExprSpecCCurveNode((SeExprSpecvsp[(3) - (4)].n)));
        }else if((SeExprSpecvsp[(3) - (4)].n) && strcmp((SeExprSpecvsp[(1) - (4)].s),"animCurve")==0){
            (SeExprSpecval.n)=remember(new SeExprSpecAnimCurveNode((SeExprSpecvsp[(3) - (4)].n)));
        }else if((SeExprSpecvsp[(3) - (4)].n)){
            // function arguments not parse of curve, ccurve, or animCurve 
            // check if there are any string args that need to be made into controls
            // but be sure to return 0 as this parseable
            if(SeExprSpecListNode* list=dynamic_cast<SeExprSpecListNode*>((SeExprSpecvsp[(3) - (4)].n))){
                for(size_t i=0;i<list->nodes.size();i++){
                    if(SeExprSpecStringNode* str=dynamic_cast<SeExprSpecStringNode*>(list->nodes[i])){
                        registerEditable("<UNKNOWN>",str);
                    }
                }
            }
            (SeExprSpecval.n)=0;
        }else (SeExprSpecval.n)=0;
      }
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 374 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    {(SeExprSpecval.n) = 0; }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 375 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    {  (SeExprSpecval.n) = 0; }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 376 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    {  (SeExprSpecval.n) = 0; }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 377 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n)=remember(new SeExprSpecScalarNode((SeExprSpecloc).first_column,(SeExprSpecloc).last_column,(SeExprSpecvsp[(1) - (1)].d))); }
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 382 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = 0;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 383 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = (SeExprSpecvsp[(1) - (1)].n);}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 388 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { 
       // ignore first argument unless it is a string (because we parse strings in weird ways)
       SeExprSpecListNode* list=new SeExprSpecListNode((SeExprSpecloc).last_column,(SeExprSpecloc).last_column);
       if((SeExprSpecvsp[(1) - (1)].n) && SPEC_IS_STR((SeExprSpecvsp[(1) - (1)].n))){
           list->add((SeExprSpecvsp[(1) - (1)].n));
       }
       remember(list);
       (SeExprSpecval.n)=list;
   }
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 397 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    {

      if((SeExprSpecvsp[(1) - (3)].n) && (SeExprSpecvsp[(3) - (3)].n) && ((SPEC_IS_NUMBER((SeExprSpecvsp[(3) - (3)].n)) || SPEC_IS_VECTOR((SeExprSpecvsp[(3) - (3)].n)) || SPEC_IS_STR((SeExprSpecvsp[(3) - (3)].n))))){
          (SeExprSpecval.n)=(SeExprSpecvsp[(1) - (3)].n);
          dynamic_cast<SeExprSpecListNode*>((SeExprSpecvsp[(1) - (3)].n))->add((SeExprSpecvsp[(3) - (3)].n));
      }else{
          (SeExprSpecval.n)=0;
      }
    }
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 409 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprSpecval.n) = (SeExprSpecvsp[(1) - (1)].n);}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 410 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"
    { 
        SeExprSpecStringNode* str=new SeExprSpecStringNode((SeExprSpecloc).first_column,(SeExprSpecloc).last_column,(SeExprSpecvsp[(1) - (1)].s));
        //registerEditable("<UNKNOWN>",str);
        // TODO: move string stuff out
        (SeExprSpecval.n) = remember(str);
      }
    break;



/* Line 1455 of yacc.c  */
#line 2312 "y.tab.c"
      default: break;
    }
  SeExprSpecYY_SYMBOL_PRINT ("-> $$ =", SeExprSpecr1[SeExprSpecn], &SeExprSpecval, &SeExprSpecloc);

  SeExprSpecYYPOPSTACK (SeExprSpeclen);
  SeExprSpeclen = 0;
  SeExprSpecYY_STACK_PRINT (SeExprSpecss, SeExprSpecssp);

  *++SeExprSpecvsp = SeExprSpecval;
  *++SeExprSpeclsp = SeExprSpecloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  SeExprSpecn = SeExprSpecr1[SeExprSpecn];

  SeExprSpecstate = SeExprSpecpgoto[SeExprSpecn - SeExprSpecYYNTOKENS] + *SeExprSpecssp;
  if (0 <= SeExprSpecstate && SeExprSpecstate <= SeExprSpecYYLAST && SeExprSpeccheck[SeExprSpecstate] == *SeExprSpecssp)
    SeExprSpecstate = SeExprSpectable[SeExprSpecstate];
  else
    SeExprSpecstate = SeExprSpecdefgoto[SeExprSpecn - SeExprSpecYYNTOKENS];

  goto SeExprSpecnewstate;


/*------------------------------------.
| SeExprSpecerrlab -- here on detecting error |
`------------------------------------*/
SeExprSpecerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!SeExprSpecerrstatus)
    {
      ++SeExprSpecnerrs;
#if ! SeExprSpecYYERROR_VERBOSE
      SeExprSpecerror (SeExprSpecYY_("syntax error"));
#else
      {
	SeExprSpecYYSIZE_T SeExprSpecsize = SeExprSpecsyntax_error (0, SeExprSpecstate, SeExprSpecchar);
	if (SeExprSpecmsg_alloc < SeExprSpecsize && SeExprSpecmsg_alloc < SeExprSpecYYSTACK_ALLOC_MAXIMUM)
	  {
	    SeExprSpecYYSIZE_T SeExprSpecalloc = 2 * SeExprSpecsize;
	    if (! (SeExprSpecsize <= SeExprSpecalloc && SeExprSpecalloc <= SeExprSpecYYSTACK_ALLOC_MAXIMUM))
	      SeExprSpecalloc = SeExprSpecYYSTACK_ALLOC_MAXIMUM;
	    if (SeExprSpecmsg != SeExprSpecmsgbuf)
	      SeExprSpecYYSTACK_FREE (SeExprSpecmsg);
	    SeExprSpecmsg = (char *) SeExprSpecYYSTACK_ALLOC (SeExprSpecalloc);
	    if (SeExprSpecmsg)
	      SeExprSpecmsg_alloc = SeExprSpecalloc;
	    else
	      {
		SeExprSpecmsg = SeExprSpecmsgbuf;
		SeExprSpecmsg_alloc = sizeof SeExprSpecmsgbuf;
	      }
	  }

	if (0 < SeExprSpecsize && SeExprSpecsize <= SeExprSpecmsg_alloc)
	  {
	    (void) SeExprSpecsyntax_error (SeExprSpecmsg, SeExprSpecstate, SeExprSpecchar);
	    SeExprSpecerror (SeExprSpecmsg);
	  }
	else
	  {
	    SeExprSpecerror (SeExprSpecYY_("syntax error"));
	    if (SeExprSpecsize != 0)
	      goto SeExprSpecexhaustedlab;
	  }
      }
#endif
    }

  SeExprSpecerror_range[0] = SeExprSpeclloc;

  if (SeExprSpecerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (SeExprSpecchar <= SeExprSpecYYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (SeExprSpecchar == SeExprSpecYYEOF)
	    SeExprSpecYYABORT;
	}
      else
	{
	  SeExprSpecdestruct ("Error: discarding",
		      SeExprSpectoken, &SeExprSpeclval, &SeExprSpeclloc);
	  SeExprSpecchar = SeExprSpecYYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto SeExprSpecerrlab1;


/*---------------------------------------------------.
| SeExprSpecerrorlab -- error raised explicitly by SeExprSpecYYERROR.  |
`---------------------------------------------------*/
SeExprSpecerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     SeExprSpecYYERROR and the label SeExprSpecerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto SeExprSpecerrorlab;

  SeExprSpecerror_range[0] = SeExprSpeclsp[1-SeExprSpeclen];
  /* Do not reclaim the symbols of the rule which action triggered
     this SeExprSpecYYERROR.  */
  SeExprSpecYYPOPSTACK (SeExprSpeclen);
  SeExprSpeclen = 0;
  SeExprSpecYY_STACK_PRINT (SeExprSpecss, SeExprSpecssp);
  SeExprSpecstate = *SeExprSpecssp;
  goto SeExprSpecerrlab1;


/*-------------------------------------------------------------.
| SeExprSpecerrlab1 -- common code for both syntax error and SeExprSpecYYERROR.  |
`-------------------------------------------------------------*/
SeExprSpecerrlab1:
  SeExprSpecerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      SeExprSpecn = SeExprSpecpact[SeExprSpecstate];
      if (SeExprSpecn != SeExprSpecYYPACT_NINF)
	{
	  SeExprSpecn += SeExprSpecYYTERROR;
	  if (0 <= SeExprSpecn && SeExprSpecn <= SeExprSpecYYLAST && SeExprSpeccheck[SeExprSpecn] == SeExprSpecYYTERROR)
	    {
	      SeExprSpecn = SeExprSpectable[SeExprSpecn];
	      if (0 < SeExprSpecn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (SeExprSpecssp == SeExprSpecss)
	SeExprSpecYYABORT;

      SeExprSpecerror_range[0] = *SeExprSpeclsp;
      SeExprSpecdestruct ("Error: popping",
		  SeExprSpecstos[SeExprSpecstate], SeExprSpecvsp, SeExprSpeclsp);
      SeExprSpecYYPOPSTACK (1);
      SeExprSpecstate = *SeExprSpecssp;
      SeExprSpecYY_STACK_PRINT (SeExprSpecss, SeExprSpecssp);
    }

  *++SeExprSpecvsp = SeExprSpeclval;

  SeExprSpecerror_range[1] = SeExprSpeclloc;
  /* Using SeExprSpecYYLLOC is tempting, but would change the location of
     the lookahead.  SeExprSpecYYLOC is available though.  */
  SeExprSpecYYLLOC_DEFAULT (SeExprSpecloc, (SeExprSpecerror_range - 1), 2);
  *++SeExprSpeclsp = SeExprSpecloc;

  /* Shift the error token.  */
  SeExprSpecYY_SYMBOL_PRINT ("Shifting", SeExprSpecstos[SeExprSpecn], SeExprSpecvsp, SeExprSpeclsp);

  SeExprSpecstate = SeExprSpecn;
  goto SeExprSpecnewstate;


/*-------------------------------------.
| SeExprSpecacceptlab -- SeExprSpecYYACCEPT comes here.  |
`-------------------------------------*/
SeExprSpecacceptlab:
  SeExprSpecresult = 0;
  goto SeExprSpecreturn;

/*-----------------------------------.
| SeExprSpecabortlab -- SeExprSpecYYABORT comes here.  |
`-----------------------------------*/
SeExprSpecabortlab:
  SeExprSpecresult = 1;
  goto SeExprSpecreturn;

#if !defined(SeExprSpecoverflow) || SeExprSpecYYERROR_VERBOSE
/*-------------------------------------------------.
| SeExprSpecexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
SeExprSpecexhaustedlab:
  SeExprSpecerror (SeExprSpecYY_("memory exhausted"));
  SeExprSpecresult = 2;
  /* Fall through.  */
#endif

SeExprSpecreturn:
  if (SeExprSpecchar != SeExprSpecYYEMPTY)
     SeExprSpecdestruct ("Cleanup: discarding lookahead",
		 SeExprSpectoken, &SeExprSpeclval, &SeExprSpeclloc);
  /* Do not reclaim the symbols of the rule which action triggered
     this SeExprSpecYYABORT or SeExprSpecYYACCEPT.  */
  SeExprSpecYYPOPSTACK (SeExprSpeclen);
  SeExprSpecYY_STACK_PRINT (SeExprSpecss, SeExprSpecssp);
  while (SeExprSpecssp != SeExprSpecss)
    {
      SeExprSpecdestruct ("Cleanup: popping",
		  SeExprSpecstos[*SeExprSpecssp], SeExprSpecvsp, SeExprSpeclsp);
      SeExprSpecYYPOPSTACK (1);
    }
#ifndef SeExprSpecoverflow
  if (SeExprSpecss != SeExprSpecssa)
    SeExprSpecYYSTACK_FREE (SeExprSpecss);
#endif
#if SeExprSpecYYERROR_VERBOSE
  if (SeExprSpecmsg != SeExprSpecmsgbuf)
    SeExprSpecYYSTACK_FREE (SeExprSpecmsg);
#endif
  /* Make sure SeExprSpecYYID is used.  */
  return SeExprSpecYYID (SeExprSpecresult);
}



/* Line 1675 of yacc.c  */
#line 418 "/home/fahome/jberlin/projects/SeExpr.linux/src/SeExprEditor/SeExprSpecParser.y"


      /* SeExprSpecerror - Report an error.  This is called by the parser.
	 (Note: the "msg" param is useless as it is usually just "sparse error".
	 so it's ignored.)
      */
static void SeExprSpecerror(const char* /*msg*/)
{
    // find start of line containing error
    int pos = SeExprSpecpos(), lineno = 1, start = 0, end = strlen(ParseStr);
    bool multiline = 0;
    for (int i = start; i < pos; i++)
	if (ParseStr[i] == '\n') { start = i + 1; lineno++; multiline=1; }

    // find end of line containing error
    for (int i = end; i > pos; i--)
	if (ParseStr[i] == '\n') { end = i - 1; multiline=1; }

    ParseError = SeExprSpectext[0] ? "Syntax error" : "Unexpected end of expression";
    if (multiline) {
	char buff[30];
	snprintf(buff, 30, " at line %d", lineno);
	ParseError += buff;
    }
    if (SeExprSpectext[0]) {
	ParseError += " near '";
	ParseError += SeExprSpectext;
    }
    ParseError += "':\n    ";

    int s = std::max(start, pos-30);
    int e = std::min(end, pos+30);

    if (s != start) ParseError += "...";
    ParseError += std::string(ParseStr, s, e-s+1);
    if (e != end) ParseError += "...";
}

extern void resetCounters(std::vector<std::pair<int,int> >& comments);


/* CallParser - This is our entrypoint from the rest of the expr library. 
   A string is passed in and a parse tree is returned.	If the tree is null,
   an error string is returned.  Any flags set during parsing are passed
   along.
 */

//static Mutex mutex;

/// Main entry point to parser
bool SeExprParse(std::vector<SeExprEdEditable*>& outputEditables,
    std::vector<std::string>& outputVariables,
    std::vector<std::pair<int,int> >& comments,
    const char* str)
{
    // TODO: this needs a mutex!

    /// Make inputs/outputs accessible to parser actions
    editables=&outputEditables;
    variables=&outputVariables;
    ParseStr=str;

    // setup and startup parser
    resetCounters(comments); // reset lineNumber and columnNumber in scanner
    SeExprSpec_buffer_state* buffer = SeExprSpec_scan_string(str); // setup lexer
    ParseResult = 0;
    int resultCode = SeExprSpecparse(); // parser (don't care if it is a parse error)
    UNUSED(resultCode);
    SeExprSpec_delete_buffer(buffer);

    // delete temporary data -- specs(mini parse tree) and tokens(strings)!
    for(size_t i=0;i<specNodes.size();i++) delete specNodes[i];
    specNodes.clear();
    for(size_t i=0;i<tokens.size();i++) free(tokens[i]);
    tokens.clear();
    return true;
}


