/*
 Copyright Disney Enterprises, Inc.  All rights reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License
 and the following modification to it: Section 6 Trademarks.
 deleted and replaced with:

 6. Trademarks. This License does not grant permission to use the
 trade names, trademarks, service marks, or product names of the
 Licensor and its affiliates, except as required for reproducing
 the content of the NOTICE file.

 You may obtain a copy of the License at
 http://www.apache.org/licenses/LICENSE-2.0
*/

%{
#ifndef MAKEDEPEND
#include <algorithm>
#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>
#endif
#include "ExprType.h"
#include "ExprNode.h"
#include "Expression.h"
#include "ExprParserLex.h"
#include "ExprParseAction.h"
#include "ExprParser.tab.h"
#include "Mutex.h"

/*******************
 parser declarations
 *******************/

// forward declarations
int SeExpr2lex(YYSTYPE* yylval_param, YYLTYPE* yyloc, void* yyscanner, SeExpr2::ParseState *state, SeExpr2::ParseData *parseData);
void SeExpr2error(YYLTYPE* yyloc, void* scanner, SeExpr2::ParseState *state, SeExpr2::ParseData* parseData, char const* msg);
char* getText (void* yyscanner);

/* These are handy node constructors for 0-3 arguments */
#define NODE(startPos,endPos,name) parseData->Remember(new SeExpr2::Expr##name(parseData->Expr),startPos,endPos)
#define NODE1(startPos,endPos,name,a) parseData->Remember(new SeExpr2::Expr##name(parseData->Expr,a),startPos,endPos)
#define NODE2(startPos,endPos,name,a,b) parseData->Remember(new SeExpr2::Expr##name(parseData->Expr,a,b),startPos,endPos)
#define NODE3(startPos,endPos,name,a,b,c) parseData->Remember(new SeExpr2::Expr##name(parseData->Expr,a,b,c),startPos,endPos)
#define NODE4(startPos,endPos,name,a,b,c,t) parseData->Remember(new SeExpr2::Expr##name(parseData->Expr,a,b,c,t),startPos,endPos)
%}

// Make re-entrant
%define api.pure full

// additional parameters for lex and parse
%param {void* scanner} {SeExpr2::ParseState *state} {SeExpr2::ParseData *parseData}

// Need for errors and comments
%locations

%union YYSTYPE {
    SeExpr2::ExprNode* n; /* a node is returned for all non-terminals to
                             build the parse tree from the leaves up. */
    double d;             /* return value for number tokens */
    char* s;              /* return value for name tokens.  Note: the string
                             is allocated with strdup() in the lexer and must
                             be freed with free() */
    struct {
        SeExpr2::ExprType::Type     type;
        int                         dim;
        SeExpr2::ExprType::Lifetime lifetime;
    } t;                           /* return value for types */

    SeExpr2::ExprType::Lifetime l; /* return value for lifetime qualifiers */
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
%left '<' '>' SEEXPR_LE SEEXPR_GE
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
      declarationList block     { parseData->ParseResult = $1; 
                                  parseData->ParseResult->setPosition(@$.first_column, @$.last_column);
                                  parseData->ParseResult->addChild($2); }
    | block                     { parseData->ParseResult = NODE(@$.first_column, @$.last_column, ModuleNode);
                                  parseData->ParseResult->addChild($1); }
    ;

declarationList:
      declaration               { $$ = NODE(@$.first_column, @$.last_column, ModuleNode); $$->addChild($1); }
    | declarationList declaration
                                { $$ = $1; $$->setPosition(@$.first_column, @$.last_column);
                                  $$->addChild($2); }
    ;

declaration:
      EXTERN typeDeclare NAME '(' typeListOptional       ')'
                                { SeExpr2::ExprType type = SeExpr2::ExprType($2.type, $2.dim, $2.lifetime);
                                  SeExpr2::ExprPrototypeNode * prototype =
                                      (SeExpr2::ExprPrototypeNode*)NODE2(@$.first_column, @$.last_column, PrototypeNode, $3, type);
                                  prototype->addArgTypes($5);
                                  parseData->Forget($5);
                                  $$ = prototype;
                                  free($3); }
    | DEF    typeDeclare NAME '(' formalTypeListOptional ')' '{' block '}'
                                { SeExpr2::ExprType type = SeExpr2::ExprType($2.type, $2.dim, $2.lifetime);
                                  SeExpr2::ExprPrototypeNode * prototype =
                                      (SeExpr2::ExprPrototypeNode*)NODE2(@$.first_column, @6.last_column, PrototypeNode, $3, type);
                                  prototype->addArgs($5);
                                  parseData->Forget($5);
                                  $$ = NODE2(@$.first_column, @$.last_column, LocalFunctionNode, prototype, $8);
                                  free($3); }
    | DEF                NAME '(' formalTypeListOptional ')' '{' block '}'
                                { SeExpr2::ExprPrototypeNode * prototype =
                                      (SeExpr2::ExprPrototypeNode*)NODE1(@$.first_column, @5.last_column, PrototypeNode, $2);
                                  prototype->addArgs($4);
                                  parseData->Forget($4);
                                  $$ = NODE2(@$.first_column, @$.last_column, LocalFunctionNode, prototype, $7);
                                  free($2); }
    ;

lifetimeOptional:
      /* empty */               { $$ = SeExpr2::ExprType::ltVARYING; }
    | LIFETIME_CONSTANT         { $$ = SeExpr2::ExprType::ltCONSTANT; }
    | LIFETIME_UNIFORM          { $$ = SeExpr2::ExprType::ltUNIFORM; }
    | LIFETIME_VARYING          { $$ = SeExpr2::ExprType::ltVARYING; }
    | LIFETIME_ERROR            { $$ = SeExpr2::ExprType::ltERROR; } //For testing purposes only
    ;

typeDeclare:
      FLOATPOINT lifetimeOptional{$$.type = SeExpr2::ExprType::tFP;
                                  $$.dim = 1;
                                  $$.lifetime = $2; }
    | FLOATPOINT '[' NUMBER ']' lifetimeOptional
                                { $$.type = ($3 > 0 ? SeExpr2::ExprType::tFP : SeExpr2::ExprType::tERROR);
                                  //TODO: This causes an error but does not report it to user. Change this.
                                  $$.dim  = ($3 > 0 ? $3 : 0);
                                  $$.lifetime = $5; }
    | STRING lifetimeOptional   { $$.type = SeExpr2::ExprType::tSTRING;
                                  $$.dim  = 1;
                                  $$.lifetime = $2; }
    ;

typeListOptional:
      /* empty */               { $$ = NODE(@$.first_column, @$.last_column, Node); }
    | typeList                  { $$ = $1; }
    ;

typeList:
      typeDeclare               { $$ = NODE(@$.first_column, @$.last_column, Node);
                                  SeExpr2::ExprType type = SeExpr2::ExprType($1.type, $1.dim, $1.lifetime);
                                  SeExpr2::ExprNode* varNode = NODE2(@$.first_column, @$.last_column, VarNode, "", type);
                                  $$->addChild(varNode); }
    | typeList ',' typeDeclare  { $$ = $1;
                                  SeExpr2::ExprType type = SeExpr2::ExprType($3.type, $3.dim, $3.lifetime);
                                  SeExpr2::ExprNode* varNode = NODE2(@3.first_column, @3.last_column, VarNode, "", type);
                                  $$->addChild(varNode); }
    ;

formalTypeListOptional:
      /* empty */               { $$ = NODE(@$.first_column, @$.last_column, Node); }
    | formalTypeList            { $$ = $1; }
    ;

formalTypeList:
      typeDeclare NAME          { $$ = NODE(@$.first_column, @$.last_column, Node);
                                  SeExpr2::ExprType type = SeExpr2::ExprType($1.type, $1.dim, $1.lifetime);
                                  SeExpr2::ExprNode* varNode = NODE2(@$.first_column, @$.last_column, VarNode, $2, type);
                                  $$->addChild(varNode);
                                  free($2); }
    | formalTypeList ',' typeDeclare NAME
                                { $$ = $1;
                                  SeExpr2::ExprType type = SeExpr2::ExprType($3.type, $3.dim, $3.lifetime);
                                  SeExpr2::ExprNode* varNode = NODE2(@3.first_column, @4.last_column, VarNode, $4, type);
                                  $$->addChild(varNode);
                                  free($4); }
    ;

block:
      assigns e                 { $$ = NODE2(@$.first_column,@$.last_column,BlockNode, $1, $2); }
    | e                         { $$ = $1; }
    ;

/* local variable assignments */
optassigns:
      /* empty */               { $$ = NODE(@$.first_column,@$.last_column,Node); /* create empty node */; }
    | assigns                   { $$ = $1; }
    ;

assigns:
      assign                    { $$ = NODE1(@$.first_column,@$.last_column,Node, $1); /* create var list */}
    | assigns assign            { $$ = $1; $1->addChild($2); /* add to list */}
    ;

assign:
      ifthenelse                { $$ = $1; }
    | VAR '=' e ';'             { $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, $3); free($1); }
    | VAR AddEq e ';'           { SeExpr2::ExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                                  SeExpr2::ExprNode* opNode=NODE3(@3.first_column,@3.first_column,BinaryOpNode,varNode,$3,'+');
                                  $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);free($1);}
    | VAR SubEq e ';'           { SeExpr2::ExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                                  SeExpr2::ExprNode* opNode=NODE3(@3.first_column,@3.first_column,BinaryOpNode,varNode,$3,'-');
                                  $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);free($1);}
    | VAR MultEq e ';'          { SeExpr2::ExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                                  SeExpr2::ExprNode* opNode=NODE3(@3.first_column,@3.first_column,BinaryOpNode,varNode,$3,'*');
                                  $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);free($1);}
    | VAR DivEq e ';'           { SeExpr2::ExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                                  SeExpr2::ExprNode* opNode=NODE3(@3.first_column,@3.first_column,BinaryOpNode,varNode,$3,'/');
                                  $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);free($1);}
    | VAR ExpEq e ';'           { SeExpr2::ExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                                  SeExpr2::ExprNode* opNode=NODE3(@3.first_column,@3.first_column,BinaryOpNode,varNode,$3,'^');
                                  $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);free($1);}
    | VAR ModEq e ';'           { SeExpr2::ExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                                  SeExpr2::ExprNode* opNode=NODE3(@3.first_column,@3.first_column,BinaryOpNode,varNode,$3,'%');
                                  $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);free($1);}
    | NAME '=' e ';'            { $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, $3); free($1); }
    | NAME AddEq e ';'          { SeExpr2::ExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                                  SeExpr2::ExprNode* opNode=NODE3(@3.first_column,@3.first_column,BinaryOpNode,varNode,$3,'+');
                                  $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);free($1);}
    | NAME SubEq e ';'          { SeExpr2::ExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                                  SeExpr2::ExprNode* opNode=NODE3(@3.first_column,@3.first_column,BinaryOpNode,varNode,$3,'-');
                                  $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);free($1);}
    | NAME MultEq e ';'         { SeExpr2::ExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                                  SeExpr2::ExprNode* opNode=NODE3(@3.first_column,@3.first_column,BinaryOpNode,varNode,$3,'*');
                                  $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);free($1);}
    | NAME DivEq e ';'          { SeExpr2::ExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                                  SeExpr2::ExprNode* opNode=NODE3(@3.first_column,@3.first_column,BinaryOpNode,varNode,$3,'/');
                                  $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);free($1);}
    | NAME ExpEq e ';'          { SeExpr2::ExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                                  SeExpr2::ExprNode* opNode=NODE3(@3.first_column,@3.first_column,BinaryOpNode,varNode,$3,'^');
                                  $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);free($1);}
    | NAME ModEq e ';'          { SeExpr2::ExprNode* varNode=NODE1(@1.first_column,@1.first_column,VarNode, $1);
                                  SeExpr2::ExprNode* opNode=NODE3(@3.first_column,@3.first_column,BinaryOpNode,varNode,$3,'%');
                                  $$ = NODE2(@$.first_column,@$.last_column,AssignNode, $1, opNode);free($1);}
    ;

ifthenelse:
      IF '(' e ')' '{' optassigns '}' optelse
                                   { $$ = NODE3(@$.first_column,@$.last_column,IfThenElseNode, $3, $6, $8); }
    ;

optelse:
      /* empty */                  { $$ = NODE(@$.first_column,@$.last_column,Node); /* create empty node */ }
    | ELSE '{' optassigns '}'      { $$ = $3; }
    | ELSE ifthenelse              { $$ = $2; }
    ;

/* An expression or sub-expression */
e:
      '(' e ')'             { $$ = $2; }
    | '[' exprlist ']'      { SeExpr2::ExprNode* newNode = NODE(@$.first_column,@$.last_column,VecNode); newNode->addChildren($2); parseData->Forget($2); $$=newNode;}
    | e '[' e ']'           { $$ = NODE2(@$.first_column,@$.last_column,SubscriptNode, $1, $3); }
    | e '?' e ':' e         { $$ = NODE3(@$.first_column,@$.last_column,CondNode, $1, $3, $5); }
    | e OR e                { $$ = NODE3(@$.first_column,@$.last_column,CompareNode, $1, $3, '|'); }
    | e AND e               { $$ = NODE3(@$.first_column,@$.last_column,CompareNode, $1, $3, '&'); }
    | e EQ e                { $$ = NODE3(@$.first_column,@$.last_column,CompareEqNode, $1, $3,'='); }
    | e NE e                { $$ = NODE3(@$.first_column,@$.last_column,CompareEqNode, $1, $3,'!'); }
    | e '<' e               { $$ = NODE3(@$.first_column,@$.last_column,CompareNode, $1, $3,'<'); }
    | e '>' e               { $$ = NODE3(@$.first_column,@$.last_column,CompareNode, $1, $3,'>'); }
    | e SEEXPR_LE e         { $$ = NODE3(@$.first_column,@$.last_column,CompareNode, $1, $3,'l'); }
    | e SEEXPR_GE e         { $$ = NODE3(@$.first_column,@$.last_column,CompareNode, $1, $3,'g'); }
    | '+' e %prec UNARY     { $$ = $2; }
    | '-' e %prec UNARY     { $$ = NODE2(@$.first_column,@$.last_column,UnaryOpNode, $2, '-'); }
    | '!' e                 { $$ = NODE2(@$.first_column,@$.last_column,UnaryOpNode, $2, '!'); }
    | '~' e                 { $$ = NODE2(@$.first_column,@$.last_column,UnaryOpNode, $2, '~'); }
    | e '+' e               { $$ = NODE3(@$.first_column,@$.last_column,BinaryOpNode, $1, $3, '+'); }
    | e '-' e               { $$ = NODE3(@$.first_column,@$.last_column,BinaryOpNode, $1, $3, '-'); }
    | e '*' e               { $$ = NODE3(@$.first_column,@$.last_column,BinaryOpNode, $1, $3, '*'); }
    | e '/' e               { $$ = NODE3(@$.first_column,@$.last_column,BinaryOpNode, $1, $3, '/'); }
    | e '%' e               { $$ = NODE3(@$.first_column,@$.last_column,BinaryOpNode, $1, $3, '%'); }
    | e '^' e               { $$ = NODE3(@$.first_column,@$.last_column,BinaryOpNode, $1, $3, '^'); }
    | NAME '(' optargs ')'  { $$ = NODE1(@$.first_column,@$.last_column,FuncNode, $1);
                              free($1); // free name string
                              // add args directly and discard arg list node
                              $$->addChildren($3); parseData->Forget($3); }
    | e ARROW NAME '(' optargs ')'
                            { $$ = NODE1(@$.first_column,@$.last_column,FuncNode, $3);
                              free($3); // free name string
                              $$->addChild($1);
                              // add args directly and discard arg list node
                              $$->addChildren($5); parseData->Forget($5); }
    | VAR                   { $$ = NODE1(@$.first_column,@$.last_column,VarNode, $1); free($1); /* free name string */ }
    | NAME                  { $$ = NODE1(@$.first_column,@$.last_column,VarNode, $1); free($1); /* free name string */ }
    | NUMBER                { $$ = NODE1(@$.first_column,@$.last_column,NumNode, $1); /*printf("line %d",@$.last_column);*/}
    | STR                   { $$ = NODE1(@$.first_column,@$.last_column,StrNode, $1); free($1); /* free string */}
    ;

exprlist:
      e                     { $$ = NODE1(@$.first_column,@$.last_column,Node,$1); }
    | exprlist ',' e        { $$ = $1;
                              $1->addChild($3); }
    ;

/* An optional argument list */
optargs:
      /* empty */           { $$ = NODE(@$.first_column,@$.last_column,Node); /* create empty node */}
    | args                  { $$ = $1; }
    ;

/* Argument list (comma-separated expression list) */
args:
      arg                   { $$ = NODE1(@$.first_column,@$.last_column,Node, $1); /* create arg list */}
    | args ',' arg          { $$ = $1; $1->addChild($3); /* add to list */}
    ;

arg:
      e                     { $$ = $1; }
    ;

%%

/* SeExpr2error - Report an error.  This is called by the parser.
   (Note: the "msg" param is useless as it is usually just "parse error".
   so it's ignored.)
*/

void SeExpr2error(YYLTYPE*,
                  void* scanner, 
                  SeExpr2::ParseState* state, 
                  SeExpr2::ParseData* parseData, 
                  char const* /*msg*/)
{
    // find start of line containing error
    int pos = state->yycolno;
    int lineno = 1, start = 0, end = strlen(parseData->ParseStr);
    bool multiline = 0;
    for (int i = start; i < pos; i++)
        if (parseData->ParseStr[i] == '\n') {
            start = i + 1;
            lineno++;
            multiline=1;
        }

    // find end of line containing error
    for (int i = end; i > pos; i--)
    if (parseData->ParseStr[i] == '\n') { 
        end = i - 1; 
        multiline=1; 
    }

    char *yytext = getText(scanner);
    parseData->ParseError = yytext[0] ? "Syntax error" : "Unexpected end of expression";
    if (multiline) {
        char buff[30];
        snprintf(buff, 30, " at line %d", lineno);
        parseData->ParseError += buff;
    }
    if (yytext[0]) {
        parseData->ParseError += " near '";
        parseData->ParseError += yytext;
    }
    parseData->ParseError += "':\n    ";

    int s = std::max(start, pos-30);
    int e = std::min(end, pos+30);

    if (s != start)
        parseData->ParseError += "...";
    parseData->ParseError += std::string(parseData->ParseStr, s, e-s+1);
    if (e != end)
        parseData->ParseError += "...";

    parseData->errorStart=start;
    parseData->errorEnd=end;
}

