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

#ifndef MAKEDEPEND
#include <sstream>
#endif
#include "SeExprType.h"

bool
SeExprType::operator==(const SeExprType & other) const
{
    return (type() == other.type() &&
            dim () == other.dim ());
};

bool
SeExprType::isUnder(const SeExprType & other) const
{
    if     (other.isAny    ()) return isUnderAny    ();
    else if(other.isValue  ()) return isUnderValue  ();
    else if(other.isNumeric()) return isUnderNumeric();
    else                       return false;
};

bool
SeExprType::isa(const SeExprType & other) const
{
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

void
SeExprType::becomeLT(const SeExprType & first,
                     const SeExprType & second)
{
    if     (first.lt() == second.lt()) becomeLT(first);
    else if(first .isLTError  ()  ||
            second.isLTError  ())    becomeLTError();
    else if(first .isLTVarying()  ||
            second.isLTVarying())    becomeLTVarying();
    else if(first .isLTUniform()  ||
            second.isLTUniform())    becomeLTUniform();
    else                             becomeLTConstant();
};

void
SeExprType::becomeLT(const SeExprType & first,
                     const SeExprType & second,
                     const SeExprType & third)
{
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

std::string
SeExprType::toString() const
{
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
