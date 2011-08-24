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

static const char* typeStrings[] = {"ERROR","FP","STRING","NONE","NUMERIC","VALUE","ANY"};

class SeExprType {
 public:
    enum Type {tERROR=0,
               tFP,
               tSTRING,
               tNONE,
               tNUMERIC,
               tVALUE,
               tANY};

    enum Lifetime {ltERROR=0,
                   ltCONSTANT,
                   ltUNIFORM,
                   ltVARYING
    };

    /*
     * Core type functions
     */

    SeExprType()
        : _type(tERROR),_n(1), _lifetime(ltERROR)
    {};

#if 1
    SeExprType(Type type)
        : _type(type), _n(1), _lifetime(ltVARYING)
    {};

    SeExprType(Type type, int n)
        : _type(type), _n(n), _lifetime(ltVARYING)
    {
        assert(_n >= 1);
        assert(_type == tFP || _n == 1);
    };

    SeExprType(Type type, int n, Lifetime lifetime)
        : _type(type), _n(n), _lifetime(lifetime)
    {
        assert(_n >= 1);
        assert(_type == tFP || _n == 1);
    };
#endif
    SeExprType(const SeExprType & other)
        : _type(other.type()), _n(other.dim()), _lifetime(other.lifetime())
    {
        assert(_n >= 1);
        assert(_type == tFP || _n == 1);
    };

    // Basic types (named constructor parameter idiom)
    SeExprType& None(){_type=tNONE;_n=1;return *this;}
    SeExprType& FP(int d){_type=tFP;_n=d;return *this;}
    SeExprType& String(){_type=tSTRING;_n=1;return *this;}
    SeExprType& Error(){_type=tERROR;_n=1;return *this;}

    // Lifetimes (named construct parameter idiom)
    SeExprType& Constant(){_lifetime=ltCONSTANT;return *this;}
    SeExprType& Uniform(){_lifetime=ltUNIFORM;return *this;}
    SeExprType& Varying(){_lifetime=ltVARYING;return *this;}
    SeExprType& LifeError(){_lifetime=ltERROR;return *this;}

    bool isFP() const {return _type==tFP;}
    bool isFP(int d) const {return _type==tFP && _n==d;}
    bool isValue() const {return _type==tFP || _type==tSTRING;}

    static bool valuesCompatible(const SeExprType& a,const SeExprType& b){
        return  (a.isString() && b.isString())
            || (a._type==tFP && b._type==tFP && (a._n==1 || b._n==1 || a._n==b._n));
    }

    //general constructors - varying
    static inline SeExprType AnyType_varying    ()      { return SeExprType(tANY,1,ltVARYING);     };
    static inline SeExprType NoneType_varying   ()      { return SeExprType(tNONE,1,ltVARYING);    };
    static inline SeExprType ValueType_varying  ()      { return SeExprType(tVALUE,1,ltVARYING);   };
    static inline SeExprType StringType_varying ()      { return SeExprType(tSTRING,1,ltVARYING);  };
    static inline SeExprType NumericType_varying()      { return SeExprType(tNUMERIC,1,ltVARYING); };
    static inline SeExprType FP1Type_varying    ()      { return SeExprType(tFP,1,ltVARYING);      };
    static inline SeExprType FPNType_varying    (int d) { return SeExprType(tFP,d,ltVARYING);      };
    static inline SeExprType ErrorType_varying  ()      { return SeExprType(tERROR,1,ltVARYING);   };

    //general constructors - uniform
    static inline SeExprType AnyType_uniform    ()      { return SeExprType(tANY,1,ltUNIFORM);     };
    static inline SeExprType NoneType_uniform   ()      { return SeExprType(tNONE,1,ltUNIFORM);    };
    static inline SeExprType ValueType_uniform  ()      { return SeExprType(tVALUE,1,ltUNIFORM);   };
    static inline SeExprType StringType_uniform ()      { return SeExprType(tSTRING,1,ltUNIFORM);  };
    static inline SeExprType NumericType_uniform()      { return SeExprType(tNUMERIC,1,ltUNIFORM); };
    static inline SeExprType FP1Type_uniform    ()      { return SeExprType(tFP,1,ltUNIFORM);      };
    static inline SeExprType FPNType_uniform    (int d) { return SeExprType(tFP,d,ltUNIFORM);      };
    static inline SeExprType ErrorType_uniform  ()      { return SeExprType(tERROR,1,ltUNIFORM);   };

    //general constructors - constant
    static inline SeExprType AnyType_constant    ()      { return SeExprType(tANY,1,ltCONSTANT);     };
    static inline SeExprType NoneType_constant   ()      { return SeExprType(tNONE,1,ltCONSTANT);    };
    static inline SeExprType ValueType_constant  ()      { return SeExprType(tVALUE,1,ltCONSTANT);   };
    static inline SeExprType StringType_constant ()      { return SeExprType(tSTRING,1,ltCONSTANT);  };
    static inline SeExprType NumericType_constant()      { return SeExprType(tNUMERIC,1,ltCONSTANT); };
    static inline SeExprType FP1Type_constant    ()      { return SeExprType(tFP,1,ltCONSTANT);      };
    static inline SeExprType FPNType_constant    (int d) { return SeExprType(tFP,d,ltCONSTANT);      };
    static inline SeExprType ErrorType_constant  ()      { return SeExprType(tERROR,1,ltCONSTANT);   };

    //general constructors - error
    static inline SeExprType AnyType_error    ()      { return SeExprType(tANY,1,ltERROR);     };
    static inline SeExprType NoneType_error   ()      { return SeExprType(tNONE,1,ltERROR);    };
    static inline SeExprType ValueType_error  ()      { return SeExprType(tVALUE,1,ltERROR);   };
    static inline SeExprType StringType_error ()      { return SeExprType(tSTRING,1,ltERROR);  };
    static inline SeExprType NumericType_error()      { return SeExprType(tNUMERIC,1,ltERROR); };
    static inline SeExprType FP1Type_error    ()      { return SeExprType(tFP,1,ltERROR);      };
    static inline SeExprType FPNType_error    (int d) { return SeExprType(tFP,d,ltERROR);      };
    static inline SeExprType ErrorType_error  ()      { return SeExprType(tERROR,1,ltERROR);   };

    //accessors
    inline Type     type    () const { return _type;     };
    inline int      dim     () const { return _n;        };
    inline Lifetime lifetime() const { return _lifetime; };

    ///Returns true if this and other match type and dimension
    inline bool operator==(const SeExprType & other) const {
        return (type    () == other.type    () &&
                dim     () == other.dim     () &&
                lifetime() == other.lifetime()   );
    };


    ///Returns true if this and other do not match on type and dimension
    inline bool operator!=(const SeExprType & other) const { return !(*this == other); };

    ///validity check: type is not an error
    inline bool isValid() const { return !isError() && !isLifetimeError(); };

    //strictly equal relation
    inline bool isAny    ()      const { return type() == tANY;                   };
    inline bool isNone   ()      const { return type() == tNONE;                  };
    //inline bool isValue  ()      const { return type() == tVALUE;                 };
    inline bool isString ()      const { return type() == tSTRING;                };
    inline bool isNumeric()      const { return type() == tNUMERIC;               };
    inline bool isFP1    ()      const { return type() == tFP      && dim() == 1; };
    inline bool isFP_gt_1()      const { return type() == tFP      && dim()  > 1; };
    inline bool isFPN    (int i) const { return type() == tFP      && dim() == i; };
    inline bool isError  ()      const { return type() == tERROR;                 };

    //general strictly equal relation
    inline bool is(const SeExprType & other) const {
        //TODO: add lifetime constraint in correctly: *this is less strict than other
        if     (*this == other)    return true;
        else if(other.isAny    ()) return isAny    ();
        else if(other.isNone   ()) return isNone   ();
        else if(other.isValue  ()) return isValue  ();
        else if(other.isString ()) return isString ();
        else if(other.isNumeric()) return isNumeric();
        else if(other.isFP1    ()) return isFP1    ();
        else if(other.isFP_gt_1()) return isFPN    (other.dim());
        else                       return false;
    };

    //strictly under relation - does not contain itself
    inline bool isUnderAny    () const { return isNone  () || isaValue  (); };
    inline bool isUnderValue  () const { return isString() || isaNumeric(); };
    inline bool isUnderNumeric() const { return isFP1   () || isFP_gt_1 (); };

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
    inline bool isaFP_gt_1()      const { return isUnderNumeric();                      };
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
        else if(other.isFP_gt_1()) return isaFPN    (other.dim());
        else                       return false;
    };

    /*
     * Lifetime-related functions
     */

    //lifetime matchers
    inline bool isLifetimeConstant() const { return lifetime() == ltCONSTANT; };
    inline bool isLifetimeUniform () const { return lifetime() == ltUNIFORM;  };
    inline bool isLifetimeVarying () const { return lifetime() == ltVARYING;  };
    inline bool isLifetimeError   () const { return lifetime() == ltERROR;    };

    //static lifetime assignment
    inline void becomeLifetimeConstant() { _lifetime = ltCONSTANT; };
    inline void becomeLifetimeUniform () { _lifetime = ltUNIFORM;  };
    inline void becomeLifetimeVarying () { _lifetime = ltVARYING;  };
    inline void becomeLifetimeError   () { _lifetime = ltERROR;    };

    //dynamic lifetime assignment
    inline void becomeLifetime(const SeExprType & first) { _lifetime = first.lifetime(); };

    //combination lifetime assignments
    inline void becomeLifetime(const SeExprType & first,
                               const SeExprType & second) {
        if     (first.lifetime() == second.lifetime()) becomeLifetime(first);
        else if(first .isLifetimeError  ()  ||
                second.isLifetimeError  ())            becomeLifetimeError();
        else if(first .isLifetimeVarying()  ||
                second.isLifetimeVarying())            becomeLifetimeVarying();
        else if(first .isLifetimeUniform()  ||
                second.isLifetimeUniform())            becomeLifetimeUniform();
        else                                           becomeLifetimeConstant();
    };
    inline void becomeLifetime(const SeExprType & first,
                               const SeExprType & second,
                               const SeExprType & third) {
        if     (first.lifetime() == second.lifetime()  &&
                first.lifetime() == third .lifetime())    becomeLifetime(first);
        else if(first .isLifetimeError  ()  ||
                second.isLifetimeError  ()  ||
                third .isLifetimeError  ())               becomeLifetimeError();
        else if(first .isLifetimeVarying()  ||
                second.isLifetimeVarying()  ||
                third .isLifetimeVarying())               becomeLifetimeVarying();
        else if(first .isLifetimeUniform()  ||
                second.isLifetimeUniform()  ||
                third .isLifetimeUniform())               becomeLifetimeUniform();
        else                                              becomeLifetimeConstant();

    };

    inline std::string toString() const {
        std::stringstream ss;

        if     (isLifetimeConstant()) ss << "constant ";
        else if(isLifetimeUniform ()) ss << "uniform ";
        else if(isLifetimeVarying ()) ss << "varying ";
        else if(isLifetimeError   ()) ss << "lifetime_error ";
        else                          ss << "Invalid_Lifetime ";

#if 0
        if     (isAny    ()) ss << "Any";
        else if(isNone   ()) ss << "None";
        else if(isString ()) ss << "String";
        else if(isNumeric()) ss << "Numeric";
        else if(isFP1    ()) ss << "FLOAT";
        else if(isFP_gt_1()) ss << "FLOAT[" << dim() << "]";
        else if(isValue  ()) ss << "Value";
        else if(isError  ()) ss << "Error";
        else                 ss << "Invalid_Type";
#endif
        ss<<typeStrings[_type];
        if(_type==tFP) ss<<"["<<_n<<"]";

        return ss.str();
    };

 private:
    Type     _type;
    int      _n;
    Lifetime _lifetime;
};

#endif
