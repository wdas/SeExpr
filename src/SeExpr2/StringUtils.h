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
#ifndef StringUtils_h
#define StringUtils_h

#include <string>

//! Unescape a few common special characters in the input @p string and return
//! the result as a new one.
inline std::string unescapeString(const std::string& string) {
    std::string output(string);
    int index = 0;
    bool special = false;
    for (char c : string) {
        if (special == true) {
            special = false;
            switch (c) {
                case 'n':   output[index++] = '\n'; break;
                case 'r':   output[index++] = '\r'; break;
                case 't':   output[index++] = '\t'; break;
                case '\\':  output[index++] = '\\'; break;
                case '"':   output[index++] = '\"'; break;
                default:
                    // leave the escape sequence as it was
                    output[index++] = '\\';
                    output[index++] = c;
            }
        } else {
            if (c == '\\') {
                special = true;
            } else {
                output[index++] = c;
            }
        }
    }
    output.resize(index);
    return output;
}

#endif
