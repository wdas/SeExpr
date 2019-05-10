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

#ifndef MAKEDEPEND
#include <string.h>
#include <string>
#include <vector>
#endif

#include <sstream>

#include <SeExpr2/ExprNode.h>
#include <SeExpr2/ExprPatterns.h>
#include "ControlSpec.h"

namespace SeExpr2 {

SpecExaminer::~SpecExaminer()
{
    std::vector<const ControlSpec*>::iterator i = _specList.begin();
    std::vector<const ControlSpec*>::iterator const e = _specList.end();
    for (; i != e; ++i)
        delete *i;
};

bool SpecExaminer::examine(const ExprNode* examinee)
{
    if (const ExprScalarAssignSpec* s_spec = ExprScalarAssignSpec::match(examinee)) {
        _specList.push_back(s_spec);
        return false;
    } else if (const ExprVectorAssignSpec* v_spec = ExprVectorAssignSpec::match(examinee)) {
        _specList.push_back(v_spec);
        return false;
    } else if (const ExprCurveAssignSpec<double>* c_spec = ExprCurveAssignSpec<double>::match(examinee)) {
        _specList.push_back(c_spec);
        return false;
    } else if (const ExprCurveAssignSpec<Vec3d>* cc_spec = ExprCurveAssignSpec<Vec3d>::match(examinee)) {
        _specList.push_back(cc_spec);
        return false;
    } else if (const ExprStrSpec* str_spec = ExprStrSpec::match(examinee)) {
        _specList.push_back(str_spec);
        return false;
    };

    return true;
};

inline std::vector<const ControlSpec*>::const_iterator SpecExaminer::begin() const
{
    return _specList.begin();
};

inline std::vector<const ControlSpec*>::const_iterator const SpecExaminer::end() const
{
    return _specList.end();
};

//! Returns true if no newline separates comment and node
inline bool isWS(const char* source, int start, int end)
{
    for (int i = start; i < end; ++i)
        if (source[i] != '\n')
            return false;
    return true;
};

//! Checks if there is whitespace in the range specified in the string
inline std::string findComment(const ExprNode& node)
{
    const Expression& expr = *node.expr();
    typedef std::vector<std::pair<int, int> > Comments;
    const Comments& comments = expr.getComments();
    const std::string& s = expr.getExpr();

    // TODO: user lower_bound to make this O(lg n) instead of O(n)
    for (Comments::const_iterator i = comments.begin(); i != comments.end(); ++i) {
        if (i->first >= node.endPos() && isWS(s.c_str(), node.endPos(), i->first))
            return s.substr(i->first, i->second - i->first + 1);
    }
    return "";
}

ExprScalarAssignSpec::ExprScalarAssignSpec(const ExprAssignNode& node)
    : ControlSpec(node), _min(0), _max(1), _val(static_cast<const ExprNumNode*>(node.child(0))->value())
{
    _name = node.name();
    std::string comment = findComment(node);
    // TODO: handle integer case
    int numParsed = sscanf(comment.c_str(), "#%lf,%lf\n", &_min, &_max);
    if (numParsed != 2) {
        _min = 0;
        _max = 1;
    }
}

std::string ExprScalarAssignSpec::toString() const
{
    std::stringstream ss;

    ss << _name << ": " << value() << " in [" << _min << "," << _max << "]" << std::endl;

    return ss.str();
}

const ExprScalarAssignSpec* ExprScalarAssignSpec::match(const ExprNode* node)
{
    if (const ExprAssignNode* assign = isScalarAssign(node))
        return new ExprScalarAssignSpec(*assign);

    return 0;
}

ExprVectorAssignSpec::ExprVectorAssignSpec(const ExprAssignNode& node)
    : ControlSpec(node)
    , _min(0)
    , _max(1)
    , _val(Vec3d(static_cast<const ExprNumNode*>(node.child(0)->child(0))->value(),
                 static_cast<const ExprNumNode*>(node.child(0)->child(1))->value(),
                 static_cast<const ExprNumNode*>(node.child(0)->child(2))->value()))
{
    _name = node.name();
    std::string comment = findComment(node);
    int numParsed = sscanf(comment.c_str(), "#%lf,%lf\n", &_min, &_max);
    if (numParsed != 2) {
        _min = 0;
        _max = 1;
    }
}

std::string ExprVectorAssignSpec::toString() const
{
    std::stringstream ss;

    ss << _name << ": " << value() << " in [" << _min << "," << _max << "]" << std::endl;
    ;

    return ss.str();
}

template <class T>
ExprCurveAssignSpec<T>::ExprCurveAssignSpec(const ExprAssignNode& node) : ControlSpec(node), _vec()
{
    _name = node.name();
    const ExprFuncNode* cnode = static_cast<const ExprFuncNode*>(node.child(0));
    _lookupText = cnode->child(0)->toString();
    int num = cnode->numChildren();
    for (int i = 1; i < num - 2; i += 3)
        _vec.push_back(typename Curve<T>::CV(
            static_cast<const ExprNumNode*>(cnode->child(i))->value(),
            static_cast<const ExprNumNode*>(cnode->child(i + 1))->value(),
            (typename Curve<T>::InterpType) static_cast<const ExprNumNode*>(cnode->child(i + 2))->value()));
}

const ExprVectorAssignSpec* ExprVectorAssignSpec::match(const ExprNode* node)
{
    if (const ExprAssignNode* assign = isVectorAssign(node)) {
        return new ExprVectorAssignSpec(*assign);
    }

    return 0;
}

template <class T>
std::string ExprCurveAssignSpec<T>::toString() const
{
    std::stringstream ss;

    ss << _name << ": "
       << "curve(" << _lookupText;
    int num = _vec.size();
    for (int i = 0; i < num; ++i)
        ss << _vec[i]._pos << _vec[i]._val << (int)_vec[i]._interp;
    ss << ");";

    return ss.str();
}

template <class T>
const ExprCurveAssignSpec<T>* ExprCurveAssignSpec<T>::match(const ExprNode* node)
{
    if (const ExprAssignNode* assign = isCurveAssign(node))
        return new ExprCurveAssignSpec(*assign);

    return 0;
}

#if 0

ExprCcurveAssignSpec::
ExprCcurveAssignSpec(const ExprAssignNode& node)
    : ControlSpec(node),
     _vec()
{
    _name=node.name();
    const ExprFuncNode* cnode(static_cast<const ExprFuncNode*>(node.child(0)));
    _lookupText=cnode->child(0)->toString();
    int num = cnode->numChildren();
    for(int i = 1; i < num - 2; i += 3)
        if(dynamic_cast<const ExprNumNode*>(cnode->child(i+1)))
            _vec.push_back(Curve<Vec3d>::CV(
                    static_cast<const ExprNumNode*>(cnode->child(i))->value(),
                    static_cast<const ExprNumNode*>(cnode->child(i+1))->value(),
                    (Curve<Vec3d>::InterpType) static_cast<const ExprNumNode*>(cnode->child(i+2))->value()));
}

std::string
ExprCcurveAssignSpec::toString() const
{
    std::stringstream ss;

    ss << _name
       << " = "
       << "ccurve("
       << _lookupText;
    int num = _vec.size();
    for(int i = 0; i < num; ++i)
        ss << ", "
           << _vec[i]._pos
           << ", "
           << _vec[i]._val
           << ", "
           << (int)_vec[i]._interp;
    ss << ");";

    return ss.str();
}

const ExprCcurveAssignSpec*
ExprCcurveAssignSpec::match(const ExprNode* node)
{
    if(const ExprAssignNode* assign = isCcurveAssign(node))
        return new ExprCcurveAssignSpec(*assign);


    return 0;
}

#endif

std::string ExprStrSpec::toString() const
{
    std::stringstream ss;
    ss << _name << ": \"" + _str + "\" ";
    switch (_type) {
    case STRING:
        ss << "STRING";
        break;
    case FILE:
        ss << "FILE";
        break;
    case DIRECTORY:
        ss << "DIRECTORY";
        break;
    default:
        ss << "INVALID";
        break;
    }
    return ss.str();
}

const ExprStrSpec* ExprStrSpec::match(const ExprNode* node)
{
    if (const ExprStrNode* strnode = isString(node)) {
        std::string comment = findComment(*node);
        char* name = new char[comment.length() + 1];
        char* type = new char[comment.length() + 1];
        int numMatched = sscanf(comment.c_str(), "#%s %s\n", type, name);

        Type newType;
        if (numMatched == 2) {
            bool valid = true;
            if (!strcmp(type, "string"))
                newType = STRING;
            else if (!strcmp(type, "file"))
                newType = FILE;
            else if (!strcmp(type, "directory"))
                newType = DIRECTORY;
            else
                valid = false;
            if (valid)
                return new ExprStrSpec(*strnode, name, newType);
        }
        delete[] name;
        delete[] type;
    }
    return 0;
}
}
