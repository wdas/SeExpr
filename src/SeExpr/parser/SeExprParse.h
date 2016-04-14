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
#include "SeExprLex.h"

#include <cassert>
#include <vector>
#include <memory>
#include <array>

typedef std::array<int, 2> Range;

template <class Policy>
class SeParser {
    typedef typename Policy::Ptr NodePtr;

  public:
    struct RangeC {
        RangeC(const Range& previous, const Range& pos) : _previous(previous), _current(pos) { _start = pos; }
        const Range operator()() {
            return std::array<int, 2>{_start[0], _current[0]};
        }
        const Range start() const { return _start; }
        const Range toPrevious() const {
            return std::array<int, 2>{_start[0], _previous[1]};
        }

      private:
        Range _start;
        const Range& _previous;
        const Range& _current;
    };

    SeParser(const std::string& inputString)
        : lexer(inputString), lookAheadToken(Lexer::END_OF_BUFFER), lookAheadTokenText(""),
          lookAheadTokenPosition(std::array<int, 2>{{0, 0}}) {}

    void getToken() {
        oldTokenPosition = tokenPosition;
        token = lookAheadToken;
        tokenText = lookAheadTokenText;
        tokenPosition = lookAheadTokenPosition;
        lookAheadToken = lexer.getToken();
        lookAheadTokenText = lexer.getTokenText();
        lookAheadTokenPosition = lexer.getTokenPosition();
        // DEBUG: std::cerr<<"token is "<<tokenText<<std::endl;
    }

    NodePtr parse() {
        getToken();
        getToken();
        NodePtr tree = module();
        if (token != Lexer::END_OF_BUFFER) throw ParseError("Parse error, unexpected continued tokens!");
        return tree;
    }

    NodePtr module() {
        RangeC r(oldTokenPosition, tokenPosition);
        NodePtr moduleTree(new typename Policy::Module(r()));
        while (token == Lexer::DEF) {
            moduleTree->addChild(declaration());
        }
        moduleTree->addChild(block());
        moduleTree->setRange(r.toPrevious());
        return moduleTree;
    }

    void ensure(bool value, const std::string& msg) {
        if (!value) {
            throw ParseError(msg + "\n" + lexer.underlineToken(tokenPosition));
        }
    }
    void ensureAndGetToken(bool value, const std::string& msg) {
        ensure(value, msg);
        getToken();
    }
    void ensureNextTwoTokens(Lexer::Token token1, Lexer::Token token2, const std::string& msg) {
        if (token != token1 && lookAheadToken != token2)
            throw ParseError(msg + "\n" + lexer.underlineToken(tokenPosition));
    }
    NodePtr declaration() {
        RangeC rangec(oldTokenPosition, tokenPosition);
        ensureAndGetToken(token == Lexer::DEF, "Expected the word 'def' in declaration");
        if (token != Lexer::IDENT) typeDeclare();
        ensureAndGetToken(token == Lexer::IDENT, "Expected identifier");
        ensureAndGetToken(token == Lexer::PAREN_OPEN, "Expected open parentheses for function declaration");
        typeList();
        ensureAndGetToken(token == Lexer::PAREN_CLOSE, "Expected close parentheses for function declaration");

        assignBlock(true);

        // TODO: return real types here instead of dummy
        return NodePtr(new typename Policy::Def(rangec()));
    }

    void typeDeclare() {
        ensureAndGetToken(token == Lexer::STRING || token == Lexer::FLOAT, "Expected type 'FLOAT' or type 'STRING'");
        if (token == Lexer::BRACKET_OPEN) {
            getToken();
            ensureAndGetToken(token == Lexer::NUM, "Expected number between brackets in type declaration");
            ensureAndGetToken(token == Lexer::BRACKET_CLOSE, "Expected closing ']'");
        }
        lifetimeOptional();
    }

    void lifetimeOptional() {
        if (token == Lexer::CONSTANT || token == Lexer::UNIFORM || token == Lexer::VARYING || token == Lexer::ERROR) {
            getToken();
        }
    }

    void typeList() {
        while (token != Lexer::PAREN_CLOSE) {
            typeDeclare();
            ensureAndGetToken(token == Lexer::IDENT, "Need variable name in formal parameter declaration");
            if (token == Lexer::COMMA)
                getToken();
            else if (token == Lexer::PAREN_CLOSE)
                break;
            else
                throw ParseError(std::string("Expected a comma or parentheses close here\n") +
                                 lexer.underlineToken(tokenPosition));
        }
    }

    bool isAssignOrMutator(Lexer::Token t) const {
        return t == Lexer::ASSIGN || t == Lexer::PLUS_EQUAL || t == Lexer::MINUS_EQUAL || t == Lexer::TIMES_EQUAL ||
               t == Lexer::DIVIDE_EQUAL || t == Lexer::MOD_EQUAL || t == Lexer::POWER_EQUAL;
    }

    NodePtr block() {
        RangeC r(oldTokenPosition, tokenPosition);
        NodePtr ret(new typename Policy::Block(r()));
        while (1) {
            if (token == Lexer::IF)
                ret->addChild(ifthenelse());
            else if (token == Lexer::IDENT && isAssignOrMutator(lookAheadToken))
                ret->addChild(assign());
            else
                break;  // done with the top part
        }
        ret->addChild(expr());
        ret->setRange(r.toPrevious());
        // ret->print(std::cout);
        return ret;
    }

    /*
        Basic expression grammar
        expr:  expr1 { -> IDENT (args) }*
        expr1: expr2  ? expr : expr
        expr2: expr3 { OR expr3 }*
        expr3: expr4 { AND expr4 }*
        expr4: expr5 { eq|ne expr5 }*
        expr5: expr6 { lt|gt|le|ge expr6 }*
        expr6: expr7 { +|- expr7}*
        expr7: expr8 { *|/|% expr8}*
        expr8: {!|~}* expr9
        expr9: {expr10 ^}* expr10
        expr10: expr11 { [ expr ] }*
        expr11: (expr) | NUM | IDENT | IDENT (args) | STR
        args: expr {, expr}*
    */

    /// expr: expr1 -> expr   (nominally)
    // NOTE: expr must contain IDENT PARENOPEN immediately next in the token stream
    NodePtr expr() {
        RangeC rangec(oldTokenPosition, tokenPosition);
        NodePtr parent = expr1();
        if (token == Lexer::ARROW) {
            getToken();
            std::string identName = tokenText;
            ensureNextTwoTokens(Lexer::IDENT,
                                Lexer::PAREN_OPEN,
                                "Arrow operator requires function call immediately after '->' operator");
            if (curriedArgument.get())
                throw ParseError(std::string("Expected only one curriedArgument in flight, call developers fast.") +
                                 "\n" + lexer.underlineToken(tokenPosition));
            curriedArgument = std::move(parent);
            return expr();
        } else
            return parent;
    }

    // expr1: expr2 ? expr : expr
    NodePtr expr1() {
        RangeC rangec(oldTokenPosition, tokenPosition);
        NodePtr curr = expr2();
        if (token == Lexer::QUESTION) {
            getToken();
            NodePtr res1 = expr();
            ensureAndGetToken(token == Lexer::COLON, "Expected : after ternary started with ?");
            NodePtr res2 = expr();
            curr = NodePtr(new typename Policy::TernaryOp(
                rangec.toPrevious(), '?', std::move(curr), std::move(res1), std::move(res2)));
        }
        return curr;
    }

    // expr2: expr3 { OR expr3 }*
    NodePtr expr2() {
        RangeC rangec(oldTokenPosition, tokenPosition);
        NodePtr curr = expr3();
        while (token == Lexer::OR) {
            std::string text = tokenText;
            getToken();
            curr = NodePtr(new typename Policy::BinaryOp(rangec.toPrevious(), '|', std::move(curr), expr3()));
        }
        return curr;
    }

    // expr3: expr4 { AND expr4 }*
    NodePtr expr3() {
        RangeC rangec(oldTokenPosition, tokenPosition);
        NodePtr curr = expr4();
        while (token == Lexer::AND) {
            std::string text = tokenText;
            getToken();
            curr = NodePtr(new typename Policy::BinaryOp(rangec.toPrevious(), '&', std::move(curr), expr4()));
        }
        return curr;
    }

    // expr4: expr5 { EQ|NE expr5 }*
    NodePtr expr4() {
        RangeC rangec(oldTokenPosition, tokenPosition);
        NodePtr curr = expr5();
        while (token == Lexer::EQUALS || token == Lexer::NOT_EQUALS) {
            std::string text = tokenText;
            getToken();
            curr = NodePtr(new typename Policy::BinaryOp(rangec.toPrevious(), '|', std::move(curr), expr5()));
        }
        return curr;
    }

    // expr5: expr6 { <|<=|>|>= expr6 }*
    NodePtr expr5() {
        RangeC rangec(oldTokenPosition, tokenPosition);
        NodePtr curr = expr6();
        while (1) {
            char op = ' ';
            if (token == Lexer::LESS)
                op = '<';
            else if (token == Lexer::LESS_EQUAL)
                op = 'l';
            else if (token == Lexer::GREATER)
                op = '>';
            else if (token == Lexer::GREATER_EQUAL)
                op = 'g';
            else
                break;
            std::string text = tokenText;
            getToken();
            curr = NodePtr(new typename Policy::BinaryOp(rangec.toPrevious(), op, std::move(curr), expr6()));
        }
        return curr;
    }

    // expr6: expr7 { +|- expr7}*
    NodePtr expr6() {
        RangeC rangec(oldTokenPosition, tokenPosition);
        NodePtr curr = expr7();
        while (token == Lexer::PLUS || token == Lexer::MINUS) {
            char code = token == Lexer::PLUS ? '+' : '-';
            std::string text = tokenText;
            getToken();
            NodePtr newChild = expr7();
            curr =
                NodePtr(new typename Policy::BinaryOp(rangec.toPrevious(), code, std::move(curr), std::move(newChild)));
        }
        return curr;
    }

    // expr7: expr8 { *|/|% expr8}*
    NodePtr expr7() {
        RangeC rangec(oldTokenPosition, tokenPosition);
        NodePtr curr = expr8();
        while (token == Lexer::TIMES || token == Lexer::DIVIDE || token == Lexer::MOD) {
            char code = token == Lexer::TIMES ? '*' : (token == Lexer::DIVIDE ? '/' : '%');
            std::string text = tokenText;
            getToken();
            NodePtr newChild = expr8();
            curr =
                NodePtr(new typename Policy::BinaryOp(rangec.toPrevious(), code, std::move(curr), std::move(newChild)));
        }
        return curr;
    }

    // expr8: {!|~|-|+}* expr9
    NodePtr expr8() {
        NodePtr parent;
        typename Policy::Base* curr = 0;
        RangeC rangec(oldTokenPosition, tokenPosition);
        while (1) {
            char op = ' ';
            if (token == Lexer::TWIDLE)
                op = '~';
            else if (token == Lexer::NOT)
                op = '!';
            else if (token == Lexer::MINUS)
                op = '-';
            else if (token == Lexer::PLUS)
                op = '+';
            else
                break;
            NodePtr unaryOp(new typename Policy::UnaryOp(rangec(), op));
            if (!parent) {
                parent = std::move(unaryOp);
                curr = parent.get();
            } else {
                curr = curr->addChild(std::move(unaryOp));
            }
            getToken();
            curr->setRange(rangec());
        }
        if (parent) {
            curr->addChild(expr9());
            curr->setRange(rangec());
            return parent;
        } else
            return expr9();
    }

    // expr9: {expr10 ^}* expr10
    NodePtr expr9() {
        RangeC rangec(oldTokenPosition, tokenPosition);
        NodePtr parent = expr10();
        typename Policy::Base* curr = 0;
        while (token == Lexer::POWER) {
            getToken();
            NodePtr newNode = expr10();
            if (!curr) {
                parent = NodePtr(new typename Policy::BinaryOp(rangec(), '^', parent, newNode));
                curr = parent.get();
            } else {
                NodePtr oldNode = curr->removeChild();
                curr = curr->addChild(NodePtr(new typename Policy::BinaryOp(rangec(), '^', oldNode, newNode)));
            }
        }
        return parent;
    }

    // expr10: expr11 { [ expr ] }*
    NodePtr expr10() {
        RangeC rangec(oldTokenPosition, tokenPosition);
        NodePtr curr = expr11();
        while (token == Lexer::BRACKET_OPEN) {
            getToken();
            NodePtr combined = NodePtr(new typename Policy::Subscript(rangec(), std::move(curr), expr()));
            curr = std::move(combined);
            ensureAndGetToken(token == Lexer::BRACKET_CLOSE, "Did not get closing bracket to subscripting operator");
            curr->setRange(rangec.toPrevious());
        }
        return curr;
    }

    // expr11: (expr) | NUM | IDENT | IDENT (args) | STR | '[' expr {, expr}* ']'
    NodePtr expr11() {
        if (token == Lexer::PAREN_OPEN) {
            getToken();
            NodePtr ret = expr();
            ensureAndGetToken(token == Lexer::PAREN_CLOSE, "Expect closing parentheses after sub-expression");
            return ret;
        } else if (token == Lexer::NUM) {
            Range range = tokenPosition;
            std::string numStr = tokenText;
            getToken();
            return NodePtr(new typename Policy::Num(range, atof(numStr.c_str())));
        } else if (token == Lexer::IDENT) {
            std::string identName = tokenText;
            RangeC range(oldTokenPosition, tokenPosition);
            getToken();
            if (token == Lexer::PAREN_OPEN) {
                NodePtr callNode(new typename Policy::Call(range(), identName));
                int realStart = curriedArgument.get() != nullptr ? curriedArgument->range()[0] : 0;  // range.start;
                getToken();
                functionArgs(callNode);
                ensureAndGetToken(token == Lexer::PAREN_CLOSE,
                                  "Expected ending parentheses after function call arguments");
                callNode->setRange(Range{realStart, oldTokenPosition[1]});
                return callNode;
            } else {
                return NodePtr(new typename Policy::Var(range.start(), identName));
            }
            // return callNode;
        } else if (token == Lexer::STRING) {
            NodePtr ret =
                NodePtr(new typename Policy::String(tokenPosition, tokenText.substr(1, tokenText.size() - 2)));
            getToken();
            return ret;
        } else if (token == Lexer::BRACKET_OPEN) {
            RangeC r(oldTokenPosition, tokenPosition);
            getToken();
            NodePtr vec(new typename Policy::Vec(r()));
            vec->addChild(expr());
            while (token == Lexer::COMMA) {
                getToken();
                vec->addChild(expr());
            }
            vec->setRange(r());
            ensureAndGetToken(token == Lexer::BRACKET_CLOSE, "Expected bracket close at end of vector");
            return vec;
        } else {
            ensure(false, "At this point I expected, an identifier, function call, literal, or sub-expression.");
        }
        return 0;
    }

    void functionArgs(NodePtr& objectToAddArgsTo) {
        // TODO: fix
        // NodePtr args(new typename Policy::Node(r()));
        if (curriedArgument) objectToAddArgsTo->addChild(std::move(curriedArgument));
        while (token != Lexer::PAREN_CLOSE) {
            objectToAddArgsTo->addChild(expr());
            if (token != Lexer::COMMA) break;
            getToken();
        }
    }

    NodePtr assign() {
        RangeC rangec(oldTokenPosition, tokenPosition);
        ensure(token == Lexer::IDENT, "Expected identifier at begin of assignment");
        std::string varName = tokenText;
        getToken();
        ensure(isAssignOrMutator(token), "Expected mutator on assignment (got '')" + tokenText);
        auto operatorToken = token;
        getToken();
        NodePtr e = expr();

        if (operatorToken != Lexer::ASSIGN) {
            // std::cerr<<"assign not got "<<tokenText<<std::endl;
            RangeC rangec(oldTokenPosition, tokenPosition);
            char code = ' ';
            switch (operatorToken) {
                case Lexer::PLUS_EQUAL:
                    code = '+';
                    break;
                case Lexer::MINUS_EQUAL:
                    code = '-';
                    break;
                case Lexer::TIMES_EQUAL:
                    code = '*';
                    break;
                case Lexer::DIVIDE_EQUAL:
                    code = '/';
                    break;
                case Lexer::MOD_EQUAL:
                    code = '%';
                    break;
                case Lexer::POWER_EQUAL:
                    code = '^';
                    break;
                default:
                    ensure(false, "Invalid assignment modifier!");
            }
            NodePtr varNode = NodePtr(new typename Policy::Var(rangec(), varName));
            e = NodePtr(new typename Policy::BinaryOp(rangec(), code, std::move(varNode), std::move(e)));
        }

        NodePtr assignNode(new typename Policy::Assign(rangec(), varName, std::move(e)));
        ensure(token == Lexer::SEMICOLON, "Expected semi-colon at end of assignment");
        getToken();
        assignNode->setRange(rangec.toPrevious());
        return assignNode;
    }

    NodePtr assignBlock(bool exprAtEnd) {
        RangeC rangec(oldTokenPosition, tokenPosition);
        NodePtr assigns(new typename Policy::Node(rangec()));
        ensureAndGetToken(token == Lexer::BRACE_OPEN, "Expected opening braces after if condition");
        while (token == Lexer::IDENT && isAssignOrMutator(lookAheadToken)) {
            assigns->addChild(assign());
        }
        if (exprAtEnd) {
            assigns->addChild(expr());
        }
        ensureAndGetToken(token == Lexer::BRACE_CLOSE, "Expected opening braces after statements");
        assigns->setRange(rangec.toPrevious());
        return assigns;
    }

    NodePtr ifthenelse() {
        RangeC rangec(oldTokenPosition, tokenPosition);
        ensureAndGetToken(token == Lexer::IF, "Expected if at begin of if then else");
        ensureAndGetToken(token == Lexer::PAREN_OPEN, "Expected open parentheses after if");
        NodePtr condition = expr();
        ensureAndGetToken(token == Lexer::PAREN_CLOSE, "Expected closing parentheses after if condition");
        // optionally handle assignments
        NodePtr ifthenelseNode(new typename Policy::IfThenElse(rangec(), condition));
        ifthenelseNode->addChild(assignBlock(false));
        if (token == Lexer::ELSE) {
            getToken();
            if (token == Lexer::IF)
                ifthenelseNode->addChild(ifthenelse());
            else
                ifthenelseNode->addChild(assignBlock(false));
        } else {
            NodePtr emptyNode(new typename Policy::Node({0, 0}));
            ifthenelseNode->addChild(std::move(emptyNode));
        }
        ifthenelseNode->setRange(rangec.toPrevious());  // TODO: want prev token
        return ifthenelseNode;
    }

  private:
    Lexer lexer;
    std::array<int, 2> oldTokenPosition;
    Lexer::Token token;  // current token
    std::string tokenText;
    std::array<int, 2> tokenPosition;
    Lexer::Token lookAheadToken;  // next token
    std::string lookAheadTokenText;
    std::array<int, 2> lookAheadTokenPosition;

    NodePtr curriedArgument;
};
