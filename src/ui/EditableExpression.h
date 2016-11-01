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
*
* @file EditableExpression.h
* @author Andrew Selle
*/
#ifndef __EditableExpression__
#define __EditableExpression__

#include <vector>
#include <string>

class Editable;

/// Factors a SeExpr into an editable expression with controls (i.e. value boxes, curve boxes)
class EditableExpression {
    std::string _expr;  // original full expression
    typedef std::vector<Editable*> Editables;
    std::vector<Editable*> _editables;  // control that can edit the expression
    std::vector<std::string> _variables;

  public:
    EditableExpression();
    ~EditableExpression();

    /// Set's expressions and parses it into "control editable form"
    void setExpr(const std::string& expr);

    /// Return a reconstructed expression using all the editable's current values
    std::string getEditedExpr() const;

    /// Check if the other editable expression has editables that all match i.e. the controls are same
    bool controlsMatch(const EditableExpression& other) const;

    /// Update the string refered to into the controls (this is only valid if controlsmatch)
    void updateString(const EditableExpression& other);

    /// Access an editable parameter
    Editable* operator[](const int i) { return _editables[i]; }

    /// Return the count of editable parameters
    size_t size() const { return _editables.size(); }

    /// Get list of commentsø
    const std::vector<std::string>& getVariables() const { return _variables; }

  private:
    /// clean memeory
    void cleanup();
};

#endif
