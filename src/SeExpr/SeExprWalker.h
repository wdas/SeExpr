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

#ifndef SeExprWalker_h
#define SeExprWalker_h

#include "SeExprNode.h"
#include "SeExprSpec.h"

class SeExprExaminer {
 public:
    virtual bool examine(SeExprNode* examinee) = 0;

    virtual void reset() = 0;
};

class SeExprConstExaminer {
 public:
    virtual bool examine(const SeExprNode* examinee) = 0;

    virtual void reset() = 0;
};

class SeExprWalker {
 public:
    SeExprWalker(SeExprExaminer * examiner)
	: _examiner(examiner)
    { _examiner->reset(); };

    /// Preorder walk
    void walk(SeExprNode * examinee);

 protected:
    void internalWalk(SeExprNode * examinee);
    void walkChildren(SeExprNode * parent);

 private:
    SeExprExaminer * _examiner;
};

class SeExprConstWalker {
 public:
    SeExprConstWalker(SeExprConstExaminer * examiner)
	: _examiner(examiner)
    { _examiner->reset(); };

    /// Preorder walk
    void walk(const SeExprNode * examinee);

 protected:
    void internalWalk(const SeExprNode * examinee);
    void walkChildren(const SeExprNode * parent);

 private:
    SeExprConstExaminer * _examiner;
};

/// Examiner that builds a list of all variable references
class SeExprVarListExaminer : public SeExprConstExaminer {

 public:
    virtual bool examine(const SeExprNode* examinee);

    virtual void reset() { _varList.clear(); };

    inline int length() const { return _varList.size(); };

    inline const SeExprVarNode* var(int i) const { return _varList[i]; };
 
 private:
    std::vector<const SeExprVarNode*> _varList;

};

/// Examiner that builds a list of specs potentially used in widgets (for qdgui)
class SeExprSpecExaminer : public SeExprConstExaminer {

 public:
    ~SeExprSpecExaminer();

    virtual bool examine(const SeExprNode* examinee);

    virtual void reset() { _specList.clear(); };

    inline int length() const { return _specList.size(); };

    inline const SeExprSpec* spec(int i) const { return _specList[i]; };

    inline std::vector<const SeExprSpec*>::const_iterator       begin() const;
    inline std::vector<const SeExprSpec*>::const_iterator const end  () const;

 private:
    std::vector<const SeExprSpec*> _specList;

};


#endif
