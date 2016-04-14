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

#ifndef ExprWalker_h
#define ExprWalker_h

namespace SeExpr2 {

template <class T, bool constnode>
struct ADD_CONST {
    typedef T TYPE;
};
template <class T>
struct ADD_CONST<T, true> {
    typedef const T TYPE;
};

template <bool constnode = false>
class Examiner {
  public:
    typedef typename ADD_CONST<ExprNode, constnode>::TYPE T_NODE;

    virtual bool examine(T_NODE* examinee) = 0;
    virtual void post(T_NODE* examinee) {};  // TODO: make this pure virt
    virtual void reset() = 0;
};

template <bool constnode = false>
class Walker {
  public:
    typedef Examiner<constnode> T_EXAMINER;
    typedef typename T_EXAMINER::T_NODE T_NODE;

    Walker(T_EXAMINER* examiner) : _examiner(examiner) {
        _examiner->reset();
    };

    /// Preorder walk
    void walk(T_NODE* examinee);

  protected:
    void internalWalk(T_NODE* examinee);
    void walkChildren(T_NODE* parent);

  private:
    T_EXAMINER* _examiner;
};

typedef Examiner<true> ConstExaminer;
typedef Walker<true> ConstWalker;
}
#endif
