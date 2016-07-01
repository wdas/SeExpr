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

#include <vector>
#include <memory>

/// A Simple AST class for representing SeExpr, useful for editors
enum class ASTType {
    Invalid,
    Module,
    Prototype,
    LocalFunction,
    Block,
    Node,
    IfThenElse,
    Vec,
    Cond,
    Compare,
    UnaryOp,
    BinaryOp,
    CompareEq,
    TernaryOp,
    Subscript,
    Var,
    Num,
    Call,
    Assign,
    String,
    Def
};
inline const char* ASTTypeToString(ASTType type) {
    switch (type) {
        case ASTType::Invalid:
            return "Invalid";
        case ASTType::Module:
            return "Module";
        case ASTType::Prototype:
            return "Prototype";
        case ASTType::LocalFunction:
            return "LocalFunction";
        case ASTType::Block:
            return "Block";
        case ASTType::Node:
            return "Node";
        case ASTType::IfThenElse:
            return "IfThenElse";
        case ASTType::Vec:
            return "Vec";
        case ASTType::Cond:
            return "Cond";
        case ASTType::Compare:
            return "Compare";
        case ASTType::UnaryOp:
            return "UnaryOp";
        case ASTType::BinaryOp:
            return "BinaryOp";
        case ASTType::CompareEq:
            return "CompareEq";
        case ASTType::TernaryOp:
            return "TernaryOp";
        case ASTType::Subscript:
            return "Subscript";
        case ASTType::Var:
            return "Var";
        case ASTType::Num:
            return "Num";
        case ASTType::Call:
            return "Call";
        case ASTType::Assign:
            return "Assign";
        case ASTType::String:
            return "String";
        case ASTType::Def:
            return "Def";
    }
    return "<invalid>";
}
class ASTNode {
  public:
    /// Allows adding arbitary number of items to a container holding unique_ptrs
    struct Adder {
        void sequence() {}
        template <typename T, typename... Targs>
        void sequence(T&& guy, Targs&&... args) {
            container.emplace_back(std::move(guy));
            sequence(args...);
        }
        Adder(std::vector<std::unique_ptr<ASTNode>>& container) : container(container) {}

      private:
        std::vector<std::unique_ptr<ASTNode>>& container;
    };

    template <typename... Args>
    ASTNode(const Range& range, const ASTType& type, Args&&... args)
        : _type(type) {
        for (int k = 0; k < 2; k++) _range[k] = range[k];
        Adder a(_children);
        a.sequence(std::forward<Args&&>(args)...);
    }

    ASTNode* addChild(std::unique_ptr<ASTNode> node) {
        _children.push_back(std::move(node));
        return _children.back().get();
    }
    std::unique_ptr<ASTNode> removeChild() {
        std::unique_ptr<ASTNode> ret(std::move(_children.back()));
        _children.pop_back();
        return ret;
    }

    void print(std::ostream& out, int indent = 1, const std::string* originalStr = 0, unsigned int mask = 0) {

        if (originalStr) {
            Range r = range();
            auto replaceNew = [](const std::string& a) {
                std::string ret;
                for (char c : a)
                    if (c == '\n')
                        ret += "\\n";
                    else
                        ret += c;
                return ret;
            };
            out << replaceNew(originalStr->substr(0, r[0]));
            out << "\033[7;10m";
            out << replaceNew(originalStr->substr(r[0], r[1] - r[0]));
            out << "\033[7;m";
            out << replaceNew(originalStr->substr(r[1], std::string::npos));
        }

        if (indent != 1) {
            out << "   ";
            for (int i = 1; i < indent - 1; i++) {
                if ((mask & (1 << i)) != 0)
                    out << "   ";
                else
                    out << "  "
                        << "\xe2\x94\x82";
                ;
            }
            out << "  ";
            if ((mask & (1 << (indent - 1))) != 0) {
                out << "\xe2\x94\x94";
            } else {
                out << "\xe2\x94\x9c";
            }
            out << "\xe2\x94\x80 ";
        } else {
            out << "   ";
        }
        // out<<std::string(indent*4,' ')<<"|- "<<_name<<"\n";
        out << display();  //<<std::endl;
        out << std::endl;
        for (auto it = _children.begin(); it != _children.end(); ++it) {
            int isEnd = it + 1 == _children.end() ? 1 : 0;
            unsigned int newMask = mask | (int(isEnd) << (indent));
            (*it)->print(out, indent + 1, originalStr, newMask);
        }
    }
    virtual std::string display() const { return ASTTypeToString(_type); }

    void setRange(const Range& range) { _range = range; }
    const Range& range() const { return _range; }

    ASTType type() const { return _type; }  // TODO: fix
    // const std::string& typeString() const{return typeToName[_type];} // TODO: fix
    const std::vector<std::unique_ptr<ASTNode>>& children() const { return _children; }

  private:
    ASTType _type;
    Range _range;
    // std::string _name;
    // int _startOffset, _endOffset;
    std::vector<std::unique_ptr<ASTNode>> _children;
};

struct ASTPolicy {
    typedef ASTNode Base;
    typedef std::unique_ptr<ASTNode> Ptr;

#define SEEXPR_AST_SUBCLASS(name)                    \
    struct name : public Base {                      \
        template <typename... Args>                  \
        name(const Range& range, Args&&... args)     \
            : Base(range, ASTType::name, args...) {} \
    };
#define SEEXPR_AST_SUBCLASS_OP(name)                                     \
    struct name : public Base {                                          \
        template <typename... Args>                                      \
        name(const Range& range, char op, Args&&... args)                \
            : Base(range, ASTType::name, args...), _op(op) {}            \
        std::string display() const { return std::string(#name) + _op; } \
        char op() const { return _op; }                                  \
        char value() const { return _op; }                               \
                                                                         \
      private:                                                           \
        char _op;                                                        \
    };
    SEEXPR_AST_SUBCLASS(Module);
    SEEXPR_AST_SUBCLASS(Prototype);
    SEEXPR_AST_SUBCLASS(LocalFunction);
    SEEXPR_AST_SUBCLASS(Block);
    SEEXPR_AST_SUBCLASS(Node);
    SEEXPR_AST_SUBCLASS(IfThenElse);
    SEEXPR_AST_SUBCLASS(Vec);
    SEEXPR_AST_SUBCLASS(Cond);
    SEEXPR_AST_SUBCLASS(Compare);
    SEEXPR_AST_SUBCLASS_OP(UnaryOp);
    SEEXPR_AST_SUBCLASS_OP(BinaryOp);
    SEEXPR_AST_SUBCLASS_OP(CompareEq);
    SEEXPR_AST_SUBCLASS_OP(TernaryOp);
    SEEXPR_AST_SUBCLASS(Subscript);
    SEEXPR_AST_SUBCLASS(Def);

    struct String : public Base {
        String(const Range& range, const std::string& s) : Base(range, ASTType::String), s(s) {}
        const std::string& value() const { return s; }
        std::string s;
        std::string display() const { return std::string("string") + " " + s; }
    };

    struct Num : public Base {
        Num(const Range& range, double num) : Base(range, ASTType::Num), num(num) {}
        double num;
        double value() const { return num; }
        std::string display() const { return std::string("num") + " " + std::to_string(num); }
    };

    struct Var : public Base {
        Var(const Range& range, const std::string& var) : Base(range, ASTType::Var), var(var) {}
        const std::string& value() const { return var; }
        std::string display() const { return std::string("var") + " '" + var + "'"; }

      private:
        std::string var;
    };

    struct Assign : public Base {
        Assign(const Range& range, const std::string& var, Ptr node)
            : Base(range, ASTType::Assign, std::move(node)), var(var) {}
        std::string display() const { return std::string("assign ") + " '" + var + "'"; }
        const std::string& value() const { return var; }

      private:
        std::string var;
    };

    struct Call : public Base {
        Call(const Range& range, const std::string& symbolIn) : Base(range, ASTType::Call), func(symbolIn) {}
        std::string display() const { return std::string("call ") + " '" + func + "'"; }
        const std::string& value() const { return func; }

      private:
        std::string func;
    };
};
