/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
*
* @file QdSeEditable.h
* @author Andrew Selle
*/
#ifndef __QdSeEditable__
#define __QdSeEditable__
#include <sstream>
#include <SeVec3d.h>
#include <SeCurve.h>
#include <cstdio>
#include <cstring>
#ifdef SEEXPR_USE_ANIMLIB
#   include <animlib/AnimCurve.h>
#   include <animlib/AnimKeyframe.h>
#endif
inline void printVal(std::stringstream& stream,double v){stream<<v;}
inline void printVal(std::stringstream& stream,const SeVec3d& v){stream<<"["<<v[0]<<","<<v[1]<<","<<v[2]<<"]";}

struct QdSeEditable
{
    std::string name;
    int startPos,endPos;


    QdSeEditable(const std::string& name,int startPos,int endPos)
        :name(name),startPos(startPos),endPos(endPos)
    {

    }

    void updatePositions(const QdSeEditable& other)
    {
        startPos=other.startPos;endPos=other.endPos;
    }

    virtual ~QdSeEditable(){} // must have this to ensure destruction

    /// parses a comment. if false is returned then delete the control from the editable
    virtual bool parseComment(const std::string& comment)=0;
    virtual std::string str() const{return std::string("<unknown>");}
    virtual void appendString(std::stringstream& stream) const=0;
    virtual bool controlsMatch(const QdSeEditable&) const=0;

};


struct QdSeNumberEditable:public QdSeEditable
{
    double v;
    double min,max;
    bool isInt;
    QdSeNumberEditable(const std::string& name,int startPos,int endPos,double val)
        :QdSeEditable(name,startPos,endPos),v(val),min(0),max(1),isInt(false)
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

    virtual bool controlsMatch(const QdSeEditable& other) const
    {
        if(const QdSeNumberEditable* o=dynamic_cast<const QdSeNumberEditable*>(&other)){
            return min==o->min && max==o->max && v==o->v && isInt==o->isInt && name==o->name;
        }else return false;
    }

};


struct QdSeVectorEditable:public QdSeEditable
{
    SeVec3d v;
    double min,max;
    bool isColor;
    QdSeVectorEditable(const std::string& name,int startPos,int endPos,const SeVec3d& val)
        :QdSeEditable(name,startPos,endPos),v(val),min(0),max(1),isColor(true)
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

    virtual bool controlsMatch(const QdSeEditable& other) const
    {
        if(const QdSeVectorEditable* o=dynamic_cast<const QdSeVectorEditable*>(&other)){
            return min==o->min && max==o->max && v==o->v && name==o->name;
        }else return false;
    }

};

struct QdSeStringEditable:public QdSeEditable
{
    std::string v;
    std::string type;
    QdSeStringEditable(int startPos,int endPos,const std::string& val)
        :QdSeEditable("unknown",startPos,endPos),v(val)
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

    virtual bool controlsMatch(const QdSeEditable& other) const
    {
        if(const QdSeStringEditable* o=dynamic_cast<const QdSeStringEditable*>(&other)){
            return v==o->v && type==o->type &&  name==o->name;
        }else return false;
    }

};

template<class TVAL>
struct QdSeGenericCurveEditable:public QdSeEditable
{
    typedef typename SeExpr::SeCurve<TVAL> Curve ;
    typedef typename SeExpr::SeCurve<TVAL>::CV CV;
    typedef typename Curve::InterpType InterpType;

    std::vector<CV> cvs;
    QdSeGenericCurveEditable(const std::string& name,int startPos,int endPos)
        :QdSeEditable(name,startPos,endPos)
    {}
    
    void add(double x,const TVAL& y,int interp){
        cvs.push_back(CV(x,y,InterpType(interp)));
    }

    bool parseComment(const std::string& comment){return true;}
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

    virtual bool controlsMatch(const QdSeEditable& other) const
    {
        if(const QdSeGenericCurveEditable* o=dynamic_cast<const QdSeGenericCurveEditable*>(&other)){
            // TODO: fix  this
//            return cvs==o->cvs && name==o->name;
            return false;
        }else return false;
    }

};
typedef QdSeGenericCurveEditable<SeVec3d> QdSeCCurveEditable;
typedef QdSeGenericCurveEditable<double> QdSeCurveEditable;


struct QdSeAnimCurveEditable:public QdSeEditable
{
    std::string name;
    int startPos,endPos;
#ifdef SEEXPR_USE_ANIMLIB
    animlib::AnimCurve curve;
#endif
    std::string link;
    std::string animationSystemCurve;
    std::string newText;

    QdSeAnimCurveEditable(const std::string& name,int startPos,int endPos)
        :QdSeEditable(name,startPos,endPos)
#ifdef SEEXPR_USE_ANIMLIB
        ,curve(animlib::AnimAttrID())
#endif
    {}

    ~QdSeAnimCurveEditable(){} // must have this to ensure destruction

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
#endif
    }
    virtual bool controlsMatch(const QdSeEditable& other) const
    {
        if(const QdSeAnimCurveEditable* o=dynamic_cast<const QdSeAnimCurveEditable*>(&other)){
            // TODO: fix  this
//            return cvs==o->cvs && name==o->name;
            return false;
        }else return false;
    }

};

#endif












