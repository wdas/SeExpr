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
* @file SeExprEdEditable.h
* @author Andrew Selle
*/
#ifndef __SeExprEdEditable__
#define __SeExprEdEditable__
#include <sstream>
#include <SeVec3d.h>
#include <SeCurve.h>
#include <cstdio>
#include <cstring>
#include <SeExprMacros.h>
#ifdef SEEXPR_USE_ANIMLIB
#   include <animlib/AnimCurve.h>
#   include <animlib/AnimKeyframe.h>
#endif
inline void printVal(std::stringstream& stream,double v){stream<<v;}
inline void printVal(std::stringstream& stream,const SeVec3d& v){stream<<"["<<v[0]<<","<<v[1]<<","<<v[2]<<"]";}

struct SeExprEdEditable
{
    std::string name;
    int startPos,endPos;


    SeExprEdEditable(const std::string& name,int startPos,int endPos)
        :name(name),startPos(startPos),endPos(endPos)
    {

    }

    void updatePositions(const SeExprEdEditable& other)
    {
        startPos=other.startPos;endPos=other.endPos;
    }

    virtual ~SeExprEdEditable(){} // must have this to ensure destruction

    /// parses a comment. if false is returned then delete the control from the editable
    virtual bool parseComment(const std::string& comment)=0;
    virtual std::string str() const{return std::string("<unknown>");}
    virtual void appendString(std::stringstream& stream) const=0;
    virtual bool controlsMatch(const SeExprEdEditable&) const=0;

};


struct SeExprEdNumberEditable:public SeExprEdEditable
{
    double v;
    double min,max;
    bool isInt;
    SeExprEdNumberEditable(const std::string& name,int startPos,int endPos,double val)
        :SeExprEdEditable(name,startPos,endPos),v(val),min(0),max(1),isInt(false)
    {}
    
    bool parseComment(const std::string& comment){
        if (comment.find('.') != std::string::npos || comment.find('e') != std::string::npos)
        {
            float fmin,fmax;
            if (sscanf(comment.c_str(),"#%f,%f",&fmin,&fmax)==2)
            {
                min=fmin;
                max=fmax;
                isInt=false;
                return true;
            }
        }
        int imin,imax;
        if (sscanf(comment.c_str(),"#%d,%d",&imin,&imax)==2)
	{
            min=imin;
            max=imax;
            isInt=true;
            return true;
        }
        return true;
    }
    std::string str() const{
        std::stringstream s;
        s<<name<<" "<<v<<" in ["<<min<<","<<max<<"] subset "<<(isInt?"Integers":"Reals");
        return s.str();
    }
    void appendString(std::stringstream& stream) const{stream<<v;}

    virtual bool controlsMatch(const SeExprEdEditable& other) const
    {
        if(const SeExprEdNumberEditable* o=dynamic_cast<const SeExprEdNumberEditable*>(&other)){
            return min==o->min && max==o->max && v==o->v && isInt==o->isInt && name==o->name;
        }else return false;
    }

};


struct SeExprEdVectorEditable:public SeExprEdEditable
{
    SeVec3d v;
    double min,max;
    bool isColor;
    SeExprEdVectorEditable(const std::string& name,int startPos,int endPos,const SeVec3d& val)
        :SeExprEdEditable(name,startPos,endPos),v(val),min(0),max(1),isColor(true)
    {}
    
    bool parseComment(const std::string& comment){
        float fmin,fmax;
        int numParsed=sscanf(comment.c_str(),"#%f,%f",&fmin,&fmax);
        if(numParsed==2){
            isColor=false;
            min=fmin;
            max=fmax;
        }
        return true;
    }
    std::string str() const{
        std::stringstream s;
        s<<name<<" "<<v<<" in ["<<min<<","<<max<<"]";
        return s.str();
    }

    void appendString(std::stringstream& stream) const{printVal(stream,v);}

    virtual bool controlsMatch(const SeExprEdEditable& other) const
    {
        if(const SeExprEdVectorEditable* o=dynamic_cast<const SeExprEdVectorEditable*>(&other)){
            return min==o->min && max==o->max && v==o->v && name==o->name;
        }else return false;
    }

};

struct SeExprEdStringEditable:public SeExprEdEditable
{
    std::string v;
    std::string type;
    SeExprEdStringEditable(int startPos,int endPos,const std::string& val)
        :SeExprEdEditable("unknown",startPos,endPos),v(val)
    {}
    
    bool parseComment(const std::string& comment){
        char namebuf[1024],typebuf[1024];
        int parsed=sscanf(comment.c_str(),"#%s %s",typebuf,namebuf);
        if(parsed==2){
            name=namebuf;
            type=typebuf;
            return true;
        }else{
            return false;
        }
    }

    void appendString(std::stringstream& stream) const{ 
        // TODO: escape strs
        stream<<"\""<<v<<"\"";
    }
    std::string str() const{
        std::stringstream s;
        s<<name<<" "<<type<<" = "<<v;
        return s.str();
    }

    virtual bool controlsMatch(const SeExprEdEditable& other) const
    {
        if(const SeExprEdStringEditable* o=dynamic_cast<const SeExprEdStringEditable*>(&other)){
            return v==o->v && type==o->type &&  name==o->name;
        }else return false;
    }

};

template<class TVAL>
struct SeExprEdGenericCurveEditable:public SeExprEdEditable
{
    typedef typename SeExpr::SeCurve<TVAL> Curve ;
    typedef typename SeExpr::SeCurve<TVAL>::CV CV;
    typedef typename Curve::InterpType InterpType;

    std::vector<CV> cvs;
    SeExprEdGenericCurveEditable(const std::string& name,int startPos,int endPos)
        :SeExprEdEditable(name,startPos,endPos)
    {}
    
    void add(double x,const TVAL& y,int interp){
        cvs.push_back(CV(x,y,InterpType(interp)));
    }

    bool parseComment(const std::string& comment){UNUSED(comment); return true;}
    std::string str() const{
        std::stringstream s;
        s<<name<<" ccurve";
        return s.str();
    }

private:
public:
    void appendString(std::stringstream& stream) const{
        for(size_t i=0,sz=cvs.size();i<sz;i++){
            const CV& cv=cvs[i];
            stream<<","<<cv._pos<<",";
            printVal(stream,cv._val);
            stream<<","<<cv._interp;
        }
    }

    virtual bool controlsMatch(const SeExprEdEditable& other) const
    {
        if(const SeExprEdGenericCurveEditable* o=dynamic_cast<const SeExprEdGenericCurveEditable*>(&other)){
            // TODO: fix  this
//            return cvs==o->cvs && name==o->name;
            UNUSED(o);
            return false;
        }else return false;
    }

};
typedef SeExprEdGenericCurveEditable<SeVec3d> SeExprEdColorCurveEditable;
typedef SeExprEdGenericCurveEditable<double> SeExprEdCurveEditable;


struct SeExprEdAnimCurveEditable:public SeExprEdEditable
{
    std::string name;
    int startPos,endPos;
#ifdef SEEXPR_USE_ANIMLIB
    animlib::AnimCurve curve;
#endif
    std::string link;
    std::string animationSystemCurve;
    std::string newText;

    SeExprEdAnimCurveEditable(const std::string& name,int startPos,int endPos)
        :SeExprEdEditable(name,startPos,endPos)
#ifdef SEEXPR_USE_ANIMLIB
        ,curve(animlib::AnimAttrID())
#endif
    {}

    ~SeExprEdAnimCurveEditable(){} // must have this to ensure destruction

    bool parseComment(const std::string& comment){
        animationSystemCurve=comment;
        return true;
    }
    std::string str() const{
        std::stringstream s;
        s<<name<<" ccurve";
        return s.str();
    }
    void appendString(std::stringstream& stream) const{
#ifdef SEEXPR_USE_ANIMLIB
        if(newText.length()>0) stream<<newText;
        else{
            stream<<",\""<<animlib::AnimCurve::infinityTypeToString(curve.getPreInfinity())<<"\"";
            stream<<",\""<<animlib::AnimCurve::infinityTypeToString(curve.getPostInfinity())<<"\"";
            stream<<","<<curve.isWeighted();
            stream<<",\""<<link<<"\"";
            for(auto it=curve.getFirstKey(),itend=curve.getEndKey();it!=itend;++it){
                const animlib::AnimKeyframe& key=*it;
                stream<<","<<key.getTime()<<","<<key.getValue()<<","<<key.getInWeight()<<","<<key.getOutWeight()<<","
                      <<key.getInAngle()<<","<<key.getOutAngle()
                      <<",\""<<animlib::AnimKeyframe::tangentTypeToString(key.getInTangentType())
                      <<"\",\""<<animlib::AnimKeyframe::tangentTypeToString(key.getOutTangentType())
                      <<"\","<<key.isWeightsLocked();
            }
        }
#else
        UNUSED(stream);
#endif
    }
    virtual bool controlsMatch(const SeExprEdEditable& other) const
    {
        if(const SeExprEdAnimCurveEditable* o=dynamic_cast<const SeExprEdAnimCurveEditable*>(&other)){
            // TODO: fix  this
//            return cvs==o->cvs && name==o->name;
            UNUSED(o);
            return false;
        }else return false;
    }

};

#endif












