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

#include <boost/python.hpp>
#include <SeExprParse.h>
#include <ASTNode.h>
#include <memory>

class AST;

// A handle to hold ASTNodes. It holds a shared_ptr of the root ASTNode to make lifetime of the whole tree held
// to lifetime of this node.
class ASTHandle {
  public:
    ASTHandle(std::shared_ptr<ASTNode> astRoot, ASTNode* node) : astRoot(astRoot), node(node) {}

    ASTType type() const { return node ? node->type() : ASTType::Invalid; }

    boost::python::list children() const {
        boost::python::list ret;
        for (const auto& it : node->children()) ret.append(new ASTHandle(astRoot, it.get()));
        return ret;
    }

    boost::python::object value() const {
        boost::python::object object;
        // exit(1);
        if (!node) return object;
        switch (node->type()) {
            case ASTType::Assign:
                return boost::python::str(static_cast<typename ASTPolicy::Assign*>(node)->value());
            case ASTType::Var:
                return boost::python::str(static_cast<typename ASTPolicy::Var*>(node)->value());
            case ASTType::Call:
                return boost::python::str(static_cast<typename ASTPolicy::Call*>(node)->value());
            case ASTType::String:
                return boost::python::str(static_cast<typename ASTPolicy::String*>(node)->value());
            case ASTType::Num:
                return boost::python::object(static_cast<typename ASTPolicy::Num*>(node)->value());
            case ASTType::BinaryOp:
                return boost::python::object(static_cast<typename ASTPolicy::BinaryOp*>(node)->value());
            case ASTType::UnaryOp:
                return boost::python::object(static_cast<typename ASTPolicy::UnaryOp*>(node)->value());
            default:
                break;  // throw std::runtime_error("Value not supported by this type");
        }
        return object;
    }

    boost::python::object range() const {
        if (node) {
            auto r = node->range();
            return boost::python::make_tuple(r[0], r[1]);
        } else
            return boost::python::object();
    }

    friend class AST;

  private:
    std::shared_ptr<ASTNode> astRoot;  // Holds astRoot to have lifetime as much as this
    ASTNode* node;                     // This is safe, because this lifetime is at least as long as expr
};

/// Hold an AST class
class AST {
  public:
    AST(const std::string& s) : _expressionString(s) {
        SeParser<ASTPolicy> parser(s);
        ASTPolicy::Ptr rootUnique = parser.parse();
        _root = std::shared_ptr<ASTNode>(rootUnique.release());  // TODO: WHY!?
    }

    ASTHandle* root() { return _root ? new ASTHandle(_root, _root.get()) : nullptr; }

    bool isValid() const { return !!_root; }

    std::shared_ptr<ASTNode> _root;
    std::string _expressionString;
};

void translateParseError(ParseError const& e) { PyErr_SetString(PyExc_RuntimeError, e.what().c_str()); }

using namespace boost::python;
BOOST_PYTHON_MODULE(core) {
    register_exception_translator<ParseError>(&translateParseError);
    class_<AST>("AST", init<std::string>()).def("isValid", &AST::isValid).def(
        "root", &AST::root, return_value_policy<manage_new_object>())
        //.def("edit",&AST::edit)
        ;

    class_<ASTHandle>("ASTHandle", no_init)
        .add_property("type", &ASTHandle::type)
        .add_property("value", &ASTHandle::value)
        .add_property("range", &ASTHandle::range)
        .def("children", &ASTHandle::children);

    enum_<ASTType>("ASTType")
        .value("Invalid", ASTType::Invalid)
        .value("Module", ASTType::Module)
        .value("Prototype", ASTType::Prototype)
        .value("LocalFunction", ASTType::LocalFunction)
        .value("Block", ASTType::Block)
        .value("Node", ASTType::Node)
        .value("IfThenElse", ASTType::IfThenElse)
        .value("Vec", ASTType::Vec)
        .value("Cond", ASTType::Cond)
        .value("Compare", ASTType::Compare)
        .value("UnaryOp", ASTType::UnaryOp)
        .value("BinaryOp", ASTType::BinaryOp)
        .value("CompareEq", ASTType::CompareEq)
        .value("TernaryOp", ASTType::TernaryOp)
        .value("Def", ASTType::Def)
        .value("Subscript", ASTType::Subscript)
        .value("Var", ASTType::Var)
        .value("Num", ASTType::Num)
        .value("Call", ASTType::Call)
        .value("Assign", ASTType::Assign)
        .value("String", ASTType::String);
}
