/*
* Copyright Disney Enterprises, Inc.  All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License
* and the following modification to it: Section 6 Trademarks.
* deleted and replaced with:
*
* 6. Trademarks. This License does not grant permission to use the
* trade names, trademarks, service marks, or product names of the
* Licensor and its affiliates, except as required for reproducing
* the content of the NOTICE file.
*
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
*/
#include "SeExprParse.h"
#include "ASTNode.h"

// typedef std::unique_ptr<ASTNode> ASTPtr;

//#ifdef TEST_PARSER

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "usage: SeExprParse <filename>" << std::endl;
        exit(1);
    }

    std::ifstream ifs(argv[1]);
    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    // std::cerr<<"PARSING! '"<<content<<"'"<<std::endl;;
    SeParser<ASTPolicy> parser(content);
    try {
        auto tree = parser.parse();
        tree->print(std::cout, 0, &content);
    } catch (const ParseError& e) {
        std::cerr << "parse error: " << e._errorStr << std::endl;
    }
    return 0;
}
//#endif
