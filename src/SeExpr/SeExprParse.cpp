#include "SeExprParse.h"
#include <cassert>
#include <vector>
#include <memory>

class ASTNode{
public:

    struct sequence{
        template<typename... T> sequence(T&&...){}
    };

    template<typename... Args>
    ASTNode(const std::string& name, Args&&... args)
    :_name(name)
    {
        //sequence{(_children.emplace_back(std::forward<Args>(args)),1)...);
    }

    void addChild(std::unique_ptr<ASTNode> node){_children.push_back(std::move(node));}

    void print(std::ostream& out, int indent){
        out<<std::string(indent,' ')<<_name<<"\n";
        for(auto it=_children.begin(); it!=_children.end(); ++it){
            (*it)->print(out,indent+1);
        }
    }
private:
    std::string _name;
    int _startOffset, _endOffset;
    std::vector<std::unique_ptr<ASTNode>> _children;
};

typedef std::unique_ptr<ASTNode> ASTPtr;

class SeParser{
public:
    SeParser(const std::string& inputString)
    :lexer(inputString.c_str())
    {

    }

    void getToken(){
        token=lookAheadToken;
        tokenText=lookAheadTokenText;
        lookAheadToken=lexer.getToken();
        lookAheadTokenText=lexer.getTokenText();
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
            throw ParseError(msg+"\n"+lexer.underlineToken());
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

/*
    expr:  expr1 { -> IDENT (args) }*
    expr1: expr2 { ? expr2 : expr2 }*
    expr2: expr3 { OR expr3 }*
    expr3: expr4 { AND expr4 }*
    expr4: expr5 { eq|ne expr5 }*
    expr5: expr6 { lt|gt|le|ge expr6 }*
    expr6: expr7 { +|- expr7}*
    expr7: expr8 { *|/|% expr8}*
    expr8: {!|~}* expr9
    expr9: {expr10 ^}* expr10
    expr10: expr11 { [ expr ] }*
    expr11: (expr) | NUM | IDENT | IDENT (args) | STR
    args: expr {, expr}*
*/

    // expr:  expr1 { -> IDENT (args) }*
    ASTPtr expr(){
        ASTPtr curr=expr1();
        curr->print(std::cout,0);
        while(token==Lexer::ARROW){
            getToken();
            ensureAndGetToken(token==Lexer::IDENT, "Expected identifier");
            ensureAndGetToken(token==Lexer::PAREN_OPEN, "Expected open parentheses");
            functionArgs();
            ensureAndGetToken(token==Lexer::PAREN_CLOSE, "Expected closing parentheses");
        }
        return ASTPtr();
    }

    // expr1: expr2 { ? expr2 : expr2 }*
    ASTPtr expr1(){
        return expr6(); // TODO: put in other crap
    }

    ASTPtr expr6(){
        ASTPtr curr=expr7();
        while(token==Lexer::PLUS || token==Lexer::MINUS){
            std::string text=tokenText;
            getToken();
            ASTPtr newChild=expr7();
            ASTPtr(new ASTNode{text,std::move(curr)    }); //, std::move(curr), std::move(newChild)});
        }
        return curr;
    }

    ASTPtr expr7(){
        return expr8();
        #if 0 //TODO: fix
        while(token==Lexer::TIMES || token==Lexer::DIVIDE || token==Lexer::MOD){
            getToken();
            expr8();
        }
        #endif
    }

    ASTPtr expr8(){
        #if 0 //TODO: fix
        while(token==Lexer::TWIDLE || token==Lexer::NOT){
            getToken();
        }
        #endif
        return expr9();
    }

    ASTPtr expr9(){
        return expr10();
        #if 0 //TODO: fix
        while(token==Lexer::POWER){
            getToken();
            expr10();
        }
        #endif
    }

    ASTPtr expr10(){
        ASTPtr curr=expr11();
        while(token==Lexer::BRACKET_OPEN){
            #if 0
            getToken();
            ASTPtr result=expr();
            ASTPtr combined(new ASTNode("bracket"));
            combined->addChild(combined);
            combined->addChild(result);
            curr=combined;
            ensureAndGetToken(token==Lexer::BRACKET_CLOSE, "Did not get closing bracket to subscripting operator");
            #endif
            // TODO: fix
        }
        return curr;
    }

    // expr11: (expr) | NUM | IDENT | IDENT (args) | STR
    ASTPtr expr11(){
        if(token==Lexer::PAREN_OPEN){
            getToken();
            expr();
            ensureAndGetToken(token==Lexer::PAREN_CLOSE,"Expect closing parentheses after sub-expression");
        }else if(token==Lexer::NUM){
            getToken();
            return ASTPtr(new ASTNode("NUM"));
        }else if(token==Lexer::IDENT){
            ASTPtr callNode(new ASTNode("CALL"));
            getToken();
            if(token==Lexer::PAREN_OPEN){
                getToken();
                callNode->addChild(functionArgs());
                ensureAndGetToken(token==Lexer::PAREN_CLOSE, "Expected ending parentheses after function call");
            }else{
                return ASTPtr(new ASTNode("IDENT"));
            }
            return callNode;
        }else if(token==Lexer::STRING){
            // TODO: do something with string
            return ASTPtr(new ASTNode("STRING"));
        }else{
            ensure(false, "Unexpected type of token.");
        }
    }

    ASTPtr functionArgs(){
        // TODO: fix
        return ASTPtr(new ASTNode("args"));
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
    std::string tokenText;
    std::string lookAheadTokenText;
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