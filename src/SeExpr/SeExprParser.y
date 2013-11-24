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
int yylex();
int yypos();
extern int yy_start;
extern char* yytext;
struct yy_buffer_state;
yy_buffer_state* yy_scan_string(const char *str);
void yy_delete_buffer(yy_buffer_state*);

/*******************
 parser declarations
 *******************/

// forward declaration
static void yyerror(const char* msg);

// local data
static const char* ParseStr;    // string being parsed
static std::string ParseError;  // error (set from yyerror)
static SeExprNode* ParseResult; // must set result here since yyparse can't return it
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
%}

%union {
    SeExprNode* n; /* a node is returned for all non-terminals to
		      build the parse tree from the leaves up. */
    double d;      // return value for number tokens
    char* s;       /* return value for name tokens.  Note: the string
		      is allocated with strdup() in the lexer and must
		      be freed with free() */
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
      assigns e                 { ParseResult = NODE2(@$.first_column,@$.last_column,BlockNode, $1, $2); }
    | e                         { ParseResult = $1; }
    ;

/* local variable assignments */
optassigns:
      /* empty */		{ $$ = NODE(@$.first_column,@$.last_column,Node); /* create empty node */; }
    | assigns			{ $$ = $1; }
    ;

assigns:
      assign			{ $$ = NODE1(@$.first_column,@$.last_column,Node, $1); /* create var list */}
    | assigns assign		{ $$ = $1; $1->addChild($2); /* add to list */}
    ;
 

assign:
      ifthenelse		{ $$ = $1; }
    | VAR '=' e ';'		{ $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, $3);  }
    | VAR AddEq e ';'              {SeExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                                SeExprNode* opNode=NODE2(@3.first_column,@3.first_column,AddNode,varNode,$3);
                                $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);}
    | VAR SubEq e ';'              {SeExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                                SeExprNode* opNode=NODE2(@3.first_column,@3.first_column,SubNode,varNode,$3);
                                $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);}
    | VAR MultEq e ';'              {SeExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                                SeExprNode* opNode=NODE2(@3.first_column,@3.first_column,MulNode,varNode,$3);
                                $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);}
    | VAR DivEq e ';'              {SeExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                                SeExprNode* opNode=NODE2(@3.first_column,@3.first_column,DivNode,varNode,$3);
                                $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);}
    | VAR ExpEq e ';'              {SeExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                                SeExprNode* opNode=NODE2(@3.first_column,@3.first_column,ExpNode,varNode,$3);
                                $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);}
    | VAR ModEq e ';'              {SeExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                                SeExprNode* opNode=NODE2(@3.first_column,@3.first_column,ModNode,varNode,$3);
                                $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);}
    | NAME '=' e ';'		{ $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, $3);  }
    | NAME AddEq e ';'              {SeExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                                SeExprNode* opNode=NODE2(@3.first_column,@3.first_column,AddNode,varNode,$3);
                                $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);}
    | NAME SubEq e ';'              {SeExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                                SeExprNode* opNode=NODE2(@3.first_column,@3.first_column,SubNode,varNode,$3);
                                $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);}
    | NAME MultEq e ';'              {SeExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                                SeExprNode* opNode=NODE2(@3.first_column,@3.first_column,MulNode,varNode,$3);
                                $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);}
    | NAME DivEq e ';'              {SeExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                                SeExprNode* opNode=NODE2(@3.first_column,@3.first_column,DivNode,varNode,$3);
                                $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);}
    | NAME ExpEq e ';'              {SeExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                                SeExprNode* opNode=NODE2(@3.first_column,@3.first_column,ExpNode,varNode,$3);
                                $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);}
    | NAME ModEq e ';'              {SeExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                                SeExprNode* opNode=NODE2(@3.first_column,@3.first_column,ModNode,varNode,$3);
                                $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);}
    ;

ifthenelse:
    IF '(' e ')' '{' optassigns '}' optelse
				{ $$ = NODE3(@$.first_column,@$.last_column,IfThenElseNode, $3, $6, $8); }
    ;

optelse:
      /* empty */		{ $$ = NODE(@$.first_column,@$.last_column,Node); /* create empty node */ }
    | ELSE '{' optassigns '}'   { $$ = $3; }
    | ELSE ifthenelse		{ $$ = $2; }
    ;

/* An expression or sub-expression */
e:
      '(' e ')'			{ $$ = $2; }
    | '[' e ',' e ',' e ']'     { $$ = NODE3(@$.first_column,@$.last_column,VecNode, $2, $4, $6); }
    | e '[' e ']'               { $$ = NODE2(@$.first_column,@$.last_column,SubscriptNode, $1, $3); }
    | e '?' e ':' e		{ $$ = NODE3(@$.first_column,@$.last_column,CondNode, $1, $3, $5); }
    | e OR e			{ $$ = NODE2(@$.first_column,@$.last_column,OrNode, $1, $3); }
    | e AND e			{ $$ = NODE2(@$.first_column,@$.last_column,AndNode, $1, $3); }
    | e EQ e			{ $$ = NODE2(@$.first_column,@$.last_column,EqNode, $1, $3); }
    | e NE e			{ $$ = NODE2(@$.first_column,@$.last_column,NeNode, $1, $3); }
    | e '<' e			{ $$ = NODE2(@$.first_column,@$.last_column,LtNode, $1, $3); }
    | e '>' e			{ $$ = NODE2(@$.first_column,@$.last_column,GtNode, $1, $3); }
    | e LE e			{ $$ = NODE2(@$.first_column,@$.last_column,LeNode, $1, $3); }
    | e GE e			{ $$ = NODE2(@$.first_column,@$.last_column,GeNode, $1, $3); }
    | '+' e %prec UNARY		{ $$ = $2; }
    | '-' e %prec UNARY		{ $$ = NODE1(@$.first_column,@$.last_column,NegNode, $2); }
    | '!' e			{ $$ = NODE1(@$.first_column,@$.last_column,NotNode, $2); }
    | '~' e			{ $$ = NODE1(@$.first_column,@$.last_column,InvertNode, $2); }
    | e '+' e			{ $$ = NODE2(@$.first_column,@$.last_column,AddNode, $1, $3); }
    | e '-' e			{ $$ = NODE2(@$.first_column,@$.last_column,SubNode, $1, $3); }
    | e '*' e			{ $$ = NODE2(@$.first_column,@$.last_column,MulNode, $1, $3); }
    | e '/' e			{ $$ = NODE2(@$.first_column,@$.last_column,DivNode, $1, $3); }
    | e '%' e			{ $$ = NODE2(@$.first_column,@$.last_column,ModNode, $1, $3); }
    | e '^' e			{ $$ = NODE2(@$.first_column,@$.last_column,ExpNode, $1, $3); }
    | NAME '(' optargs ')'	{ $$ = NODE1(@$.first_column,@$.last_column,FuncNode, $1); 
				  // add args directly and discard arg list node
				  $$->addChildren($3); Forget($3); }
    | e ARROW NAME '(' optargs ')'
    				{ $$ = NODE1(@$.first_column,@$.last_column,FuncNode, $3); 
				  $$->addChild($1);
				  // add args directly and discard arg list node
				  $$->addChildren($5); Forget($5); } 
    | VAR			{ $$ = NODE1(@$.first_column,@$.last_column,VarNode, $1); }
    | NAME			{ $$ = NODE1(@$.first_column,@$.last_column,VarNode, $1); }
    | NUMBER			{ $$ = NODE1(@$.first_column,@$.last_column,NumNode, $1); /*printf("line %d",@$.last_column);*/}
    ;

/* An optional argument list */
optargs:
      /* empty */		{ $$ = NODE(@$.first_column,@$.last_column,Node); /* create empty node */}
    | args			{ $$ = $1; }
    ;

/* Argument list (comma-separated expression list) */
args:
      arg			{ $$ = NODE1(@$.first_column,@$.last_column,Node, $1); /* create arg list */}
    | args ',' arg		{ $$ = $1; $1->addChild($3); /* add to list */}
    ;

arg:
      e				{ $$ = $1; }
    | STR			{ $$ = NODE1(@$.first_column,@$.last_column,StrNode, $1);}
    ;

%%

      /* yyerror - Report an error.  This is called by the parser.
	 (Note: the "msg" param is useless as it is usually just "parse error".
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
    yy_buffer_state* buffer = yy_scan_string(str);
    ParseResult = 0;
    int resultCode = yyparse();
    yy_delete_buffer(buffer);
    SeExprlex_destroy ();
    if (resultCode == 0) {
	// success
	error = "";
	parseTree = ParseResult;
    }
    else {
	// failure
	error = ParseError;
        errorStart=yylloc.first_column;
        errorEnd=yylloc.last_column;
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
