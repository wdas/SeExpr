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

/* Don't generate yywrap since everything is in one string */
%option noyywrap
/* Don't generate unput since it's unused and gcc complains... */
%option nounput

%{
#include <ctype.h>
#include <string.h>
#include <string>
#include <math.h>
#include <vector>

#include "SePlatform.h"
#include "SeExprParser.h"

#ifdef SEEXPR_WIN32
#    define YY_NO_UNISTD_H
#    define YY_SKIP_YYWRAP
#endif

#ifndef MAKEDEPEND
#    include "SeExprParser.tab.h"
#endif

// TODO: make this thread safe
static int columnNumber=0; // really buffer position
static int lineNumber=0; // not used
static std::vector<char*>* stringTokens=0;
void resetCounters(std::vector<char*>* stringTokensIn){
   stringTokens=stringTokensIn;
   columnNumber=lineNumber=0;
}

#define YY_USER_ACTION { \
    yylloc.first_line=lineNumber;yylloc.first_column=columnNumber; \
    columnNumber+=yyleng;\
    yylloc.last_column=columnNumber;yylloc.last_line=lineNumber;} 

%}

D                       [0-9]
E                       [Ee][+-]?{D}+
REAL                    {D}+({E})?|{D}*"."{D}+({E})?|{D}+"."{D}*({E})?
IDENT                   [a-zA-Z_][a-zA-Z0-9_.]*

%%
BEGIN(0);

if			{ return IF; }
else			{ return ELSE; }

"||"                    { return OR; }
"&&"                    { return AND; }
"=="                    { return EQ; }
"!="                    { return NE; }
"<="                    { return LE; }
">="                    { return GE; }
"->"			{ return ARROW; }
"+="                    { return AddEq; }
"-="                    { return SubEq; }
"*="                    { return MultEq; }
"/="                    { return DivEq; }
"%="                    { return ModEq; }
"^="                    { return ExpEq; }

PI			{ yylval.d = M_PI; return NUMBER; }
E			{ yylval.d = M_E; return NUMBER; }
linear			{ yylval.d = 0; return NUMBER; }
smooth			{ yylval.d = 1; return NUMBER; }
gaussian		{ yylval.d = 2; return NUMBER; }
box			{ yylval.d = 3; return NUMBER; }

{REAL}			{ yylval.d = atof(yytext); return NUMBER; }
\"(\\\"|[^"\n])*\"	{ /* match quoted string, allow embedded quote, \" */
			  yylval.s = strdup(&yytext[1]); 
                          stringTokens->push_back(yylval.s); 
			  yylval.s[strlen(yylval.s)-1] = '\0';
                          return STR; }
\'(\\\'|[^'\n])*\'	{ /* match quoted string, allow embedded quote, \' */
			  yylval.s = strdup(&yytext[1]); 
                          stringTokens->push_back(yylval.s); 
			  yylval.s[strlen(yylval.s)-1] = '\0';
                          return STR; }
${IDENT}		{ yylval.s = strdup(&yytext[1]); stringTokens->push_back(yylval.s); return VAR; }
${IDENT}"::"{IDENT}	{ yylval.s = strdup(&yytext[1]); stringTokens->push_back(yylval.s); return VAR; }
{IDENT}			{ yylval.s = strdup(yytext); stringTokens->push_back(yylval.s); return NAME; }

"\\n"			/* ignore quoted newline */;
"\\t"			/* ignore quoted tab */;
[ \t\n]			/* ignore whitespace */;
\#([^\\\n]|\\[^n\n])*	/* skip trailing comment (up to end of line) */;

<*>.			{ return yytext[0]; }

%%

/* Gets index of current token (corresponding to yytext).  
   Used for error reporting.
 */
int yypos()
{
    return yy_c_buf_p - YY_CURRENT_BUFFER->yy_ch_buf - yyleng;
}
