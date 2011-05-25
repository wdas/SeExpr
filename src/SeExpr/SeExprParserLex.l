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

/* Don't generate yywrap since everything is in one string */
%option noyywrap
/* Don't generate unput since it's unused and gcc complains... */
%option nounput

%{
#ifndef MAKEDEPEND
#    include <ctype.h>
#    include <string.h>
#    include <string>
#    include <math.h>
#    include <vector>
#endif

#include "SePlatform.h"
#include "SeExprParser.h"
#include "SeExprNode.h"
#include "SeExpression.h"

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
std::vector<std::pair<int,int> >* comments=0;
//! For lexer's internal use only!
void SeExprLexerResetState(std::vector<std::pair<int,int> >& commentsIn){
    comments=&commentsIn;
    columnNumber=lineNumber=0;
}

int yypos();

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
			  yylval.s[strlen(yylval.s)-1] = '\0';
                          return STR; }
\'(\\\'|[^'\n])*\'	{ /* match quoted string, allow embedded quote, \' */
			  yylval.s = strdup(&yytext[1]); 
			  yylval.s[strlen(yylval.s)-1] = '\0';
                          return STR; }
${IDENT}		{ yylval.s = strdup(&yytext[1]); return VAR; }
${IDENT}"::"{IDENT}	{ yylval.s = strdup(&yytext[1]); return VAR; }
{IDENT}			{ yylval.s = strdup(yytext); return NAME; }

"\\n"			/* ignore quoted newline */;
"\\t"			/* ignore quoted tab */;
[ \t\n]			/* ignore whitespace */;
\#([^\\\n]|\\[^n\n])*	{ /* match comment */ 
                         int startPos=yypos(),endPos=yypos()+strlen(&yytext[1])+1;
			  comments->push_back(std::pair<int,int>(startPos,endPos));}

<*>.			{ return yytext[0]; }

%%

/* Gets index of current token (corresponding to yytext).  
   Used for error reporting.
 */
int yypos()
{
    return yy_c_buf_p - YY_CURRENT_BUFFER->yy_ch_buf - yyleng;
}


