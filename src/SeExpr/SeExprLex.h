#pragma once

#include <iostream>
#include <string>
#include <map>
#include <stdexcept>
#include <cstdio>
#include <fstream>


struct ParseError{
    ParseError(const std::string& errorStr)
    :_errorStr(errorStr)
    {}

    std::string _errorStr;
};

class Lexer{
public:

    enum Token{
        END_OF_BUFFER=0,
        NUM,
        IDENT,
        EXTERN,
        DEF,
        FLOAT,
        STRING,
        CONSTANT,
        UNIFORM,
        VARYING,
        ERROR,
        IF,
        ELSE,
        NOT, // !
        OR, // ||
        AND, // &&
        EQUALS, // ==
        NOT_EQUALS,  // != 
        ASSIGN, // = 
        GREATER, // >
        LESS, // <
        GREATER_EQUAL, // >=
        LESS_EQUAL, // <=
        PLUS, //+
        PLUS_EQUAL, //+=
        MINUS, //-
        MINUS_EQUAL, //-=
        TIMES, //*
        TIMES_EQUAL, //*=
        DIVIDE, //*
        DIVIDE_EQUAL, // /=
        MOD, //^
        MOD_EQUAL, // ^=
        POWER, //^
        POWER_EQUAL, // ^=
        ARROW, //->
        TWIDLE, //~
        BRACKET_OPEN, //[
        BRACKET_CLOSE, //]
        BRACE_OPEN, //{
        BRACE_CLOSE, //}
        PAREN_OPEN, //(
        PAREN_CLOSE, //)
        QUESTION,
        COLON,
        SEMICOLON,
        COMMA
    };

    struct Buffer{

        Buffer(const char* bufIn)
        :buf(bufIn),curr(buf)
        {}
        // read current character
        char operator()() const{return *curr;}
        // Move character pointer backward
        void operator--(){
            // TODO: if you hit the edge this is wrong!
            if (*curr != '\0') curr--; 
            if(curr<buf) throw std::runtime_error("Unpop");
        }
        // Move character pointer forward (or not if we are at EOF)
        void operator++() {
            if(*curr != 0){
                curr++;
                if(*curr == '\n'){
                    line++;
                    col=0;
                }else{
                    col++;
                }
            }
        }

        void mark(){
            start=curr;
        }

        std::string markToCurr() const{
            std::string stringOut(start,curr);
            //std::copy(start,curr,stringOut.begin());
            return stringOut;
        }

        std::string underlineToken() const{
            const char *startLine, *endLine;
            int offset=0;
            for(startLine = start; start > buf && *startLine != '\n'; startLine--, offset++);
            for(endLine = curr;  *endLine != '\0' && *endLine != '\n'; endLine++);
            std::string newString(startLine,endLine-startLine+1);
            return "Line "+std::to_string(line)+ ": "+newString + "\n" + std::string(offset, ' ') + std::string(curr-start+1, '-');
        }

    private:
        const char* buf;
        const char* curr;
        const char* start;
        int line;
        int col;
    };


    Lexer(const char* bufIn)
    :buffer(bufIn)
    {
        populateReservedWords();
    }

    static std::string getTokenName(Token tok);

    std::string getTokenText() const{
        return buffer.markToCurr();
    }

    std::string underlineToken() const {
        return buffer.underlineToken();
    }

    Token getToken(){
        // First eat anything that is whitespace and comments
        bool ateCommentsOrWhitespace=true;
        while(ateCommentsOrWhitespace){
            ateCommentsOrWhitespace=false;
            // skip whitespace
            for(;buffer() == ' ' || buffer() == '\t' || buffer() == '\n'; ++buffer) ateCommentsOrWhitespace=true;
            // eat comments
            if(buffer() == '#'){
                while(buffer() != '\n' && buffer() != '\0') ++buffer;
                ++buffer;
                ateCommentsOrWhitespace=true;
            }
        }
        buffer.mark();

        // Look for idents and reserved words
        if(buffer()=='$' || isalpha(buffer())){
            std::string ident;
            //ident.push_back(buffer());
            if(buffer()=='$') ++buffer;
            for(;isalpha(buffer()) || buffer() == '_' || isdigit(buffer());++buffer) ident.push_back(buffer());
            auto it=reservedWords.find(ident);
            return it==reservedWords.end() ? IDENT : it->second;
        }

        // Look for quoted strings.
        if(buffer()=='"' || buffer() == '\''){
            std::string s;
            ++buffer;
            for(;;++buffer){
                if(buffer()=='\\'){
                    ++buffer;
                    s.push_back(buffer());
                }else if(buffer()=='\n' || buffer()=='\0'){
                    throw std::runtime_error("unterminated string at '"+s+"'");
                    return END_OF_BUFFER;
                }else if(buffer()=='"' || buffer()=='\''){
                    ++buffer;
                    break;
                }else{
                    s.push_back(buffer());
                }
            }
            return STRING;

        }
        // Look for numbers
        if(isdigit(buffer()) || buffer()=='.'){
            recognizeNumber();
            return NUM;
        }

        // next operators
        char firstChar = buffer();
        ++buffer;
        switch(firstChar){
            case '|': if(buffer() == '|'){++buffer;return OR;}break;
            case '&': if(buffer() == '&'){++buffer;return AND;}break;
            case '=': if(buffer()=='='){++buffer;return EQUALS;} else return ASSIGN;break;
            case '!': if(buffer()=='='){++buffer;return NOT_EQUALS;} else return NOT;break;
            case '>': if(buffer()=='='){++buffer;return GREATER_EQUAL;} else return GREATER;break;
            case '<': if(buffer()=='='){++buffer;return LESS_EQUAL;} else return LESS;break;
            case '+': if(buffer()=='='){++buffer;return PLUS_EQUAL;} else return PLUS;break;
            case '-': 
                if(buffer()=='='){++buffer;return MINUS_EQUAL;}
                if(buffer()=='>'){++buffer;return ARROW;}               
                else return MINUS;
                break;
            case '*': if(buffer()=='='){++buffer;return TIMES_EQUAL;} else return TIMES;break;
            case '/': if(buffer()=='='){++buffer;return DIVIDE_EQUAL;} else return DIVIDE;break;
            case '%': if(buffer()=='='){++buffer;return MOD_EQUAL;} else return MOD;break;
            case '^': if(buffer()=='='){++buffer;return POWER_EQUAL;} else return POWER;break;
            case '~': return TWIDLE;break;
            case '[': return BRACKET_OPEN;break;
            case ']': return BRACKET_CLOSE;break;
            case '{': return BRACE_OPEN;break;
            case '}': return BRACE_CLOSE;break;
            case '(': return PAREN_OPEN;break;
            case ')': return PAREN_CLOSE;break;
            case '?': return QUESTION;break;
            case ':': return COLON;break;
            case ';': return SEMICOLON;break;
            case ',': return COMMA;break;

        }

        if(buffer()=='\0') return END_OF_BUFFER;
        throw std::runtime_error("Invalid token");
    }

    int readDigits(std::string& numBuf){
        int count=0;
        for(;isdigit(buffer());++buffer,count++) 
            numBuf.push_back(buffer());
        return count;
    }

    void recognizeNumber(){
        double number;
        std::string numBuf;
        numBuf.reserve(32);
        readDigits(numBuf);
        if(buffer() == '.'){
            numBuf.push_back(buffer());
            ++buffer;
            readDigits(numBuf);
        }
        if(buffer() == 'e' || buffer() == 'E'){
            numBuf.push_back(buffer());
            ++buffer; //skip past e
            if(buffer()=='+' || buffer()=='-'){
                numBuf.push_back(buffer());
                ++buffer;
            }
            int count=readDigits(numBuf);
            if(count <= 0){
                throw std::runtime_error("Failed to get digits after e in number literal");
            }
        }
    }

    Token parseOperator(char primary, Token primaryToken, char secondary, Token secondaryToken){
        if(buffer() == primary){
            ++buffer;
            if(buffer() == secondary) return secondaryToken;
            --buffer;
            return primaryToken;
        }
        return END_OF_BUFFER;
    }

    static void populateReservedWords();

private:
    Buffer buffer;
    static std::map<std::string, Token> reservedWords;
};
