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
#include "SeExprLex.h"
static const char* tokenNames[] = {"END_OF_BUFFER",
                                   "NUM",
                                   "IDENT",
                                   "EXTERN",
                                   "DEF",
                                   "FLOAT",
                                   "STRING",
                                   "CONSTANT",
                                   "UNIFORM",
                                   "VARYING",
                                   "ERROR",
                                   "IF",
                                   "ELSE",
                                   "NOT",            // !
                                   "OR",             // ||
                                   "AND",            // &&
                                   "EQUALS",         // ==
                                   "NOT_EQUALS",     // !=
                                   "ASSIGN",         // =
                                   "GREATER",        // >
                                   "LESS",           // <
                                   "GREATER_EQUAL",  // >=
                                   "LESS_EQUAL",     // <=
                                   "PLUS",           //+
                                   "PLUS_EQUAL",     //+=
                                   "MINUS",          //-
                                   "MINUS_EQUAL",    //-=
                                   "TIMES",          //*
                                   "TIMES_EQUAL",    //*=
                                   "DIVIDE",         //*
                                   "DIVIDE_EQUAL",   // /=
                                   "MOD",            //^
                                   "MOD_EQUAL",      // ^=
                                   "POWER",          //^
                                   "POWER_EQUAL",    // ^=
                                   "ARROW",          //->
                                   "TWIDLE",         //~
                                   "BRACKET_OPEN",   //[
                                   "BRACKET_CLOSE",  //]
                                   "BRACE_OPEN",     //{
                                   "BRACE_CLOSE",    //}
                                   "PAREN_OPEN",     //(
                                   "PAREN_CLOSE",    //)
                                   "QUESTION",
                                   "COLON",
                                   "SEMICOLON",
                                   "COMMA"};

std::string Lexer::getTokenName(Token tok)
{
    return tokenNames[tok];
}

std::map<std::string, Lexer::Token> Lexer::reservedWords;
void Lexer::populateReservedWords()
{
    static bool initialized = false;
    if (!initialized) {
        // TODO: acquire mutex
        initialized = true;
        reservedWords["extern"] = EXTERN;
        reservedWords["def"] = DEF;
        reservedWords["FLOAT"] = FLOAT;
        reservedWords["STRING"] = STRING;
        reservedWords["CONSTANT"] = CONSTANT;
        reservedWords["UNIFORM"] = UNIFORM;
        reservedWords["VARYING"] = VARYING;
        reservedWords["ERROR"] = ERROR;
        reservedWords["if"] = IF;
        reservedWords["else"] = ELSE;
        // TODO: E, PI, linear, gaussian, smooth, box
    }
}

#if TEST_LEXER

int main(int argc, char* argv[])
{
    std::ifstream ifs(argv[1]);
    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    Lexer lexer(content.c_str());
    Lexer::Token token;
    do {
        token = lexer.getToken();
        std::cerr << "Token " << token << " (" << tokenNames[token] << "]"
                  << " '" << lexer.getTokenText() << "'" << std::endl;
    } while (token != Lexer::END_OF_BUFFER);
}

#endif
