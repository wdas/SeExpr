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

/* All symbols defined below should begin with ExprSpec or ExprSpecYY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define ExprSpecYYBISON 1

/* Bison version.  */
#define ExprSpecYYBISON_VERSION "2.7"

/* Skeleton name.  */
#define ExprSpecYYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define ExprSpecYYPURE 0

/* Push parsers.  */
#define ExprSpecYYPUSH 0

/* Pull parsers.  */
#define ExprSpecYYPULL 1


/* Substitute the variable and function names.  */
#define ExprSpecparse         ExprSpecparse
#define ExprSpeclex           ExprSpeclex
#define ExprSpecerror         ExprSpecerror
#define ExprSpeclval          ExprSpeclval
#define ExprSpecchar          ExprSpecchar
#define ExprSpecdebug         ExprSpecdebug
#define ExprSpecnerrs         ExprSpecnerrs
#define ExprSpeclloc          ExprSpeclloc

/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 18 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"

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
#include <SeExpr2/Platform.h>
#include <SeExpr2/Mutex.h>
#include "ExprSpecType.h"
#include "Editable.h"
#include "ExprDeepWater.h"


/******************
 lexer declarations
 ******************/


#define SPEC_IS_NUMBER(x) \
    (dynamic_cast<ExprSpecScalarNode*>(x) != 0)
#define SPEC_IS_VECTOR(x) \
    (dynamic_cast<ExprSpecVectorNode*>(x) != 0)
#define SPEC_IS_STR(x) \
    (dynamic_cast<ExprSpecStringNode*>(x) != 0)

// declarations of functions and data in ExprParser.y
int ExprSpeclex();
int ExprSpecpos();
extern int ExprSpec_start;
extern char* ExprSpectext;
struct ExprSpec_buffer_state;
ExprSpec_buffer_state* ExprSpec_scan_string(const char *str);
void ExprSpec_delete_buffer(ExprSpec_buffer_state*);

//#####################################
// Keep track of mini parse tree nodes

// temporary to the parse... all pointers deleted at end of parse
static std::vector<ExprSpecNode*> specNodes;
/// Remember the spec node, so we can delete it later
static ExprSpecNode* remember(ExprSpecNode* node)
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
static std::vector<Editable*>* editables;
static std::vector<std::string>* variables;

static const char* ParseStr;    // string being parsed
static std::string ParseError;  // error (set from ExprSpecerror)
static ExprSpecNode* ParseResult; // must set result here since ExprSpecparse can't return it


//######################################################################
// Helpers used by actions to register data


/// Remember that there is an assignment to this variable (For autocomplete)
static void specRegisterVariable(const char* var)
{
    variables->push_back(var);
}

/// Variable Assignment/String literal should be turned into an editable
/// an editable is the data part of a control (it's model essentially)
static void specRegisterEditable(const char* var,ExprSpecNode* node)
{
    //std::cerr<<"we have editable var "<<var<<std::endl;
    if(!node){
        //std::cerr<<"   null ptr "<<var<<std::endl;
    }else if(ExprSpecScalarNode* n=dynamic_cast<ExprSpecScalarNode*>(node)){
        editables->push_back(new NumberEditable(var,node->startPos,node->endPos,n->v));
    }else if(ExprSpecVectorNode* n=dynamic_cast<ExprSpecVectorNode*>(node)){
        editables->push_back(new VectorEditable(var,node->startPos,node->endPos,n->v));
    }else if(ExprSpecStringNode* n=dynamic_cast<ExprSpecStringNode*>(node)){
        editables->push_back(new StringEditable(node->startPos,node->endPos,n->v));
    }else if(ExprSpecCCurveNode* n=dynamic_cast<ExprSpecCCurveNode*>(node)){
        if(ExprSpecListNode* args=dynamic_cast<ExprSpecListNode*>(n->args)){
            if((args->nodes.size())%3==0){
                ColorCurveEditable* ccurve=new ColorCurveEditable(var,node->startPos,node->endPos);
                bool valid=true;
                for(size_t i=0;i<args->nodes.size();i+=3){
                    ExprSpecScalarNode* xnode=dynamic_cast<ExprSpecScalarNode*>(args->nodes[i]);
                    ExprSpecVectorNode* ynode=dynamic_cast<ExprSpecVectorNode*>(args->nodes[i+1]);
                    ExprSpecScalarNode* interpnode=dynamic_cast<ExprSpecScalarNode*>(args->nodes[i+2]);
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
    }else if(ExprSpecCurveNode* n=dynamic_cast<ExprSpecCurveNode*>(node)){
        if(ExprSpecListNode* args=dynamic_cast<ExprSpecListNode*>(n->args)){
            if((args->nodes.size())%3==0){
                CurveEditable* ccurve=new CurveEditable(var,node->startPos,node->endPos);
                bool valid=true;
                for(size_t i=0;i<args->nodes.size();i+=3){
                    ExprSpecScalarNode* xnode=dynamic_cast<ExprSpecScalarNode*>(args->nodes[i]);
                    ExprSpecScalarNode* ynode=dynamic_cast<ExprSpecScalarNode*>(args->nodes[i+1]);
                    ExprSpecScalarNode* interpnode=dynamic_cast<ExprSpecScalarNode*>(args->nodes[i+2]);
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
    }else if(ExprSpecColorSwatchNode* n=dynamic_cast<ExprSpecColorSwatchNode*>(node)){
        if(ExprSpecListNode* args=dynamic_cast<ExprSpecListNode*>(n->args)){
            if(args->nodes.size()>0){
                ColorSwatchEditable* swatch=new ColorSwatchEditable(var,node->startPos,node->endPos);
                bool valid=true;
                for(size_t i=0;i<args->nodes.size();i++){
                    ExprSpecVectorNode* colornode=dynamic_cast<ExprSpecVectorNode*>(args->nodes[i]);
                    if(colornode){
                        swatch->add(colornode->v);
                    }else{
                        valid=false;
                    }
                }
                if(valid) editables->push_back(swatch);
                else delete swatch;
            }
        }
    }else if(ExprSpecAnimCurveNode* n=dynamic_cast<ExprSpecAnimCurveNode*>(node)){
        if(ExprSpecListNode* args=dynamic_cast<ExprSpecListNode*>(n->args)){
            // need 3 items for pre inf and post inf and weighting, plus 9 items per key
            if((args->nodes.size()-4)%9==0){
                AnimCurveEditable* animCurve=new AnimCurveEditable(var,node->startPos,node->endPos);
                bool valid=true;


#ifdef SEEXPR_USE_ANIMLIB
                if(ExprSpecStringNode* s=dynamic_cast<ExprSpecStringNode*>(args->nodes[0])){
                    animCurve->curve.setPreInfinity(animlib::AnimCurve::stringToInfinityType(s->v));
                }else valid=false;
                if(ExprSpecStringNode* s=dynamic_cast<ExprSpecStringNode*>(args->nodes[1])){
                    animCurve->curve.setPostInfinity(animlib::AnimCurve::stringToInfinityType(s->v));
                }else valid=false;
                if(ExprSpecScalarNode* v=dynamic_cast<ExprSpecScalarNode*>(args->nodes[2])){
                    animCurve->curve.setWeighted(bool(v->v));
                }
                if(ExprSpecStringNode* v=dynamic_cast<ExprSpecStringNode*>(args->nodes[3])){
                    animCurve->link=v->v;
                }

                for(size_t i=4;i<args->nodes.size();i+=9){
                    ExprSpecScalarNode* xnode=dynamic_cast<ExprSpecScalarNode*>(args->nodes[i]);
                    ExprSpecScalarNode* ynode=dynamic_cast<ExprSpecScalarNode*>(args->nodes[i+1]);
                    ExprSpecScalarNode* inWeight=dynamic_cast<ExprSpecScalarNode*>(args->nodes[i+2]);
                    ExprSpecScalarNode* outWeight=dynamic_cast<ExprSpecScalarNode*>(args->nodes[i+3]);
                    ExprSpecScalarNode* inAngle=dynamic_cast<ExprSpecScalarNode*>(args->nodes[i+4]);
                    ExprSpecScalarNode* outAngle=dynamic_cast<ExprSpecScalarNode*>(args->nodes[i+5]);
                    ExprSpecStringNode* inTangType=dynamic_cast<ExprSpecStringNode*>(args->nodes[i+6]);
                    ExprSpecStringNode* outTangType=dynamic_cast<ExprSpecStringNode*>(args->nodes[i+7]);
                    ExprSpecScalarNode* weighted=dynamic_cast<ExprSpecScalarNode*>(args->nodes[i+8]);
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
    }else if(ExprSpecDeepWaterNode* n=dynamic_cast<ExprSpecDeepWaterNode*>(node)){
        if(ExprSpecListNode* args=dynamic_cast<ExprSpecListNode*>(n->args)){
            if(args->nodes.size()==12){
                DeepWaterEditable* deepWater=new DeepWaterEditable(var,node->startPos,node->endPos);
                bool valid=true;

                ExprSpecScalarNode* resolution=dynamic_cast<ExprSpecScalarNode*>(args->nodes[0]);
                ExprSpecScalarNode* tileSize=dynamic_cast<ExprSpecScalarNode*>(args->nodes[1]);
                ExprSpecScalarNode* lengthCutoff=dynamic_cast<ExprSpecScalarNode*>(args->nodes[2]);
                ExprSpecScalarNode* amplitude=dynamic_cast<ExprSpecScalarNode*>(args->nodes[3]);
                ExprSpecScalarNode* windAngle=dynamic_cast<ExprSpecScalarNode*>(args->nodes[4]);
                ExprSpecScalarNode* windSpeed=dynamic_cast<ExprSpecScalarNode*>(args->nodes[5]);
                ExprSpecScalarNode* directionalFactorExponent=dynamic_cast<ExprSpecScalarNode*>(args->nodes[6]);
                ExprSpecScalarNode* directionalReflectionDamping=dynamic_cast<ExprSpecScalarNode*>(args->nodes[7]);
                ExprSpecVectorNode* flowDirection=dynamic_cast<ExprSpecVectorNode*>(args->nodes[8]);
                ExprSpecScalarNode* sharpen=dynamic_cast<ExprSpecScalarNode*>(args->nodes[9]);
                ExprSpecScalarNode* time=dynamic_cast<ExprSpecScalarNode*>(args->nodes[10]);
                ExprSpecScalarNode* filterWidth=dynamic_cast<ExprSpecScalarNode*>(args->nodes[11]);
                if(resolution && tileSize && lengthCutoff && amplitude && windAngle && windSpeed && directionalFactorExponent && directionalReflectionDamping && flowDirection && sharpen && time && filterWidth){
                    deepWater->setParams(SeDeepWaterParams(resolution->v, tileSize->v, lengthCutoff->v, amplitude->v, windAngle->v, windSpeed->v, directionalFactorExponent->v, directionalReflectionDamping->v, flowDirection->v, sharpen->v, time->v, filterWidth->v));
                }else{
                    valid=false;
                }

                if(valid) editables->push_back(deepWater);
                else delete deepWater;
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
static void ExprSpecerror(const char* msg);


/* Line 371 of yacc.c  */
#line 329 "y.tab.c"

# ifndef ExprSpecYY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define ExprSpecYY_NULL nullptr
#  else
#   define ExprSpecYY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef ExprSpecYYERROR_VERBOSE
# undef ExprSpecYYERROR_VERBOSE
# define ExprSpecYYERROR_VERBOSE 1
#else
# define ExprSpecYYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef ExprSpecYY_EXPRSPEC_Y_TAB_H_INCLUDED
# define ExprSpecYY_EXPRSPEC_Y_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef ExprSpecYYDEBUG
# define ExprSpecYYDEBUG 0
#endif
#if ExprSpecYYDEBUG
extern int ExprSpecdebug;
#endif

/* Tokens.  */
#ifndef ExprSpecYYTOKENTYPE
# define ExprSpecYYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum ExprSpectokentype {
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



#if ! defined ExprSpecYYSTYPE && ! defined ExprSpecYYSTYPE_IS_DECLARED
typedef union ExprSpecYYSTYPE
{
/* Line 387 of yacc.c  */
#line 271 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"

    ExprSpecNode* n;
    double d;      // return value for number tokens
    char* s;       /* return value for name tokens.  Note: UNLIKE the regular parser, this is not strdup()'dthe string */


/* Line 387 of yacc.c  */
#line 423 "y.tab.c"
} ExprSpecYYSTYPE;
# define ExprSpecYYSTYPE_IS_TRIVIAL 1
# define ExprSpecstype ExprSpecYYSTYPE /* obsolescent; will be withdrawn */
# define ExprSpecYYSTYPE_IS_DECLARED 1
#endif

#if ! defined ExprSpecYYLTYPE && ! defined ExprSpecYYLTYPE_IS_DECLARED
typedef struct ExprSpecYYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} ExprSpecYYLTYPE;
# define ExprSpecltype ExprSpecYYLTYPE /* obsolescent; will be withdrawn */
# define ExprSpecYYLTYPE_IS_DECLARED 1
# define ExprSpecYYLTYPE_IS_TRIVIAL 1
#endif

extern ExprSpecYYSTYPE ExprSpeclval;
extern ExprSpecYYLTYPE ExprSpeclloc;
#ifdef ExprSpecYYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int ExprSpecparse (void *ExprSpecYYPARSE_PARAM);
#else
int ExprSpecparse ();
#endif
#else /* ! ExprSpecYYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int ExprSpecparse (void);
#else
int ExprSpecparse ();
#endif
#endif /* ! ExprSpecYYPARSE_PARAM */

#endif /* !ExprSpecYY_EXPRSPEC_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 464 "y.tab.c"

#ifdef short
# undef short
#endif

#ifdef ExprSpecYYTYPE_UINT8
typedef ExprSpecYYTYPE_UINT8 ExprSpectype_uint8;
#else
typedef unsigned char ExprSpectype_uint8;
#endif

#ifdef ExprSpecYYTYPE_INT8
typedef ExprSpecYYTYPE_INT8 ExprSpectype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char ExprSpectype_int8;
#else
typedef short int ExprSpectype_int8;
#endif

#ifdef ExprSpecYYTYPE_UINT16
typedef ExprSpecYYTYPE_UINT16 ExprSpectype_uint16;
#else
typedef unsigned short int ExprSpectype_uint16;
#endif

#ifdef ExprSpecYYTYPE_INT16
typedef ExprSpecYYTYPE_INT16 ExprSpectype_int16;
#else
typedef short int ExprSpectype_int16;
#endif

#ifndef ExprSpecYYSIZE_T
# ifdef __SIZE_TYPE__
#  define ExprSpecYYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define ExprSpecYYSIZE_T size_t
# elif ! defined ExprSpecYYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define ExprSpecYYSIZE_T size_t
# else
#  define ExprSpecYYSIZE_T unsigned int
# endif
#endif

#define ExprSpecYYSIZE_MAXIMUM ((ExprSpecYYSIZE_T) -1)

#ifndef ExprSpecYY_
# if defined ExprSpecYYENABLE_NLS && ExprSpecYYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define ExprSpecYY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef ExprSpecYY_
#  define ExprSpecYY_(Msgid) Msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define ExprSpecYYUSE(E) ((void) (E))
#else
# define ExprSpecYYUSE(E) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define ExprSpecYYID(N) (N)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
ExprSpecYYID (int ExprSpeci)
#else
static int
ExprSpecYYID (ExprSpeci)
    int ExprSpeci;
#endif
{
  return ExprSpeci;
}
#endif

#if ! defined ExprSpecoverflow || ExprSpecYYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef ExprSpecYYSTACK_USE_ALLOCA
#  if ExprSpecYYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define ExprSpecYYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define ExprSpecYYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define ExprSpecYYSTACK_ALLOC alloca
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

# ifdef ExprSpecYYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define ExprSpecYYSTACK_FREE(Ptr) do { /* empty */; } while (ExprSpecYYID (0))
#  ifndef ExprSpecYYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define ExprSpecYYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define ExprSpecYYSTACK_ALLOC ExprSpecYYMALLOC
#  define ExprSpecYYSTACK_FREE ExprSpecYYFREE
#  ifndef ExprSpecYYSTACK_ALLOC_MAXIMUM
#   define ExprSpecYYSTACK_ALLOC_MAXIMUM ExprSpecYYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined ExprSpecYYMALLOC || defined malloc) \
	     && (defined ExprSpecYYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef ExprSpecYYMALLOC
#   define ExprSpecYYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (ExprSpecYYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef ExprSpecYYFREE
#   define ExprSpecYYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined ExprSpecoverflow || ExprSpecYYERROR_VERBOSE */


#if (! defined ExprSpecoverflow \
     && (! defined __cplusplus \
	 || (defined ExprSpecYYLTYPE_IS_TRIVIAL && ExprSpecYYLTYPE_IS_TRIVIAL \
	     && defined ExprSpecYYSTYPE_IS_TRIVIAL && ExprSpecYYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union ExprSpecalloc
{
  ExprSpectype_int16 ExprSpecss_alloc;
  ExprSpecYYSTYPE ExprSpecvs_alloc;
  ExprSpecYYLTYPE ExprSpecls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define ExprSpecYYSTACK_GAP_MAXIMUM (sizeof (union ExprSpecalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define ExprSpecYYSTACK_BYTES(N) \
     ((N) * (sizeof (ExprSpectype_int16) + sizeof (ExprSpecYYSTYPE) + sizeof (ExprSpecYYLTYPE)) \
      + 2 * ExprSpecYYSTACK_GAP_MAXIMUM)

# define ExprSpecYYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables ExprSpecYYSIZE and ExprSpecYYSTACKSIZE give the old and new number of
   elements in the stack, and ExprSpecYYPTR gives the new location of the
   stack.  Advance ExprSpecYYPTR to a properly aligned location for the next
   stack.  */
# define ExprSpecYYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	ExprSpecYYSIZE_T ExprSpecnewbytes;						\
	ExprSpecYYCOPY (&ExprSpecptr->Stack_alloc, Stack, ExprSpecsize);			\
	Stack = &ExprSpecptr->Stack_alloc;					\
	ExprSpecnewbytes = ExprSpecstacksize * sizeof (*Stack) + ExprSpecYYSTACK_GAP_MAXIMUM; \
	ExprSpecptr += ExprSpecnewbytes / sizeof (*ExprSpecptr);				\
      }									\
    while (ExprSpecYYID (0))

#endif

#if defined ExprSpecYYCOPY_NEEDED && ExprSpecYYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef ExprSpecYYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define ExprSpecYYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define ExprSpecYYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          ExprSpecYYSIZE_T ExprSpeci;                         \
          for (ExprSpeci = 0; ExprSpeci < (Count); ExprSpeci++)   \
            (Dst)[ExprSpeci] = (Src)[ExprSpeci];            \
        }                                       \
      while (ExprSpecYYID (0))
#  endif
# endif
#endif /* !ExprSpecYYCOPY_NEEDED */

/* ExprSpecYYFINAL -- State number of the termination state.  */
#define ExprSpecYYFINAL  40
/* ExprSpecYYLAST -- Last index in ExprSpecYYTABLE.  */
#define ExprSpecYYLAST   693

/* ExprSpecYYNTOKENS -- Number of terminals.  */
#define ExprSpecYYNTOKENS  44
/* ExprSpecYYNNTS -- Number of nonterminals.  */
#define ExprSpecYYNNTS  11
/* ExprSpecYYNRULES -- Number of rules.  */
#define ExprSpecYYNRULES  59
/* ExprSpecYYNRULES -- Number of states.  */
#define ExprSpecYYNSTATES  139

/* ExprSpecYYTRANSLATE(ExprSpecYYLEX) -- Bison symbol number corresponding to ExprSpecYYLEX.  */
#define ExprSpecYYUNDEFTOK  2
#define ExprSpecYYMAXUTOK   277

#define ExprSpecYYTRANSLATE(ExprSpecYYX)						\
  ((unsigned int) (ExprSpecYYX) <= ExprSpecYYMAXUTOK ? ExprSpectranslate[ExprSpecYYX] : ExprSpecYYUNDEFTOK)

/* ExprSpecYYTRANSLATE[ExprSpecYYLEX] -- Bison symbol number corresponding to ExprSpecYYLEX.  */
static const ExprSpectype_uint8 ExprSpectranslate[] =
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

#if ExprSpecYYDEBUG
/* ExprSpecYYPRHS[ExprSpecYYN] -- Index of the first RHS symbol of rule number ExprSpecYYN in
   ExprSpecYYRHS.  */
static const ExprSpectype_uint8 ExprSpecprhs[] =
{
       0,     0,     3,     6,     8,     9,    11,    13,    16,    18,
      23,    28,    33,    38,    43,    48,    53,    58,    63,    68,
      73,    78,    83,    88,    97,    98,   103,   106,   110,   118,
     123,   129,   133,   137,   141,   145,   149,   153,   157,   161,
     164,   167,   170,   173,   177,   181,   185,   189,   193,   197,
     202,   209,   211,   213,   215,   216,   218,   220,   224,   226
};

/* ExprSpecYYRHS -- A `-1'-separated list of the rules' RHS.  */
static const ExprSpectype_int8 ExprSpecrhs[] =
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

/* ExprSpecYYRLINE[ExprSpecYYN] -- source line where rule number ExprSpecYYN was defined.  */
static const ExprSpectype_uint16 ExprSpecrline[] =
{
       0,   313,   313,   314,   319,   320,   324,   325,   330,   331,
     335,   336,   337,   338,   339,   340,   341,   345,   346,   347,
     348,   349,   350,   354,   359,   360,   361,   366,   367,   372,
     373,   374,   375,   376,   377,   378,   379,   380,   381,   382,
     383,   392,   393,   394,   395,   396,   397,   398,   399,   400,
     425,   426,   427,   428,   433,   434,   439,   448,   460,   461
};
#endif

#if ExprSpecYYDEBUG || ExprSpecYYERROR_VERBOSE || 0
/* ExprSpecYYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at ExprSpecYYNTOKENS, nonterminals.  */
static const char *const ExprSpectname[] =
{
  "$end", "error", "$undefined", "IF", "ELSE", "NAME", "VAR", "STR",
  "NUMBER", "AddEq", "SubEq", "MultEq", "DivEq", "ExpEq", "ModEq", "'('",
  "')'", "ARROW", "':'", "'?'", "OR", "AND", "NE", "EQ", "'<'", "'>'",
  "GE", "LE", "'+'", "'-'", "'*'", "'/'", "'%'", "'!'", "'~'", "UNARY",
  "'^'", "'['", "'='", "';'", "'{'", "'}'", "','", "']'", "$accept",
  "expr", "optassigns", "assigns", "assign", "ifthenelse", "optelse", "e",
  "optargs", "args", "arg", ExprSpecYY_NULL
};
#endif

# ifdef ExprSpecYYPRINT
/* ExprSpecYYTOKNUM[ExprSpecYYLEX-NUM] -- Internal token number corresponding to
   token ExprSpecYYLEX-NUM.  */
static const ExprSpectype_uint16 ExprSpectoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,    40,    41,   270,    58,    63,
     271,   272,   273,   274,    60,    62,   275,   276,    43,    45,
      42,    47,    37,    33,   126,   277,    94,    91,    61,    59,
     123,   125,    44,    93
};
# endif

/* ExprSpecYYR1[ExprSpecYYN] -- Symbol number of symbol that rule ExprSpecYYN derives.  */
static const ExprSpectype_uint8 ExprSpecr1[] =
{
       0,    44,    45,    45,    46,    46,    47,    47,    48,    48,
      48,    48,    48,    48,    48,    48,    48,    48,    48,    48,
      48,    48,    48,    49,    50,    50,    50,    51,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    51,
      51,    51,    51,    51,    52,    52,    53,    53,    54,    54
};

/* ExprSpecYYR2[ExprSpecYYN] -- Number of symbols composing right hand side of rule ExprSpecYYN.  */
static const ExprSpectype_uint8 ExprSpecr2[] =
{
       0,     2,     2,     1,     0,     1,     1,     2,     1,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     8,     0,     4,     2,     3,     7,     4,
       5,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       2,     2,     2,     3,     3,     3,     3,     3,     3,     4,
       6,     1,     1,     1,     0,     1,     1,     3,     1,     1
};

/* ExprSpecYYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when ExprSpecYYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const ExprSpectype_uint8 ExprSpecdefact[] =
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

/* ExprSpecYYDEFGOTO[NTERM-NUM].  */
static const ExprSpectype_int16 ExprSpecdefgoto[] =
{
      -1,    11,   127,   128,    13,    14,   134,    68,    69,    70,
      71
};

/* ExprSpecYYPACT[STATE-NUM] -- Index in ExprSpecYYTABLE of the portion describing
   STATE-NUM.  */
#define ExprSpecYYPACT_NINF -65
static const ExprSpectype_int16 ExprSpecpact[] =
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

/* ExprSpecYYPGOTO[NTERM-NUM].  */
static const ExprSpectype_int8 ExprSpecpgoto[] =
{
     -65,   -65,   -64,    77,   -11,   -55,   -65,     0,   -38,   -65,
     -27
};

/* ExprSpecYYTABLE[ExprSpecYYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If ExprSpecYYTABLE_NINF, syntax error.  */
#define ExprSpecYYTABLE_NINF -1
static const ExprSpectype_uint8 ExprSpectable[] =
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

#define ExprSpecpact_value_is_default(Yystate) \
  (!!((Yystate) == (-65)))

#define ExprSpectable_value_is_error(Yytable_value) \
  ExprSpecYYID (0)

static const ExprSpectype_int16 ExprSpeccheck[] =
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

/* ExprSpecYYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const ExprSpectype_uint8 ExprSpecstos[] =
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

#define ExprSpecerrok		(ExprSpecerrstatus = 0)
#define ExprSpecclearin	(ExprSpecchar = ExprSpecYYEMPTY)
#define ExprSpecYYEMPTY		(-2)
#define ExprSpecYYEOF		0

#define ExprSpecYYACCEPT	goto ExprSpecacceptlab
#define ExprSpecYYABORT		goto ExprSpecabortlab
#define ExprSpecYYERROR		goto ExprSpecerrorlab


/* Like ExprSpecYYERROR except do call ExprSpecerror.  This remains here temporarily
   to ease the transition to the new meaning of ExprSpecYYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   ExprSpecYYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define ExprSpecYYFAIL		goto ExprSpecerrlab
#if defined ExprSpecYYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     ExprSpecYYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define ExprSpecYYRECOVERING()  (!!ExprSpecerrstatus)

#define ExprSpecYYBACKUP(Token, Value)                                  \
do                                                              \
  if (ExprSpecchar == ExprSpecYYEMPTY)                                        \
    {                                                           \
      ExprSpecchar = (Token);                                         \
      ExprSpeclval = (Value);                                         \
      ExprSpecYYPOPSTACK (ExprSpeclen);                                       \
      ExprSpecstate = *ExprSpecssp;                                         \
      goto ExprSpecbackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      ExprSpecerror (ExprSpecYY_("syntax error: cannot back up")); \
      ExprSpecYYERROR;							\
    }								\
while (ExprSpecYYID (0))

/* Error token number */
#define ExprSpecYYTERROR	1
#define ExprSpecYYERRCODE	256


/* ExprSpecYYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef ExprSpecYYLLOC_DEFAULT
# define ExprSpecYYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (ExprSpecYYID (N))                                                     \
        {                                                               \
          (Current).first_line   = ExprSpecYYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = ExprSpecYYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = ExprSpecYYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = ExprSpecYYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            ExprSpecYYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            ExprSpecYYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (ExprSpecYYID (0))
#endif

#define ExprSpecYYRHSLOC(Rhs, K) ((Rhs)[K])


/* ExprSpecYY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef ExprSpecYY_LOCATION_PRINT
# if defined ExprSpecYYLTYPE_IS_TRIVIAL && ExprSpecYYLTYPE_IS_TRIVIAL

/* Print *ExprSpecYYLOCP on ExprSpecYYO.  Private, do not rely on its existence. */

__attribute__((__unused__))
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static unsigned
ExprSpec_location_print_ (FILE *ExprSpeco, ExprSpecYYLTYPE const * const ExprSpeclocp)
#else
static unsigned
ExprSpec_location_print_ (ExprSpeco, ExprSpeclocp)
    FILE *ExprSpeco;
    ExprSpecYYLTYPE const * const ExprSpeclocp;
#endif
{
  unsigned res = 0;
  int end_col = 0 != ExprSpeclocp->last_column ? ExprSpeclocp->last_column - 1 : 0;
  if (0 <= ExprSpeclocp->first_line)
    {
      res += fprintf (ExprSpeco, "%d", ExprSpeclocp->first_line);
      if (0 <= ExprSpeclocp->first_column)
        res += fprintf (ExprSpeco, ".%d", ExprSpeclocp->first_column);
    }
  if (0 <= ExprSpeclocp->last_line)
    {
      if (ExprSpeclocp->first_line < ExprSpeclocp->last_line)
        {
          res += fprintf (ExprSpeco, "-%d", ExprSpeclocp->last_line);
          if (0 <= end_col)
            res += fprintf (ExprSpeco, ".%d", end_col);
        }
      else if (0 <= end_col && ExprSpeclocp->first_column < end_col)
        res += fprintf (ExprSpeco, "-%d", end_col);
    }
  return res;
 }

#  define ExprSpecYY_LOCATION_PRINT(File, Loc)          \
  ExprSpec_location_print_ (File, &(Loc))

# else
#  define ExprSpecYY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* ExprSpecYYLEX -- calling `ExprSpeclex' with the right arguments.  */
#ifdef ExprSpecYYLEX_PARAM
# define ExprSpecYYLEX ExprSpeclex (ExprSpecYYLEX_PARAM)
#else
# define ExprSpecYYLEX ExprSpeclex ()
#endif

/* Enable debugging if requested.  */
#if ExprSpecYYDEBUG

# ifndef ExprSpecYYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define ExprSpecYYFPRINTF fprintf
# endif

# define ExprSpecYYDPRINTF(Args)			\
do {						\
  if (ExprSpecdebug)					\
    ExprSpecYYFPRINTF Args;				\
} while (ExprSpecYYID (0))

# define ExprSpecYY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (ExprSpecdebug)								  \
    {									  \
      ExprSpecYYFPRINTF (stderr, "%s ", Title);					  \
      ExprSpec_symbol_print (stderr,						  \
		  Type, Value, Location); \
      ExprSpecYYFPRINTF (stderr, "\n");						  \
    }									  \
} while (ExprSpecYYID (0))


/*--------------------------------.
| Print this symbol on ExprSpecYYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
ExprSpec_symbol_value_print (FILE *ExprSpecoutput, int ExprSpectype, ExprSpecYYSTYPE const * const ExprSpecvaluep, ExprSpecYYLTYPE const * const ExprSpeclocationp)
#else
static void
ExprSpec_symbol_value_print (ExprSpecoutput, ExprSpectype, ExprSpecvaluep, ExprSpeclocationp)
    FILE *ExprSpecoutput;
    int ExprSpectype;
    ExprSpecYYSTYPE const * const ExprSpecvaluep;
    ExprSpecYYLTYPE const * const ExprSpeclocationp;
#endif
{
  FILE *ExprSpeco = ExprSpecoutput;
  ExprSpecYYUSE (ExprSpeco);
  if (!ExprSpecvaluep)
    return;
  ExprSpecYYUSE (ExprSpeclocationp);
# ifdef ExprSpecYYPRINT
  if (ExprSpectype < ExprSpecYYNTOKENS)
    ExprSpecYYPRINT (ExprSpecoutput, ExprSpectoknum[ExprSpectype], *ExprSpecvaluep);
# else
  ExprSpecYYUSE (ExprSpecoutput);
# endif
  switch (ExprSpectype)
    {
      default:
        break;
    }
}


/*--------------------------------.
| Print this symbol on ExprSpecYYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
ExprSpec_symbol_print (FILE *ExprSpecoutput, int ExprSpectype, ExprSpecYYSTYPE const * const ExprSpecvaluep, ExprSpecYYLTYPE const * const ExprSpeclocationp)
#else
static void
ExprSpec_symbol_print (ExprSpecoutput, ExprSpectype, ExprSpecvaluep, ExprSpeclocationp)
    FILE *ExprSpecoutput;
    int ExprSpectype;
    ExprSpecYYSTYPE const * const ExprSpecvaluep;
    ExprSpecYYLTYPE const * const ExprSpeclocationp;
#endif
{
  if (ExprSpectype < ExprSpecYYNTOKENS)
    ExprSpecYYFPRINTF (ExprSpecoutput, "token %s (", ExprSpectname[ExprSpectype]);
  else
    ExprSpecYYFPRINTF (ExprSpecoutput, "nterm %s (", ExprSpectname[ExprSpectype]);

  ExprSpecYY_LOCATION_PRINT (ExprSpecoutput, *ExprSpeclocationp);
  ExprSpecYYFPRINTF (ExprSpecoutput, ": ");
  ExprSpec_symbol_value_print (ExprSpecoutput, ExprSpectype, ExprSpecvaluep, ExprSpeclocationp);
  ExprSpecYYFPRINTF (ExprSpecoutput, ")");
}

/*------------------------------------------------------------------.
| ExprSpec_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
ExprSpec_stack_print (ExprSpectype_int16 *ExprSpecbottom, ExprSpectype_int16 *ExprSpectop)
#else
static void
ExprSpec_stack_print (ExprSpecbottom, ExprSpectop)
    ExprSpectype_int16 *ExprSpecbottom;
    ExprSpectype_int16 *ExprSpectop;
#endif
{
  ExprSpecYYFPRINTF (stderr, "Stack now");
  for (; ExprSpecbottom <= ExprSpectop; ExprSpecbottom++)
    {
      int ExprSpecbot = *ExprSpecbottom;
      ExprSpecYYFPRINTF (stderr, " %d", ExprSpecbot);
    }
  ExprSpecYYFPRINTF (stderr, "\n");
}

# define ExprSpecYY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (ExprSpecdebug)							\
    ExprSpec_stack_print ((Bottom), (Top));				\
} while (ExprSpecYYID (0))


/*------------------------------------------------.
| Report that the ExprSpecYYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
ExprSpec_reduce_print (ExprSpecYYSTYPE *ExprSpecvsp, ExprSpecYYLTYPE *ExprSpeclsp, int ExprSpecrule)
#else
static void
ExprSpec_reduce_print (ExprSpecvsp, ExprSpeclsp, ExprSpecrule)
    ExprSpecYYSTYPE *ExprSpecvsp;
    ExprSpecYYLTYPE *ExprSpeclsp;
    int ExprSpecrule;
#endif
{
  int ExprSpecnrhs = ExprSpecr2[ExprSpecrule];
  int ExprSpeci;
  unsigned long int ExprSpeclno = ExprSpecrline[ExprSpecrule];
  ExprSpecYYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     ExprSpecrule - 1, ExprSpeclno);
  /* The symbols being reduced.  */
  for (ExprSpeci = 0; ExprSpeci < ExprSpecnrhs; ExprSpeci++)
    {
      ExprSpecYYFPRINTF (stderr, "   $%d = ", ExprSpeci + 1);
      ExprSpec_symbol_print (stderr, ExprSpecrhs[ExprSpecprhs[ExprSpecrule] + ExprSpeci],
		       &(ExprSpecvsp[(ExprSpeci + 1) - (ExprSpecnrhs)])
		       , &(ExprSpeclsp[(ExprSpeci + 1) - (ExprSpecnrhs)])		       );
      ExprSpecYYFPRINTF (stderr, "\n");
    }
}

# define ExprSpecYY_REDUCE_PRINT(Rule)		\
do {					\
  if (ExprSpecdebug)				\
    ExprSpec_reduce_print (ExprSpecvsp, ExprSpeclsp, Rule); \
} while (ExprSpecYYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int ExprSpecdebug;
#else /* !ExprSpecYYDEBUG */
# define ExprSpecYYDPRINTF(Args)
# define ExprSpecYY_SYMBOL_PRINT(Title, Type, Value, Location)
# define ExprSpecYY_STACK_PRINT(Bottom, Top)
# define ExprSpecYY_REDUCE_PRINT(Rule)
#endif /* !ExprSpecYYDEBUG */


/* ExprSpecYYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	ExprSpecYYINITDEPTH
# define ExprSpecYYINITDEPTH 200
#endif

/* ExprSpecYYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   ExprSpecYYSTACK_ALLOC_MAXIMUM < ExprSpecYYSTACK_BYTES (ExprSpecYYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef ExprSpecYYMAXDEPTH
# define ExprSpecYYMAXDEPTH 10000
#endif


#if ExprSpecYYERROR_VERBOSE

# ifndef ExprSpecstrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define ExprSpecstrlen strlen
#  else
/* Return the length of ExprSpecYYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static ExprSpecYYSIZE_T
ExprSpecstrlen (const char *ExprSpecstr)
#else
static ExprSpecYYSIZE_T
ExprSpecstrlen (ExprSpecstr)
    const char *ExprSpecstr;
#endif
{
  ExprSpecYYSIZE_T ExprSpeclen;
  for (ExprSpeclen = 0; ExprSpecstr[ExprSpeclen]; ExprSpeclen++)
    continue;
  return ExprSpeclen;
}
#  endif
# endif

# ifndef ExprSpecstpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define ExprSpecstpcpy stpcpy
#  else
/* Copy ExprSpecYYSRC to ExprSpecYYDEST, returning the address of the terminating '\0' in
   ExprSpecYYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
ExprSpecstpcpy (char *ExprSpecdest, const char *ExprSpecsrc)
#else
static char *
ExprSpecstpcpy (ExprSpecdest, ExprSpecsrc)
    char *ExprSpecdest;
    const char *ExprSpecsrc;
#endif
{
  char *ExprSpecd = ExprSpecdest;
  const char *ExprSpecs = ExprSpecsrc;

  while ((*ExprSpecd++ = *ExprSpecs++) != '\0')
    continue;

  return ExprSpecd - 1;
}
#  endif
# endif

# ifndef ExprSpectnamerr
/* Copy to ExprSpecYYRES the contents of ExprSpecYYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for ExprSpecerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  ExprSpecYYSTR is taken from ExprSpectname.  If ExprSpecYYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static ExprSpecYYSIZE_T
ExprSpectnamerr (char *ExprSpecres, const char *ExprSpecstr)
{
  if (*ExprSpecstr == '"')
    {
      ExprSpecYYSIZE_T ExprSpecn = 0;
      char const *ExprSpecp = ExprSpecstr;

      for (;;)
	switch (*++ExprSpecp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++ExprSpecp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (ExprSpecres)
	      ExprSpecres[ExprSpecn] = *ExprSpecp;
	    ExprSpecn++;
	    break;

	  case '"':
	    if (ExprSpecres)
	      ExprSpecres[ExprSpecn] = '\0';
	    return ExprSpecn;
	  }
    do_not_strip_quotes: ;
    }

  if (! ExprSpecres)
    return ExprSpecstrlen (ExprSpecstr);

  return ExprSpecstpcpy (ExprSpecres, ExprSpecstr) - ExprSpecres;
}
# endif

/* Copy into *ExprSpecYYMSG, which is of size *ExprSpecYYMSG_ALLOC, an error message
   about the unexpected token ExprSpecYYTOKEN for the state stack whose top is
   ExprSpecYYSSP.

   Return 0 if *ExprSpecYYMSG was successfully written.  Return 1 if *ExprSpecYYMSG is
   not large enough to hold the message.  In that case, also set
   *ExprSpecYYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
ExprSpecsyntax_error (ExprSpecYYSIZE_T *ExprSpecmsg_alloc, char **ExprSpecmsg,
                ExprSpectype_int16 *ExprSpecssp, int ExprSpectoken)
{
  ExprSpecYYSIZE_T ExprSpecsize0 = ExprSpectnamerr (ExprSpecYY_NULL, ExprSpectname[ExprSpectoken]);
  ExprSpecYYSIZE_T ExprSpecsize = ExprSpecsize0;
  enum { ExprSpecYYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *ExprSpecformat = ExprSpecYY_NULL;
  /* Arguments of ExprSpecformat. */
  char const *ExprSpecarg[ExprSpecYYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int ExprSpeccount = 0;

  /* There are many possibilities here to consider:
     - Assume ExprSpecYYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  ExprSpecYYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in ExprSpecchar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated ExprSpecchar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (ExprSpectoken != ExprSpecYYEMPTY)
    {
      int ExprSpecn = ExprSpecpact[*ExprSpecssp];
      ExprSpecarg[ExprSpeccount++] = ExprSpectname[ExprSpectoken];
      if (!ExprSpecpact_value_is_default (ExprSpecn))
        {
          /* Start ExprSpecYYX at -ExprSpecYYN if negative to avoid negative indexes in
             ExprSpecYYCHECK.  In other words, skip the first -ExprSpecYYN actions for
             this state because they are default actions.  */
          int ExprSpecxbegin = ExprSpecn < 0 ? -ExprSpecn : 0;
          /* Stay within bounds of both ExprSpeccheck and ExprSpectname.  */
          int ExprSpecchecklim = ExprSpecYYLAST - ExprSpecn + 1;
          int ExprSpecxend = ExprSpecchecklim < ExprSpecYYNTOKENS ? ExprSpecchecklim : ExprSpecYYNTOKENS;
          int ExprSpecx;

          for (ExprSpecx = ExprSpecxbegin; ExprSpecx < ExprSpecxend; ++ExprSpecx)
            if (ExprSpeccheck[ExprSpecx + ExprSpecn] == ExprSpecx && ExprSpecx != ExprSpecYYTERROR
                && !ExprSpectable_value_is_error (ExprSpectable[ExprSpecx + ExprSpecn]))
              {
                if (ExprSpeccount == ExprSpecYYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    ExprSpeccount = 1;
                    ExprSpecsize = ExprSpecsize0;
                    break;
                  }
                ExprSpecarg[ExprSpeccount++] = ExprSpectname[ExprSpecx];
                {
                  ExprSpecYYSIZE_T ExprSpecsize1 = ExprSpecsize + ExprSpectnamerr (ExprSpecYY_NULL, ExprSpectname[ExprSpecx]);
                  if (! (ExprSpecsize <= ExprSpecsize1
                         && ExprSpecsize1 <= ExprSpecYYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  ExprSpecsize = ExprSpecsize1;
                }
              }
        }
    }

  switch (ExprSpeccount)
    {
# define ExprSpecYYCASE_(N, S)                      \
      case N:                               \
        ExprSpecformat = S;                       \
      break
      ExprSpecYYCASE_(0, ExprSpecYY_("syntax error"));
      ExprSpecYYCASE_(1, ExprSpecYY_("syntax error, unexpected %s"));
      ExprSpecYYCASE_(2, ExprSpecYY_("syntax error, unexpected %s, expecting %s"));
      ExprSpecYYCASE_(3, ExprSpecYY_("syntax error, unexpected %s, expecting %s or %s"));
      ExprSpecYYCASE_(4, ExprSpecYY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      ExprSpecYYCASE_(5, ExprSpecYY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef ExprSpecYYCASE_
    }

  {
    ExprSpecYYSIZE_T ExprSpecsize1 = ExprSpecsize + ExprSpecstrlen (ExprSpecformat);
    if (! (ExprSpecsize <= ExprSpecsize1 && ExprSpecsize1 <= ExprSpecYYSTACK_ALLOC_MAXIMUM))
      return 2;
    ExprSpecsize = ExprSpecsize1;
  }

  if (*ExprSpecmsg_alloc < ExprSpecsize)
    {
      *ExprSpecmsg_alloc = 2 * ExprSpecsize;
      if (! (ExprSpecsize <= *ExprSpecmsg_alloc
             && *ExprSpecmsg_alloc <= ExprSpecYYSTACK_ALLOC_MAXIMUM))
        *ExprSpecmsg_alloc = ExprSpecYYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *ExprSpecp = *ExprSpecmsg;
    int ExprSpeci = 0;
    while ((*ExprSpecp = *ExprSpecformat) != '\0')
      if (*ExprSpecp == '%' && ExprSpecformat[1] == 's' && ExprSpeci < ExprSpeccount)
        {
          ExprSpecp += ExprSpectnamerr (ExprSpecp, ExprSpecarg[ExprSpeci++]);
          ExprSpecformat += 2;
        }
      else
        {
          ExprSpecp++;
          ExprSpecformat++;
        }
  }
  return 0;
}
#endif /* ExprSpecYYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
ExprSpecdestruct (const char *ExprSpecmsg, int ExprSpectype, ExprSpecYYSTYPE *ExprSpecvaluep, ExprSpecYYLTYPE *ExprSpeclocationp)
#else
static void
ExprSpecdestruct (ExprSpecmsg, ExprSpectype, ExprSpecvaluep, ExprSpeclocationp)
    const char *ExprSpecmsg;
    int ExprSpectype;
    ExprSpecYYSTYPE *ExprSpecvaluep;
    ExprSpecYYLTYPE *ExprSpeclocationp;
#endif
{
  ExprSpecYYUSE (ExprSpecvaluep);
  ExprSpecYYUSE (ExprSpeclocationp);

  if (!ExprSpecmsg)
    ExprSpecmsg = "Deleting";
  ExprSpecYY_SYMBOL_PRINT (ExprSpecmsg, ExprSpectype, ExprSpecvaluep, ExprSpeclocationp);

  switch (ExprSpectype)
    {

      default:
        break;
    }
}




/* The lookahead symbol.  */
int ExprSpecchar;


#ifndef ExprSpecYY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define ExprSpecYY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define ExprSpecYY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef ExprSpecYY_INITIAL_VALUE
# define ExprSpecYY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
ExprSpecYYSTYPE ExprSpeclval ExprSpecYY_INITIAL_VALUE(ExprSpecval_default);

/* Location data for the lookahead symbol.  */
ExprSpecYYLTYPE ExprSpeclloc
# if defined ExprSpecYYLTYPE_IS_TRIVIAL && ExprSpecYYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;


/* Number of syntax errors so far.  */
int ExprSpecnerrs;


/*----------.
| ExprSpecparse.  |
`----------*/

#ifdef ExprSpecYYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
ExprSpecparse (void *ExprSpecYYPARSE_PARAM)
#else
int
ExprSpecparse (ExprSpecYYPARSE_PARAM)
    void *ExprSpecYYPARSE_PARAM;
#endif
#else /* ! ExprSpecYYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
ExprSpecparse (void)
#else
int
ExprSpecparse ()

#endif
#endif
{
    int ExprSpecstate;
    /* Number of tokens to shift before error messages enabled.  */
    int ExprSpecerrstatus;

    /* The stacks and their tools:
       `ExprSpecss': related to states.
       `ExprSpecvs': related to semantic values.
       `ExprSpecls': related to locations.

       Refer to the stacks through separate pointers, to allow ExprSpecoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    ExprSpectype_int16 ExprSpecssa[ExprSpecYYINITDEPTH];
    ExprSpectype_int16 *ExprSpecss;
    ExprSpectype_int16 *ExprSpecssp;

    /* The semantic value stack.  */
    ExprSpecYYSTYPE ExprSpecvsa[ExprSpecYYINITDEPTH];
    ExprSpecYYSTYPE *ExprSpecvs;
    ExprSpecYYSTYPE *ExprSpecvsp;

    /* The location stack.  */
    ExprSpecYYLTYPE ExprSpeclsa[ExprSpecYYINITDEPTH];
    ExprSpecYYLTYPE *ExprSpecls;
    ExprSpecYYLTYPE *ExprSpeclsp;

    /* The locations where the error started and ended.  */
    ExprSpecYYLTYPE ExprSpecerror_range[3];

    ExprSpecYYSIZE_T ExprSpecstacksize;

  int ExprSpecn;
  int ExprSpecresult;
  /* Lookahead token as an internal (translated) token number.  */
  int ExprSpectoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  ExprSpecYYSTYPE ExprSpecval;
  ExprSpecYYLTYPE ExprSpecloc;

#if ExprSpecYYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char ExprSpecmsgbuf[128];
  char *ExprSpecmsg = ExprSpecmsgbuf;
  ExprSpecYYSIZE_T ExprSpecmsg_alloc = sizeof ExprSpecmsgbuf;
#endif

#define ExprSpecYYPOPSTACK(N)   (ExprSpecvsp -= (N), ExprSpecssp -= (N), ExprSpeclsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int ExprSpeclen = 0;

  ExprSpecssp = ExprSpecss = ExprSpecssa;
  ExprSpecvsp = ExprSpecvs = ExprSpecvsa;
  ExprSpeclsp = ExprSpecls = ExprSpeclsa;
  ExprSpecstacksize = ExprSpecYYINITDEPTH;

  ExprSpecYYDPRINTF ((stderr, "Starting parse\n"));

  ExprSpecstate = 0;
  ExprSpecerrstatus = 0;
  ExprSpecnerrs = 0;
  ExprSpecchar = ExprSpecYYEMPTY; /* Cause a token to be read.  */
  ExprSpeclsp[0] = ExprSpeclloc;
  goto ExprSpecsetstate;

/*------------------------------------------------------------.
| ExprSpecnewstate -- Push a new state, which is found in ExprSpecstate.  |
`------------------------------------------------------------*/
 ExprSpecnewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  ExprSpecssp++;

 ExprSpecsetstate:
  *ExprSpecssp = ExprSpecstate;

  if (ExprSpecss + ExprSpecstacksize - 1 <= ExprSpecssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      ExprSpecYYSIZE_T ExprSpecsize = ExprSpecssp - ExprSpecss + 1;

#ifdef ExprSpecoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	ExprSpecYYSTYPE *ExprSpecvs1 = ExprSpecvs;
	ExprSpectype_int16 *ExprSpecss1 = ExprSpecss;
	ExprSpecYYLTYPE *ExprSpecls1 = ExprSpecls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if ExprSpecoverflow is a macro.  */
	ExprSpecoverflow (ExprSpecYY_("memory exhausted"),
		    &ExprSpecss1, ExprSpecsize * sizeof (*ExprSpecssp),
		    &ExprSpecvs1, ExprSpecsize * sizeof (*ExprSpecvsp),
		    &ExprSpecls1, ExprSpecsize * sizeof (*ExprSpeclsp),
		    &ExprSpecstacksize);

	ExprSpecls = ExprSpecls1;
	ExprSpecss = ExprSpecss1;
	ExprSpecvs = ExprSpecvs1;
      }
#else /* no ExprSpecoverflow */
# ifndef ExprSpecYYSTACK_RELOCATE
      goto ExprSpecexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (ExprSpecYYMAXDEPTH <= ExprSpecstacksize)
	goto ExprSpecexhaustedlab;
      ExprSpecstacksize *= 2;
      if (ExprSpecYYMAXDEPTH < ExprSpecstacksize)
	ExprSpecstacksize = ExprSpecYYMAXDEPTH;

      {
	ExprSpectype_int16 *ExprSpecss1 = ExprSpecss;
	union ExprSpecalloc *ExprSpecptr =
	  (union ExprSpecalloc *) ExprSpecYYSTACK_ALLOC (ExprSpecYYSTACK_BYTES (ExprSpecstacksize));
	if (! ExprSpecptr)
	  goto ExprSpecexhaustedlab;
	ExprSpecYYSTACK_RELOCATE (ExprSpecss_alloc, ExprSpecss);
	ExprSpecYYSTACK_RELOCATE (ExprSpecvs_alloc, ExprSpecvs);
	ExprSpecYYSTACK_RELOCATE (ExprSpecls_alloc, ExprSpecls);
#  undef ExprSpecYYSTACK_RELOCATE
	if (ExprSpecss1 != ExprSpecssa)
	  ExprSpecYYSTACK_FREE (ExprSpecss1);
      }
# endif
#endif /* no ExprSpecoverflow */

      ExprSpecssp = ExprSpecss + ExprSpecsize - 1;
      ExprSpecvsp = ExprSpecvs + ExprSpecsize - 1;
      ExprSpeclsp = ExprSpecls + ExprSpecsize - 1;

      ExprSpecYYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) ExprSpecstacksize));

      if (ExprSpecss + ExprSpecstacksize - 1 <= ExprSpecssp)
	ExprSpecYYABORT;
    }

  ExprSpecYYDPRINTF ((stderr, "Entering state %d\n", ExprSpecstate));

  if (ExprSpecstate == ExprSpecYYFINAL)
    ExprSpecYYACCEPT;

  goto ExprSpecbackup;

/*-----------.
| ExprSpecbackup.  |
`-----------*/
ExprSpecbackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  ExprSpecn = ExprSpecpact[ExprSpecstate];
  if (ExprSpecpact_value_is_default (ExprSpecn))
    goto ExprSpecdefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* ExprSpecYYCHAR is either ExprSpecYYEMPTY or ExprSpecYYEOF or a valid lookahead symbol.  */
  if (ExprSpecchar == ExprSpecYYEMPTY)
    {
      ExprSpecYYDPRINTF ((stderr, "Reading a token: "));
      ExprSpecchar = ExprSpecYYLEX;
    }

  if (ExprSpecchar <= ExprSpecYYEOF)
    {
      ExprSpecchar = ExprSpectoken = ExprSpecYYEOF;
      ExprSpecYYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      ExprSpectoken = ExprSpecYYTRANSLATE (ExprSpecchar);
      ExprSpecYY_SYMBOL_PRINT ("Next token is", ExprSpectoken, &ExprSpeclval, &ExprSpeclloc);
    }

  /* If the proper action on seeing token ExprSpecYYTOKEN is to reduce or to
     detect an error, take that action.  */
  ExprSpecn += ExprSpectoken;
  if (ExprSpecn < 0 || ExprSpecYYLAST < ExprSpecn || ExprSpeccheck[ExprSpecn] != ExprSpectoken)
    goto ExprSpecdefault;
  ExprSpecn = ExprSpectable[ExprSpecn];
  if (ExprSpecn <= 0)
    {
      if (ExprSpectable_value_is_error (ExprSpecn))
        goto ExprSpecerrlab;
      ExprSpecn = -ExprSpecn;
      goto ExprSpecreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (ExprSpecerrstatus)
    ExprSpecerrstatus--;

  /* Shift the lookahead token.  */
  ExprSpecYY_SYMBOL_PRINT ("Shifting", ExprSpectoken, &ExprSpeclval, &ExprSpeclloc);

  /* Discard the shifted token.  */
  ExprSpecchar = ExprSpecYYEMPTY;

  ExprSpecstate = ExprSpecn;
  ExprSpecYY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++ExprSpecvsp = ExprSpeclval;
  ExprSpecYY_IGNORE_MAYBE_UNINITIALIZED_END
  *++ExprSpeclsp = ExprSpeclloc;
  goto ExprSpecnewstate;


/*-----------------------------------------------------------.
| ExprSpecdefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
ExprSpecdefault:
  ExprSpecn = ExprSpecdefact[ExprSpecstate];
  if (ExprSpecn == 0)
    goto ExprSpecerrlab;
  goto ExprSpecreduce;


/*-----------------------------.
| ExprSpecreduce -- Do a reduction.  |
`-----------------------------*/
ExprSpecreduce:
  /* ExprSpecn is the number of a rule to reduce with.  */
  ExprSpeclen = ExprSpecr2[ExprSpecn];

  /* If ExprSpecYYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets ExprSpecYYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to ExprSpecYYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that ExprSpecYYVAL may be used uninitialized.  */
  ExprSpecval = ExprSpecvsp[1-ExprSpeclen];

  /* Default location.  */
  ExprSpecYYLLOC_DEFAULT (ExprSpecloc, (ExprSpeclsp - ExprSpeclen), ExprSpeclen);
  ExprSpecYY_REDUCE_PRINT (ExprSpecn);
  switch (ExprSpecn)
    {
        case 2:
/* Line 1792 of yacc.c  */
#line 313 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { ParseResult = 0; }
    break;

  case 3:
/* Line 1792 of yacc.c  */
#line 314 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { ParseResult = 0; }
    break;

  case 4:
/* Line 1792 of yacc.c  */
#line 319 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 5:
/* Line 1792 of yacc.c  */
#line 320 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 6:
/* Line 1792 of yacc.c  */
#line 324 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 7:
/* Line 1792 of yacc.c  */
#line 325 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 8:
/* Line 1792 of yacc.c  */
#line 330 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 9:
/* Line 1792 of yacc.c  */
#line 331 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    {
        specRegisterVariable((ExprSpecvsp[(1) - (4)].s));
        specRegisterEditable((ExprSpecvsp[(1) - (4)].s),(ExprSpecvsp[(3) - (4)].n));
      }
    break;

  case 10:
/* Line 1792 of yacc.c  */
#line 335 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 11:
/* Line 1792 of yacc.c  */
#line 336 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 12:
/* Line 1792 of yacc.c  */
#line 337 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 13:
/* Line 1792 of yacc.c  */
#line 338 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 14:
/* Line 1792 of yacc.c  */
#line 339 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 15:
/* Line 1792 of yacc.c  */
#line 340 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 16:
/* Line 1792 of yacc.c  */
#line 341 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    {
        specRegisterVariable((ExprSpecvsp[(1) - (4)].s));
        specRegisterEditable((ExprSpecvsp[(1) - (4)].s),(ExprSpecvsp[(3) - (4)].n));
      }
    break;

  case 17:
/* Line 1792 of yacc.c  */
#line 345 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    {  (ExprSpecval.n) = 0; }
    break;

  case 18:
/* Line 1792 of yacc.c  */
#line 346 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    {  (ExprSpecval.n) = 0; }
    break;

  case 19:
/* Line 1792 of yacc.c  */
#line 347 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    {  (ExprSpecval.n) = 0; }
    break;

  case 20:
/* Line 1792 of yacc.c  */
#line 348 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    {  (ExprSpecval.n) = 0; }
    break;

  case 21:
/* Line 1792 of yacc.c  */
#line 349 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    {  (ExprSpecval.n) = 0; }
    break;

  case 22:
/* Line 1792 of yacc.c  */
#line 350 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    {  (ExprSpecval.n) = 0; }
    break;

  case 23:
/* Line 1792 of yacc.c  */
#line 355 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 24:
/* Line 1792 of yacc.c  */
#line 359 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 25:
/* Line 1792 of yacc.c  */
#line 360 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0;}
    break;

  case 26:
/* Line 1792 of yacc.c  */
#line 361 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0;}
    break;

  case 27:
/* Line 1792 of yacc.c  */
#line 366 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 28:
/* Line 1792 of yacc.c  */
#line 367 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    {
        if(SPEC_IS_NUMBER((ExprSpecvsp[(2) - (7)].n)) && SPEC_IS_NUMBER((ExprSpecvsp[(4) - (7)].n)) && SPEC_IS_NUMBER((ExprSpecvsp[(6) - (7)].n))){
            (ExprSpecval.n)=remember(new ExprSpecVectorNode((ExprSpecloc).first_column,(ExprSpecloc).last_column,(ExprSpecvsp[(2) - (7)].n),(ExprSpecvsp[(4) - (7)].n),(ExprSpecvsp[(6) - (7)].n)));
        }else (ExprSpecval.n)=0;
      }
    break;

  case 29:
/* Line 1792 of yacc.c  */
#line 372 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 30:
/* Line 1792 of yacc.c  */
#line 373 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 31:
/* Line 1792 of yacc.c  */
#line 374 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 32:
/* Line 1792 of yacc.c  */
#line 375 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 33:
/* Line 1792 of yacc.c  */
#line 376 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 34:
/* Line 1792 of yacc.c  */
#line 377 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 35:
/* Line 1792 of yacc.c  */
#line 378 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 36:
/* Line 1792 of yacc.c  */
#line 379 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 37:
/* Line 1792 of yacc.c  */
#line 380 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 38:
/* Line 1792 of yacc.c  */
#line 381 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 39:
/* Line 1792 of yacc.c  */
#line 382 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = (ExprSpecvsp[(2) - (2)].n); }
    break;

  case 40:
/* Line 1792 of yacc.c  */
#line 383 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    {
        if(SPEC_IS_NUMBER((ExprSpecvsp[(2) - (2)].n))){
            ExprSpecScalarNode* node=(ExprSpecScalarNode*)(ExprSpecvsp[(2) - (2)].n);
            node->v*=-1;
            node->startPos=(ExprSpecloc).first_column;
            node->endPos=(ExprSpecloc).last_column;
            (ExprSpecval.n)=(ExprSpecvsp[(2) - (2)].n);
        }else (ExprSpecval.n)=0;
      }
    break;

  case 41:
/* Line 1792 of yacc.c  */
#line 392 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 42:
/* Line 1792 of yacc.c  */
#line 393 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 43:
/* Line 1792 of yacc.c  */
#line 394 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 44:
/* Line 1792 of yacc.c  */
#line 395 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 45:
/* Line 1792 of yacc.c  */
#line 396 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 46:
/* Line 1792 of yacc.c  */
#line 397 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 47:
/* Line 1792 of yacc.c  */
#line 398 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 48:
/* Line 1792 of yacc.c  */
#line 399 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
    break;

  case 49:
/* Line 1792 of yacc.c  */
#line 400 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    {
        if((ExprSpecvsp[(3) - (4)].n) && strcmp((ExprSpecvsp[(1) - (4)].s),"curve")==0){
            (ExprSpecval.n)=remember(new ExprSpecCurveNode((ExprSpecvsp[(3) - (4)].n)));
        }else if((ExprSpecvsp[(3) - (4)].n) && strcmp((ExprSpecvsp[(1) - (4)].s),"ccurve")==0){
            (ExprSpecval.n)=remember(new ExprSpecCCurveNode((ExprSpecvsp[(3) - (4)].n)));
        }else if((ExprSpecvsp[(3) - (4)].n) && strcmp((ExprSpecvsp[(1) - (4)].s),"swatch")==0){
            (ExprSpecval.n)=remember(new ExprSpecColorSwatchNode((ExprSpecvsp[(3) - (4)].n)));
        }else if((ExprSpecvsp[(3) - (4)].n) && strcmp((ExprSpecvsp[(1) - (4)].s),"animCurve")==0){
            (ExprSpecval.n)=remember(new ExprSpecAnimCurveNode((ExprSpecvsp[(3) - (4)].n)));
        }else if((ExprSpecvsp[(3) - (4)].n) && strcmp((ExprSpecvsp[(1) - (4)].s),"deepWater")==0){
            (ExprSpecval.n)=remember(new ExprSpecDeepWaterNode((ExprSpecvsp[(3) - (4)].n)));
        }else if((ExprSpecvsp[(3) - (4)].n)){
            // function arguments not parse of curve, ccurve, or animCurve
            // check if there are any string args that need to be made into controls
            // but be sure to return 0 as this parseable
            if(ExprSpecListNode* list=dynamic_cast<ExprSpecListNode*>((ExprSpecvsp[(3) - (4)].n))){
                for(size_t i=0;i<list->nodes.size();i++){
                    if(ExprSpecStringNode* str=dynamic_cast<ExprSpecStringNode*>(list->nodes[i])){
                        specRegisterEditable("<UNKNOWN>",str);
                    }
                }
            }
            (ExprSpecval.n)=0;
        }else (ExprSpecval.n)=0;
      }
    break;

  case 50:
/* Line 1792 of yacc.c  */
#line 425 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    {(ExprSpecval.n) = 0; }
    break;

  case 51:
/* Line 1792 of yacc.c  */
#line 426 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    {  (ExprSpecval.n) = 0; }
    break;

  case 52:
/* Line 1792 of yacc.c  */
#line 427 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    {  (ExprSpecval.n) = 0; }
    break;

  case 53:
/* Line 1792 of yacc.c  */
#line 428 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n)=remember(new ExprSpecScalarNode((ExprSpecloc).first_column,(ExprSpecloc).last_column,(ExprSpecvsp[(1) - (1)].d))); }
    break;

  case 54:
/* Line 1792 of yacc.c  */
#line 433 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = 0;}
    break;

  case 55:
/* Line 1792 of yacc.c  */
#line 434 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = (ExprSpecvsp[(1) - (1)].n);}
    break;

  case 56:
/* Line 1792 of yacc.c  */
#line 439 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    {
       // ignore first argument unless it is a string (because we parse strings in weird ways)
       ExprSpecListNode* list=new ExprSpecListNode((ExprSpecloc).last_column,(ExprSpecloc).last_column);
       if((ExprSpecvsp[(1) - (1)].n) && SPEC_IS_STR((ExprSpecvsp[(1) - (1)].n))){
           list->add((ExprSpecvsp[(1) - (1)].n));
       }
       remember(list);
       (ExprSpecval.n)=list;
   }
    break;

  case 57:
/* Line 1792 of yacc.c  */
#line 448 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    {

      if((ExprSpecvsp[(1) - (3)].n) && (ExprSpecvsp[(3) - (3)].n) && ((SPEC_IS_NUMBER((ExprSpecvsp[(3) - (3)].n)) || SPEC_IS_VECTOR((ExprSpecvsp[(3) - (3)].n)) || SPEC_IS_STR((ExprSpecvsp[(3) - (3)].n))))){
          (ExprSpecval.n)=(ExprSpecvsp[(1) - (3)].n);
          dynamic_cast<ExprSpecListNode*>((ExprSpecvsp[(1) - (3)].n))->add((ExprSpecvsp[(3) - (3)].n));
      }else{
          (ExprSpecval.n)=0;
      }
    }
    break;

  case 58:
/* Line 1792 of yacc.c  */
#line 460 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    { (ExprSpecval.n) = (ExprSpecvsp[(1) - (1)].n);}
    break;

  case 59:
/* Line 1792 of yacc.c  */
#line 461 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"
    {
        ExprSpecStringNode* str=new ExprSpecStringNode((ExprSpecloc).first_column,(ExprSpecloc).last_column,(ExprSpecvsp[(1) - (1)].s));
        //specRegisterEditable("<UNKNOWN>",str);
        // TODO: move string stuff out
        (ExprSpecval.n) = remember(str);
      }
    break;


/* Line 1792 of yacc.c  */
#line 2391 "y.tab.c"
      default: break;
    }
  /* User semantic actions sometimes alter ExprSpecchar, and that requires
     that ExprSpectoken be updated with the new translation.  We take the
     approach of translating immediately before every use of ExprSpectoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     ExprSpecYYABORT, ExprSpecYYACCEPT, or ExprSpecYYERROR immediately after altering ExprSpecchar or
     if it invokes ExprSpecYYBACKUP.  In the case of ExprSpecYYABORT or ExprSpecYYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of ExprSpecYYERROR or ExprSpecYYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  ExprSpecYY_SYMBOL_PRINT ("-> $$ =", ExprSpecr1[ExprSpecn], &ExprSpecval, &ExprSpecloc);

  ExprSpecYYPOPSTACK (ExprSpeclen);
  ExprSpeclen = 0;
  ExprSpecYY_STACK_PRINT (ExprSpecss, ExprSpecssp);

  *++ExprSpecvsp = ExprSpecval;
  *++ExprSpeclsp = ExprSpecloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  ExprSpecn = ExprSpecr1[ExprSpecn];

  ExprSpecstate = ExprSpecpgoto[ExprSpecn - ExprSpecYYNTOKENS] + *ExprSpecssp;
  if (0 <= ExprSpecstate && ExprSpecstate <= ExprSpecYYLAST && ExprSpeccheck[ExprSpecstate] == *ExprSpecssp)
    ExprSpecstate = ExprSpectable[ExprSpecstate];
  else
    ExprSpecstate = ExprSpecdefgoto[ExprSpecn - ExprSpecYYNTOKENS];

  goto ExprSpecnewstate;


/*------------------------------------.
| ExprSpecerrlab -- here on detecting error |
`------------------------------------*/
ExprSpecerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  ExprSpectoken = ExprSpecchar == ExprSpecYYEMPTY ? ExprSpecYYEMPTY : ExprSpecYYTRANSLATE (ExprSpecchar);

  /* If not already recovering from an error, report this error.  */
  if (!ExprSpecerrstatus)
    {
      ++ExprSpecnerrs;
#if ! ExprSpecYYERROR_VERBOSE
      ExprSpecerror (ExprSpecYY_("syntax error"));
#else
# define ExprSpecYYSYNTAX_ERROR ExprSpecsyntax_error (&ExprSpecmsg_alloc, &ExprSpecmsg, \
                                        ExprSpecssp, ExprSpectoken)
      {
        char const *ExprSpecmsgp = ExprSpecYY_("syntax error");
        int ExprSpecsyntax_error_status;
        ExprSpecsyntax_error_status = ExprSpecYYSYNTAX_ERROR;
        if (ExprSpecsyntax_error_status == 0)
          ExprSpecmsgp = ExprSpecmsg;
        else if (ExprSpecsyntax_error_status == 1)
          {
            if (ExprSpecmsg != ExprSpecmsgbuf)
              ExprSpecYYSTACK_FREE (ExprSpecmsg);
            ExprSpecmsg = (char *) ExprSpecYYSTACK_ALLOC (ExprSpecmsg_alloc);
            if (!ExprSpecmsg)
              {
                ExprSpecmsg = ExprSpecmsgbuf;
                ExprSpecmsg_alloc = sizeof ExprSpecmsgbuf;
                ExprSpecsyntax_error_status = 2;
              }
            else
              {
                ExprSpecsyntax_error_status = ExprSpecYYSYNTAX_ERROR;
                ExprSpecmsgp = ExprSpecmsg;
              }
          }
        ExprSpecerror (ExprSpecmsgp);
        if (ExprSpecsyntax_error_status == 2)
          goto ExprSpecexhaustedlab;
      }
# undef ExprSpecYYSYNTAX_ERROR
#endif
    }

  ExprSpecerror_range[1] = ExprSpeclloc;

  if (ExprSpecerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (ExprSpecchar <= ExprSpecYYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (ExprSpecchar == ExprSpecYYEOF)
	    ExprSpecYYABORT;
	}
      else
	{
	  ExprSpecdestruct ("Error: discarding",
		      ExprSpectoken, &ExprSpeclval, &ExprSpeclloc);
	  ExprSpecchar = ExprSpecYYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto ExprSpecerrlab1;


/*---------------------------------------------------.
| ExprSpecerrorlab -- error raised explicitly by ExprSpecYYERROR.  |
`---------------------------------------------------*/
ExprSpecerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     ExprSpecYYERROR and the label ExprSpecerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto ExprSpecerrorlab;

  ExprSpecerror_range[1] = ExprSpeclsp[1-ExprSpeclen];
  /* Do not reclaim the symbols of the rule which action triggered
     this ExprSpecYYERROR.  */
  ExprSpecYYPOPSTACK (ExprSpeclen);
  ExprSpeclen = 0;
  ExprSpecYY_STACK_PRINT (ExprSpecss, ExprSpecssp);
  ExprSpecstate = *ExprSpecssp;
  goto ExprSpecerrlab1;


/*-------------------------------------------------------------.
| ExprSpecerrlab1 -- common code for both syntax error and ExprSpecYYERROR.  |
`-------------------------------------------------------------*/
ExprSpecerrlab1:
  ExprSpecerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      ExprSpecn = ExprSpecpact[ExprSpecstate];
      if (!ExprSpecpact_value_is_default (ExprSpecn))
	{
	  ExprSpecn += ExprSpecYYTERROR;
	  if (0 <= ExprSpecn && ExprSpecn <= ExprSpecYYLAST && ExprSpeccheck[ExprSpecn] == ExprSpecYYTERROR)
	    {
	      ExprSpecn = ExprSpectable[ExprSpecn];
	      if (0 < ExprSpecn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (ExprSpecssp == ExprSpecss)
	ExprSpecYYABORT;

      ExprSpecerror_range[1] = *ExprSpeclsp;
      ExprSpecdestruct ("Error: popping",
		  ExprSpecstos[ExprSpecstate], ExprSpecvsp, ExprSpeclsp);
      ExprSpecYYPOPSTACK (1);
      ExprSpecstate = *ExprSpecssp;
      ExprSpecYY_STACK_PRINT (ExprSpecss, ExprSpecssp);
    }

  ExprSpecYY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++ExprSpecvsp = ExprSpeclval;
  ExprSpecYY_IGNORE_MAYBE_UNINITIALIZED_END

  ExprSpecerror_range[2] = ExprSpeclloc;
  /* Using ExprSpecYYLLOC is tempting, but would change the location of
     the lookahead.  ExprSpecYYLOC is available though.  */
  ExprSpecYYLLOC_DEFAULT (ExprSpecloc, ExprSpecerror_range, 2);
  *++ExprSpeclsp = ExprSpecloc;

  /* Shift the error token.  */
  ExprSpecYY_SYMBOL_PRINT ("Shifting", ExprSpecstos[ExprSpecn], ExprSpecvsp, ExprSpeclsp);

  ExprSpecstate = ExprSpecn;
  goto ExprSpecnewstate;


/*-------------------------------------.
| ExprSpecacceptlab -- ExprSpecYYACCEPT comes here.  |
`-------------------------------------*/
ExprSpecacceptlab:
  ExprSpecresult = 0;
  goto ExprSpecreturn;

/*-----------------------------------.
| ExprSpecabortlab -- ExprSpecYYABORT comes here.  |
`-----------------------------------*/
ExprSpecabortlab:
  ExprSpecresult = 1;
  goto ExprSpecreturn;

#if !defined ExprSpecoverflow || ExprSpecYYERROR_VERBOSE
/*-------------------------------------------------.
| ExprSpecexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
ExprSpecexhaustedlab:
  ExprSpecerror (ExprSpecYY_("memory exhausted"));
  ExprSpecresult = 2;
  /* Fall through.  */
#endif

ExprSpecreturn:
  if (ExprSpecchar != ExprSpecYYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      ExprSpectoken = ExprSpecYYTRANSLATE (ExprSpecchar);
      ExprSpecdestruct ("Cleanup: discarding lookahead",
                  ExprSpectoken, &ExprSpeclval, &ExprSpeclloc);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this ExprSpecYYABORT or ExprSpecYYACCEPT.  */
  ExprSpecYYPOPSTACK (ExprSpeclen);
  ExprSpecYY_STACK_PRINT (ExprSpecss, ExprSpecssp);
  while (ExprSpecssp != ExprSpecss)
    {
      ExprSpecdestruct ("Cleanup: popping",
		  ExprSpecstos[*ExprSpecssp], ExprSpecvsp, ExprSpeclsp);
      ExprSpecYYPOPSTACK (1);
    }
#ifndef ExprSpecoverflow
  if (ExprSpecss != ExprSpecssa)
    ExprSpecYYSTACK_FREE (ExprSpecss);
#endif
#if ExprSpecYYERROR_VERBOSE
  if (ExprSpecmsg != ExprSpecmsgbuf)
    ExprSpecYYSTACK_FREE (ExprSpecmsg);
#endif
  /* Make sure ExprSpecYYID is used.  */
  return ExprSpecYYID (ExprSpecresult);
}


/* Line 2055 of yacc.c  */
#line 469 "/disney/users/jberlin/projects/seexpr2/src/ui/ExprSpecParser.y"


/* ExprSpecerror - Report an error.  This is called by the parser.
(Note: the "msg" param is useless as it is usually just "sparse error".
so it's ignored.)
*/
static void ExprSpecerror(const char* /*msg*/)
{
    // find start of line containing error
    int pos = ExprSpecpos(), lineno = 1, start = 0, end = strlen(ParseStr);
    bool multiline = 0;
    for (int i = start; i < pos; i++)
	if (ParseStr[i] == '\n') { start = i + 1; lineno++; multiline=1; }

    // find end of line containing error
    for (int i = end; i > pos; i--)
	if (ParseStr[i] == '\n') { end = i - 1; multiline=1; }

    ParseError = ExprSpectext[0] ? "Syntax error" : "Unexpected end of expression";
    if (multiline) {
	char buff[30];
	snprintf(buff, 30, " at line %d", lineno);
	ParseError += buff;
    }
    if (ExprSpectext[0]) {
	ParseError += " near '";
	ParseError += ExprSpectext;
    }
    ParseError += "':\n    ";

    int s = std::max(start, pos-30);
    int e = std::min(end, pos+30);

    if (s != start) ParseError += "...";
    ParseError += std::string(ParseStr, s, e-s+1);
    if (e != end) ParseError += "...";
}

extern void specResetCounters(std::vector<std::pair<int,int> >& comments);


/* CallParser - This is our entrypoint from the rest of the expr library. 
   A string is passed in and a parse tree is returned.	If the tree is null,
   an error string is returned.  Any flags set during parsing are passed
   along.
 */

static SeExprInternal2::Mutex mutex;

/// Main entry point to parser
bool ExprSpecParse(std::vector<Editable*>& outputEditables,
    std::vector<std::string>& outputVariables,
    std::vector<std::pair<int,int> >& comments,
    const char* str)
{
    SeExprInternal2::AutoMutex locker(mutex);

    /// Make inputs/outputs accessible to parser actions
    editables=&outputEditables;
    variables=&outputVariables;
    ParseStr=str;

    // setup and startup parser
    specResetCounters(comments); // reset lineNumber and columnNumber in scanner
    ExprSpec_buffer_state* buffer = ExprSpec_scan_string(str); // setup lexer
    ParseResult = 0;
    int resultCode = ExprSpecparse(); // parser (don't care if it is a parse error)
    UNUSED(resultCode);
    ExprSpec_delete_buffer(buffer);

    // delete temporary data -- specs(mini parse tree) and tokens(strings)!
    for(size_t i=0;i<specNodes.size();i++) delete specNodes[i];
    specNodes.clear();
    for(size_t i=0;i<tokens.size();i++) free(tokens[i]);
    tokens.clear();
    return true;
}

