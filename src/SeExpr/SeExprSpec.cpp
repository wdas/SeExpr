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
 CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
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

#ifndef MAKEDEPEND
#include <string.h>
#include <string>
#include <vector>
#endif

#include <sstream>

#include "SeExprNode.h"
#include "SeExprPatterns.h"
#include "SeExprSpec.h"


//! Returns true if no newline separates comment and node
inline bool isWS(const char* source,int start, int end) {
    for(int i = start; i < end; ++i)
        if(source[i] != '\n') return false;
    return true;
};

//! Checks if there is whitespace in the range specified in the string
inline std::string findComment(const SeExprNode& node){
    const SeExpression& expr=*node.expr();
    typedef std::vector<std::pair<int,int> > Comments;
    const Comments& comments=expr.getComments();
    const std::string& s=expr.getExpr();

    // TODO: user lower_bound to make this O(lg n) instead of O(n)
    for(Comments::const_iterator i=comments.begin();i!=comments.end();++i){
        if(i->first>=node.endPos() && isWS(s.c_str(),node.endPos(),i->first))
            return s.substr(i->first,i->second-i->first+1);
    }
    return "";
}

SeExprScalarAssignSpec::
SeExprScalarAssignSpec(const SeExprAssignNode& node)
    : SeExprSpec(node),_min(0),_max(1),
     _val(static_cast<const SeExprNumNode*>(node.child(0))->value())
     
{
    _name=node.name();
    std::string comment=findComment(node);
    // TODO: handle integer case
    int numParsed=sscanf(comment.c_str(),"#%lf,%lf\n",&_min,&_max);
    if(numParsed != 2){_min=0;_max=1;}
}

std::string
SeExprScalarAssignSpec::toString() const
{
    std::stringstream ss;

    ss << _name
       << ": "
       << value()
       << " in ["<<_min<<","<<_max<<"]"<<std::endl;

    return ss.str();
}

const SeExprScalarAssignSpec*
SeExprScalarAssignSpec::match(const SeExprNode* node)
{
    if(const SeExprAssignNode* assign = isScalarAssign(node))
        return new SeExprScalarAssignSpec(*assign);

    return 0;
}

SeExprVectorAssignSpec::
SeExprVectorAssignSpec(const SeExprAssignNode& node)
    : SeExprSpec(node),_min(0),_max(1),
     _val(SeVec3d(static_cast<const SeExprNumNode*>(node.child(0)->child(0))->value(),
             static_cast<const SeExprNumNode*>(node.child(0)->child(1))->value(),
             static_cast<const SeExprNumNode*>(node.child(0)->child(2))->value()))
{
    _name=node.name();
    std::string comment=findComment(node);
    int numParsed=sscanf(comment.c_str(),"#%lf,%lf\n",&_min,&_max);
    if(numParsed != 2){_min=0;_max=1;}
}

std::string
SeExprVectorAssignSpec::toString() const
{
    std::stringstream ss;

    ss << _name
       << ": "
       << value()
       << " in ["<<_min<<","<<_max<<"]"<<std::endl;;

    return ss.str();
}


SeExprCurveAssignSpec::
SeExprCurveAssignSpec(const SeExprAssignNode& node)
    : SeExprSpec(node),
     _vec()
{
    _name=node.name();
        const SeExprFuncNode* cnode=static_cast<const SeExprFuncNode*>(node.child(0));
        _lookupText=cnode->child(0)->toString();
        int num = cnode->numChildren();
        for(int i = 1; i < num - 2; i += 3)
            _vec.push_back(SeExpr::SeCurve<double>::CV(
                    static_cast<const SeExprNumNode*>(cnode->child(i))->value(),
                    static_cast<const SeExprNumNode*>(cnode->child(i+1))->value(),
                    (SeExpr::SeCurve<double>::InterpType) static_cast<const SeExprNumNode*>(cnode->child(i+2))->value()));
}

const SeExprVectorAssignSpec*
SeExprVectorAssignSpec::match(const SeExprNode* node)
{
    if(const SeExprAssignNode* assign = isVectorAssign(node)) {
        return new SeExprVectorAssignSpec(*assign);
    }

    return 0;
}

std::string
SeExprCurveAssignSpec::toString() const
{
    std::stringstream ss;

    ss << _name
       << ": "
       << "curve("
       << _lookupText;
    int num = _vec.size();
    for(int i = 0; i < num; ++i)
        ss << _vec[i]._pos<< _vec[i]._val<< (int)_vec[i]._interp;
    ss << ");";

    return ss.str();
}

const SeExprCurveAssignSpec*
SeExprCurveAssignSpec::match(const SeExprNode* node)
{
    if(const SeExprAssignNode* assign = isCurveAssign(node))
        return new SeExprCurveAssignSpec(*assign);

    return 0;
}

SeExprCcurveAssignSpec::
SeExprCcurveAssignSpec(const SeExprAssignNode& node)
    : SeExprSpec(node),
     _vec()
{
    _name=node.name();
    const SeExprFuncNode* cnode(static_cast<const SeExprFuncNode*>(node.child(0)));
    _lookupText=cnode->child(0)->toString();
    int num = cnode->numChildren();
    for(int i = 1; i < num - 2; i += 3)
        if(dynamic_cast<const SeExprNumNode*>(cnode->child(i+1)))
            _vec.push_back(SeExpr::SeCurve<SeVec3d>::CV(
                    static_cast<const SeExprNumNode*>(cnode->child(i))->value(),
                    static_cast<const SeExprNumNode*>(cnode->child(i+1))->value(),
                    (SeExpr::SeCurve<SeVec3d>::InterpType) static_cast<const SeExprNumNode*>(cnode->child(i+2))->value()));
}

std::string
SeExprCcurveAssignSpec::toString() const
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

const SeExprCcurveAssignSpec*
SeExprCcurveAssignSpec::match(const SeExprNode* node)
{
    if(const SeExprAssignNode* assign = isCcurveAssign(node))
        return new SeExprCcurveAssignSpec(*assign);


    return 0;
}

std::string
SeExprStrSpec::toString() const
{
    std::stringstream ss;
    ss<<_name<<": \""+_str+"\" ";
    switch(_type){
        case STRING: ss<<"STRING";break;
        case FILE: ss<<"FILE";break;
        case DIRECTORY: ss<<"DIRECTORY";break;
        default: ss<<"INVALID";break;
    }
    return ss.str();
}

const SeExprStrSpec*
SeExprStrSpec::match(const SeExprNode* node)
{
    if(const SeExprStrNode* strnode = isString(node)){
        std::string comment=findComment(*node);
        char* name=new char[comment.length()+1];
        char* type=new char[comment.length()+1];
        int numMatched=sscanf(comment.c_str(),"#%s %s\n",type,name);

        Type newType;
        if(numMatched==2){
            bool valid=true;
            if(!strcmp(type,"string")) newType=STRING;
            else if(!strcmp(type,"file")) newType=FILE;
            else if(!strcmp(type,"directory")) newType=DIRECTORY;
            else valid=false;
            if(valid) return new SeExprStrSpec(*strnode,name,newType);
        }
        delete [] name; delete [] type;
    }
    return 0;
}

