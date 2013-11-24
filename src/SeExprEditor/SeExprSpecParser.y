/*
* Copyright Disney Enterprises, Inc.  All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License
* and the following modification to it: Section 6 Trademarks.
* deleted and replaced with:
*
* 6. Trademarks. This License does not grant permission to use the
* trade names, trademarks, service marks, or product names of the
* Licensor and its affiliates, except as required for reproducing
* the content of the NOTICE file.
*
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
*/

%{
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
int yylex();
int yypos();
extern int yy_start;
extern char* yytext;
struct yy_buffer_state;
yy_buffer_state* yy_scan_string(const char *str);
void yy_delete_buffer(yy_buffer_state*);

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
static std::string ParseError;  // error (set from yyerror)
static SeExprSpecNode* ParseResult; // must set result here since yyparse can't return it


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
static void yyerror(const char* msg);

%}

%union {
    SeExprSpecNode* n;
    double d;      // return value for number tokens
    char* s;       /* return value for name tokens.  Note: UNLIKE the regular parser, this is not strdup()'dthe string */
}

%token IF ELSE
%token <s> NAME VAR STR
%token <d> NUMBER
%token AddEq SubEq MultEq DivEq ExpEq ModEq
%token '(' ')' 
%left ARROW
%nonassoc ':'
%nonassoc '?'
%left OR
%left AND
%left EQ NE
%left '<' '>' LE GE
%left '+' '-'
%left '*' '/' '%'
%right UNARY '!' '~'
%right '^'
%left '['
%type <n> optassigns assigns assign ifthenelse optelse e optargs args arg

/* Some notes about the parse tree construction:

   Each rule first parses its children and then returns a new node
   that implements the particular rule (an arithmetic op, a function
   call, or whatever).  Sometimes the child node is just passed up (in
   the case of a parenthesized expression or a unary '+' for
   instance).  But in all cases, a rule returns a parse node which
   represents a complete sub-tree.  Finally, the "expr" rule returns
   the root node which represents the completed parse tree.
*/
%%

// TODO: Change grammar to have option to choose to allow variables of the form
//       $foo or foo. Currently we allow either.

/* The root expression rule */
expr:
      assigns e                 { ParseResult = 0; }
    | e                         { ParseResult = 0; }
    ;

/* local variable assignments */
optassigns:
      /* empty */		{ $$ = 0; }
    | assigns			{ $$ = 0; }
    ;

assigns:
      assign			{ $$ = 0; }
    | assigns assign		{ $$ = 0; }
    ;
 

assign:
      ifthenelse		{ $$ = 0; }
    | VAR '=' e ';'		{ 
        registerVariable($1);
        registerEditable($1,$3);
      }
    | VAR AddEq e ';'           { $$ = 0; }
    | VAR SubEq e ';'           { $$ = 0; }
    | VAR MultEq e ';'          { $$ = 0; }
    | VAR DivEq e ';'           { $$ = 0; }
    | VAR ExpEq e ';'           { $$ = 0; }
    | VAR ModEq e ';'           { $$ = 0; }
    | NAME '=' e ';'		{ 
        registerVariable($1);
        registerEditable($1,$3);
      }
    | NAME AddEq e ';'          {  $$ = 0; }
    | NAME SubEq e ';'          {  $$ = 0; }
    | NAME MultEq e ';'         {  $$ = 0; }
    | NAME DivEq e ';'          {  $$ = 0; }
    | NAME ExpEq e ';'          {  $$ = 0; }
    | NAME ModEq e ';'          {  $$ = 0; }
    ;

ifthenelse:
    IF '(' e ')' '{' optassigns '}' optelse
    { $$ = 0; }
    ;

optelse:
/* empty */		{ $$ = 0; }
| ELSE '{' optassigns '}'   { $$ = 0;}
| ELSE ifthenelse		{ $$ = 0;}
    ;

/* An expression or sub-expression */
e:
      '(' e ')'			{ $$ = 0; }
    | '[' e ',' e ',' e ']'     { 
        if(SPEC_IS_NUMBER($2) && SPEC_IS_NUMBER($4) && SPEC_IS_NUMBER($6)){
            $$=remember(new SeExprSpecVectorNode(@$.first_column,@$.last_column,$2,$4,$6));
        }else $$=0;
      }
    | e '[' e ']'               { $$ = 0; }
    | e '?' e ':' e		{ $$ = 0; }
    | e OR e			{ $$ = 0; }
    | e AND e			{ $$ = 0; }
    | e EQ e			{ $$ = 0; }
    | e NE e			{ $$ = 0; }
    | e '<' e			{ $$ = 0; }
    | e '>' e			{ $$ = 0; }
    | e LE e			{ $$ = 0; }
    | e GE e			{ $$ = 0; }
    | '+' e %prec UNARY		{ $$ = $2; }
    | '-' e %prec UNARY		{ 
        if(SPEC_IS_NUMBER($2)){
            SeExprSpecScalarNode* node=(SeExprSpecScalarNode*)$2;
            node->v*=-1;
            node->startPos=@$.first_column;
            node->endPos=@$.last_column;
            $$=$2;
        }else $$=0;
      }
    | '!' e			{ $$ = 0; }
    | '~' e			{ $$ = 0; }
    | e '+' e			{ $$ = 0; }
    | e '-' e			{ $$ = 0; }
    | e '*' e			{ $$ = 0; }
    | e '/' e			{ $$ = 0; }
    | e '%' e			{ $$ = 0; } 
    | e '^' e			{ $$ = 0; }
    | NAME '(' optargs ')'	{ 
        if($3 && strcmp($1,"curve")==0){
            $$=remember(new SeExprSpecCurveNode($3));
        }else if($3 && strcmp($1,"ccurve")==0){
            $$=remember(new SeExprSpecCCurveNode($3));
        }else if($3 && strcmp($1,"animCurve")==0){
            $$=remember(new SeExprSpecAnimCurveNode($3));
        }else if($3){
            // function arguments not parse of curve, ccurve, or animCurve 
            // check if there are any string args that need to be made into controls
            // but be sure to return 0 as this parseable
            if(SeExprSpecListNode* list=dynamic_cast<SeExprSpecListNode*>($3)){
                for(size_t i=0;i<list->nodes.size();i++){
                    if(SeExprSpecStringNode* str=dynamic_cast<SeExprSpecStringNode*>(list->nodes[i])){
                        registerEditable("<UNKNOWN>",str);
                    }
                }
            }
            $$=0;
        }else $$=0;
      }
    | e ARROW NAME '(' optargs ')'{$$ = 0; }
    | VAR			{  $$ = 0; }
    | NAME			{  $$ = 0; }
    | NUMBER			{ $$=remember(new SeExprSpecScalarNode(@$.first_column,@$.last_column,$1)); }
    ;

/* An optional argument list */
optargs:
      /* empty */		{ $$ = 0;}
    | args			{ $$ = $1;}
    ;

/* Argument list (comma-separated expression list) */
args:
   arg	{ 
       // ignore first argument unless it is a string (because we parse strings in weird ways)
       SeExprSpecListNode* list=new SeExprSpecListNode(@$.last_column,@$.last_column);
       if($1 && SPEC_IS_STR($1)){
           list->add($1);
       }
       remember(list);
       $$=list;
   }
  | args ',' arg {

      if($1 && $3 && ((SPEC_IS_NUMBER($3) || SPEC_IS_VECTOR($3) || SPEC_IS_STR($3)))){
          $$=$1;
          dynamic_cast<SeExprSpecListNode*>($1)->add($3);
      }else{
          $$=0;
      }
    }
    ;

arg:
      e				{ $$ = $1;}
    | STR			{ 
        SeExprSpecStringNode* str=new SeExprSpecStringNode(@$.first_column,@$.last_column,$1);
        //registerEditable("<UNKNOWN>",str);
        // TODO: move string stuff out
        $$ = remember(str);
      }
    ;

%%

      /* yyerror - Report an error.  This is called by the parser.
	 (Note: the "msg" param is useless as it is usually just "sparse error".
	 so it's ignored.)
      */
static void yyerror(const char* /*msg*/)
{
    // find start of line containing error
    int pos = yypos(), lineno = 1, start = 0, end = strlen(ParseStr);
    bool multiline = 0;
    for (int i = start; i < pos; i++)
	if (ParseStr[i] == '\n') { start = i + 1; lineno++; multiline=1; }

    // find end of line containing error
    for (int i = end; i > pos; i--)
	if (ParseStr[i] == '\n') { end = i - 1; multiline=1; }

    ParseError = yytext[0] ? "Syntax error" : "Unexpected end of expression";
    if (multiline) {
	char buff[30];
	snprintf(buff, 30, " at line %d", lineno);
	ParseError += buff;
    }
    if (yytext[0]) {
	ParseError += " near '";
	ParseError += yytext;
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
    yy_buffer_state* buffer = yy_scan_string(str); // setup lexer
    ParseResult = 0;
    int resultCode = yyparse(); // parser (don't care if it is a parse error)
    UNUSED(resultCode);
    yy_delete_buffer(buffer);

    // delete temporary data -- specs(mini parse tree) and tokens(strings)!
    for(size_t i=0;i<specNodes.size();i++) delete specNodes[i];
    specNodes.clear();
    for(size_t i=0;i<tokens.size();i++) free(tokens[i]);
    tokens.clear();
    return true;
}

