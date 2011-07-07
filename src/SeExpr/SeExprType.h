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
        if(other.isAny    ()) return isUnderAny    ();
        if(other.isValue  ()) return isUnderValue  ();
        if(other.isNumeric()) return isUnderNumeric();
        /*else*/              return false;
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
        if(*this == other)    return true;
        if(other.isAny    ()) return isaAny    ();
        if(other.isNone   ()) return isNone    ();
        if(other.isValue  ()) return isaValue  ();
        if(other.isString ()) return isString  ();
        if(other.isNumeric()) return isaNumeric();
        if(other.isFP1    ()) return isaFP1    ();
        if(other.isFPN    ()) return isaFPN    (other.dim());
        /*else*/              return false;
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

    inline std::string toString() const {
        if(isAny    ()) return "Any";
        if(isNone   ()) return "None";
        if(isValue  ()) return "Value";
        if(isString ()) return "String";
        if(isNumeric()) return "Numeric";
        if(isFP1    ()) return "FP1";
        if(isFPN    ()) return "FP" + dim();
        if(isError  ()) return "Error";
        return "toString Error";
    };

 private:
    Type _type;
    int  _n;
};

#endif
