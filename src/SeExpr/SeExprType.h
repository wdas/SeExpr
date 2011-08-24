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
#ifndef SeExprType_h
#define SeExprType_h

#include <vector>
#include <string>
#include <map>
#include <cassert>
#include <sstream>

/** Describes an allowable type in the SeExpr parse tree
  There are four classes of types in SeExpr: tERROR, tFP, tSTRING, tNONE.
  These can be further modified by the lifetime modifier ltERROR, ltVARYING, ltUNIFORM, ltCONSTANT
  Typically a user constructs a type by doing something like this to get a 3-vector Float

  SeExprType().FP(3).Varying() // make varying
  SeExprType().FP(3).Uniform() // make uniform
 
 **/
class SeExprType {
 public:
    //! Possible types
    enum Type {tERROR=0, ///< Error type (bad things happened here or upstream in tree)
               tFP,      ///< Floating point type (this combines with _d member to make vectors)
               tSTRING,  ///< String type
               tNONE};   ///< None type (anything like function prototypes or blocks of assignments)

    //! Lifetimes that are possible for type, note the order is from highest to lowest priority for promotion
    enum Lifetime {ltERROR=0, ///< Error in lifetime (uniform data depending on varying etc.)
                   ltVARYING, ///< Varying data (i.e. changes per evaluation point)
                   ltUNIFORM, ///< Uniform data (i.e. changes only on grids or pixel tiles, depending on how expr used)
                   ltCONSTANT ///< Constant data (i.e. sub parts of the tree that need only be computed once)
    };

    //! Default constructor for a type (error and lifetime error)
    SeExprType()
        : _type(tERROR),_n(1), _lifetime(ltERROR)
    {};


    //! Fully specified type
    SeExprType(Type type, int n, Lifetime lifetime)
        : _type(type), _n(n), _lifetime(lifetime)
    {
        assert(_n >= 1);
        assert(_type == tFP || _n == 1);
    };

    //! Copy constructor
    SeExprType(const SeExprType & other)
        : _type(other.type()), _n(other.dim()), _lifetime(other.lifetime())
    {
        assert(_n >= 1);
        assert(_type == tFP || _n == 1);
    };

    ///Returns true if this and other do not match on type and dimension
    inline bool operator!=(const SeExprType & other) const { return !(*this == other); };

    ///Returns true if this and other match type and dimension
    inline bool operator==(const SeExprType & other) const {
        return (type    () == other.type    () &&
                dim     () == other.dim     () &&
                lifetime() == other.lifetime()   );
    };

    ///@{ @name Basic type mutator constructors

    /// Mutate this into a none type
    SeExprType& None(){_type=tNONE;_n=1;return *this;}
    /// Mutate this into a floating point type of dimension d  
    SeExprType& FP(int d){_type=tFP;_n=d;return *this;}
    /// Mutate this into a string type 
    SeExprType& String(){_type=tSTRING;_n=1;return *this;}
    /// Mutate this into an error type 
    SeExprType& Error(){_type=tERROR;_n=1;return *this;}

    ///@}
    ///@{ @name Basic lifetime mutator constructors

    /// Mutate this into a constant lifetime 
    SeExprType& Constant(){_lifetime=ltCONSTANT;return *this;}
    /// Mutate this into a uniform lifetime 
    SeExprType& Uniform(){_lifetime=ltUNIFORM;return *this;}
    /// Mutate this into a varying lifetime 
    SeExprType& Varying(){_lifetime=ltVARYING;return *this;}
    /// Mutate this into a lifetime error 
    SeExprType& LifeError(){_lifetime=ltERROR;return *this;}
    ///@}

    /// @{ @name Lifetime propagation and demotion

    //! Assign the lifetime from type a to be my type
    SeExprType& setLifetime(const SeExprType & a) { _lifetime = a.lifetime(); return *this; };

    //! Combine the lifetimes (min wins) of a and b and then assign them to this
    SeExprType& setLifetime(const SeExprType & a,const SeExprType & b) {
        a.lifetime() < b.lifetime() ? setLifetime(a) : setLifetime(b);
        return *this;
    }

    //! Combine the lifetimes (min wins) of a and b and then assign them to this
    SeExprType&  setLifetime(const SeExprType & a,const SeExprType & b,const SeExprType & c) {
        setLifetime(a,b);
        setLifetime(*this,c);
        return *this;
    };
    ///@} 

    //####################################################################
    /// @{ @name Accessors and predicates

    //accessors
    inline Type type () const { return _type;     };
    inline int dim () const { return _n;        };
    inline Lifetime lifetime() const { return _lifetime; };

    //! Direct is predicate checks
    bool isFP() const {return _type==tFP;}
    bool isFP(int d) const {return _type==tFP && _n==d;}
    bool isValue() const {return _type==tFP || _type==tSTRING;}
    bool isValid() const {return !isError() && !isLifetimeError();}
    bool isError() const {return type() == tERROR;}
    bool isString() const {return type() == tSTRING;}
    bool isNone() const{return type() ==tNONE;}

    //! Checks if value types are compatible.
    static bool valuesCompatible(const SeExprType& a,const SeExprType& b){
        return  (a.isString() && b.isString())
            || (a._type==tFP && b._type==tFP && (a._n==1 || b._n==1 || a._n==b._n));
    }

    /// @}

    ///validity check: type is not an error

    //lifetime matchers
    inline bool isLifetimeConstant() const { return lifetime() == ltCONSTANT; };
    inline bool isLifetimeUniform() const { return lifetime() == ltUNIFORM; };
    inline bool isLifetimeVarying() const { return lifetime() == ltVARYING; };
    inline bool isLifetimeError() const { return lifetime() == ltERROR; };

    inline bool isLifeCompatible(const SeExprType& o) const{
        return o.lifetime()>=lifetime();
    }


    //! Stringify the type into a printable string
    inline std::string toString() const {
        std::stringstream ss;

        if(isLifetimeConstant()) ss << "constant ";
        else if(isLifetimeUniform ()) ss << "uniform ";
        else if(isLifetimeVarying ()) ss << "varying ";
        else if(isLifetimeError   ()) ss << "lifetime_error ";
        else  ss << "Invalid_Lifetime ";

        if(isError()) ss << "Error";
        else if(isFP(1)) ss << "Float";
        else if(isFP()) ss << "Float[" << dim() << "]";
        else if(isString ()) ss << "String";
        else if(isNone()) ss << "None";
        else ss << "Invalid_Type";
        return ss.str();
    };

 private:
    //! Class of type)
    Type     _type;
    //! Dimension of type _n==1 ignored if _type != FP
    int _n;
    //! lifetime of type
    Lifetime _lifetime;
};

#endif
