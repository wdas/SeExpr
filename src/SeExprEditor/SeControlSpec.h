/*
 SEEXPR SOFTWARE
 Copyright 2011 Disney Enterprises, Inc. All rights reserved
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are
 met:
 
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in
 the documentation and/or other materials provided with the
 distribution.
 
 * The names "Disney", "Walt Disney Pictures", "Walt Disney Animation
 Studios" or the names of its contributors may NOT be used to
 endorse or promote products derived from this software without
 specific prior written permission from Walt Disney Pictures.
 
 Disclaimer: THIS SOFTWARE IS PROVIDED BY WALT DISNEY PICTURES AND
 CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIESname, INCLUDING,
 BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR PURPOSE, NONINFRINGEMENT AND TITLE ARE DISCLAIMED.
 IN NO EVENT SHALL WALT DISNEY PICTURES, THE COPYRIGHT HOLDER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND BASED ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*/

#ifndef SeControlSpec_h
#define SeControlSpec_h

#ifndef MAKEDEPEND
#include <string.h>
#include <string>
#include <vector>
#endif
#include <cstdio>

#include "SeExprWalker.h"
#include "SeExprNode.h"
#include "SeCurve.h"

namespace SeExpr{

//! Generic SeExpression control specification
class ControlSpec {
 public:
    ControlSpec(const SeExprNode& node)
        :_start(node.startPos()), _end(node.endPos())
    {};

    //! Generates a replacement string based on changes to the spec
    virtual std::string toString() const = 0;
 protected:
    //! Name of control
    std::string _name;
    //! Start position of text in original source
    int _start;
    //! End position of text in original source
    int _end;
};

//! Variable equals scalar control specification
class SeExprScalarAssignSpec : public ControlSpec {
 public:
    SeExprScalarAssignSpec(const SeExprAssignNode& node);
    virtual std::string toString() const;
    inline double value() const { return _val; };
    static const SeExprScalarAssignSpec* match(const SeExprNode* node);

 private:
    //! Range of values
    double _min,_max;
    //! Current Value
    double _val;
};

//! Variable equals vector control specification
class SeExprVectorAssignSpec : public ControlSpec {
 public:
    SeExprVectorAssignSpec(const SeExprAssignNode& node);
    virtual std::string toString() const;
    inline const SeVec3d & value() const { return _val; };
    static const SeExprVectorAssignSpec* match(const SeExprNode* node);

 private:
    //! Range of values
    double _min,_max;
    //! Current Value
    SeVec3d _val;
};

//! Curve assignment expression. Assignment of curve to a variable.
template<class T>
class SeExprCurveAssignSpec : public ControlSpec {

 public:
    SeExprCurveAssignSpec(const SeExprAssignNode& node);
    virtual std::string toString() const;
    static const SeExprCurveAssignSpec* match(const SeExprNode* node);

 private:
    //! Lookup subexpression text
    std::string _lookupText;
    //! Control points of curve spline
    std::vector<typename SeExpr::SeCurve<T>::CV> _vec;
};

class SeExprStrSpec : public ControlSpec {
    enum Type {STRING,FILE,DIRECTORY};
 public:
    //! Takes name and type comments and takes ownership of them!
    SeExprStrSpec(const SeExprStrNode& node,char* name,Type type)
        : ControlSpec(node),_str(node.str()),_type(type)
    {
        _name=name;
    }

    virtual std::string toString() const;
    static const SeExprStrSpec* match(const SeExprNode* node);
private:
    std::string _str;
    Type _type;
};



/// Examiner that builds a list of specs potentially used in widgets (for qdgui)
class SpecExaminer : public Examiner<true> {

 public:
    ~SpecExaminer();

    virtual bool examine(const SeExprNode* examinee);
    virtual void reset() { _specList.clear(); };
    inline int length() const { return _specList.size(); };
    inline const ControlSpec* spec(int i) const { return _specList[i]; };
    inline std::vector<const ControlSpec*>::const_iterator       begin() const;
    inline std::vector<const ControlSpec*>::const_iterator const end  () const;

 private:
    std::vector<const ControlSpec*> _specList;

};

}
#endif
