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

/* All symbols defined below should begin with SeExprEdSpec or SeExprEdSpecYY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define SeExprEdSpecYYBISON 1

/* Bison version.  */
#define SeExprEdSpecYYBISON_VERSION "2.7"

/* Skeleton name.  */
#define SeExprEdSpecYYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define SeExprEdSpecYYPURE 0

/* Push parsers.  */
#define SeExprEdSpecYYPUSH 0

/* Pull parsers.  */
#define SeExprEdSpecYYPULL 1


/* Substitute the variable and function names.  */
#define SeExprEdSpecparse         SeExprEdSpecparse
#define SeExprEdSpeclex           SeExprEdSpeclex
#define SeExprEdSpecerror         SeExprEdSpecerror
#define SeExprEdSpeclval          SeExprEdSpeclval
#define SeExprEdSpecchar          SeExprEdSpecchar
#define SeExprEdSpecdebug         SeExprEdSpecdebug
#define SeExprEdSpecnerrs         SeExprEdSpecnerrs
#define SeExprEdSpeclloc          SeExprEdSpeclloc

/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 18 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"

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
#include "SeMutex.h"
#include "SeExprSpecType.h"
#include "SeExprEdEditable.h"
#include "SeExprEdDeepWater.h"


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
int SeExprEdSpeclex();
int SeExprEdSpecpos();
extern int SeExprEdSpec_start;
extern char* SeExprEdSpectext;
struct SeExprEdSpec_buffer_state;
SeExprEdSpec_buffer_state* SeExprEdSpec_scan_string(const char *str);
void SeExprEdSpec_delete_buffer(SeExprEdSpec_buffer_state*);

//#####################################
// Keep track of mini parse tree nodes

// temporary to the parse... all pointers deleted at end of parse
static std::vector<SeExprSpecNode*> specNodes;
/// Remember the spec node, so we can delete it later
static SeExprSpecNode* remember(SeExprSpecNode* node)
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
static std::string ParseError;  // error (set from SeExprEdSpecerror)
static SeExprSpecNode* ParseResult; // must set result here since SeExprEdSpecparse can't return it


//######################################################################
// Helpers used by actions to register data


/// Remember that there is an assignment to this variable (For autocomplete)
static void specRegisterVariable(const char* var)
{
    variables->push_back(var);
}

/// Variable Assignment/String literal should be turned into an editable
/// an editable is the data part of a control (it's model essentially)
static void specRegisterEditable(const char* var,SeExprSpecNode* node)
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
    }else if(SeExprSpecColorSwatchNode* n=dynamic_cast<SeExprSpecColorSwatchNode*>(node)){
        if(SeExprSpecListNode* args=dynamic_cast<SeExprSpecListNode*>(n->args)){
            if(args->nodes.size()>0){
                SeExprEdColorSwatchEditable* swatch=new SeExprEdColorSwatchEditable(var,node->startPos,node->endPos);
                bool valid=true;
                for(size_t i=0;i<args->nodes.size();i++){
                    SeExprSpecVectorNode* colornode=dynamic_cast<SeExprSpecVectorNode*>(args->nodes[i]);
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
    }else if(SeExprSpecDeepWaterNode* n=dynamic_cast<SeExprSpecDeepWaterNode*>(node)){
        if(SeExprSpecListNode* args=dynamic_cast<SeExprSpecListNode*>(n->args)){
            if(args->nodes.size()==12){
                SeExprEdDeepWaterEditable* deepWater=new SeExprEdDeepWaterEditable(var,node->startPos,node->endPos);
                bool valid=true;

                SeExprSpecScalarNode* resolution=dynamic_cast<SeExprSpecScalarNode*>(args->nodes[0]);
                SeExprSpecScalarNode* tileSize=dynamic_cast<SeExprSpecScalarNode*>(args->nodes[1]);
                SeExprSpecScalarNode* lengthCutoff=dynamic_cast<SeExprSpecScalarNode*>(args->nodes[2]);
                SeExprSpecScalarNode* amplitude=dynamic_cast<SeExprSpecScalarNode*>(args->nodes[3]);
                SeExprSpecScalarNode* windAngle=dynamic_cast<SeExprSpecScalarNode*>(args->nodes[4]);
                SeExprSpecScalarNode* windSpeed=dynamic_cast<SeExprSpecScalarNode*>(args->nodes[5]);
                SeExprSpecScalarNode* directionalFactorExponent=dynamic_cast<SeExprSpecScalarNode*>(args->nodes[6]);
                SeExprSpecScalarNode* directionalReflectionDamping=dynamic_cast<SeExprSpecScalarNode*>(args->nodes[7]);
                SeExprSpecVectorNode* flowDirection=dynamic_cast<SeExprSpecVectorNode*>(args->nodes[8]);
                SeExprSpecScalarNode* sharpen=dynamic_cast<SeExprSpecScalarNode*>(args->nodes[9]);
                SeExprSpecScalarNode* time=dynamic_cast<SeExprSpecScalarNode*>(args->nodes[10]);
                SeExprSpecScalarNode* filterWidth=dynamic_cast<SeExprSpecScalarNode*>(args->nodes[11]);
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
static void SeExprEdSpecerror(const char* msg);


/* Line 371 of yacc.c  */
#line 329 "y.tab.c"

# ifndef SeExprEdSpecYY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define SeExprEdSpecYY_NULL nullptr
#  else
#   define SeExprEdSpecYY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef SeExprEdSpecYYERROR_VERBOSE
# undef SeExprEdSpecYYERROR_VERBOSE
# define SeExprEdSpecYYERROR_VERBOSE 1
#else
# define SeExprEdSpecYYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef SeExprEdSpecYY_SEEXPREDSPEC_Y_TAB_H_INCLUDED
# define SeExprEdSpecYY_SEEXPREDSPEC_Y_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef SeExprEdSpecYYDEBUG
# define SeExprEdSpecYYDEBUG 0
#endif
#if SeExprEdSpecYYDEBUG
extern int SeExprEdSpecdebug;
#endif

/* Tokens.  */
#ifndef SeExprEdSpecYYTOKENTYPE
# define SeExprEdSpecYYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum SeExprEdSpectokentype {
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



#if ! defined SeExprEdSpecYYSTYPE && ! defined SeExprEdSpecYYSTYPE_IS_DECLARED
typedef union SeExprEdSpecYYSTYPE
{
/* Line 387 of yacc.c  */
#line 271 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"

    SeExprSpecNode* n;
    double d;      // return value for number tokens
    char* s;       /* return value for name tokens.  Note: UNLIKE the regular parser, this is not strdup()'dthe string */


/* Line 387 of yacc.c  */
#line 423 "y.tab.c"
} SeExprEdSpecYYSTYPE;
# define SeExprEdSpecYYSTYPE_IS_TRIVIAL 1
# define SeExprEdSpecstype SeExprEdSpecYYSTYPE /* obsolescent; will be withdrawn */
# define SeExprEdSpecYYSTYPE_IS_DECLARED 1
#endif

#if ! defined SeExprEdSpecYYLTYPE && ! defined SeExprEdSpecYYLTYPE_IS_DECLARED
typedef struct SeExprEdSpecYYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} SeExprEdSpecYYLTYPE;
# define SeExprEdSpecltype SeExprEdSpecYYLTYPE /* obsolescent; will be withdrawn */
# define SeExprEdSpecYYLTYPE_IS_DECLARED 1
# define SeExprEdSpecYYLTYPE_IS_TRIVIAL 1
#endif

extern SeExprEdSpecYYSTYPE SeExprEdSpeclval;
extern SeExprEdSpecYYLTYPE SeExprEdSpeclloc;
#ifdef SeExprEdSpecYYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int SeExprEdSpecparse (void *SeExprEdSpecYYPARSE_PARAM);
#else
int SeExprEdSpecparse ();
#endif
#else /* ! SeExprEdSpecYYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int SeExprEdSpecparse (void);
#else
int SeExprEdSpecparse ();
#endif
#endif /* ! SeExprEdSpecYYPARSE_PARAM */

#endif /* !SeExprEdSpecYY_SEEXPREDSPEC_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 464 "y.tab.c"

#ifdef short
# undef short
#endif

#ifdef SeExprEdSpecYYTYPE_UINT8
typedef SeExprEdSpecYYTYPE_UINT8 SeExprEdSpectype_uint8;
#else
typedef unsigned char SeExprEdSpectype_uint8;
#endif

#ifdef SeExprEdSpecYYTYPE_INT8
typedef SeExprEdSpecYYTYPE_INT8 SeExprEdSpectype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char SeExprEdSpectype_int8;
#else
typedef short int SeExprEdSpectype_int8;
#endif

#ifdef SeExprEdSpecYYTYPE_UINT16
typedef SeExprEdSpecYYTYPE_UINT16 SeExprEdSpectype_uint16;
#else
typedef unsigned short int SeExprEdSpectype_uint16;
#endif

#ifdef SeExprEdSpecYYTYPE_INT16
typedef SeExprEdSpecYYTYPE_INT16 SeExprEdSpectype_int16;
#else
typedef short int SeExprEdSpectype_int16;
#endif

#ifndef SeExprEdSpecYYSIZE_T
# ifdef __SIZE_TYPE__
#  define SeExprEdSpecYYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define SeExprEdSpecYYSIZE_T size_t
# elif ! defined SeExprEdSpecYYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define SeExprEdSpecYYSIZE_T size_t
# else
#  define SeExprEdSpecYYSIZE_T unsigned int
# endif
#endif

#define SeExprEdSpecYYSIZE_MAXIMUM ((SeExprEdSpecYYSIZE_T) -1)

#ifndef SeExprEdSpecYY_
# if defined SeExprEdSpecYYENABLE_NLS && SeExprEdSpecYYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define SeExprEdSpecYY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef SeExprEdSpecYY_
#  define SeExprEdSpecYY_(Msgid) Msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define SeExprEdSpecYYUSE(E) ((void) (E))
#else
# define SeExprEdSpecYYUSE(E) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define SeExprEdSpecYYID(N) (N)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
SeExprEdSpecYYID (int SeExprEdSpeci)
#else
static int
SeExprEdSpecYYID (SeExprEdSpeci)
    int SeExprEdSpeci;
#endif
{
  return SeExprEdSpeci;
}
#endif

#if ! defined SeExprEdSpecoverflow || SeExprEdSpecYYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef SeExprEdSpecYYSTACK_USE_ALLOCA
#  if SeExprEdSpecYYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define SeExprEdSpecYYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define SeExprEdSpecYYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define SeExprEdSpecYYSTACK_ALLOC alloca
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

# ifdef SeExprEdSpecYYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define SeExprEdSpecYYSTACK_FREE(Ptr) do { /* empty */; } while (SeExprEdSpecYYID (0))
#  ifndef SeExprEdSpecYYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define SeExprEdSpecYYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define SeExprEdSpecYYSTACK_ALLOC SeExprEdSpecYYMALLOC
#  define SeExprEdSpecYYSTACK_FREE SeExprEdSpecYYFREE
#  ifndef SeExprEdSpecYYSTACK_ALLOC_MAXIMUM
#   define SeExprEdSpecYYSTACK_ALLOC_MAXIMUM SeExprEdSpecYYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined SeExprEdSpecYYMALLOC || defined malloc) \
	     && (defined SeExprEdSpecYYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef SeExprEdSpecYYMALLOC
#   define SeExprEdSpecYYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (SeExprEdSpecYYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef SeExprEdSpecYYFREE
#   define SeExprEdSpecYYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined SeExprEdSpecoverflow || SeExprEdSpecYYERROR_VERBOSE */


#if (! defined SeExprEdSpecoverflow \
     && (! defined __cplusplus \
	 || (defined SeExprEdSpecYYLTYPE_IS_TRIVIAL && SeExprEdSpecYYLTYPE_IS_TRIVIAL \
	     && defined SeExprEdSpecYYSTYPE_IS_TRIVIAL && SeExprEdSpecYYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union SeExprEdSpecalloc
{
  SeExprEdSpectype_int16 SeExprEdSpecss_alloc;
  SeExprEdSpecYYSTYPE SeExprEdSpecvs_alloc;
  SeExprEdSpecYYLTYPE SeExprEdSpecls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define SeExprEdSpecYYSTACK_GAP_MAXIMUM (sizeof (union SeExprEdSpecalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define SeExprEdSpecYYSTACK_BYTES(N) \
     ((N) * (sizeof (SeExprEdSpectype_int16) + sizeof (SeExprEdSpecYYSTYPE) + sizeof (SeExprEdSpecYYLTYPE)) \
      + 2 * SeExprEdSpecYYSTACK_GAP_MAXIMUM)

# define SeExprEdSpecYYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables SeExprEdSpecYYSIZE and SeExprEdSpecYYSTACKSIZE give the old and new number of
   elements in the stack, and SeExprEdSpecYYPTR gives the new location of the
   stack.  Advance SeExprEdSpecYYPTR to a properly aligned location for the next
   stack.  */
# define SeExprEdSpecYYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	SeExprEdSpecYYSIZE_T SeExprEdSpecnewbytes;						\
	SeExprEdSpecYYCOPY (&SeExprEdSpecptr->Stack_alloc, Stack, SeExprEdSpecsize);			\
	Stack = &SeExprEdSpecptr->Stack_alloc;					\
	SeExprEdSpecnewbytes = SeExprEdSpecstacksize * sizeof (*Stack) + SeExprEdSpecYYSTACK_GAP_MAXIMUM; \
	SeExprEdSpecptr += SeExprEdSpecnewbytes / sizeof (*SeExprEdSpecptr);				\
      }									\
    while (SeExprEdSpecYYID (0))

#endif

#if defined SeExprEdSpecYYCOPY_NEEDED && SeExprEdSpecYYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef SeExprEdSpecYYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define SeExprEdSpecYYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define SeExprEdSpecYYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          SeExprEdSpecYYSIZE_T SeExprEdSpeci;                         \
          for (SeExprEdSpeci = 0; SeExprEdSpeci < (Count); SeExprEdSpeci++)   \
            (Dst)[SeExprEdSpeci] = (Src)[SeExprEdSpeci];            \
        }                                       \
      while (SeExprEdSpecYYID (0))
#  endif
# endif
#endif /* !SeExprEdSpecYYCOPY_NEEDED */

/* SeExprEdSpecYYFINAL -- State number of the termination state.  */
#define SeExprEdSpecYYFINAL  40
/* SeExprEdSpecYYLAST -- Last index in SeExprEdSpecYYTABLE.  */
#define SeExprEdSpecYYLAST   693

/* SeExprEdSpecYYNTOKENS -- Number of terminals.  */
#define SeExprEdSpecYYNTOKENS  44
/* SeExprEdSpecYYNNTS -- Number of nonterminals.  */
#define SeExprEdSpecYYNNTS  11
/* SeExprEdSpecYYNRULES -- Number of rules.  */
#define SeExprEdSpecYYNRULES  59
/* SeExprEdSpecYYNRULES -- Number of states.  */
#define SeExprEdSpecYYNSTATES  139

/* SeExprEdSpecYYTRANSLATE(SeExprEdSpecYYLEX) -- Bison symbol number corresponding to SeExprEdSpecYYLEX.  */
#define SeExprEdSpecYYUNDEFTOK  2
#define SeExprEdSpecYYMAXUTOK   277

#define SeExprEdSpecYYTRANSLATE(SeExprEdSpecYYX)						\
  ((unsigned int) (SeExprEdSpecYYX) <= SeExprEdSpecYYMAXUTOK ? SeExprEdSpectranslate[SeExprEdSpecYYX] : SeExprEdSpecYYUNDEFTOK)

/* SeExprEdSpecYYTRANSLATE[SeExprEdSpecYYLEX] -- Bison symbol number corresponding to SeExprEdSpecYYLEX.  */
static const SeExprEdSpectype_uint8 SeExprEdSpectranslate[] =
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

#if SeExprEdSpecYYDEBUG
/* SeExprEdSpecYYPRHS[SeExprEdSpecYYN] -- Index of the first RHS symbol of rule number SeExprEdSpecYYN in
   SeExprEdSpecYYRHS.  */
static const SeExprEdSpectype_uint8 SeExprEdSpecprhs[] =
{
       0,     0,     3,     6,     8,     9,    11,    13,    16,    18,
      23,    28,    33,    38,    43,    48,    53,    58,    63,    68,
      73,    78,    83,    88,    97,    98,   103,   106,   110,   118,
     123,   129,   133,   137,   141,   145,   149,   153,   157,   161,
     164,   167,   170,   173,   177,   181,   185,   189,   193,   197,
     202,   209,   211,   213,   215,   216,   218,   220,   224,   226
};

/* SeExprEdSpecYYRHS -- A `-1'-separated list of the rules' RHS.  */
static const SeExprEdSpectype_int8 SeExprEdSpecrhs[] =
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

/* SeExprEdSpecYYRLINE[SeExprEdSpecYYN] -- source line where rule number SeExprEdSpecYYN was defined.  */
static const SeExprEdSpectype_uint16 SeExprEdSpecrline[] =
{
       0,   313,   313,   314,   319,   320,   324,   325,   330,   331,
     335,   336,   337,   338,   339,   340,   341,   345,   346,   347,
     348,   349,   350,   354,   359,   360,   361,   366,   367,   372,
     373,   374,   375,   376,   377,   378,   379,   380,   381,   382,
     383,   392,   393,   394,   395,   396,   397,   398,   399,   400,
     425,   426,   427,   428,   433,   434,   439,   448,   460,   461
};
#endif

#if SeExprEdSpecYYDEBUG || SeExprEdSpecYYERROR_VERBOSE || 0
/* SeExprEdSpecYYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at SeExprEdSpecYYNTOKENS, nonterminals.  */
static const char *const SeExprEdSpectname[] =
{
  "$end", "error", "$undefined", "IF", "ELSE", "NAME", "VAR", "STR",
  "NUMBER", "AddEq", "SubEq", "MultEq", "DivEq", "ExpEq", "ModEq", "'('",
  "')'", "ARROW", "':'", "'?'", "OR", "AND", "NE", "EQ", "'<'", "'>'",
  "GE", "LE", "'+'", "'-'", "'*'", "'/'", "'%'", "'!'", "'~'", "UNARY",
  "'^'", "'['", "'='", "';'", "'{'", "'}'", "','", "']'", "$accept",
  "expr", "optassigns", "assigns", "assign", "ifthenelse", "optelse", "e",
  "optargs", "args", "arg", SeExprEdSpecYY_NULL
};
#endif

# ifdef SeExprEdSpecYYPRINT
/* SeExprEdSpecYYTOKNUM[SeExprEdSpecYYLEX-NUM] -- Internal token number corresponding to
   token SeExprEdSpecYYLEX-NUM.  */
static const SeExprEdSpectype_uint16 SeExprEdSpectoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,    40,    41,   270,    58,    63,
     271,   272,   273,   274,    60,    62,   275,   276,    43,    45,
      42,    47,    37,    33,   126,   277,    94,    91,    61,    59,
     123,   125,    44,    93
};
# endif

/* SeExprEdSpecYYR1[SeExprEdSpecYYN] -- Symbol number of symbol that rule SeExprEdSpecYYN derives.  */
static const SeExprEdSpectype_uint8 SeExprEdSpecr1[] =
{
       0,    44,    45,    45,    46,    46,    47,    47,    48,    48,
      48,    48,    48,    48,    48,    48,    48,    48,    48,    48,
      48,    48,    48,    49,    50,    50,    50,    51,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    51,
      51,    51,    51,    51,    52,    52,    53,    53,    54,    54
};

/* SeExprEdSpecYYR2[SeExprEdSpecYYN] -- Number of symbols composing right hand side of rule SeExprEdSpecYYN.  */
static const SeExprEdSpectype_uint8 SeExprEdSpecr2[] =
{
       0,     2,     2,     1,     0,     1,     1,     2,     1,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     8,     0,     4,     2,     3,     7,     4,
       5,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       2,     2,     2,     3,     3,     3,     3,     3,     3,     4,
       6,     1,     1,     1,     0,     1,     1,     3,     1,     1
};

/* SeExprEdSpecYYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when SeExprEdSpecYYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const SeExprEdSpectype_uint8 SeExprEdSpecdefact[] =
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

/* SeExprEdSpecYYDEFGOTO[NTERM-NUM].  */
static const SeExprEdSpectype_int16 SeExprEdSpecdefgoto[] =
{
      -1,    11,   127,   128,    13,    14,   134,    68,    69,    70,
      71
};

/* SeExprEdSpecYYPACT[STATE-NUM] -- Index in SeExprEdSpecYYTABLE of the portion describing
   STATE-NUM.  */
#define SeExprEdSpecYYPACT_NINF -65
static const SeExprEdSpectype_int16 SeExprEdSpecpact[] =
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

/* SeExprEdSpecYYPGOTO[NTERM-NUM].  */
static const SeExprEdSpectype_int8 SeExprEdSpecpgoto[] =
{
     -65,   -65,   -64,    77,   -11,   -55,   -65,     0,   -38,   -65,
     -27
};

/* SeExprEdSpecYYTABLE[SeExprEdSpecYYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If SeExprEdSpecYYTABLE_NINF, syntax error.  */
#define SeExprEdSpecYYTABLE_NINF -1
static const SeExprEdSpectype_uint8 SeExprEdSpectable[] =
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

#define SeExprEdSpecpact_value_is_default(Yystate) \
  (!!((Yystate) == (-65)))

#define SeExprEdSpectable_value_is_error(Yytable_value) \
  SeExprEdSpecYYID (0)

static const SeExprEdSpectype_int16 SeExprEdSpeccheck[] =
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

/* SeExprEdSpecYYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const SeExprEdSpectype_uint8 SeExprEdSpecstos[] =
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

#define SeExprEdSpecerrok		(SeExprEdSpecerrstatus = 0)
#define SeExprEdSpecclearin	(SeExprEdSpecchar = SeExprEdSpecYYEMPTY)
#define SeExprEdSpecYYEMPTY		(-2)
#define SeExprEdSpecYYEOF		0

#define SeExprEdSpecYYACCEPT	goto SeExprEdSpecacceptlab
#define SeExprEdSpecYYABORT		goto SeExprEdSpecabortlab
#define SeExprEdSpecYYERROR		goto SeExprEdSpecerrorlab


/* Like SeExprEdSpecYYERROR except do call SeExprEdSpecerror.  This remains here temporarily
   to ease the transition to the new meaning of SeExprEdSpecYYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   SeExprEdSpecYYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define SeExprEdSpecYYFAIL		goto SeExprEdSpecerrlab
#if defined SeExprEdSpecYYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     SeExprEdSpecYYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define SeExprEdSpecYYRECOVERING()  (!!SeExprEdSpecerrstatus)

#define SeExprEdSpecYYBACKUP(Token, Value)                                  \
do                                                              \
  if (SeExprEdSpecchar == SeExprEdSpecYYEMPTY)                                        \
    {                                                           \
      SeExprEdSpecchar = (Token);                                         \
      SeExprEdSpeclval = (Value);                                         \
      SeExprEdSpecYYPOPSTACK (SeExprEdSpeclen);                                       \
      SeExprEdSpecstate = *SeExprEdSpecssp;                                         \
      goto SeExprEdSpecbackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      SeExprEdSpecerror (SeExprEdSpecYY_("syntax error: cannot back up")); \
      SeExprEdSpecYYERROR;							\
    }								\
while (SeExprEdSpecYYID (0))

/* Error token number */
#define SeExprEdSpecYYTERROR	1
#define SeExprEdSpecYYERRCODE	256


/* SeExprEdSpecYYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef SeExprEdSpecYYLLOC_DEFAULT
# define SeExprEdSpecYYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (SeExprEdSpecYYID (N))                                                     \
        {                                                               \
          (Current).first_line   = SeExprEdSpecYYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = SeExprEdSpecYYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = SeExprEdSpecYYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = SeExprEdSpecYYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            SeExprEdSpecYYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            SeExprEdSpecYYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (SeExprEdSpecYYID (0))
#endif

#define SeExprEdSpecYYRHSLOC(Rhs, K) ((Rhs)[K])


/* SeExprEdSpecYY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef SeExprEdSpecYY_LOCATION_PRINT
# if defined SeExprEdSpecYYLTYPE_IS_TRIVIAL && SeExprEdSpecYYLTYPE_IS_TRIVIAL

/* Print *SeExprEdSpecYYLOCP on SeExprEdSpecYYO.  Private, do not rely on its existence. */

__attribute__((__unused__))
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static unsigned
SeExprEdSpec_location_print_ (FILE *SeExprEdSpeco, SeExprEdSpecYYLTYPE const * const SeExprEdSpeclocp)
#else
static unsigned
SeExprEdSpec_location_print_ (SeExprEdSpeco, SeExprEdSpeclocp)
    FILE *SeExprEdSpeco;
    SeExprEdSpecYYLTYPE const * const SeExprEdSpeclocp;
#endif
{
  unsigned res = 0;
  int end_col = 0 != SeExprEdSpeclocp->last_column ? SeExprEdSpeclocp->last_column - 1 : 0;
  if (0 <= SeExprEdSpeclocp->first_line)
    {
      res += fprintf (SeExprEdSpeco, "%d", SeExprEdSpeclocp->first_line);
      if (0 <= SeExprEdSpeclocp->first_column)
        res += fprintf (SeExprEdSpeco, ".%d", SeExprEdSpeclocp->first_column);
    }
  if (0 <= SeExprEdSpeclocp->last_line)
    {
      if (SeExprEdSpeclocp->first_line < SeExprEdSpeclocp->last_line)
        {
          res += fprintf (SeExprEdSpeco, "-%d", SeExprEdSpeclocp->last_line);
          if (0 <= end_col)
            res += fprintf (SeExprEdSpeco, ".%d", end_col);
        }
      else if (0 <= end_col && SeExprEdSpeclocp->first_column < end_col)
        res += fprintf (SeExprEdSpeco, "-%d", end_col);
    }
  return res;
 }

#  define SeExprEdSpecYY_LOCATION_PRINT(File, Loc)          \
  SeExprEdSpec_location_print_ (File, &(Loc))

# else
#  define SeExprEdSpecYY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* SeExprEdSpecYYLEX -- calling `SeExprEdSpeclex' with the right arguments.  */
#ifdef SeExprEdSpecYYLEX_PARAM
# define SeExprEdSpecYYLEX SeExprEdSpeclex (SeExprEdSpecYYLEX_PARAM)
#else
# define SeExprEdSpecYYLEX SeExprEdSpeclex ()
#endif

/* Enable debugging if requested.  */
#if SeExprEdSpecYYDEBUG

# ifndef SeExprEdSpecYYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define SeExprEdSpecYYFPRINTF fprintf
# endif

# define SeExprEdSpecYYDPRINTF(Args)			\
do {						\
  if (SeExprEdSpecdebug)					\
    SeExprEdSpecYYFPRINTF Args;				\
} while (SeExprEdSpecYYID (0))

# define SeExprEdSpecYY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (SeExprEdSpecdebug)								  \
    {									  \
      SeExprEdSpecYYFPRINTF (stderr, "%s ", Title);					  \
      SeExprEdSpec_symbol_print (stderr,						  \
		  Type, Value, Location); \
      SeExprEdSpecYYFPRINTF (stderr, "\n");						  \
    }									  \
} while (SeExprEdSpecYYID (0))


/*--------------------------------.
| Print this symbol on SeExprEdSpecYYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
SeExprEdSpec_symbol_value_print (FILE *SeExprEdSpecoutput, int SeExprEdSpectype, SeExprEdSpecYYSTYPE const * const SeExprEdSpecvaluep, SeExprEdSpecYYLTYPE const * const SeExprEdSpeclocationp)
#else
static void
SeExprEdSpec_symbol_value_print (SeExprEdSpecoutput, SeExprEdSpectype, SeExprEdSpecvaluep, SeExprEdSpeclocationp)
    FILE *SeExprEdSpecoutput;
    int SeExprEdSpectype;
    SeExprEdSpecYYSTYPE const * const SeExprEdSpecvaluep;
    SeExprEdSpecYYLTYPE const * const SeExprEdSpeclocationp;
#endif
{
  FILE *SeExprEdSpeco = SeExprEdSpecoutput;
  SeExprEdSpecYYUSE (SeExprEdSpeco);
  if (!SeExprEdSpecvaluep)
    return;
  SeExprEdSpecYYUSE (SeExprEdSpeclocationp);
# ifdef SeExprEdSpecYYPRINT
  if (SeExprEdSpectype < SeExprEdSpecYYNTOKENS)
    SeExprEdSpecYYPRINT (SeExprEdSpecoutput, SeExprEdSpectoknum[SeExprEdSpectype], *SeExprEdSpecvaluep);
# else
  SeExprEdSpecYYUSE (SeExprEdSpecoutput);
# endif
  switch (SeExprEdSpectype)
    {
      default:
        break;
    }
}


/*--------------------------------.
| Print this symbol on SeExprEdSpecYYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
SeExprEdSpec_symbol_print (FILE *SeExprEdSpecoutput, int SeExprEdSpectype, SeExprEdSpecYYSTYPE const * const SeExprEdSpecvaluep, SeExprEdSpecYYLTYPE const * const SeExprEdSpeclocationp)
#else
static void
SeExprEdSpec_symbol_print (SeExprEdSpecoutput, SeExprEdSpectype, SeExprEdSpecvaluep, SeExprEdSpeclocationp)
    FILE *SeExprEdSpecoutput;
    int SeExprEdSpectype;
    SeExprEdSpecYYSTYPE const * const SeExprEdSpecvaluep;
    SeExprEdSpecYYLTYPE const * const SeExprEdSpeclocationp;
#endif
{
  if (SeExprEdSpectype < SeExprEdSpecYYNTOKENS)
    SeExprEdSpecYYFPRINTF (SeExprEdSpecoutput, "token %s (", SeExprEdSpectname[SeExprEdSpectype]);
  else
    SeExprEdSpecYYFPRINTF (SeExprEdSpecoutput, "nterm %s (", SeExprEdSpectname[SeExprEdSpectype]);

  SeExprEdSpecYY_LOCATION_PRINT (SeExprEdSpecoutput, *SeExprEdSpeclocationp);
  SeExprEdSpecYYFPRINTF (SeExprEdSpecoutput, ": ");
  SeExprEdSpec_symbol_value_print (SeExprEdSpecoutput, SeExprEdSpectype, SeExprEdSpecvaluep, SeExprEdSpeclocationp);
  SeExprEdSpecYYFPRINTF (SeExprEdSpecoutput, ")");
}

/*------------------------------------------------------------------.
| SeExprEdSpec_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
SeExprEdSpec_stack_print (SeExprEdSpectype_int16 *SeExprEdSpecbottom, SeExprEdSpectype_int16 *SeExprEdSpectop)
#else
static void
SeExprEdSpec_stack_print (SeExprEdSpecbottom, SeExprEdSpectop)
    SeExprEdSpectype_int16 *SeExprEdSpecbottom;
    SeExprEdSpectype_int16 *SeExprEdSpectop;
#endif
{
  SeExprEdSpecYYFPRINTF (stderr, "Stack now");
  for (; SeExprEdSpecbottom <= SeExprEdSpectop; SeExprEdSpecbottom++)
    {
      int SeExprEdSpecbot = *SeExprEdSpecbottom;
      SeExprEdSpecYYFPRINTF (stderr, " %d", SeExprEdSpecbot);
    }
  SeExprEdSpecYYFPRINTF (stderr, "\n");
}

# define SeExprEdSpecYY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (SeExprEdSpecdebug)							\
    SeExprEdSpec_stack_print ((Bottom), (Top));				\
} while (SeExprEdSpecYYID (0))


/*------------------------------------------------.
| Report that the SeExprEdSpecYYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
SeExprEdSpec_reduce_print (SeExprEdSpecYYSTYPE *SeExprEdSpecvsp, SeExprEdSpecYYLTYPE *SeExprEdSpeclsp, int SeExprEdSpecrule)
#else
static void
SeExprEdSpec_reduce_print (SeExprEdSpecvsp, SeExprEdSpeclsp, SeExprEdSpecrule)
    SeExprEdSpecYYSTYPE *SeExprEdSpecvsp;
    SeExprEdSpecYYLTYPE *SeExprEdSpeclsp;
    int SeExprEdSpecrule;
#endif
{
  int SeExprEdSpecnrhs = SeExprEdSpecr2[SeExprEdSpecrule];
  int SeExprEdSpeci;
  unsigned long int SeExprEdSpeclno = SeExprEdSpecrline[SeExprEdSpecrule];
  SeExprEdSpecYYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     SeExprEdSpecrule - 1, SeExprEdSpeclno);
  /* The symbols being reduced.  */
  for (SeExprEdSpeci = 0; SeExprEdSpeci < SeExprEdSpecnrhs; SeExprEdSpeci++)
    {
      SeExprEdSpecYYFPRINTF (stderr, "   $%d = ", SeExprEdSpeci + 1);
      SeExprEdSpec_symbol_print (stderr, SeExprEdSpecrhs[SeExprEdSpecprhs[SeExprEdSpecrule] + SeExprEdSpeci],
		       &(SeExprEdSpecvsp[(SeExprEdSpeci + 1) - (SeExprEdSpecnrhs)])
		       , &(SeExprEdSpeclsp[(SeExprEdSpeci + 1) - (SeExprEdSpecnrhs)])		       );
      SeExprEdSpecYYFPRINTF (stderr, "\n");
    }
}

# define SeExprEdSpecYY_REDUCE_PRINT(Rule)		\
do {					\
  if (SeExprEdSpecdebug)				\
    SeExprEdSpec_reduce_print (SeExprEdSpecvsp, SeExprEdSpeclsp, Rule); \
} while (SeExprEdSpecYYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int SeExprEdSpecdebug;
#else /* !SeExprEdSpecYYDEBUG */
# define SeExprEdSpecYYDPRINTF(Args)
# define SeExprEdSpecYY_SYMBOL_PRINT(Title, Type, Value, Location)
# define SeExprEdSpecYY_STACK_PRINT(Bottom, Top)
# define SeExprEdSpecYY_REDUCE_PRINT(Rule)
#endif /* !SeExprEdSpecYYDEBUG */


/* SeExprEdSpecYYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	SeExprEdSpecYYINITDEPTH
# define SeExprEdSpecYYINITDEPTH 200
#endif

/* SeExprEdSpecYYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SeExprEdSpecYYSTACK_ALLOC_MAXIMUM < SeExprEdSpecYYSTACK_BYTES (SeExprEdSpecYYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef SeExprEdSpecYYMAXDEPTH
# define SeExprEdSpecYYMAXDEPTH 10000
#endif


#if SeExprEdSpecYYERROR_VERBOSE

# ifndef SeExprEdSpecstrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define SeExprEdSpecstrlen strlen
#  else
/* Return the length of SeExprEdSpecYYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static SeExprEdSpecYYSIZE_T
SeExprEdSpecstrlen (const char *SeExprEdSpecstr)
#else
static SeExprEdSpecYYSIZE_T
SeExprEdSpecstrlen (SeExprEdSpecstr)
    const char *SeExprEdSpecstr;
#endif
{
  SeExprEdSpecYYSIZE_T SeExprEdSpeclen;
  for (SeExprEdSpeclen = 0; SeExprEdSpecstr[SeExprEdSpeclen]; SeExprEdSpeclen++)
    continue;
  return SeExprEdSpeclen;
}
#  endif
# endif

# ifndef SeExprEdSpecstpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define SeExprEdSpecstpcpy stpcpy
#  else
/* Copy SeExprEdSpecYYSRC to SeExprEdSpecYYDEST, returning the address of the terminating '\0' in
   SeExprEdSpecYYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
SeExprEdSpecstpcpy (char *SeExprEdSpecdest, const char *SeExprEdSpecsrc)
#else
static char *
SeExprEdSpecstpcpy (SeExprEdSpecdest, SeExprEdSpecsrc)
    char *SeExprEdSpecdest;
    const char *SeExprEdSpecsrc;
#endif
{
  char *SeExprEdSpecd = SeExprEdSpecdest;
  const char *SeExprEdSpecs = SeExprEdSpecsrc;

  while ((*SeExprEdSpecd++ = *SeExprEdSpecs++) != '\0')
    continue;

  return SeExprEdSpecd - 1;
}
#  endif
# endif

# ifndef SeExprEdSpectnamerr
/* Copy to SeExprEdSpecYYRES the contents of SeExprEdSpecYYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for SeExprEdSpecerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  SeExprEdSpecYYSTR is taken from SeExprEdSpectname.  If SeExprEdSpecYYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static SeExprEdSpecYYSIZE_T
SeExprEdSpectnamerr (char *SeExprEdSpecres, const char *SeExprEdSpecstr)
{
  if (*SeExprEdSpecstr == '"')
    {
      SeExprEdSpecYYSIZE_T SeExprEdSpecn = 0;
      char const *SeExprEdSpecp = SeExprEdSpecstr;

      for (;;)
	switch (*++SeExprEdSpecp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++SeExprEdSpecp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (SeExprEdSpecres)
	      SeExprEdSpecres[SeExprEdSpecn] = *SeExprEdSpecp;
	    SeExprEdSpecn++;
	    break;

	  case '"':
	    if (SeExprEdSpecres)
	      SeExprEdSpecres[SeExprEdSpecn] = '\0';
	    return SeExprEdSpecn;
	  }
    do_not_strip_quotes: ;
    }

  if (! SeExprEdSpecres)
    return SeExprEdSpecstrlen (SeExprEdSpecstr);

  return SeExprEdSpecstpcpy (SeExprEdSpecres, SeExprEdSpecstr) - SeExprEdSpecres;
}
# endif

/* Copy into *SeExprEdSpecYYMSG, which is of size *SeExprEdSpecYYMSG_ALLOC, an error message
   about the unexpected token SeExprEdSpecYYTOKEN for the state stack whose top is
   SeExprEdSpecYYSSP.

   Return 0 if *SeExprEdSpecYYMSG was successfully written.  Return 1 if *SeExprEdSpecYYMSG is
   not large enough to hold the message.  In that case, also set
   *SeExprEdSpecYYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
SeExprEdSpecsyntax_error (SeExprEdSpecYYSIZE_T *SeExprEdSpecmsg_alloc, char **SeExprEdSpecmsg,
                SeExprEdSpectype_int16 *SeExprEdSpecssp, int SeExprEdSpectoken)
{
  SeExprEdSpecYYSIZE_T SeExprEdSpecsize0 = SeExprEdSpectnamerr (SeExprEdSpecYY_NULL, SeExprEdSpectname[SeExprEdSpectoken]);
  SeExprEdSpecYYSIZE_T SeExprEdSpecsize = SeExprEdSpecsize0;
  enum { SeExprEdSpecYYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *SeExprEdSpecformat = SeExprEdSpecYY_NULL;
  /* Arguments of SeExprEdSpecformat. */
  char const *SeExprEdSpecarg[SeExprEdSpecYYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int SeExprEdSpeccount = 0;

  /* There are many possibilities here to consider:
     - Assume SeExprEdSpecYYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  SeExprEdSpecYYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in SeExprEdSpecchar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated SeExprEdSpecchar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (SeExprEdSpectoken != SeExprEdSpecYYEMPTY)
    {
      int SeExprEdSpecn = SeExprEdSpecpact[*SeExprEdSpecssp];
      SeExprEdSpecarg[SeExprEdSpeccount++] = SeExprEdSpectname[SeExprEdSpectoken];
      if (!SeExprEdSpecpact_value_is_default (SeExprEdSpecn))
        {
          /* Start SeExprEdSpecYYX at -SeExprEdSpecYYN if negative to avoid negative indexes in
             SeExprEdSpecYYCHECK.  In other words, skip the first -SeExprEdSpecYYN actions for
             this state because they are default actions.  */
          int SeExprEdSpecxbegin = SeExprEdSpecn < 0 ? -SeExprEdSpecn : 0;
          /* Stay within bounds of both SeExprEdSpeccheck and SeExprEdSpectname.  */
          int SeExprEdSpecchecklim = SeExprEdSpecYYLAST - SeExprEdSpecn + 1;
          int SeExprEdSpecxend = SeExprEdSpecchecklim < SeExprEdSpecYYNTOKENS ? SeExprEdSpecchecklim : SeExprEdSpecYYNTOKENS;
          int SeExprEdSpecx;

          for (SeExprEdSpecx = SeExprEdSpecxbegin; SeExprEdSpecx < SeExprEdSpecxend; ++SeExprEdSpecx)
            if (SeExprEdSpeccheck[SeExprEdSpecx + SeExprEdSpecn] == SeExprEdSpecx && SeExprEdSpecx != SeExprEdSpecYYTERROR
                && !SeExprEdSpectable_value_is_error (SeExprEdSpectable[SeExprEdSpecx + SeExprEdSpecn]))
              {
                if (SeExprEdSpeccount == SeExprEdSpecYYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    SeExprEdSpeccount = 1;
                    SeExprEdSpecsize = SeExprEdSpecsize0;
                    break;
                  }
                SeExprEdSpecarg[SeExprEdSpeccount++] = SeExprEdSpectname[SeExprEdSpecx];
                {
                  SeExprEdSpecYYSIZE_T SeExprEdSpecsize1 = SeExprEdSpecsize + SeExprEdSpectnamerr (SeExprEdSpecYY_NULL, SeExprEdSpectname[SeExprEdSpecx]);
                  if (! (SeExprEdSpecsize <= SeExprEdSpecsize1
                         && SeExprEdSpecsize1 <= SeExprEdSpecYYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  SeExprEdSpecsize = SeExprEdSpecsize1;
                }
              }
        }
    }

  switch (SeExprEdSpeccount)
    {
# define SeExprEdSpecYYCASE_(N, S)                      \
      case N:                               \
        SeExprEdSpecformat = S;                       \
      break
      SeExprEdSpecYYCASE_(0, SeExprEdSpecYY_("syntax error"));
      SeExprEdSpecYYCASE_(1, SeExprEdSpecYY_("syntax error, unexpected %s"));
      SeExprEdSpecYYCASE_(2, SeExprEdSpecYY_("syntax error, unexpected %s, expecting %s"));
      SeExprEdSpecYYCASE_(3, SeExprEdSpecYY_("syntax error, unexpected %s, expecting %s or %s"));
      SeExprEdSpecYYCASE_(4, SeExprEdSpecYY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      SeExprEdSpecYYCASE_(5, SeExprEdSpecYY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef SeExprEdSpecYYCASE_
    }

  {
    SeExprEdSpecYYSIZE_T SeExprEdSpecsize1 = SeExprEdSpecsize + SeExprEdSpecstrlen (SeExprEdSpecformat);
    if (! (SeExprEdSpecsize <= SeExprEdSpecsize1 && SeExprEdSpecsize1 <= SeExprEdSpecYYSTACK_ALLOC_MAXIMUM))
      return 2;
    SeExprEdSpecsize = SeExprEdSpecsize1;
  }

  if (*SeExprEdSpecmsg_alloc < SeExprEdSpecsize)
    {
      *SeExprEdSpecmsg_alloc = 2 * SeExprEdSpecsize;
      if (! (SeExprEdSpecsize <= *SeExprEdSpecmsg_alloc
             && *SeExprEdSpecmsg_alloc <= SeExprEdSpecYYSTACK_ALLOC_MAXIMUM))
        *SeExprEdSpecmsg_alloc = SeExprEdSpecYYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *SeExprEdSpecp = *SeExprEdSpecmsg;
    int SeExprEdSpeci = 0;
    while ((*SeExprEdSpecp = *SeExprEdSpecformat) != '\0')
      if (*SeExprEdSpecp == '%' && SeExprEdSpecformat[1] == 's' && SeExprEdSpeci < SeExprEdSpeccount)
        {
          SeExprEdSpecp += SeExprEdSpectnamerr (SeExprEdSpecp, SeExprEdSpecarg[SeExprEdSpeci++]);
          SeExprEdSpecformat += 2;
        }
      else
        {
          SeExprEdSpecp++;
          SeExprEdSpecformat++;
        }
  }
  return 0;
}
#endif /* SeExprEdSpecYYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
SeExprEdSpecdestruct (const char *SeExprEdSpecmsg, int SeExprEdSpectype, SeExprEdSpecYYSTYPE *SeExprEdSpecvaluep, SeExprEdSpecYYLTYPE *SeExprEdSpeclocationp)
#else
static void
SeExprEdSpecdestruct (SeExprEdSpecmsg, SeExprEdSpectype, SeExprEdSpecvaluep, SeExprEdSpeclocationp)
    const char *SeExprEdSpecmsg;
    int SeExprEdSpectype;
    SeExprEdSpecYYSTYPE *SeExprEdSpecvaluep;
    SeExprEdSpecYYLTYPE *SeExprEdSpeclocationp;
#endif
{
  SeExprEdSpecYYUSE (SeExprEdSpecvaluep);
  SeExprEdSpecYYUSE (SeExprEdSpeclocationp);

  if (!SeExprEdSpecmsg)
    SeExprEdSpecmsg = "Deleting";
  SeExprEdSpecYY_SYMBOL_PRINT (SeExprEdSpecmsg, SeExprEdSpectype, SeExprEdSpecvaluep, SeExprEdSpeclocationp);

  switch (SeExprEdSpectype)
    {

      default:
        break;
    }
}




/* The lookahead symbol.  */
int SeExprEdSpecchar;


#ifndef SeExprEdSpecYY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define SeExprEdSpecYY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define SeExprEdSpecYY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef SeExprEdSpecYY_INITIAL_VALUE
# define SeExprEdSpecYY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
SeExprEdSpecYYSTYPE SeExprEdSpeclval SeExprEdSpecYY_INITIAL_VALUE(SeExprEdSpecval_default);

/* Location data for the lookahead symbol.  */
SeExprEdSpecYYLTYPE SeExprEdSpeclloc
# if defined SeExprEdSpecYYLTYPE_IS_TRIVIAL && SeExprEdSpecYYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;


/* Number of syntax errors so far.  */
int SeExprEdSpecnerrs;


/*----------.
| SeExprEdSpecparse.  |
`----------*/

#ifdef SeExprEdSpecYYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
SeExprEdSpecparse (void *SeExprEdSpecYYPARSE_PARAM)
#else
int
SeExprEdSpecparse (SeExprEdSpecYYPARSE_PARAM)
    void *SeExprEdSpecYYPARSE_PARAM;
#endif
#else /* ! SeExprEdSpecYYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
SeExprEdSpecparse (void)
#else
int
SeExprEdSpecparse ()

#endif
#endif
{
    int SeExprEdSpecstate;
    /* Number of tokens to shift before error messages enabled.  */
    int SeExprEdSpecerrstatus;

    /* The stacks and their tools:
       `SeExprEdSpecss': related to states.
       `SeExprEdSpecvs': related to semantic values.
       `SeExprEdSpecls': related to locations.

       Refer to the stacks through separate pointers, to allow SeExprEdSpecoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    SeExprEdSpectype_int16 SeExprEdSpecssa[SeExprEdSpecYYINITDEPTH];
    SeExprEdSpectype_int16 *SeExprEdSpecss;
    SeExprEdSpectype_int16 *SeExprEdSpecssp;

    /* The semantic value stack.  */
    SeExprEdSpecYYSTYPE SeExprEdSpecvsa[SeExprEdSpecYYINITDEPTH];
    SeExprEdSpecYYSTYPE *SeExprEdSpecvs;
    SeExprEdSpecYYSTYPE *SeExprEdSpecvsp;

    /* The location stack.  */
    SeExprEdSpecYYLTYPE SeExprEdSpeclsa[SeExprEdSpecYYINITDEPTH];
    SeExprEdSpecYYLTYPE *SeExprEdSpecls;
    SeExprEdSpecYYLTYPE *SeExprEdSpeclsp;

    /* The locations where the error started and ended.  */
    SeExprEdSpecYYLTYPE SeExprEdSpecerror_range[3];

    SeExprEdSpecYYSIZE_T SeExprEdSpecstacksize;

  int SeExprEdSpecn;
  int SeExprEdSpecresult;
  /* Lookahead token as an internal (translated) token number.  */
  int SeExprEdSpectoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  SeExprEdSpecYYSTYPE SeExprEdSpecval;
  SeExprEdSpecYYLTYPE SeExprEdSpecloc;

#if SeExprEdSpecYYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char SeExprEdSpecmsgbuf[128];
  char *SeExprEdSpecmsg = SeExprEdSpecmsgbuf;
  SeExprEdSpecYYSIZE_T SeExprEdSpecmsg_alloc = sizeof SeExprEdSpecmsgbuf;
#endif

#define SeExprEdSpecYYPOPSTACK(N)   (SeExprEdSpecvsp -= (N), SeExprEdSpecssp -= (N), SeExprEdSpeclsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int SeExprEdSpeclen = 0;

  SeExprEdSpecssp = SeExprEdSpecss = SeExprEdSpecssa;
  SeExprEdSpecvsp = SeExprEdSpecvs = SeExprEdSpecvsa;
  SeExprEdSpeclsp = SeExprEdSpecls = SeExprEdSpeclsa;
  SeExprEdSpecstacksize = SeExprEdSpecYYINITDEPTH;

  SeExprEdSpecYYDPRINTF ((stderr, "Starting parse\n"));

  SeExprEdSpecstate = 0;
  SeExprEdSpecerrstatus = 0;
  SeExprEdSpecnerrs = 0;
  SeExprEdSpecchar = SeExprEdSpecYYEMPTY; /* Cause a token to be read.  */
  SeExprEdSpeclsp[0] = SeExprEdSpeclloc;
  goto SeExprEdSpecsetstate;

/*------------------------------------------------------------.
| SeExprEdSpecnewstate -- Push a new state, which is found in SeExprEdSpecstate.  |
`------------------------------------------------------------*/
 SeExprEdSpecnewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  SeExprEdSpecssp++;

 SeExprEdSpecsetstate:
  *SeExprEdSpecssp = SeExprEdSpecstate;

  if (SeExprEdSpecss + SeExprEdSpecstacksize - 1 <= SeExprEdSpecssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      SeExprEdSpecYYSIZE_T SeExprEdSpecsize = SeExprEdSpecssp - SeExprEdSpecss + 1;

#ifdef SeExprEdSpecoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	SeExprEdSpecYYSTYPE *SeExprEdSpecvs1 = SeExprEdSpecvs;
	SeExprEdSpectype_int16 *SeExprEdSpecss1 = SeExprEdSpecss;
	SeExprEdSpecYYLTYPE *SeExprEdSpecls1 = SeExprEdSpecls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if SeExprEdSpecoverflow is a macro.  */
	SeExprEdSpecoverflow (SeExprEdSpecYY_("memory exhausted"),
		    &SeExprEdSpecss1, SeExprEdSpecsize * sizeof (*SeExprEdSpecssp),
		    &SeExprEdSpecvs1, SeExprEdSpecsize * sizeof (*SeExprEdSpecvsp),
		    &SeExprEdSpecls1, SeExprEdSpecsize * sizeof (*SeExprEdSpeclsp),
		    &SeExprEdSpecstacksize);

	SeExprEdSpecls = SeExprEdSpecls1;
	SeExprEdSpecss = SeExprEdSpecss1;
	SeExprEdSpecvs = SeExprEdSpecvs1;
      }
#else /* no SeExprEdSpecoverflow */
# ifndef SeExprEdSpecYYSTACK_RELOCATE
      goto SeExprEdSpecexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (SeExprEdSpecYYMAXDEPTH <= SeExprEdSpecstacksize)
	goto SeExprEdSpecexhaustedlab;
      SeExprEdSpecstacksize *= 2;
      if (SeExprEdSpecYYMAXDEPTH < SeExprEdSpecstacksize)
	SeExprEdSpecstacksize = SeExprEdSpecYYMAXDEPTH;

      {
	SeExprEdSpectype_int16 *SeExprEdSpecss1 = SeExprEdSpecss;
	union SeExprEdSpecalloc *SeExprEdSpecptr =
	  (union SeExprEdSpecalloc *) SeExprEdSpecYYSTACK_ALLOC (SeExprEdSpecYYSTACK_BYTES (SeExprEdSpecstacksize));
	if (! SeExprEdSpecptr)
	  goto SeExprEdSpecexhaustedlab;
	SeExprEdSpecYYSTACK_RELOCATE (SeExprEdSpecss_alloc, SeExprEdSpecss);
	SeExprEdSpecYYSTACK_RELOCATE (SeExprEdSpecvs_alloc, SeExprEdSpecvs);
	SeExprEdSpecYYSTACK_RELOCATE (SeExprEdSpecls_alloc, SeExprEdSpecls);
#  undef SeExprEdSpecYYSTACK_RELOCATE
	if (SeExprEdSpecss1 != SeExprEdSpecssa)
	  SeExprEdSpecYYSTACK_FREE (SeExprEdSpecss1);
      }
# endif
#endif /* no SeExprEdSpecoverflow */

      SeExprEdSpecssp = SeExprEdSpecss + SeExprEdSpecsize - 1;
      SeExprEdSpecvsp = SeExprEdSpecvs + SeExprEdSpecsize - 1;
      SeExprEdSpeclsp = SeExprEdSpecls + SeExprEdSpecsize - 1;

      SeExprEdSpecYYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) SeExprEdSpecstacksize));

      if (SeExprEdSpecss + SeExprEdSpecstacksize - 1 <= SeExprEdSpecssp)
	SeExprEdSpecYYABORT;
    }

  SeExprEdSpecYYDPRINTF ((stderr, "Entering state %d\n", SeExprEdSpecstate));

  if (SeExprEdSpecstate == SeExprEdSpecYYFINAL)
    SeExprEdSpecYYACCEPT;

  goto SeExprEdSpecbackup;

/*-----------.
| SeExprEdSpecbackup.  |
`-----------*/
SeExprEdSpecbackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  SeExprEdSpecn = SeExprEdSpecpact[SeExprEdSpecstate];
  if (SeExprEdSpecpact_value_is_default (SeExprEdSpecn))
    goto SeExprEdSpecdefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* SeExprEdSpecYYCHAR is either SeExprEdSpecYYEMPTY or SeExprEdSpecYYEOF or a valid lookahead symbol.  */
  if (SeExprEdSpecchar == SeExprEdSpecYYEMPTY)
    {
      SeExprEdSpecYYDPRINTF ((stderr, "Reading a token: "));
      SeExprEdSpecchar = SeExprEdSpecYYLEX;
    }

  if (SeExprEdSpecchar <= SeExprEdSpecYYEOF)
    {
      SeExprEdSpecchar = SeExprEdSpectoken = SeExprEdSpecYYEOF;
      SeExprEdSpecYYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      SeExprEdSpectoken = SeExprEdSpecYYTRANSLATE (SeExprEdSpecchar);
      SeExprEdSpecYY_SYMBOL_PRINT ("Next token is", SeExprEdSpectoken, &SeExprEdSpeclval, &SeExprEdSpeclloc);
    }

  /* If the proper action on seeing token SeExprEdSpecYYTOKEN is to reduce or to
     detect an error, take that action.  */
  SeExprEdSpecn += SeExprEdSpectoken;
  if (SeExprEdSpecn < 0 || SeExprEdSpecYYLAST < SeExprEdSpecn || SeExprEdSpeccheck[SeExprEdSpecn] != SeExprEdSpectoken)
    goto SeExprEdSpecdefault;
  SeExprEdSpecn = SeExprEdSpectable[SeExprEdSpecn];
  if (SeExprEdSpecn <= 0)
    {
      if (SeExprEdSpectable_value_is_error (SeExprEdSpecn))
        goto SeExprEdSpecerrlab;
      SeExprEdSpecn = -SeExprEdSpecn;
      goto SeExprEdSpecreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (SeExprEdSpecerrstatus)
    SeExprEdSpecerrstatus--;

  /* Shift the lookahead token.  */
  SeExprEdSpecYY_SYMBOL_PRINT ("Shifting", SeExprEdSpectoken, &SeExprEdSpeclval, &SeExprEdSpeclloc);

  /* Discard the shifted token.  */
  SeExprEdSpecchar = SeExprEdSpecYYEMPTY;

  SeExprEdSpecstate = SeExprEdSpecn;
  SeExprEdSpecYY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++SeExprEdSpecvsp = SeExprEdSpeclval;
  SeExprEdSpecYY_IGNORE_MAYBE_UNINITIALIZED_END
  *++SeExprEdSpeclsp = SeExprEdSpeclloc;
  goto SeExprEdSpecnewstate;


/*-----------------------------------------------------------.
| SeExprEdSpecdefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
SeExprEdSpecdefault:
  SeExprEdSpecn = SeExprEdSpecdefact[SeExprEdSpecstate];
  if (SeExprEdSpecn == 0)
    goto SeExprEdSpecerrlab;
  goto SeExprEdSpecreduce;


/*-----------------------------.
| SeExprEdSpecreduce -- Do a reduction.  |
`-----------------------------*/
SeExprEdSpecreduce:
  /* SeExprEdSpecn is the number of a rule to reduce with.  */
  SeExprEdSpeclen = SeExprEdSpecr2[SeExprEdSpecn];

  /* If SeExprEdSpecYYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets SeExprEdSpecYYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to SeExprEdSpecYYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that SeExprEdSpecYYVAL may be used uninitialized.  */
  SeExprEdSpecval = SeExprEdSpecvsp[1-SeExprEdSpeclen];

  /* Default location.  */
  SeExprEdSpecYYLLOC_DEFAULT (SeExprEdSpecloc, (SeExprEdSpeclsp - SeExprEdSpeclen), SeExprEdSpeclen);
  SeExprEdSpecYY_REDUCE_PRINT (SeExprEdSpecn);
  switch (SeExprEdSpecn)
    {
        case 2:
/* Line 1792 of yacc.c  */
#line 313 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { ParseResult = 0; }
    break;

  case 3:
/* Line 1792 of yacc.c  */
#line 314 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { ParseResult = 0; }
    break;

  case 4:
/* Line 1792 of yacc.c  */
#line 319 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 5:
/* Line 1792 of yacc.c  */
#line 320 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 6:
/* Line 1792 of yacc.c  */
#line 324 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 7:
/* Line 1792 of yacc.c  */
#line 325 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 8:
/* Line 1792 of yacc.c  */
#line 330 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 9:
/* Line 1792 of yacc.c  */
#line 331 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    {
        specRegisterVariable((SeExprEdSpecvsp[(1) - (4)].s));
        specRegisterEditable((SeExprEdSpecvsp[(1) - (4)].s),(SeExprEdSpecvsp[(3) - (4)].n));
      }
    break;

  case 10:
/* Line 1792 of yacc.c  */
#line 335 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 11:
/* Line 1792 of yacc.c  */
#line 336 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 12:
/* Line 1792 of yacc.c  */
#line 337 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 13:
/* Line 1792 of yacc.c  */
#line 338 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 14:
/* Line 1792 of yacc.c  */
#line 339 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 15:
/* Line 1792 of yacc.c  */
#line 340 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 16:
/* Line 1792 of yacc.c  */
#line 341 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    {
        specRegisterVariable((SeExprEdSpecvsp[(1) - (4)].s));
        specRegisterEditable((SeExprEdSpecvsp[(1) - (4)].s),(SeExprEdSpecvsp[(3) - (4)].n));
      }
    break;

  case 17:
/* Line 1792 of yacc.c  */
#line 345 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    {  (SeExprEdSpecval.n) = 0; }
    break;

  case 18:
/* Line 1792 of yacc.c  */
#line 346 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    {  (SeExprEdSpecval.n) = 0; }
    break;

  case 19:
/* Line 1792 of yacc.c  */
#line 347 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    {  (SeExprEdSpecval.n) = 0; }
    break;

  case 20:
/* Line 1792 of yacc.c  */
#line 348 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    {  (SeExprEdSpecval.n) = 0; }
    break;

  case 21:
/* Line 1792 of yacc.c  */
#line 349 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    {  (SeExprEdSpecval.n) = 0; }
    break;

  case 22:
/* Line 1792 of yacc.c  */
#line 350 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    {  (SeExprEdSpecval.n) = 0; }
    break;

  case 23:
/* Line 1792 of yacc.c  */
#line 355 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 24:
/* Line 1792 of yacc.c  */
#line 359 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 25:
/* Line 1792 of yacc.c  */
#line 360 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0;}
    break;

  case 26:
/* Line 1792 of yacc.c  */
#line 361 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0;}
    break;

  case 27:
/* Line 1792 of yacc.c  */
#line 366 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 28:
/* Line 1792 of yacc.c  */
#line 367 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    {
        if(SPEC_IS_NUMBER((SeExprEdSpecvsp[(2) - (7)].n)) && SPEC_IS_NUMBER((SeExprEdSpecvsp[(4) - (7)].n)) && SPEC_IS_NUMBER((SeExprEdSpecvsp[(6) - (7)].n))){
            (SeExprEdSpecval.n)=remember(new SeExprSpecVectorNode((SeExprEdSpecloc).first_column,(SeExprEdSpecloc).last_column,(SeExprEdSpecvsp[(2) - (7)].n),(SeExprEdSpecvsp[(4) - (7)].n),(SeExprEdSpecvsp[(6) - (7)].n)));
        }else (SeExprEdSpecval.n)=0;
      }
    break;

  case 29:
/* Line 1792 of yacc.c  */
#line 372 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 30:
/* Line 1792 of yacc.c  */
#line 373 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 31:
/* Line 1792 of yacc.c  */
#line 374 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 32:
/* Line 1792 of yacc.c  */
#line 375 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 33:
/* Line 1792 of yacc.c  */
#line 376 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 34:
/* Line 1792 of yacc.c  */
#line 377 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 35:
/* Line 1792 of yacc.c  */
#line 378 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 36:
/* Line 1792 of yacc.c  */
#line 379 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 37:
/* Line 1792 of yacc.c  */
#line 380 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 38:
/* Line 1792 of yacc.c  */
#line 381 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 39:
/* Line 1792 of yacc.c  */
#line 382 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = (SeExprEdSpecvsp[(2) - (2)].n); }
    break;

  case 40:
/* Line 1792 of yacc.c  */
#line 383 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    {
        if(SPEC_IS_NUMBER((SeExprEdSpecvsp[(2) - (2)].n))){
            SeExprSpecScalarNode* node=(SeExprSpecScalarNode*)(SeExprEdSpecvsp[(2) - (2)].n);
            node->v*=-1;
            node->startPos=(SeExprEdSpecloc).first_column;
            node->endPos=(SeExprEdSpecloc).last_column;
            (SeExprEdSpecval.n)=(SeExprEdSpecvsp[(2) - (2)].n);
        }else (SeExprEdSpecval.n)=0;
      }
    break;

  case 41:
/* Line 1792 of yacc.c  */
#line 392 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 42:
/* Line 1792 of yacc.c  */
#line 393 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 43:
/* Line 1792 of yacc.c  */
#line 394 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 44:
/* Line 1792 of yacc.c  */
#line 395 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 45:
/* Line 1792 of yacc.c  */
#line 396 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 46:
/* Line 1792 of yacc.c  */
#line 397 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 47:
/* Line 1792 of yacc.c  */
#line 398 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 48:
/* Line 1792 of yacc.c  */
#line 399 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0; }
    break;

  case 49:
/* Line 1792 of yacc.c  */
#line 400 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    {
        if((SeExprEdSpecvsp[(3) - (4)].n) && strcmp((SeExprEdSpecvsp[(1) - (4)].s),"curve")==0){
            (SeExprEdSpecval.n)=remember(new SeExprSpecCurveNode((SeExprEdSpecvsp[(3) - (4)].n)));
        }else if((SeExprEdSpecvsp[(3) - (4)].n) && strcmp((SeExprEdSpecvsp[(1) - (4)].s),"ccurve")==0){
            (SeExprEdSpecval.n)=remember(new SeExprSpecCCurveNode((SeExprEdSpecvsp[(3) - (4)].n)));
        }else if((SeExprEdSpecvsp[(3) - (4)].n) && strcmp((SeExprEdSpecvsp[(1) - (4)].s),"swatch")==0){
            (SeExprEdSpecval.n)=remember(new SeExprSpecColorSwatchNode((SeExprEdSpecvsp[(3) - (4)].n)));
        }else if((SeExprEdSpecvsp[(3) - (4)].n) && strcmp((SeExprEdSpecvsp[(1) - (4)].s),"animCurve")==0){
            (SeExprEdSpecval.n)=remember(new SeExprSpecAnimCurveNode((SeExprEdSpecvsp[(3) - (4)].n)));
        }else if((SeExprEdSpecvsp[(3) - (4)].n) && strcmp((SeExprEdSpecvsp[(1) - (4)].s),"deepWater")==0){
            (SeExprEdSpecval.n)=remember(new SeExprSpecDeepWaterNode((SeExprEdSpecvsp[(3) - (4)].n)));
        }else if((SeExprEdSpecvsp[(3) - (4)].n)){
            // function arguments not parse of curve, ccurve, or animCurve
            // check if there are any string args that need to be made into controls
            // but be sure to return 0 as this parseable
            if(SeExprSpecListNode* list=dynamic_cast<SeExprSpecListNode*>((SeExprEdSpecvsp[(3) - (4)].n))){
                for(size_t i=0;i<list->nodes.size();i++){
                    if(SeExprSpecStringNode* str=dynamic_cast<SeExprSpecStringNode*>(list->nodes[i])){
                        specRegisterEditable("<UNKNOWN>",str);
                    }
                }
            }
            (SeExprEdSpecval.n)=0;
        }else (SeExprEdSpecval.n)=0;
      }
    break;

  case 50:
/* Line 1792 of yacc.c  */
#line 425 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    {(SeExprEdSpecval.n) = 0; }
    break;

  case 51:
/* Line 1792 of yacc.c  */
#line 426 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    {  (SeExprEdSpecval.n) = 0; }
    break;

  case 52:
/* Line 1792 of yacc.c  */
#line 427 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    {  (SeExprEdSpecval.n) = 0; }
    break;

  case 53:
/* Line 1792 of yacc.c  */
#line 428 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n)=remember(new SeExprSpecScalarNode((SeExprEdSpecloc).first_column,(SeExprEdSpecloc).last_column,(SeExprEdSpecvsp[(1) - (1)].d))); }
    break;

  case 54:
/* Line 1792 of yacc.c  */
#line 433 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = 0;}
    break;

  case 55:
/* Line 1792 of yacc.c  */
#line 434 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = (SeExprEdSpecvsp[(1) - (1)].n);}
    break;

  case 56:
/* Line 1792 of yacc.c  */
#line 439 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    {
       // ignore first argument unless it is a string (because we parse strings in weird ways)
       SeExprSpecListNode* list=new SeExprSpecListNode((SeExprEdSpecloc).last_column,(SeExprEdSpecloc).last_column);
       if((SeExprEdSpecvsp[(1) - (1)].n) && SPEC_IS_STR((SeExprEdSpecvsp[(1) - (1)].n))){
           list->add((SeExprEdSpecvsp[(1) - (1)].n));
       }
       remember(list);
       (SeExprEdSpecval.n)=list;
   }
    break;

  case 57:
/* Line 1792 of yacc.c  */
#line 448 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    {

      if((SeExprEdSpecvsp[(1) - (3)].n) && (SeExprEdSpecvsp[(3) - (3)].n) && ((SPEC_IS_NUMBER((SeExprEdSpecvsp[(3) - (3)].n)) || SPEC_IS_VECTOR((SeExprEdSpecvsp[(3) - (3)].n)) || SPEC_IS_STR((SeExprEdSpecvsp[(3) - (3)].n))))){
          (SeExprEdSpecval.n)=(SeExprEdSpecvsp[(1) - (3)].n);
          dynamic_cast<SeExprSpecListNode*>((SeExprEdSpecvsp[(1) - (3)].n))->add((SeExprEdSpecvsp[(3) - (3)].n));
      }else{
          (SeExprEdSpecval.n)=0;
      }
    }
    break;

  case 58:
/* Line 1792 of yacc.c  */
#line 460 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    { (SeExprEdSpecval.n) = (SeExprEdSpecvsp[(1) - (1)].n);}
    break;

  case 59:
/* Line 1792 of yacc.c  */
#line 461 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"
    {
        SeExprSpecStringNode* str=new SeExprSpecStringNode((SeExprEdSpecloc).first_column,(SeExprEdSpecloc).last_column,(SeExprEdSpecvsp[(1) - (1)].s));
        //specRegisterEditable("<UNKNOWN>",str);
        // TODO: move string stuff out
        (SeExprEdSpecval.n) = remember(str);
      }
    break;


/* Line 1792 of yacc.c  */
#line 2391 "y.tab.c"
      default: break;
    }
  /* User semantic actions sometimes alter SeExprEdSpecchar, and that requires
     that SeExprEdSpectoken be updated with the new translation.  We take the
     approach of translating immediately before every use of SeExprEdSpectoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     SeExprEdSpecYYABORT, SeExprEdSpecYYACCEPT, or SeExprEdSpecYYERROR immediately after altering SeExprEdSpecchar or
     if it invokes SeExprEdSpecYYBACKUP.  In the case of SeExprEdSpecYYABORT or SeExprEdSpecYYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of SeExprEdSpecYYERROR or SeExprEdSpecYYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  SeExprEdSpecYY_SYMBOL_PRINT ("-> $$ =", SeExprEdSpecr1[SeExprEdSpecn], &SeExprEdSpecval, &SeExprEdSpecloc);

  SeExprEdSpecYYPOPSTACK (SeExprEdSpeclen);
  SeExprEdSpeclen = 0;
  SeExprEdSpecYY_STACK_PRINT (SeExprEdSpecss, SeExprEdSpecssp);

  *++SeExprEdSpecvsp = SeExprEdSpecval;
  *++SeExprEdSpeclsp = SeExprEdSpecloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  SeExprEdSpecn = SeExprEdSpecr1[SeExprEdSpecn];

  SeExprEdSpecstate = SeExprEdSpecpgoto[SeExprEdSpecn - SeExprEdSpecYYNTOKENS] + *SeExprEdSpecssp;
  if (0 <= SeExprEdSpecstate && SeExprEdSpecstate <= SeExprEdSpecYYLAST && SeExprEdSpeccheck[SeExprEdSpecstate] == *SeExprEdSpecssp)
    SeExprEdSpecstate = SeExprEdSpectable[SeExprEdSpecstate];
  else
    SeExprEdSpecstate = SeExprEdSpecdefgoto[SeExprEdSpecn - SeExprEdSpecYYNTOKENS];

  goto SeExprEdSpecnewstate;


/*------------------------------------.
| SeExprEdSpecerrlab -- here on detecting error |
`------------------------------------*/
SeExprEdSpecerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  SeExprEdSpectoken = SeExprEdSpecchar == SeExprEdSpecYYEMPTY ? SeExprEdSpecYYEMPTY : SeExprEdSpecYYTRANSLATE (SeExprEdSpecchar);

  /* If not already recovering from an error, report this error.  */
  if (!SeExprEdSpecerrstatus)
    {
      ++SeExprEdSpecnerrs;
#if ! SeExprEdSpecYYERROR_VERBOSE
      SeExprEdSpecerror (SeExprEdSpecYY_("syntax error"));
#else
# define SeExprEdSpecYYSYNTAX_ERROR SeExprEdSpecsyntax_error (&SeExprEdSpecmsg_alloc, &SeExprEdSpecmsg, \
                                        SeExprEdSpecssp, SeExprEdSpectoken)
      {
        char const *SeExprEdSpecmsgp = SeExprEdSpecYY_("syntax error");
        int SeExprEdSpecsyntax_error_status;
        SeExprEdSpecsyntax_error_status = SeExprEdSpecYYSYNTAX_ERROR;
        if (SeExprEdSpecsyntax_error_status == 0)
          SeExprEdSpecmsgp = SeExprEdSpecmsg;
        else if (SeExprEdSpecsyntax_error_status == 1)
          {
            if (SeExprEdSpecmsg != SeExprEdSpecmsgbuf)
              SeExprEdSpecYYSTACK_FREE (SeExprEdSpecmsg);
            SeExprEdSpecmsg = (char *) SeExprEdSpecYYSTACK_ALLOC (SeExprEdSpecmsg_alloc);
            if (!SeExprEdSpecmsg)
              {
                SeExprEdSpecmsg = SeExprEdSpecmsgbuf;
                SeExprEdSpecmsg_alloc = sizeof SeExprEdSpecmsgbuf;
                SeExprEdSpecsyntax_error_status = 2;
              }
            else
              {
                SeExprEdSpecsyntax_error_status = SeExprEdSpecYYSYNTAX_ERROR;
                SeExprEdSpecmsgp = SeExprEdSpecmsg;
              }
          }
        SeExprEdSpecerror (SeExprEdSpecmsgp);
        if (SeExprEdSpecsyntax_error_status == 2)
          goto SeExprEdSpecexhaustedlab;
      }
# undef SeExprEdSpecYYSYNTAX_ERROR
#endif
    }

  SeExprEdSpecerror_range[1] = SeExprEdSpeclloc;

  if (SeExprEdSpecerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (SeExprEdSpecchar <= SeExprEdSpecYYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (SeExprEdSpecchar == SeExprEdSpecYYEOF)
	    SeExprEdSpecYYABORT;
	}
      else
	{
	  SeExprEdSpecdestruct ("Error: discarding",
		      SeExprEdSpectoken, &SeExprEdSpeclval, &SeExprEdSpeclloc);
	  SeExprEdSpecchar = SeExprEdSpecYYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto SeExprEdSpecerrlab1;


/*---------------------------------------------------.
| SeExprEdSpecerrorlab -- error raised explicitly by SeExprEdSpecYYERROR.  |
`---------------------------------------------------*/
SeExprEdSpecerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     SeExprEdSpecYYERROR and the label SeExprEdSpecerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto SeExprEdSpecerrorlab;

  SeExprEdSpecerror_range[1] = SeExprEdSpeclsp[1-SeExprEdSpeclen];
  /* Do not reclaim the symbols of the rule which action triggered
     this SeExprEdSpecYYERROR.  */
  SeExprEdSpecYYPOPSTACK (SeExprEdSpeclen);
  SeExprEdSpeclen = 0;
  SeExprEdSpecYY_STACK_PRINT (SeExprEdSpecss, SeExprEdSpecssp);
  SeExprEdSpecstate = *SeExprEdSpecssp;
  goto SeExprEdSpecerrlab1;


/*-------------------------------------------------------------.
| SeExprEdSpecerrlab1 -- common code for both syntax error and SeExprEdSpecYYERROR.  |
`-------------------------------------------------------------*/
SeExprEdSpecerrlab1:
  SeExprEdSpecerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      SeExprEdSpecn = SeExprEdSpecpact[SeExprEdSpecstate];
      if (!SeExprEdSpecpact_value_is_default (SeExprEdSpecn))
	{
	  SeExprEdSpecn += SeExprEdSpecYYTERROR;
	  if (0 <= SeExprEdSpecn && SeExprEdSpecn <= SeExprEdSpecYYLAST && SeExprEdSpeccheck[SeExprEdSpecn] == SeExprEdSpecYYTERROR)
	    {
	      SeExprEdSpecn = SeExprEdSpectable[SeExprEdSpecn];
	      if (0 < SeExprEdSpecn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (SeExprEdSpecssp == SeExprEdSpecss)
	SeExprEdSpecYYABORT;

      SeExprEdSpecerror_range[1] = *SeExprEdSpeclsp;
      SeExprEdSpecdestruct ("Error: popping",
		  SeExprEdSpecstos[SeExprEdSpecstate], SeExprEdSpecvsp, SeExprEdSpeclsp);
      SeExprEdSpecYYPOPSTACK (1);
      SeExprEdSpecstate = *SeExprEdSpecssp;
      SeExprEdSpecYY_STACK_PRINT (SeExprEdSpecss, SeExprEdSpecssp);
    }

  SeExprEdSpecYY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++SeExprEdSpecvsp = SeExprEdSpeclval;
  SeExprEdSpecYY_IGNORE_MAYBE_UNINITIALIZED_END

  SeExprEdSpecerror_range[2] = SeExprEdSpeclloc;
  /* Using SeExprEdSpecYYLLOC is tempting, but would change the location of
     the lookahead.  SeExprEdSpecYYLOC is available though.  */
  SeExprEdSpecYYLLOC_DEFAULT (SeExprEdSpecloc, SeExprEdSpecerror_range, 2);
  *++SeExprEdSpeclsp = SeExprEdSpecloc;

  /* Shift the error token.  */
  SeExprEdSpecYY_SYMBOL_PRINT ("Shifting", SeExprEdSpecstos[SeExprEdSpecn], SeExprEdSpecvsp, SeExprEdSpeclsp);

  SeExprEdSpecstate = SeExprEdSpecn;
  goto SeExprEdSpecnewstate;


/*-------------------------------------.
| SeExprEdSpecacceptlab -- SeExprEdSpecYYACCEPT comes here.  |
`-------------------------------------*/
SeExprEdSpecacceptlab:
  SeExprEdSpecresult = 0;
  goto SeExprEdSpecreturn;

/*-----------------------------------.
| SeExprEdSpecabortlab -- SeExprEdSpecYYABORT comes here.  |
`-----------------------------------*/
SeExprEdSpecabortlab:
  SeExprEdSpecresult = 1;
  goto SeExprEdSpecreturn;

#if !defined SeExprEdSpecoverflow || SeExprEdSpecYYERROR_VERBOSE
/*-------------------------------------------------.
| SeExprEdSpecexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
SeExprEdSpecexhaustedlab:
  SeExprEdSpecerror (SeExprEdSpecYY_("memory exhausted"));
  SeExprEdSpecresult = 2;
  /* Fall through.  */
#endif

SeExprEdSpecreturn:
  if (SeExprEdSpecchar != SeExprEdSpecYYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      SeExprEdSpectoken = SeExprEdSpecYYTRANSLATE (SeExprEdSpecchar);
      SeExprEdSpecdestruct ("Cleanup: discarding lookahead",
                  SeExprEdSpectoken, &SeExprEdSpeclval, &SeExprEdSpeclloc);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this SeExprEdSpecYYABORT or SeExprEdSpecYYACCEPT.  */
  SeExprEdSpecYYPOPSTACK (SeExprEdSpeclen);
  SeExprEdSpecYY_STACK_PRINT (SeExprEdSpecss, SeExprEdSpecssp);
  while (SeExprEdSpecssp != SeExprEdSpecss)
    {
      SeExprEdSpecdestruct ("Cleanup: popping",
		  SeExprEdSpecstos[*SeExprEdSpecssp], SeExprEdSpecvsp, SeExprEdSpeclsp);
      SeExprEdSpecYYPOPSTACK (1);
    }
#ifndef SeExprEdSpecoverflow
  if (SeExprEdSpecss != SeExprEdSpecssa)
    SeExprEdSpecYYSTACK_FREE (SeExprEdSpecss);
#endif
#if SeExprEdSpecYYERROR_VERBOSE
  if (SeExprEdSpecmsg != SeExprEdSpecmsgbuf)
    SeExprEdSpecYYSTACK_FREE (SeExprEdSpecmsg);
#endif
  /* Make sure SeExprEdSpecYYID is used.  */
  return SeExprEdSpecYYID (SeExprEdSpecresult);
}


/* Line 2055 of yacc.c  */
#line 469 "/disney/users/jberlin/projects/llvm-sandbox/seexpr/src/SeExprEditor/SeExprSpecParser.y"


/* SeExprEdSpecerror - Report an error.  This is called by the parser.
(Note: the "msg" param is useless as it is usually just "sparse error".
so it's ignored.)
*/
static void SeExprEdSpecerror(const char* /*msg*/)
{
    // find start of line containing error
    int pos = SeExprEdSpecpos(), lineno = 1, start = 0, end = strlen(ParseStr);
    bool multiline = 0;
    for (int i = start; i < pos; i++)
	if (ParseStr[i] == '\n') { start = i + 1; lineno++; multiline=1; }

    // find end of line containing error
    for (int i = end; i > pos; i--)
	if (ParseStr[i] == '\n') { end = i - 1; multiline=1; }

    ParseError = SeExprEdSpectext[0] ? "Syntax error" : "Unexpected end of expression";
    if (multiline) {
	char buff[30];
	snprintf(buff, 30, " at line %d", lineno);
	ParseError += buff;
    }
    if (SeExprEdSpectext[0]) {
	ParseError += " near '";
	ParseError += SeExprEdSpectext;
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

static SeExprInternal::Mutex mutex;

/// Main entry point to parser
bool SeExprSpecParse(std::vector<SeExprEdEditable*>& outputEditables,
    std::vector<std::string>& outputVariables,
    std::vector<std::pair<int,int> >& comments,
    const char* str)
{
    SeExprInternal::AutoMutex locker(mutex);

    /// Make inputs/outputs accessible to parser actions
    editables=&outputEditables;
    variables=&outputVariables;
    ParseStr=str;

    // setup and startup parser
    specResetCounters(comments); // reset lineNumber and columnNumber in scanner
    SeExprEdSpec_buffer_state* buffer = SeExprEdSpec_scan_string(str); // setup lexer
    ParseResult = 0;
    int resultCode = SeExprEdSpecparse(); // parser (don't care if it is a parse error)
    UNUSED(resultCode);
    SeExprEdSpec_delete_buffer(buffer);

    // delete temporary data -- specs(mini parse tree) and tokens(strings)!
    for(size_t i=0;i<specNodes.size();i++) delete specNodes[i];
    specNodes.clear();
    for(size_t i=0;i<tokens.size();i++) free(tokens[i]);
    tokens.clear();
    return true;
}

