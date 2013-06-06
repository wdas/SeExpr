/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
*
* @file EditableExpression.h
* @author Andrew Selle
*/
#ifndef __EditableExpression__
#define __EditableExpression__

#include <vector>
#include <string>

class SeExprEdEditable;

/// Factors a SeExpr into an editable expression with controls (i.e. value boxes, curve boxes)
class SeExprEdEditableExpression{
    std::string _expr; // original full expression
    typedef std::vector<SeExprEdEditable*> Editables;
    std::vector<SeExprEdEditable*> _editables; // control that can edit the expression
    std::vector<std::string> _variables;
public:
    SeExprEdEditableExpression();
    ~SeExprEdEditableExpression();

    /// Set's expressions and parses it into "control editable form"
    void setExpr(const std::string& expr);

    /// Return a reconstructed expression using all the editable's current values
    std::string getEditedExpr() const;

    /// Check if the other editable expression has editables that all match i.e. the controls are same
    bool controlsMatch(const SeExprEdEditableExpression& other) const;

    /// Update the string refered to into the controls (this is only valid if controlsmatch)
    void updateString(const SeExprEdEditableExpression& other);

    /// Access an editable parameter
    SeExprEdEditable* operator[](const int i){return _editables[i];}

    /// Return the count of editable parameters
    size_t size() const{return _editables.size();}

    /// Get list of commentsø
    const std::vector<std::string>& getVariables() const{return _variables;}
private:
    /// clean memeory
    void cleanup();
};

#endif
