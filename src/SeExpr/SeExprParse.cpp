#include "SeExprParse.h"
#include <cassert>

class SeParser{
public:
    SeParser(const std::string& inputString)
    :lexer(inputString.c_str())
    {

    }

    void getToken(){
        token=lookAheadToken;
        lookAheadToken=lexer.getToken();
    }

    void parse(){
        std::cerr<<"we!"<<std::endl;
        getToken(); 
        getToken();
        module();
    }

    void module(){
        //std::cerr<<"we!"<<std::endl;
        //std::cerr<<"tok is "<<Lexer::getTokenName(token)<<" '"<<lexer.getTokenText()<<"'"<<std::endl;;
        while(token==Lexer::DEF) declaration();
        block();
    }

#if 0
    void ensure(bool value){
        if(!value)
            throw ParseError("Parse error");
    }
#endif
    void ensure(bool value, const std::string& msg){
        if(!value){
            throw ParseError(lexer.underlineToken());
        }
    }
    void ensureAndGetToken(bool value, const std::string& msg){
        ensure(value, msg);
        getToken();
    }
    void declaration(){
        ensureAndGetToken(token==Lexer::DEF, "Expected the word 'def' in declaration");
        if(token!=Lexer::IDENT) typeDeclare();
        ensureAndGetToken(token==Lexer::IDENT, "Expected identifier");
        ensure(token==Lexer::PAREN_OPEN, "Expected open parentheses for function declaration");
        typeList();
        ensureAndGetToken(token==Lexer::PAREN_CLOSE, "Expected close parentheses for function declaration");
    }

    void typeDeclare(){
        ensureAndGetToken(token==Lexer::STRING || token==Lexer::FLOAT, "Expected type 'FLOAT' or type 'STRING'");
        ensureAndGetToken(token==Lexer::BRACKET_OPEN, "Expected '['");
        ensureAndGetToken(token==Lexer::NUM, "Expected number here");
        ensureAndGetToken(token==Lexer::BRACKET_CLOSE, "Expected closing ']'");
        lifetimeOptional();
    }

    void lifetimeOptional(){
        if(token==Lexer::CONSTANT || token==Lexer::UNIFORM || token==Lexer::VARYING || token==Lexer::ERROR){
            getToken();
        }
    }

    void typeList(){
        while(token==Lexer::STRING || token==Lexer::FLOAT)
            typeDeclare();
    }

    bool isAssignOrMutator(Lexer::Token t) const{
        return t == Lexer::ASSIGN || t == Lexer::PLUS_EQUAL 
                || t == Lexer::MINUS_EQUAL || t == Lexer::TIMES_EQUAL || t == Lexer::DIVIDE_EQUAL
                || t == Lexer::MOD_EQUAL || t == Lexer::POWER_EQUAL;
    }

    void block(){
        while(1){
            if(token == Lexer::IF) ifthenelse();
            else if(token == Lexer::IDENT && isAssignOrMutator(lookAheadToken)){
                assign();
            }else{
                break; // done with the top part
            }
        }
        expr();
    }

    void expr(){
//      if(token == Lexer::ARROW)
        if(token == Lexer::NUM){
            getToken();
        }
    }

    void assign(){
        ensure(token == Lexer::IDENT, "Expected identifier at begin of assignment");
        getToken();
        ensure(isAssignOrMutator(token), "Expected mutator on assignment");
        getToken();
        expr();
        ensure(token == Lexer::SEMICOLON, "Expected semi-colon at end of assignment");
        getToken();
    }

    void ifthenelse(){
        assert(false);
    }

private:
    Lexer lexer;
    Lexer::Token token; // current token
    Lexer::Token lookAheadToken; // next token
};

#ifdef TEST_PARSER

int main(int argc,char*argv[])
{
    std::ifstream ifs(argv[1]);
    std::string content((std::istreambuf_iterator<char>(ifs)),
        std::istreambuf_iterator<char>());
    std::cerr<<"PARSING! '"<<content<<"'"<<std::endl;;
    SeParser parser(content.c_str());
    try{
        parser.parse();
    }catch(const ParseError& e){
        std::cerr<<"parse error: "<<e._errorStr<<std::endl;
    }
    return 0;
}
#endif