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

#ifndef SeExprSpec_h
#define SeExprSpec_h

#ifndef MAKEDEPEND
#include <string.h>
#include <string>
#include <vector>
#endif
#include <cstdio>

#include "SeExprNode.h"
#include "SeCurve.h"

class SeExprSpec {
 public:
    SeExprSpec(const SeExprNode& node)
        :_start(node.startPos()), _end(node.endPos())
    {};

    virtual std::string toString() const = 0;
 protected:
    std::string _name;
    int _start;
    int _end;
};


class SeExprScalarAssignSpec : public SeExprSpec {
 public:
    SeExprScalarAssignSpec(const SeExprAssignNode& node);
    virtual std::string toString() const;
    inline double value() const { return _val; };
    static const SeExprScalarAssignSpec* match(const SeExprNode* node);

 private:
    double _min,_max;
    double _val;
};

class SeExprVectorAssignSpec : public SeExprSpec {
 public:
    SeExprVectorAssignSpec(const SeExprAssignNode& node);
    virtual std::string toString() const;
    inline const SeVec3d & value() const { return _val; };
    static const SeExprVectorAssignSpec* match(const SeExprNode* node);

 private:
    double _min,_max;
    SeVec3d _val;
};

class SeExprCurveAssignSpec : public SeExprSpec {

 public:
    SeExprCurveAssignSpec(const SeExprAssignNode& node);
    virtual std::string toString() const;
    static const SeExprCurveAssignSpec* match(const SeExprNode* node);

 private:
    std::string _lookupText;
    const SeExprFuncNode* cnode;
    std::vector<SeExpr::SeCurve<double>::CV> _vec;
};

class SeExprCcurveAssignSpec : public SeExprSpec {
public:
    SeExprCcurveAssignSpec(const SeExprAssignNode& node);
    virtual std::string toString() const;
    static const SeExprCcurveAssignSpec* match(const SeExprNode* node);

 private:
    std::string _lookupText;
    std::vector<SeExpr::SeCurve<SeVec3d>::CV> _vec;
};

class SeExprStrSpec : public SeExprSpec {
    enum Type {STRING,FILE,DIRECTORY};
 public:
    //! Takes name and type comments and takes ownership of them!
    SeExprStrSpec(const SeExprStrNode& node,char* name,Type type)
        : SeExprSpec(node),_str(node.str()),_type(type)
    {
        _name=name;
    }

    virtual std::string toString() const;
    static const SeExprStrSpec* match(const SeExprNode* node);
private:
    std::string _str;
    Type _type;
};

#endif
