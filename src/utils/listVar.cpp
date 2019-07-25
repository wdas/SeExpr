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

#include <SeExpr2/Expression.h>
#include <SeExpr2/ExprWalker.h>
#include <SeExpr2/ExprPatterns.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>

using namespace SeExpr2;

/// Examiner that builds a list of all variable references
class VarListExaminer : public ConstExaminer {
  public:
    virtual bool examine(T_NODE* examinee)
    {
        if (const ExprVarNode* var = isVariable(examinee)) {
            _varList.push_back(var);
            return false;
        };
        return true;
    }
    virtual void reset()
    {
        _varList.clear();
    };
    inline int length() const
    {
        return _varList.size();
    };
    inline const ExprVarNode* var(int i) const
    {
        return _varList[i];
    };

  private:
    std::vector<const ExprVarNode*> _varList;
};

/**
   @file listVar.cpp
*/
//! Simple expression class to list out variable uses
class ListVarExpr : public Expression {
  public:
    //! Constructor that takes the expression to parse
    ListVarExpr(const std::string& expr) : Expression(expr), _hasWalked(false), examiner(), walker(&examiner){};

    //! Empty constructor
    ListVarExpr() : Expression(), _hasWalked(false), examiner(), walker(&examiner){};

    void walk()
    {
        _hasWalked = true;
        walker.walk(parseTree());
    };

    bool hasWalked()
    {
        return _hasWalked;
    };

    int count() const
    {
        if (isValid() && _hasWalked) {
            return examiner.length();
        };
        return 0;
    };

  private:
    bool _hasWalked;
    VarListExaminer examiner;
    ConstWalker walker;

    //! resolve function that only supports one external variable 'x'
    ExprVarRef* resolveVar(const std::string&) const
    {
        return 0;
    };
};

void quit(const std::string& str)
{
    if (str == "quit" || str == "q")
        exit(0);
};

int main()
{
    ListVarExpr expr;
    std::string str;

    std::cout << "SeExpr Basic Variable Use Finder\n *Note: Does not "
              << "find variables being assigned to, only use.*";

    while (true) {
        std::cout << std::endl << "> ";
        // std::cin >> str;
        getline(std::cin, str);

        if (std::cin.eof()) {
            std::cout << std::endl;
            str = "q";
        };

        quit(str);
        expr.setExpr(str);

        if (!expr.isValid()) {
            std::cerr << "Expression failed: " << expr.parseError() << std::endl;
        } else {
            std::cout << "   " << expr.evalFP() << std::endl;
            expr.walk();
            std::cout << "   number of variable refs: " << expr.count() << std::endl;
        };
    };

    return 0;
}
