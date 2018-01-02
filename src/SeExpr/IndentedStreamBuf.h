/*
CONFIDENTIAL INFORMATION: This software is the confidential and proprietary
information of Walt Disney Animation Studios ("WDAS"). This software may not
be used, disclosed, reproduced or distributed for any purpose without prior
written authorization and license from WDAS. Reproduction of any section of
this software must include this legend and all copyright notices.
© Disney Enterprises, Inc. All rights reserved.
*/

#pragma once

#include <streambuf>

class IndentedStreamBuf : public std::streambuf {
  protected:
    virtual int overflow(int ch) {
        if (_atNewline && ch != '\n') {
            _dst->sputn(_indentStr.data(), _indentStr.size());
        }
        _atNewline = ch == '\n';
        return _dst->sputc(ch);
    }

  public:
    explicit IndentedStreamBuf(std::streambuf* dest, int indent = 4)
        : _dst(dest), _atNewline(true), _indentStr(indent, ' '), _os(NULL) {}
    explicit IndentedStreamBuf(std::ostream& dest, int indent = 4)
        : _dst(dest.rdbuf()), _atNewline(true), _indentStr(indent, ' '), _os(&dest) {
        _os->rdbuf(this);
    }
    virtual ~IndentedStreamBuf() {
        if (_os != NULL) {
            _os->rdbuf(_dst);
        }
    }

  private:
    std::streambuf* _dst;
    bool _atNewline;
    std::string _indentStr;
    std::ostream* _os;
};