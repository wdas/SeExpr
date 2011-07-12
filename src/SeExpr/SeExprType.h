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

class SeExprType {
 public:
    enum Type {tFP,
               tSTRING,
               tNONE,
               tERROR,
               tNUMERIC,
               tVALUE,
               tANY};

    enum Lifetime {ltCONSTANT,
                   ltUNIFORM,
                   ltVARYING,
                   ltERROR};

    SeExprType()
        : _type(tERROR),_n(1), _lifetime(ltERROR)
    {};

    SeExprType(Type type, int n)
        : _type(type), _n(n), _lifetime(ltVARYING)
    {
        assert(n >= 1);
        assert(type == tFP || n == 1);
    };

    SeExprType(Type type, int n, Lifetime lt)
        : _type(type), _n(n), _lifetime(lt)
    {
        assert(n >= 1);
        assert(type == tFP || n == 1);
    };

    inline Type     type() const { return _type;     };
    inline int      dim () const { return _n;        };
    inline Lifetime lt  () const { return _lifetime; };

    inline bool operator!=(const SeExprType & other) const { return !(*this == other); };

    ///Returns true if this and other match type and dimension
    inline bool operator==(const SeExprType & other) const {
        return (type() == other.type() &&
                dim () == other.dim ());
    };

    ///type is not an error
    inline bool isValid() const { return !isError(); };

    //strictly equal relation
    inline bool isAny    ()      const { return type() == tANY;                   };
    inline bool isNone   ()      const { return type() == tNONE;                  };
    inline bool isValue  ()      const { return type() == tVALUE;                 };
    inline bool isString ()      const { return type() == tSTRING;                };
    inline bool isNumeric()      const { return type() == tNUMERIC;               };
    inline bool isFP1    ()      const { return type() == tFP      && dim() == 1; };
    inline bool isFPN    ()      const { return type() == tFP      && dim()  > 1; };
    inline bool isFPN    (int i) const { return type() == tFP      && dim() == i; };
    inline bool isError  ()      const { return type() == tERROR;                 };

    //general strictly equal relation
    inline bool is(const SeExprType & other) const { return (*this == other); };

    //strictly under relation - does not contain itself
    inline bool isUnderAny    () const { return isNone  () || isaValue  (); };
    inline bool isUnderValue  () const { return isString() || isaNumeric(); };
    inline bool isUnderNumeric() const { return isFP1   () || isFPN     (); };

    //general strictly under relation
    inline bool isUnder(const SeExprType & other) const {
        if     (other.isAny    ()) return isUnderAny    ();
        else if(other.isValue  ()) return isUnderValue  ();
        else if(other.isNumeric()) return isUnderNumeric();
        else                       return false;
    };

    //equivalent (under, equal, or promotable) relation
    inline bool isaAny    ()      const { return isAny         () || isUnderAny    ();  };
    inline bool isaNone   ()      const { return isNone        ();                      };
    inline bool isaValue  ()      const { return isValue       () || isUnderValue  ();  };
    inline bool isaString ()      const { return isString      ();                      };
    inline bool isaNumeric()      const { return isNumeric     () || isUnderNumeric();  };
    inline bool isaFP1    ()      const { return isUnderNumeric();                      };
    inline bool isaFPN    ()      const { return isUnderNumeric();                      };
    inline bool isaFPN    (int i) const { return isFP1         () || isFPN         (i); };

    //general equivalent (under, equal, or promotable) relation
    inline bool isa(const SeExprType & other) const {
        if     (*this == other)    return true;
        else if(other.isAny    ()) return isaAny    ();
        else if(other.isNone   ()) return isaNone   ();
        else if(other.isValue  ()) return isaValue  ();
        else if(other.isString ()) return isaString ();
        else if(other.isNumeric()) return isaNumeric();
        else if(other.isFP1    ()) return isaFP1    ();
        else if(other.isFPN    ()) return isaFPN    (other.dim());
        else                       return false;
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

    /*
     * Lifetime-related functions
     */

    //
    inline bool isLTConstant() const { return lt() == ltCONSTANT; };
    inline bool isLTUniform () const { return lt() == ltUNIFORM;  };
    inline bool isLTVarying () const { return lt() == ltVARYING;  };
    inline bool isLTError   () const { return lt() == ltERROR;    };

    inline void becomeLTConstant() { _lifetime = ltCONSTANT; };
    inline void becomeLTUniform () { _lifetime = ltUNIFORM;  };
    inline void becomeLTVarying () { _lifetime = ltVARYING;  };
    inline void becomeLTError   () { _lifetime = ltERROR;    };

    inline void becomeLT(const SeExprType & first)   { _lifetime = first.lt(); };
    inline void becomeLT(const SeExprType & first,
                         const SeExprType & second) {
        if     (first.lt() == second.lt()) becomeLT(first);
        else if(first .isLTError  ()  ||
                second.isLTError  ())    becomeLTError();
        else if(first .isLTVarying()  ||
                second.isLTVarying())    becomeLTVarying();
        else if(first .isLTUniform()  ||
                second.isLTUniform())    becomeLTUniform();
        else                             becomeLTConstant();
    };
    inline void becomeLT(const SeExprType & first,
                         const SeExprType & second,
                         const SeExprType & third) {
        if     (first.lt() == second.lt()  &&
                first.lt() == third .lt()) becomeLT(first);
        else if(first .isLTError  ()  ||
                second.isLTError  ()  ||
                third .isLTError  ())    becomeLTError();
        else if(first .isLTVarying()  ||
                second.isLTVarying()  ||
                third .isLTVarying())    becomeLTVarying();
        else if(first .isLTUniform()  ||
                second.isLTUniform()  ||
                third .isLTUniform())    becomeLTUniform();
        else                             becomeLTConstant();
    };

    inline std::string toString() const {
        std::stringstream ss;
        if     (isAny    ()) ss << "Any";
        else if(isNone   ()) ss << "None";
        else if(isValue  ()) ss << "Value";
        else if(isString ()) ss << "String";
        else if(isNumeric()) ss << "Numeric";
        else if(isFP1    ()) ss << "FLOAT[1]";
        else if(isFPN    ()) ss << "FLOAT[" << dim() << "]";
        else if(isError  ()) ss << "Error";
        else                 ss << "toString Type Error";

        if     (isLTConstant()) ss << "(c)";
        else if(isLTUniform ()) ss << "(u)";
        else if(isLTVarying ()) ss << "(v)";
        else if(isLTError   ()) ss << "(e)";
        else                    ss << "(toString Lifetime Error)";

        return ss.str();
    };

 private:
    Type     _type;
    int      _n;
    Lifetime _lifetime;
};

#endif
