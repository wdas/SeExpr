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
#ifndef ExprType_h
#define ExprType_h

#include <vector>
#include <string>
#include <map>
#include <cassert>
#include <sstream>

#pragma push_macro("None")
#undef None

namespace SeExpr2 {
/** Describes an allowable type in the SeExpr parse tree
  There are four classes of types in SeExpr: tERROR, tFP, tSTRING, tNONE.
  These can be further modified by the lifetime modifier ltERROR, ltVARYING, ltUNIFORM, ltCONSTANT
  Typically a user constructs a type by doing something like this to get a 3-vector Float

  ExprType().FP(3).Varying() // make varying
  ExprType().FP(3).Uniform() // make uniform

 **/
class ExprType {
  public:
    //! Possible types
    enum Type {
        tERROR = 0,  ///< Error type (bad things happened here or upstream in tree)
        tFP,         ///< Floating point type (this combines with _d member to make vectors)
        tSTRING,     ///< String type
        tNONE
    };  ///< None type (anything like function prototypes or blocks of assignments)

    //! Lifetimes that are possible for type, note the order is from highest to lowest priority for promotion
    enum Lifetime {
        ltERROR = 0,  ///< Error in lifetime (uniform data depending on varying etc.)
        ltVARYING,    ///< Varying data (i.e. changes per evaluation point)
        ltUNIFORM,    ///< Uniform data (i.e. changes only on grids or pixel tiles, depending on how expr used)
        ltCONSTANT    ///< Constant data (i.e. sub parts of the tree that need only be computed once)
    };

    //! Default constructor for a type (error and lifetime error)
    ExprType() : _type(tERROR), _n(1), _lifetime(ltERROR) {}

    //! Fully specified type
    ExprType(Type type, int n, Lifetime lifetime) : _type(type), _n(n), _lifetime(lifetime) {
        assert(_n >= 1);
        assert(_type == tFP || _n == 1);
    }

    //! Copy constructor
    ExprType(const ExprType& other) : _type(other.type()), _n(other.dim()), _lifetime(other.lifetime()) {
        assert(_n >= 1);
        assert(_type == tFP || _n == 1);
    }

    /// Returns true if this and other do not match on type and dimension
    bool operator!=(const ExprType& other) const { return !(*this == other); }

    /// Returns true if this and other match type and dimension
    bool operator==(const ExprType& other) const {
        return (type() == other.type() && dim() == other.dim() && lifetime() == other.lifetime());
    }

    ///@{ @name Basic type mutator constructors

    /// Mutate this into a none type
    // TODO: "None" is bad name, /usr/include/X11/X.h potentially text replaces it.
    ExprType& None() {
        _type = tNONE;
        _n = 1;
        return *this;
    }
    /// Mutate this into a floating point type of dimension d
    ExprType& FP(int d) {
        _type = tFP;
        _n = d;
        return *this;
    }
    /// Mutate this into a string type
    ExprType& String() {
        _type = tSTRING;
        _n = 1;
        return *this;
    }
    /// Mutate this into an error type
    ExprType& Error() {
        _type = tERROR;
        _n = 1;
        return *this;
    }

    ///@}
    ///@{ @name Basic lifetime mutator constructors

    /// Mutate this into a constant lifetime
    ExprType& Constant() {
        _lifetime = ltCONSTANT;
        return *this;
    }
    /// Mutate this into a uniform lifetime
    ExprType& Uniform() {
        _lifetime = ltUNIFORM;
        return *this;
    }
    /// Mutate this into a varying lifetime
    ExprType& Varying() {
        _lifetime = ltVARYING;
        return *this;
    }
    /// Mutate this into a lifetime error
    ExprType& LifeError() {
        _lifetime = ltERROR;
        return *this;
    }
    ///@}

    /// @{ @name Lifetime propagation and demotion

    //! Assign the lifetime from type a to be my type
    ExprType& setLifetime(const ExprType& a) {
        _lifetime = a.lifetime();
        return *this;
    }

    //! Combine the lifetimes (min wins) of a and b and then assign them to this
    ExprType& setLifetime(const ExprType& a, const ExprType& b) {
        a.lifetime() < b.lifetime() ? setLifetime(a) : setLifetime(b);
        return *this;
    }

    //! Combine the lifetimes (min wins) of a and b and then assign them to this
    ExprType& setLifetime(const ExprType& a, const ExprType& b, const ExprType& c) {
        setLifetime(a, b);
        setLifetime(*this, c);
        return *this;
    }
    ///@}

    //####################################################################
    /// @{ @name Accessors and predicates

    // accessors
    Type type() const { return _type; }
    int dim() const { return _n; }
    Lifetime lifetime() const { return _lifetime; }

    //! Direct is predicate checks
    bool isFP() const { return _type == tFP; }
    bool isFP(int d) const { return _type == tFP && _n == d; }
    bool isValue() const { return _type == tFP || _type == tSTRING; }
    bool isValid() const { return !isError() && !isLifetimeError(); }
    bool isError() const { return type() == tERROR; }
    bool isString() const { return type() == tSTRING; }
    bool isNone() const { return type() == tNONE; }

    //! Checks if value types are compatible.
    static bool valuesCompatible(const ExprType& a, const ExprType& b) {
        return (a.isString() && b.isString()) ||
               (a._type == tFP && b._type == tFP && (a._n == 1 || b._n == 1 || a._n == b._n));
    }

    /// @}

    /// validity check: type is not an error

    // lifetime matchers
    bool isLifetimeConstant() const { return lifetime() == ltCONSTANT; }
    bool isLifetimeUniform() const { return lifetime() == ltUNIFORM; }
    bool isLifetimeVarying() const { return lifetime() == ltVARYING; }
    bool isLifetimeError() const { return lifetime() == ltERROR; }

    bool isLifeCompatible(const ExprType& o) const { return o.lifetime() >= lifetime(); }

    //! Stringify the type into a printable string
    std::string toString() const {
        std::stringstream ss;

        if (isLifetimeConstant())
            ss << "constant ";
        else if (isLifetimeUniform())
            ss << "uniform ";
        else if (isLifetimeVarying())
            ss << "varying ";
        else if (isLifetimeError())
            ss << "lifetime_error ";
        else
            ss << "Invalid_Lifetime ";

        if (isError())
            ss << "Error";
        else if (isFP(1))
            ss << "Float";
        else if (isFP())
            ss << "Float[" << dim() << "]";
        else if (isString())
            ss << "String";
        else if (isNone())
            ss << "None";
        else
            ss << "Invalid_Type";
        return ss.str();
    }

  private:
    //! Class of type)
    Type _type;
    //! Dimension of type _n==1 ignored if _type != FP
    int _n;
    //! lifetime of type
    Lifetime _lifetime;
};

/// Quick way to get a vector type i.e. 3 vec is TypeVec(3)
inline ExprType TypeVec(int n) { return ExprType().FP(n).Varying(); }

}  // namespace

#pragma pop_macro("None")

#endif
