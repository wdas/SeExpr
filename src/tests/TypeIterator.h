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
#ifndef TYPEITERATOR_H
#define TYPEITERATOR_H

#include <Expression.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "ExprNode.h"
#include "ExprFunc.h"
#include "TypeBuilder.h"

class TypeTesterExpr;  // forward declaration for parent pointer

/**
   @file TypeIterator.h
*/

class EnvironmentManager {
  public:
    EnvironmentManager(const std::string &name, TypeTesterExpr *parent) : _name(name), _parent(parent) {};

    inline TypeTesterExpr *parent() {
        return _parent;
    };
    inline TypeTesterExpr *parent() const {
        return _parent;
    };

    inline void set(const std::string &from) {
        parent()->setVar(_name, from);
    };

    inline ExprType type(const std::string &from) const {
        return parent()->resolveVar(from)->type();
    };

    inline std::string toString(const std::string &from) const {
        return type(from).toString();
    };

  private:
    std::string _name;
    TypeTesterExpr *_parent;
};

class Counter {
  public:
    Counter(int max) : _max(max), _current(0) {};

    inline int max() const {
        return _max;
    };
    inline int remaining() const {
        return max() - _current;
    };
    inline int start() {
        _current = 1;
        return remaining();
    };
    inline int next() {
        if (remaining() > 0) _current++;
        return remaining();
    };

  private:
    int _max;
    int _current;
};

class PrimaryTypeIterator {
  public:
    PrimaryTypeIterator(const std::string &var, TypeTesterExpr *parent)
        : _manager(var, parent), _counter(5), _current("Iterator Error") {};

    inline int start() {
        set("F1");
        return _counter.start();
    };

    inline int next() {
        int at = _counter.next();
        if (at == 3)
            set("F2");
        else if (at == 2)
            set("F3");
        else if (at == 1)
            set("ST");
        else if (at == 0)
            set("SE");
        return at;
    };

    inline int max() const {
        return _counter.max();
    };
    inline ExprType current() const {
        return _manager.type(_current);
    };
    inline std::string toString() const {
        return _manager.toString(_current);
    };
    inline int remaining() const {
        return _counter.remaining();
    };

  private:
    inline void set(const std::string &str) {
        _current = str;
        _manager.set(str);
    };

    EnvironmentManager _manager;
    Counter _counter;
    std::string _current;
};

class LifetimeTypeIterator {
  public:
    LifetimeTypeIterator(const std::string &var, TypeTesterExpr *parent)
        : _manager(var, parent), _counter(4), _current("Iterator Error") {};

    inline int start() {
        set("LC");
        return _counter.start();
    };

    inline int next() {
        int at = _counter.next();
        if (at == 2)
            set("LU");
        else if (at == 1)
            set("LV");
        else if (at == 0)
            set("LE");
        return at;
    };

    inline int max() const {
        return _counter.max();
    };
    inline ExprType current() const {
        return _manager.type(_current);
    };
    inline std::string toString() const {
        return _manager.toString(_current);
    };
    inline int remaining() const {
        return _counter.remaining();
    };

  private:
    inline void set(const std::string &str) {
        _current = str;
        _manager.set(str);
    };

    EnvironmentManager _manager;
    Counter _counter;
    std::string _current;
};

template <typename IteratorClass>
class DoubleTypeIterator {
  public:
    DoubleTypeIterator(const std::string &var1, const std::string &var2, TypeTesterExpr *parent)
        : _first(var1, parent), _second(var2, parent) {};

    inline int start() {
        _first.start();
        _second.start();

        return remaining();
    };

    inline int next() {
        if (_second.remaining() > 0)
            _second.next();
        else if (_first.remaining() > 0) {
            _second.start();
            _first.next();
        }

        return remaining();
    };

    inline int max() const {
        return _first.max() * _second.max();
    };

    inline ExprType first() const {
        return _first.current();
    };
    inline ExprType second() const {
        return _second.current();
    };

    inline std::string toString() const {
        return (_first.toString() + " " + _second.toString());
    };

    inline int remaining() const {
        return (_second.max() * _first.remaining() + _second.remaining());
    };

  private:
    IteratorClass _first;
    IteratorClass _second;
};

template <typename IteratorClass>
class TripleTypeIterator {
  public:
    TripleTypeIterator(const std::string &var1,
                       const std::string &var2,
                       const std::string &var3,
                       TypeTesterExpr *parent)
        : _first(var1, parent), _second(var2, var3, parent) {};

    inline int start() {
        _first.start();
        _second.start();

        return remaining();
    };

    inline int next() {
        if (_second.remaining() > 0)
            _second.next();
        else if (_first.remaining() > 0) {
            _second.start();
            _first.next();
        }

        return remaining();
    };

    inline int max() const {
        return _first.max() * _second.max();
    };

    inline ExprType first() const {
        return _first.current();
    };
    inline ExprType second() const {
        return _second.first();
    };
    inline ExprType third() const {
        return _second.second();
    };

    inline std::string toString() const {
        return (_first.toString() + " " + _second.toString());
    };

    inline int remaining() const {
        return (_second.max() * _first.remaining() + _second.remaining());
    };

  private:
    IteratorClass _first;
    DoubleTypeIterator<IteratorClass> _second;
};

class SingleWholeTypeIterator {
  public:
    typedef ExprType (*ProcType)(const ExprType &);

    SingleWholeTypeIterator(const std::string &var, const ProcType proc, TypeTesterExpr *parent)
        : _primary(var, parent), _lifetime(var, parent), _proc(proc), _switch(false) {};

    inline int start() {
        _primary.start();
        _switch = false;

        return remaining();
    };

    inline int next() {
        if (_primary.remaining() > 0)
            _primary.next();
        else if (_switch == false) {
            _lifetime.start();
            _switch = true;
        } else
            _lifetime.next();

        return remaining();
    };

    inline ExprType result() const {
        return _proc(current());
    };

    inline const std::string givenString() const {
        return current().toString();
    };

    inline int remaining() const {
        return _primary.remaining() + _lifetime.remaining();
    };

    inline ExprType current() const {
        if (!_switch)
            return _primary.current();
        else
            return _lifetime.current();
    };

  private:
    PrimaryTypeIterator _primary;
    LifetimeTypeIterator _lifetime;
    ProcType _proc;
    bool _switch;
};

class DoubleWholeTypeIterator {
  public:
    typedef ExprType (*ProcType)(const ExprType &, const ExprType &);

    DoubleWholeTypeIterator(const std::string &var1,
                            const std::string &var2,
                            const ProcType proc,
                            TypeTesterExpr *parent)
        : _primary(var1, var2, parent), _lifetime(var1, var2, parent), _proc(proc), _switch(false) {};

    inline int start() {
        _primary.start();
        _switch = false;
        return remaining();
    };

    inline int next() {
        if (_primary.remaining() > 0)
            _primary.next();
        else if (_switch == false) {
            _lifetime.start();
            _switch = true;
        } else
            _lifetime.next();

        return remaining();
    };

    inline ExprType result() const {
        return _proc(first(), second());
    };

    inline const std::string givenString() const {
        return (first().toString() + " " + second().toString());
    };

    inline int remaining() const {
        return _primary.remaining() + _lifetime.remaining();
    };

    inline ExprType first() const {
        if (!_switch)
            return _primary.first();
        else
            return _lifetime.first();
    };

    inline ExprType second() const {
        if (!_switch)
            return _primary.second();
        else
            return _lifetime.second();
    };

  private:
    DoubleTypeIterator<PrimaryTypeIterator> _primary;
    DoubleTypeIterator<LifetimeTypeIterator> _lifetime;
    ProcType _proc;
    bool _switch;
};

class TripleWholeTypeIterator {
  public:
    typedef ExprType (*ProcType)(const ExprType &, const ExprType &, const ExprType &);

    TripleWholeTypeIterator(const std::string &var1,
                            const std::string &var2,
                            const std::string &var3,
                            const ProcType proc,
                            TypeTesterExpr *parent)
        : _primary(var1, var2, var3, parent), _lifetime(var1, var2, var3, parent), _proc(proc), _switch(false) {};

    inline int start() {
        _primary.start();
        _switch = false;
        return remaining();
    };

    inline int next() {
        if (_primary.remaining() > 0)
            _primary.next();
        else if (_switch == false) {
            _lifetime.start();
            _switch = true;
        } else
            _lifetime.next();

        return remaining();
    };

    inline ExprType result() const {
        return _proc(first(), second(), third());
    };

    inline const std::string givenString() const {
        return (first().toString() + " " + second().toString() + " " + third().toString());
    };

    inline int remaining() const {
        return _primary.remaining() + _lifetime.remaining();
    };

    inline ExprType first() const {
        if (!_switch)
            return _primary.first();
        else
            return _lifetime.first();
    };

    inline ExprType second() const {
        if (!_switch)
            return _primary.second();
        else
            return _lifetime.second();
    };

    inline ExprType third() const {
        if (!_switch)
            return _primary.third();
        else
            return _lifetime.third();
    };

  private:
    TripleTypeIterator<PrimaryTypeIterator> _primary;
    TripleTypeIterator<LifetimeTypeIterator> _lifetime;
    ProcType _proc;
    bool _switch;
};

#endif  // TYPEITERATOR_H
