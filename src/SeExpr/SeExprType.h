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

class SeExprType {
 public:
    enum Type {tFP,
               tSTRING,
               tNONE,
               tERROR,
               tNUMERIC,
               tVALUE,
               tANY};

    SeExprType()
        : _type(tERROR),_n(1)
    {};

    SeExprType(Type type, int n)
        : _type(type), _n(n)
    {
        assert(n >= 1);
    };

    inline Type type() const { return _type; };
    inline int  dim () const { return _n;    };

    inline bool operator!=(const SeExprType & other) const { return !(*this == other); };

    ///Returns true if this and other match type and dimension
    inline bool operator==(const SeExprType & other) const {
        return (type() == other.type() &&
                dim () == other.dim ());
    };

    ///type is not an error or an uninitialized type
    inline bool isValid  ()      const { return !isError();                  };

    inline bool isAny    ()      const { return type() == tANY;              };
    inline bool isNone   ()      const { return type() == tNONE;             };
    inline bool isValue  ()      const { return type() == tVALUE;            };
    inline bool isString ()      const { return type() == tSTRING;           };
    inline bool isNumeric()      const { return type() == tNUMERIC;          };
    inline bool isFP     ()      const { return type() == tFP;               };
    inline bool isFP1    ()      const { return isFP()        && dim() == 1; };
    inline bool isFPN    (int i) const { return isFP()        && dim() == i; };
    inline bool isFPN    ()      const { return isFP()        && dim()  > 1; };
    inline bool isError  ()      const { return type() == tERROR;            };

    inline bool isUnderAny    () const { return isNone  () || isUnderValue(); };
    inline bool isUnderValue  () const { return isString() || isFP        (); };
    inline bool isUnderNumeric() const { return isFP    ();                   };

    inline bool isa(const SeExprType & other) const {
        if(*this == other)           //this and other are equal
            return true;
        else if(other.isAny())       //other is Any, this should be under Any
            return isUnderAny();
        else if(other.isValue())     //other is Value, this should be under Value
            return isUnderValue();
        else if(other.isNumeric())   //other is Numeric, this should be under Numeric
            return isFP();
        else if(other.isFP1())       //other is FP1, this should be under Numeric
            return isFP();
        else if(other.isFP())        //other is FPN, this should be either FP1 or FPN(other.dim())
            return isFP1() || isFPN(other.dim());
        else
            return false;
    };

    inline bool check(const SeExprType & other) const {
        return isValid() && isa(other);
    };

    static inline SeExprType AnyType    ()      { return SeExprType(tANY,1);     };
    static inline SeExprType NoneType   ()      { return SeExprType(tNONE,1);    };
    static inline SeExprType ValueType  ()      { return SeExprType(tVALUE,1);   };
    static inline SeExprType StringType ()      { return SeExprType(tSTRING,1);  };
    static inline SeExprType NumericType()      { return SeExprType(tNUMERIC,1); };
    static inline SeExprType FP1Type    ()      { return SeExprType(tFP,1);      };
    static inline SeExprType FPNType    (int d) { return SeExprType(tFP,d);      };
    static inline SeExprType ErrorType  ()      { return SeExprType(tERROR,1);   };

    ///two types are compatible vectors if:
    ///  both types are numeric and
    ///  either both have the same dimension or
    ///         at least one has dimension of 1 (is a scalar)
    inline bool compatibleNum(const SeExprType & other) const {
        return (isUnderNumeric()       &&
                other.isUnderNumeric() &&
                (dim()       == other.dim() ||
                 dim()       == 1           ||
                 other.dim() == 1));
    };

    /* inline bool compatible(const SeExprType & other) const { */
    /*     return (isValid()       && */
    /*             other.isValid() && */
    /*             (*this == other || */
    /*              compatibleNum(other))); */
    /* }; */

    inline std::string toString() const {
        if(isAny    ()            ) return "Any";
        if(isNone   ()            ) return "None";
        if(isValue  ()            ) return "Value";
        if(isString ()            ) return "String";
        if(isNumeric()            ) return "Numeric";
        if(isFP1    ()            ) return "FP1";
        if(isFP     () && !isFP1()) return "FP" + dim();
        if(isError  ()            ) return "Error";
        return "toString Error";
    };

 private:
    Type _type;
    int  _n;
};

#endif
