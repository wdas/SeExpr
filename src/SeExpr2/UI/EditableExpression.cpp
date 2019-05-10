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
#include "Editable.h"
#include "EditableExpression.h"
#include <sstream>

bool ExprSpecParse(std::vector<Editable*>& literals,
                   std::vector<std::string>& variables,
                   std::vector<std::pair<int, int> >& comments,
                   const char* str);

EditableExpression::EditableExpression()
{
}

EditableExpression::~EditableExpression()
{
    cleanup();
}

void EditableExpression::setExpr(const std::string& expr)
{
    // get rid of old data
    cleanup();

    // run parser
    _expr = expr;
    std::vector<std::pair<int, int> > comments;
    ExprSpecParse(_editables, _variables, comments, _expr.c_str());

    for (Editables::iterator it = _editables.begin(); it != _editables.end();) {
        Editable& literal = **it;
        int endPos = literal.endPos;
        std::string comment = "";
        for (size_t ci = 0; ci < comments.size(); ci++) {
            if (comments[ci].first >= endPos) {
                // check to make sure there is no newlines between end of editable and comment
                size_t pos = _expr.find('\n', endPos);
                if (pos == std::string::npos || pos >= (size_t)comments[ci].second) {
                    comment = _expr.substr(comments[ci].first, comments[ci].second - comments[ci].first);
                    break;
                }
            }
        }
        bool keepEditable = literal.parseComment(comment);
        if (!keepEditable) {  // TODO: this is potentially quadratic if we remove a bunch
            delete &literal;
            it = _editables.erase(it);
        } else {
            ++it;
        }
    }
}

void EditableExpression::cleanup()
{
    for (size_t i = 0; i < _editables.size(); i++)
        delete _editables[i];
    _editables.clear();
    _variables.clear();
}

std::string EditableExpression::getEditedExpr() const
{
    int offset = 0;
    std::stringstream stream;
    for (size_t i = 0, sz = _editables.size(); i < sz; i++) {
        Editable& literal = *_editables[i];
        stream << _expr.substr(offset, literal.startPos - offset);
        literal.appendString(stream);
        offset = literal.endPos;
    }
    stream << _expr.substr(offset, _expr.size() - offset);
    return stream.str();
}

void EditableExpression::updateString(const EditableExpression& other)
{
    // TODO: move semantics?
    _variables = other._variables;
    _expr = other._expr;
    _variables = other._variables;
    for (size_t i = 0, sz = _editables.size(); i < sz; i++) {
        Editable& literal = *_editables[i];
        Editable& otherLiteral = *other._editables[i];
        assert(literal.controlsMatch(otherLiteral));
        literal.updatePositions(otherLiteral);
    }
}

bool EditableExpression::controlsMatch(const EditableExpression& other) const
{
    if (_editables.size() != other._editables.size())
        return false;

    for (size_t i = 0, sz = _editables.size(); i < sz; i++) {
        const Editable& literal = *_editables[i];
        const Editable& otherLiteral = *other._editables[i];
        if (!literal.controlsMatch(otherLiteral))
            return false;
    }
    return true;
}
