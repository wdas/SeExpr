/*
 SEEXPR SOFTWARE
 Copyright 2011 Disney Enterprises, Inc. All rights reserved
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are
 met:
 
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in
 the documentation and/or other materials provided with the
 distribution.
 
 * The names "Disney", "Walt Disney Pictures", "Walt Disney Animation
 Studios" or the names of its contributors may NOT be used to
 endorse or promote products derived from this software without
 specific prior written permission from Walt Disney Pictures.
 
 Disclaimer: THIS SOFTWARE IS PROVIDED BY WALT DISNEY PICTURES AND
 CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR PURPOSE, NONINFRINGEMENT AND TITLE ARE DISCLAIMED.
 IN NO EVENT SHALL WALT DISNEY PICTURES, THE COPYRIGHT HOLDER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND BASED ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*/

%{
#ifndef MAKEDEPEND
#include <algorithm>
#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>
#endif
#include "SeExprType.h"
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
#define NODE4(startPos,endPos,name,a,b,c,t) Remember(new SeExpr##name(Expr,a,b,c,t),startPos,endPos)
%}

%union {
    SeExprNode* n; /* a node is returned for all non-terminals to
		      build the parse tree from the leaves up. */
    double d;      // return value for number tokens
    char* s;       /* return value for name tokens.  Note: the string
		      is allocated with strdup() in the lexer and must
		      be freed with free() */
    struct {
        SeExprType::Type     type;
        int                  dim;
        SeExprType::Lifetime lifetime;
    } t;  // return value for types
    SeExprType::Lifetime l; // return value for lifetime qualifiers
}

%token IF ELSE EXTERN DEF FLOATPOINT STRING
%token <s> NAME VAR STR
%token <d> NUMBER
%token <l> LIFETIME_CONSTANT LIFETIME_UNIFORM LIFETIME_VARYING LIFETIME_ERROR
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
%type <t> typeDeclare
%type <l> lifetimeOptional
%type <n> module declarationList declaration typeListOptional typeList formalTypeListOptional formalTypeList
%type <n> block optassigns assigns assign ifthenelse optelse e optargs args arg exprlist

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
module:
      declarationList block     { ParseResult = $1; ParseResult->setPosition(@$.first_column, @$.last_column);
                                  ParseResult->addChild($2); }
    | block                     { ParseResult = NODE(@$.first_column, @$.last_column, ModuleNode);
                                  ParseResult->addChild($1); }
    ;

declarationList:
      declaration               { $$ = NODE(@$.first_column, @$.last_column, ModuleNode); $$->addChild($1); }
    | declarationList declaration
                                { $$ = $1; $$->setPosition(@$.first_column, @$.last_column);
                                  $$->addChild($2); }
    ;

declaration:
      EXTERN typeDeclare NAME '(' typeListOptional       ')'
                                { SeExprType type = SeExprType($2.type, $2.dim, $2.lifetime);
                                    SeExprPrototypeNode * prototype =
                                        (SeExprPrototypeNode*)NODE2(@$.first_column, @$.last_column, PrototypeNode, $3, type);
                                  prototype->addArgTypes($5);
                                  Forget($5);
                                  $$ = prototype;
                                  free($3); }
    | DEF    typeDeclare NAME '(' formalTypeListOptional ')' '{' block '}'
                                { SeExprType type = SeExprType($2.type, $2.dim, $2.lifetime);
                                  SeExprPrototypeNode * prototype =
                                      (SeExprPrototypeNode*)NODE2(@$.first_column, @6.last_column, PrototypeNode, $3, type);
                                  prototype->addArgs($5);
                                  Forget($5);
                                  $$ = NODE2(@$.first_column, @$.last_column, LocalFunctionNode, prototype, $8);
                                  free($3); }
    | DEF                NAME '(' formalTypeListOptional ')' '{' block '}'
                                { SeExprPrototypeNode * prototype =
                                        (SeExprPrototypeNode*)NODE1(@$.first_column, @5.last_column, PrototypeNode, $2);
                                  prototype->addArgs($4);
                                  Forget($4);
                                  $$ = NODE2(@$.first_column, @$.last_column, LocalFunctionNode, prototype, $7);
                                  free($2); }
    ;

lifetimeOptional:
      /* empty */               { $$ = SeExprType::ltVARYING; }
    | LIFETIME_CONSTANT         { $$ = SeExprType::ltCONSTANT; }
    | LIFETIME_UNIFORM          { $$ = SeExprType::ltUNIFORM; }
    | LIFETIME_VARYING          { $$ = SeExprType::ltVARYING; }
    | LIFETIME_ERROR            { $$ = SeExprType::ltERROR; } //For testing purposes only
    ;

typeDeclare:
      FLOATPOINT lifetimeOptional{$$.type     = SeExprType::tFP;
                                  $$.dim      = 1;
                                  $$.lifetime = $2; }
    | FLOATPOINT '[' NUMBER ']' lifetimeOptional
                                { $$.type = ($3 > 0 ? SeExprType::tFP : SeExprType::tERROR);
                                  //TODO: This causes an error but does not report it to user. Change this.
                                  $$.dim  = ($3 > 0 ? $3 : 0);
                                  $$.lifetime = $5; }
    | STRING lifetimeOptional   { $$.type = SeExprType::tSTRING;
                                  $$.dim  = 1;
                                  $$.lifetime = $2; }
    ;

typeListOptional:
      /* empty */               { $$ = NODE(@$.first_column, @$.last_column, Node); }
    | typeList                  { $$ = $1; }
    ;

typeList:
      typeDeclare               { $$ = NODE(@$.first_column, @$.last_column, Node);
                                  SeExprType type = SeExprType($1.type, $1.dim, $1.lifetime);
                                  SeExprNode* varNode = NODE2(@$.first_column, @$.last_column, VarNode, 0, type);
                                  $$->addChild(varNode); }
    | typeList ',' typeDeclare  { $$ = $1;
                                  SeExprType type = SeExprType($3.type, $3.dim, $3.lifetime);
                                  SeExprNode* varNode = NODE2(@3.first_column, @3.last_column, VarNode, 0, type);
                                  $$->addChild(varNode); }
    ;

formalTypeListOptional:
      /* empty */               { $$ = NODE(@$.first_column, @$.last_column, Node); }
    | formalTypeList            { $$ = $1; }
    ;

formalTypeList:
      typeDeclare VAR           { $$ = NODE(@$.first_column, @$.last_column, Node);
                                  SeExprType type = SeExprType($1.type, $1.dim, $1.lifetime);
                                  SeExprNode* varNode = NODE2(@$.first_column, @$.last_column, VarNode, $2, type);
                                  $$->addChild(varNode);
                                  free($2); }
    | formalTypeList ',' typeDeclare VAR
                                { $$ = $1;
                                  SeExprType type = SeExprType($3.type, $3.dim, $3.lifetime);
                                  SeExprNode* varNode = NODE2(@3.first_column, @4.last_column, VarNode, $4, type);
                                  $$->addChild(varNode);
                                  free($4); }
    ;

block:
      assigns e                 { $$ = NODE2(@$.first_column,@$.last_column,BlockNode, $1, $2); }
    | e                         { $$ = $1; }
    ;

/* local variable assignments */
optassigns:
      /* empty */		{ $$ = NODE(@$.first_column,@$.last_column,Node); /* create empty node */; }
    | assigns			{ $$ = $1; }
    ;

assigns:
      assign  		        { $$ = NODE1(@$.first_column,@$.last_column,Node, $1); /* create var list */}
    | assigns assign    	{ $$ = $1; $1->addChild($2); /* add to list */}
    ;

assign:
      ifthenelse		{ $$ = $1; }
    | VAR '=' e ';'		{ $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, $3); free($1); }
    | VAR AddEq e ';'              {SeExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                               SeExprNode* opNode=NODE3(@3.first_column,@3.first_column,BinaryOpNode,varNode,$3,'+');
                                $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);free($1);}
    | VAR SubEq e ';'              {SeExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                               SeExprNode* opNode=NODE3(@3.first_column,@3.first_column,BinaryOpNode,varNode,$3,'-');
                                $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);free($1);}
    | VAR MultEq e ';'              {SeExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                               SeExprNode* opNode=NODE3(@3.first_column,@3.first_column,BinaryOpNode,varNode,$3,'*');
                                $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);free($1);}
    | VAR DivEq e ';'              {SeExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                               SeExprNode* opNode=NODE3(@3.first_column,@3.first_column,BinaryOpNode,varNode,$3,'/');
                                $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);free($1);}
    | VAR ExpEq e ';'              {SeExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                               SeExprNode* opNode=NODE3(@3.first_column,@3.first_column,BinaryOpNode,varNode,$3,'^');
                                $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);free($1);}
    | VAR ModEq e ';'              {SeExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                               SeExprNode* opNode=NODE3(@3.first_column,@3.first_column,BinaryOpNode,varNode,$3,'%');
                                $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);free($1);}
    | NAME '=' e ';'		{ $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, $3); free($1); }
    | NAME AddEq e ';'              {SeExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                               SeExprNode* opNode=NODE3(@3.first_column,@3.first_column,BinaryOpNode,varNode,$3,'+');
                                $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);free($1);}
    | NAME SubEq e ';'              {SeExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                               SeExprNode* opNode=NODE3(@3.first_column,@3.first_column,BinaryOpNode,varNode,$3,'-');
                                $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);free($1);}
    | NAME MultEq e ';'              {SeExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                               SeExprNode* opNode=NODE3(@3.first_column,@3.first_column,BinaryOpNode,varNode,$3,'*');
                                $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);free($1);}
    | NAME DivEq e ';'              {SeExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                               SeExprNode* opNode=NODE3(@3.first_column,@3.first_column,BinaryOpNode,varNode,$3,'/');
                                $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);free($1);}
    | NAME ExpEq e ';'              {SeExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                               SeExprNode* opNode=NODE3(@3.first_column,@3.first_column,BinaryOpNode,varNode,$3,'^');
                                $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);free($1);}
    | NAME ModEq e ';'              {SeExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                               SeExprNode* opNode=NODE3(@3.first_column,@3.first_column,BinaryOpNode,varNode,$3,'%');
                                $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);free($1);}
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
    | '[' exprlist ']'          { $$ = NODE1(@$.first_column,@$.last_column,VecNode, $2); }
    | e '[' e ']'               { $$ = NODE2(@$.first_column,@$.last_column,SubscriptNode, $1, $3); }
    | e '?' e ':' e		{ $$ = NODE3(@$.first_column,@$.last_column,CondNode, $1, $3, $5); }
    | e OR e			{ $$ = NODE3(@$.first_column,@$.last_column,CompareNode, $1, $3, '|'); }
    | e AND e			{ $$ = NODE3(@$.first_column,@$.last_column,CompareNode, $1, $3, '&'); }
    | e EQ e			{ $$ = NODE3(@$.first_column,@$.last_column,CompareEqNode, $1, $3,'='); }
    | e NE e			{ $$ = NODE3(@$.first_column,@$.last_column,CompareEqNode, $1, $3,'!'); }
    | e '<' e			{ $$ = NODE3(@$.first_column,@$.last_column,CompareNode, $1, $3,'<'); }
    | e '>' e			{ $$ = NODE3(@$.first_column,@$.last_column,CompareNode, $1, $3,'>'); }
    | e LE e			{ $$ = NODE3(@$.first_column,@$.last_column,CompareNode, $1, $3,'l'); }
    | e GE e			{ $$ = NODE3(@$.first_column,@$.last_column,CompareNode, $1, $3,'g'); }
    | '+' e %prec UNARY		{ $$ = $2; }
    | '-' e %prec UNARY		{ $$ = NODE2(@$.first_column,@$.last_column,UnaryOpNode, $2, '-'); }
    | '!' e			{ $$ = NODE2(@$.first_column,@$.last_column,UnaryOpNode, $2, '!'); }
    | '~' e			{ $$ = NODE2(@$.first_column,@$.last_column,UnaryOpNode, $2, '~'); }
    | e '+' e			{ $$ = NODE3(@$.first_column,@$.last_column,BinaryOpNode, $1, $3, '+'); }
    | e '-' e			{ $$ = NODE3(@$.first_column,@$.last_column,BinaryOpNode, $1, $3, '-'); }
    | e '*' e			{ $$ = NODE3(@$.first_column,@$.last_column,BinaryOpNode, $1, $3, '*'); }
    | e '/' e			{ $$ = NODE3(@$.first_column,@$.last_column,BinaryOpNode, $1, $3, '/'); }
    | e '%' e			{ $$ = NODE3(@$.first_column,@$.last_column,BinaryOpNode, $1, $3, '%'); }
    | e '^' e			{ $$ = NODE3(@$.first_column,@$.last_column,BinaryOpNode, $1, $3, '^'); }
    | NAME '(' optargs ')'	{ $$ = NODE1(@$.first_column,@$.last_column,FuncNode, $1); 
				  free($1); // free name string
				  // add args directly and discard arg list node
				  $$->addChildren($3); Forget($3); }
    | e ARROW NAME '(' optargs ')'
    				{ $$ = NODE1(@$.first_column,@$.last_column,FuncNode, $3); 
				  free($3); // free name string
				  $$->addChild($1);
				  // add args directly and discard arg list node
				  $$->addChildren($5); Forget($5); }
    | VAR			{ $$ = NODE1(@$.first_column,@$.last_column,VarNode, $1); free($1); /* free name string */ }
    | NAME			{ $$ = NODE1(@$.first_column,@$.last_column,VarNode, $1); free($1); /* free name string */ }
    | NUMBER			{ $$ = NODE1(@$.first_column,@$.last_column,NumNode, $1); /*printf("line %d",@$.last_column);*/}
    | STR			{ $$ = NODE1(@$.first_column,@$.last_column,StrNode, $1); free($1); /* free string */}
    ;

exprlist:
      e                         { $$ = NODE1(@$.first_column,@$.last_column,Node,$1); }
    | exprlist ',' e            { $$ = $1;
                                  $1->addChild($3); }
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

extern void SeExprLexerResetState(std::vector<std::pair<int,int> >& comments);

static SeExprInternal::Mutex mutex;

bool SeExprParse(SeExprNode*& parseTree,
    std::string& error, int& errorStart, int& errorEnd,
    std::vector<std::pair<int,int> >& comments,
    const SeExpression* expr, const char* str, bool wantVec)
{
    SeExprInternal::AutoMutex locker(mutex);

    // glue around crippled C interface - ugh!
    Expr = expr;
    ParseStr = str;
    SeExprLexerResetState(comments);
    yy_buffer_state* buffer = yy_scan_string(str);
    ParseResult = 0;
    int resultCode = yyparse();
    yy_delete_buffer(buffer);

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

