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
* @file Editable.h
* @author Andrew Selle
*/
#ifndef __Editable__
#define __Editable__
#include <sstream>
#include <SeExpr2/Vec.h>
#include <SeExpr2/Curve.h>
#include <cstdio>
#include <cstring>
#ifdef SEEXPR_USE_ANIMLIB
#include <animlib/AnimCurve.h>
#include <animlib/AnimKeyframe.h>
#endif
#include <ExprDeepWater.h>

inline void printVal(std::stringstream& stream, double v) { stream << v; }
inline void printVal(std::stringstream& stream, const SeExpr2::Vec3d& v) {
    stream << "[" << v[0] << "," << v[1] << "," << v[2] << "]";
}

#define UNUSED(x) (void)(x)

struct Editable {
    std::string name;
    int startPos, endPos;

    Editable(const std::string& name, int startPos, int endPos) : name(name), startPos(startPos), endPos(endPos) {}

    void updatePositions(const Editable& other) {
        startPos = other.startPos;
        endPos = other.endPos;
    }

    virtual ~Editable() {}  // must have this to ensure destruction

    /// parses a comment. if false is returned then delete the control from the editable
    virtual bool parseComment(const std::string& comment) = 0;
    virtual std::string str() const { return std::string("<unknown>"); }
    virtual void appendString(std::stringstream& stream) const = 0;
    virtual bool controlsMatch(const Editable&) const = 0;
};

struct NumberEditable : public Editable {
    double v;
    double min, max;
    bool isInt;
    NumberEditable(const std::string& name, int startPos, int endPos, double val)
        : Editable(name, startPos, endPos), v(val), min(0), max(1), isInt(false) {}

    bool parseComment(const std::string& comment) {
        if (comment.find('.') != std::string::npos || comment.find('e') != std::string::npos) {
            float fmin, fmax;
            if (sscanf(comment.c_str(), "#%f,%f", &fmin, &fmax) == 2) {
                min = fmin;
                max = fmax;
                isInt = false;
                return true;
            }
        }
        int imin, imax;
        if (sscanf(comment.c_str(), "#%d,%d", &imin, &imax) == 2) {
            min = imin;
            max = imax;
            isInt = true;
            return true;
        }
        return true;
    }
    std::string str() const {
        std::stringstream s;
        s << name << " " << v << " in [" << min << "," << max << "] subset " << (isInt ? "Integers" : "Reals");
        return s.str();
    }
    void appendString(std::stringstream& stream) const { stream << v; }

    virtual bool controlsMatch(const Editable& other) const {
        if (const NumberEditable* o = dynamic_cast<const NumberEditable*>(&other)) {
            return min == o->min && max == o->max && v == o->v && isInt == o->isInt && name == o->name;
        } else
            return false;
    }
};

struct VectorEditable : public Editable {
    SeExpr2::Vec3d v;
    double min, max;
    bool isColor;
    VectorEditable(const std::string& name, int startPos, int endPos, const SeExpr2::Vec3d& val)
        : Editable(name, startPos, endPos), v(val), min(0), max(1), isColor(true) {}

    bool parseComment(const std::string& comment) {
        float fmin, fmax;
        int numParsed = sscanf(comment.c_str(), "#%f,%f", &fmin, &fmax);
        if (numParsed == 2) {
            isColor = false;
            min = fmin;
            max = fmax;
        }
        return true;
    }
    std::string str() const {
        std::stringstream s;
        s << name << " " << v << " in [" << min << "," << max << "]";
        return s.str();
    }

    void appendString(std::stringstream& stream) const { printVal(stream, v); }

    virtual bool controlsMatch(const Editable& other) const {
        if (const VectorEditable* o = dynamic_cast<const VectorEditable*>(&other)) {
            return min == o->min && max == o->max && v == o->v && name == o->name;
        } else
            return false;
    }
};

struct StringEditable : public Editable {
    std::string v;
    std::string type;
    StringEditable(int startPos, int endPos, const std::string& val) : Editable("unknown", startPos, endPos), v(val) {}

    bool parseComment(const std::string& comment) {
        char namebuf[1024], typebuf[1024];
        int parsed = sscanf(comment.c_str(), "#%s %s", typebuf, namebuf);
        if (parsed == 2) {
            name = namebuf;
            type = typebuf;
            return true;
        } else {
            return false;
        }
    }

    void appendString(std::stringstream& stream) const {
        // TODO: escape strs
        stream << "\"" << v << "\"";
    }
    std::string str() const {
        std::stringstream s;
        s << name << " " << type << " = " << v;
        return s.str();
    }

    virtual bool controlsMatch(const Editable& other) const {
        if (const StringEditable* o = dynamic_cast<const StringEditable*>(&other)) {
            return v == o->v && type == o->type && name == o->name;
        } else
            return false;
    }
};

template <class TVAL>
struct GenericCurveEditable : public Editable {
    typedef typename SeExpr2::Curve<TVAL> Curve;
    typedef typename SeExpr2::Curve<TVAL>::CV CV;
    typedef typename Curve::InterpType InterpType;

    std::vector<CV> cvs;
    GenericCurveEditable(const std::string& name, int startPos, int endPos) : Editable(name, startPos, endPos) {}

    void add(double x, const TVAL& y, int interp) { cvs.push_back(CV(x, y, InterpType(interp))); }

    bool parseComment(const std::string& /*comment*/) { return true; }
    std::string str() const {
        std::stringstream s;
        s << name << " ccurve";
        return s.str();
    }

  private:
  public:
    void appendString(std::stringstream& stream) const {
        for (size_t i = 0, sz = cvs.size(); i < sz; i++) {
            const CV& cv = cvs[i];
            stream << "," << cv._pos << ",";
            printVal(stream, cv._val);
            stream << "," << cv._interp;
        }
    }

    virtual bool controlsMatch(const Editable& other) const {
        if (const GenericCurveEditable* o = dynamic_cast<const GenericCurveEditable*>(&other)) {
            // TODO: fix  this
            //            return cvs==o->cvs && name==o->name;
            UNUSED(o);
            return false;
        } else
            return false;
    }
};
typedef GenericCurveEditable<SeExpr2::Vec3d> ColorCurveEditable;
typedef GenericCurveEditable<double> CurveEditable;

struct AnimCurveEditable : public Editable {
    std::string name;
    int startPos, endPos;
#ifdef SEEXPR_USE_ANIMLIB
    animlib::AnimCurve curve;
#endif
    std::string link;
    std::string animationSystemCurve;
    std::string newText;

    AnimCurveEditable(const std::string& name, int startPos, int endPos)
        : Editable(name, startPos, endPos)
#ifdef SEEXPR_USE_ANIMLIB
          ,
          curve(animlib::AnimAttrID())
#endif
    {
    }

    ~AnimCurveEditable() {}  // must have this to ensure destruction

    bool parseComment(const std::string& comment) {
        animationSystemCurve = comment;
        return true;
    }
    std::string str() const {
        std::stringstream s;
        s << name << " ccurve";
        return s.str();
    }
    void appendString(std::stringstream& stream) const {
#ifdef SEEXPR_USE_ANIMLIB
        if (newText.length() > 0)
            stream << newText;
        else {
            stream << ",\"" << animlib::AnimCurve::infinityTypeToString(curve.getPreInfinity()) << "\"";
            stream << ",\"" << animlib::AnimCurve::infinityTypeToString(curve.getPostInfinity()) << "\"";
            stream << "," << curve.isWeighted();
            stream << ",\"" << link << "\"";
            for (auto it = curve.getFirstKey(), itend = curve.getEndKey(); it != itend; ++it) {
                const animlib::AnimKeyframe& key = *it;
                stream << "," << key.getTime() << "," << key.getValue() << "," << key.getInWeight() << ","
                       << key.getOutWeight() << "," << key.getInAngle() << "," << key.getOutAngle() << ",\""
                       << animlib::AnimKeyframe::tangentTypeToString(key.getInTangentType()) << "\",\""
                       << animlib::AnimKeyframe::tangentTypeToString(key.getOutTangentType()) << "\","
                       << key.isWeightsLocked();
            }
        }
#else
        UNUSED(stream);
#endif
    }
    virtual bool controlsMatch(const Editable& other) const {
        if (const AnimCurveEditable* o = dynamic_cast<const AnimCurveEditable*>(&other)) {
            // TODO: fix  this
            //            return cvs==o->cvs && name==o->name;
            UNUSED(o);
            return false;
        } else
            return false;
    }
};

struct ColorSwatchEditable : public Editable {
    std::vector<SeExpr2::Vec3d> colors;
    std::string labelType;

    ColorSwatchEditable(const std::string& name, int startPos, int endPos) : Editable(name, startPos, endPos) {}

    bool parseComment(const std::string& comment) {
        char labelbuf[1024];
        int parsed = sscanf(comment.c_str(), "#%s", labelbuf);
        if (parsed == 1) {
            labelType = labelbuf;
            return true;
        }
        return true;
    }

    std::string str() const {
        std::stringstream s;
        s << name << " swatch";
        return s.str();
    }

    void appendString(std::stringstream& stream) const {
        for (size_t i = 0, sz = colors.size(); i < sz; i++) {
            const SeExpr2::Vec3d& color = colors[i];
            stream << ",";
            printVal(stream, color);
        }
    }

    virtual bool controlsMatch(const Editable& other) const {
        if (const ColorSwatchEditable* o = dynamic_cast<const ColorSwatchEditable*>(&other)) {
            // TODO: determine when controls match
            UNUSED(o);
            return false;
        } else
            return false;
    }

    void add(const SeExpr2::Vec3d& value) { colors.push_back(value); }

    void change(int index, const SeExpr2::Vec3d& value) { colors[index] = value; }

    void remove(int index) { colors.erase(colors.begin() + index); }

    void print() {
        std::cerr << "\nColorSwatchEditable:\n";
        for (unsigned int i = 0; i < colors.size(); i++) {
            std::cerr << colors[i][0] << ", " << colors[i][1] << ", " << colors[i][2] << std::endl;
        }
    }
};

struct DeepWaterEditable : public Editable {
    SeDeepWaterParams params;

    DeepWaterEditable(const std::string& name, int startPos, int endPos) : Editable(name, startPos, endPos) {}

    void setParams(const SeDeepWaterParams& paramsIn) { params = paramsIn; }

    bool parseComment(const std::string& /*comment*/) { return true; }

    std::string str() const {
        std::stringstream s;
        s << name << " deepWater";
        return s.str();
    }

    void appendString(std::stringstream& stream) const {
        stream << "," << params.resolution;
        stream << "," << params.tileSize;
        stream << "," << params.lengthCutoff;
        stream << "," << params.amplitude;
        stream << "," << params.windAngle;
        stream << "," << params.windSpeed;
        stream << "," << params.directionalFactorExponent;
        stream << "," << params.directionalReflectionDamping << ",";
        printVal(stream, params.flowDirection);
        stream << "," << params.sharpen;
        stream << "," << params.time;
        stream << "," << params.filterWidth;
    }

    virtual bool controlsMatch(const Editable& other) const {
        if (const DeepWaterEditable* o = dynamic_cast<const DeepWaterEditable*>(&other)) {
            // TODO: determine when controls match
            UNUSED(o);
            return false;
        } else
            return false;
    }
};

#endif
