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

#pragma once

namespace SeExpr2 {

struct ParseData {
    // error (set from yyerror)
    std::string ParseError;
    int errorStart, errorEnd;

    const char* ParseStr;  // string being parsed
    std::vector<SeExpr2::ExprNode*> ParseNodes;
    SeExpr2::ExprNode* ParseResult;   // must set result here since yyparse can't return it
    const SeExpr2::Expression* Expr;  // used for parenting created SeExprOp's

    /* The list of nodes being built is remembered locally here.
       Eventually (if there are no syntax errors) ownership of the nodes
       will belong solely to the parse tree and the parent expression.
       However, if there is a syntax error, we must loop through this list
       and free any nodes that were allocated before the error to avoid a
       memory leak. */

    inline SeExpr2::ExprNode* Remember(SeExpr2::ExprNode* n, const int startPos, const int endPos)
    {
        ParseNodes.push_back(n);
        n->setPosition(startPos, endPos);
        return n;
    }

    // Remove node from list -- but DON'T delete; it's been added elsewhere in parsetree
    inline void Forget(SeExpr2::ExprNode* n)
    {
        ParseNodes.erase(std::find(ParseNodes.begin(), ParseNodes.end(), n));
    }
};

struct ParseState {
    ParseState(std::vector<std::pair<int, int> >* comments) : yycolno(0), comments(comments)
    {
    }
    int yycolno = 0;  // really buffer position
    std::vector<std::pair<int, int> >* comments = 0;
};
}
