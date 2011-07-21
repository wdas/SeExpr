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
#include <SeExpression.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "SeExprNode.h"
#include "SeExprFunc.h"
#include "TypeBuilder.h"
#include "TypeTester.h"

/**
   @file TypeIterator.h
*/

class AbstractTypeIterator {
public:
    typedef SeExprType(*FindResultOne)  (const SeExprType &);
    typedef SeExprType(*FindResultTwo)  (const SeExprType &, const SeExprType &);
    typedef SeExprType(*FindResultThree)(const SeExprType &, const SeExprType &, const SeExprType &);

    AbstractTypeIterator(TypeTesterExpr * parent)
        : _parent(parent)
    {};

    virtual void start() = 0;
    virtual void next () = 0;
    virtual bool isEnd() const = 0;

    virtual       SeExprType  result     () const = 0;
    virtual const std::string givenString() const = 0;

protected:
    inline TypeTesterExpr * parent () { return _parent; };

private:
    TypeTesterExpr * _parent;
};

class AbstractVarTypeIterator : public AbstractTypeIterator {
public:
    AbstractVarTypeIterator(const std::string & var, int typeCount, TypeTesterExpr * parent)
        : AbstractTypeIterator(parent), _name(var), _max(typeCount), _state(0)
    {};

    virtual void start()       { _state = 0;            };
    virtual void next ()       { _state++;              };
    virtual bool isEnd() const { return _state >= _max; };

    virtual       SeExprType current     () const = 0;
    virtual       SeExprType  result     () const = 0;
    virtual const std::string givenString() const = 0;

protected:
    void set  (const std::string & from)       { parent()->setVar(_name, from); };
    int  state()                         const { return _state;                 };

private:
    std::string _name;
    int         _max;
    int         _state;
};

class PrimaryVarTypeIterator : public AbstractVarTypeIterator {
public:
    PrimaryVarTypeIterator(const std::string & var, TypeTesterExpr * parent)
        : AbstractVarTypeIterator(var, 5, parent)
    {};

    virtual void start() {
        AbstractVarTypeIterator::start();
        set("F1");
    };

    virtual void next() {
        AbstractVarTypeIterator::next();

        if     (state() == 1) set("F2");
        else if(state() == 2) set("F3");
        else if(state() == 3) set("ST");
        else if(state() == 4) set("SE");
    };

    virtual SeExprType current() const {
        if     (state() == 0) return SeExprType::FP1Type   ();
        else if(state() == 1) return SeExprType::FPNType   (2);
        else if(state() == 2) return SeExprType::FPNType   (3);
        else if(state() == 3) return SeExprType::StringType();
        else                  return SeExprType::ErrorType (); //state() == 4
    };

    virtual SeExprType result() const { return SeExprType::ErrorType(); };

    virtual const std::string givenString() const {
        if     (state() == 0) return SeExprType::FP1Type   () .toString();
        else if(state() == 1) return SeExprType::FPNType   (2).toString();
        else if(state() == 2) return SeExprType::FPNType   (3).toString();
        else if(state() == 3) return SeExprType::StringType() .toString();
        else                  return SeExprType::ErrorType () .toString(); //state() == 4
    };
};

class LifetimeVarTypeIterator : public AbstractVarTypeIterator {
public:
    LifetimeVarTypeIterator(const std::string & var, TypeTesterExpr * parent)
        : AbstractVarTypeIterator(var, 4, parent)
    {};

    virtual void start() {
        AbstractVarTypeIterator::start();
        set("LC");
    };

    virtual void next() {
        AbstractVarTypeIterator::next();

        if     (state() == 1) set("LU");
        else if(state() == 2) set("LV");
        else if(state() == 3) set("LE");
    };

    virtual SeExprType current() const {
        if     (state() == 0) return SeExprType::FP1Type_c();
        else if(state() == 1) return SeExprType::FP1Type_u();
        else if(state() == 2) return SeExprType::FP1Type_v();
        else                  return SeExprType::FP1Type_e(); //state() == 3
    };

    virtual SeExprType result() const { return SeExprType::ErrorType(); };

    virtual const std::string givenString() const {
        if     (state() == 0) return SeExprType::FP1Type_c().toString();
        else if(state() == 1) return SeExprType::FP1Type_u().toString();
        else if(state() == 2) return SeExprType::FP1Type_v().toString();
        else                  return SeExprType::FP1Type_e().toString(); //state() == 3
    };
};

class SingleWholeTypeIterator : public AbstractTypeIterator {
 public:
    SingleWholeTypeIterator(const std::string & var,
                            const AbstractTypeIterator::FindResultOne proc,
                            TypeTesterExpr * parent)
        : AbstractTypeIterator(     parent),
        _primary              (var, parent),
        _lifetime             (var, parent),
        _proc                 (proc),
        _inLT                 (false)
    {};

    virtual void start() { _primary.start(); };

    virtual void next() {
        if     (!_primary.isEnd()) { _primary .next (); if(_primary.isEnd()){_lifetime.start(); _inLT = true; } }
        else if(!_inLT)            { _lifetime.start(); _inLT = true; }
        else                         _lifetime.next ();
    };

    virtual bool isEnd() const { return _lifetime.isEnd(); };

    virtual SeExprType result() const { return _proc(current()); };

    virtual const std::string givenString() const { return current().toString(); };

 protected:
    SeExprType current() const {
        if(!_inLT) return _primary .current();
        else       return _lifetime.current();
    };

 private:
    typedef AbstractTypeIterator::FindResultOne ProcType;

    PrimaryVarTypeIterator  _primary;
    LifetimeVarTypeIterator _lifetime;
    ProcType                _proc;
    bool                    _inLT;
};

class DoubleWholeTypeIterator : public AbstractTypeIterator {
 public:
    DoubleWholeTypeIterator(const std::string & var1,
                            const std::string & var2,
                            const AbstractTypeIterator::FindResultTwo proc,
                            TypeTesterExpr * parent)
        : AbstractTypeIterator(      parent),
        _primary1             (var1, parent),
        _lifetime1            (var1, parent),
        _primary2             (var2, parent),
        _lifetime2            (var2, parent),
        _proc                 (proc),
        _inLT                 (false)
    {};

    virtual void start()       {
        _primary1.start();
        _primary2.start();
    };

    virtual void next ()       {
        if     (!_primary2 .isEnd())   _primary2 .next ();
        else if(!_primary1 .isEnd()) { _primary2 .start(); _primary1 .next ();               }
        else if(!_inLT)              { _lifetime2.start(); _lifetime1.start(); _inLT = true; }
        else if(!_lifetime2.isEnd())   _lifetime2.next ();
        else if(!_lifetime1.isEnd()) { _lifetime2.start(); _lifetime1.next ();               };
    };

    virtual bool isEnd() const { return _lifetime2.isEnd(); };

    virtual SeExprType result() const { return _proc(first_current(), second_current()); };

    virtual const std::string givenString() const { return (first_current() .toString() + " " +
                                                            second_current().toString()); };

 protected:
    SeExprType first_current() const {
        if(!_inLT) return _primary1 .current();
        else       return _lifetime1.current();
    };

    SeExprType second_current() const {
        if(!_inLT) return _primary2 .current();
        else       return _lifetime2.current();
    };

private:
    typedef AbstractTypeIterator::FindResultTwo ProcType;

    PrimaryVarTypeIterator  _primary1;
    LifetimeVarTypeIterator _lifetime1;
    PrimaryVarTypeIterator  _primary2;
    LifetimeVarTypeIterator _lifetime2;
    ProcType                _proc;
    bool                    _inLT;
};

class TripleWholeTypeIterator : public AbstractTypeIterator {
public:
    TripleWholeTypeIterator(const std::string & var1,
                            const std::string & var2,
                            const std::string & var3,
                            const AbstractTypeIterator::FindResultThree proc,
                            TypeTesterExpr * parent)
        : AbstractTypeIterator(      parent),
        _primary1             (var1, parent),
        _lifetime1            (var1, parent),
        _primary2             (var2, parent),
        _lifetime2            (var2, parent),
        _primary3             (var3, parent),
        _lifetime3            (var3, parent),
        _proc                 (proc),
        _inLT                 (false)
    {};

    virtual void start()       {
        _primary1.start();
        _primary2.start();
        _primary3.start();
    };

    virtual void next ()       {
        if     (!_primary3 .isEnd())   _primary3 .next ();
        else if(!_primary2 .isEnd()) { _primary3 .start(); _primary2 .next ();                                   }
        else if(!_primary1 .isEnd()) { _primary3 .start(); _primary2 .start(); _primary1 .next ();               }
        else if(!_inLT)              { _lifetime3.start(); _lifetime2.start(); _lifetime1.start(); _inLT = true; }
        else if(!_lifetime3.isEnd())   _lifetime3.next ();
        else if(!_lifetime2.isEnd()) { _lifetime3.start(); _lifetime2.next ();                                   }
        else if(!_lifetime1.isEnd()) { _lifetime3.start(); _lifetime2.start(); _lifetime1.next ();               };
    };

    virtual bool isEnd() const { return _lifetime3.isEnd(); };

    virtual SeExprType result() const { return _proc(first_current(), second_current(), third_current()); };

    virtual const std::string givenString() const { return (first_current ().toString() + " " +
                                                            second_current().toString() + " " +
                                                            third_current ().toString()); };

 protected:
    SeExprType first_current() const {
        if(!_inLT) return _primary1 .current();
        else       return _lifetime1.current();
    };

    SeExprType second_current() const {
        if(!_inLT) return _primary2 .current();
        else       return _lifetime2.current();
    };

    SeExprType third_current() const {
        if(!_inLT) return _primary3 .current();
        else       return _lifetime3.current();
    };

private:
    typedef AbstractTypeIterator::FindResultThree ProcType;

    PrimaryVarTypeIterator  _primary1;
    LifetimeVarTypeIterator _lifetime1;
    PrimaryVarTypeIterator  _primary2;
    LifetimeVarTypeIterator _lifetime2;
    PrimaryVarTypeIterator  _primary3;
    LifetimeVarTypeIterator _lifetime3;
    ProcType                _proc;
    bool                    _inLT;
};
