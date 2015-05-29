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


#include <Expression.h>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;
using namespace SeExpr2;

class llvmexpr: public Expression {
public:
    //! Constructor that takes the expression to parse
    llvmexpr(const std::string& expr, const ExprType & type = ExprType().FP(3)) :
        Expression(expr, type)
    {}
};

// TODO: turn off parsing output.
int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "need a filename as argument\n";
        return 1;
    }

    // Test fp
    std::string line;
    std::ifstream FSProfileData(argv[1], std::ifstream::in);
    while (std::getline(FSProfileData, line)) {
        if(!line.size() || line.at(0) == ';')
            continue;

        string::size_type start = line.find_first_of('"');
        ++start;
        string::size_type end = line.find_last_of('"');
        string exprStr(line, start, end-start);

        std::cout << left << "\n";
        std::istringstream iss(string(line, end+1), std::istringstream::in);
        int dim = 0;
        iss >> dim;
        std::cout << "exprStr: " << exprStr << std::endl;
        std::cout << "dim: " << dim << std::endl;

        llvmexpr expr(exprStr, ExprType().FP(dim));
        if(!expr.isValid()) {
            std::cerr << expr.parseError() << std::endl;
            assert(false);
        }

        const double *result = expr.evalFP();

        for(int i=0; i < dim; ++i)
            std::cout << result[i] << ' ';
        std::cout << std::endl;
    }

    if(!argv[2])
        return 0;
    FSProfileData.close();

    // Test string
    FSProfileData.open(argv[2], std::ifstream::in);
    while (std::getline(FSProfileData, line)) {
        if(!line.size() || line.at(0) == ';')
            continue;

        std::cout << "exprStr: " << line << std::endl;
        llvmexpr expr(line, ExprType().String());
        if(!expr.isValid()) {
            std::cerr << expr.parseError() << std::endl;
            assert(false);
        }

        const char *result = expr.evalStr();

        std::cout << result;
    }
}
