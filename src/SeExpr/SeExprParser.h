/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
*/
#ifndef SeExprParser_h
#define SeExprParser_h

#include <string>
#include <vector>


class SeExprNode;
class SeExpression;
bool SeExprParse(SeExprNode*& parseTree, std::string& error, int& errorStart, int& errorEnd,
    const SeExpression* expr, const char* str, std::vector<char*>* stringTokens);

#endif
