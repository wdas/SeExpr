#include "SeExprParse.h"
#include <cassert>
#include <vector>
#include <memory>

class ASTNode{
public:
    /// Allows adding arbitary number of items to a container holding unique_ptrs
    struct Adder{
        void sequence(){}
        template<typename T,typename... Targs> void sequence(T&& guy,Targs&&... args){
            adder.emplace_back(std::move(guy));
            sequence(args...);
        }
        Adder(std::vector<std::unique_ptr<ASTNode>>& container):container(container){}
    private:
        std::vector<std::unique_ptr<ASTNode>>& container;
    };

    template<typename... Args>
    ASTNode(const std::string& name, Args&&... args)
    :_name(name)
    {
        Adder a(_children);
        a.sequence(std::forward<Args&&>(args)...);
    }

    ASTNode* addChild(std::unique_ptr<ASTNode> node){_children.push_back(std::move(node));return _children.back().get();}
    std::unique_ptr<ASTNode> removeChild(){
        std::unique_ptr<ASTNode> ret(std::move(_children.back()));
        _children.pop_back();
        return ret;
    }

    void print(std::ostream& out, int indent){
        out<<std::string(indent,' ')<<_name<<"\n";
        for(auto it=_children.begin(); it!=_children.end(); ++it){
            (*it)->print(out,indent+1);
        }
    }
private:
    std::string _name;
    //int _startOffset, _endOffset;
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
        tokenPosition=lookAheadTokenPosition;
        std::cerr<<"Token is '"<<tokenText<<"' at "<<tokenPosition[0]<<std::endl;
        lookAheadToken=lexer.getToken();
        lookAheadTokenText=lexer.getTokenText();
        lookAheadTokenPosition=lexer.getTokenPosition();
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
            throw ParseError(msg+"\n"+lexer.underlineToken(tokenPosition));
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

    ASTPtr block(){
        ASTPtr ret(new ASTNode("block"));
        while(1){
            if(token == Lexer::IF) 
                ret->addChild(ifthenelse());
            else if(token == Lexer::IDENT && isAssignOrMutator(lookAheadToken)) 
                ret->addChild(assign());
            else
                break; // done with the top part
        }
        ret->addChild(expr());
        ret->print(std::cout,0);
        return ret;
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
        ASTPtr parent=expr1();
        while(token==Lexer::ARROW){
            getToken();
            ensureAndGetToken(token==Lexer::IDENT, "Expected identifier");
            ensureAndGetToken(token==Lexer::PAREN_OPEN, "Expected open parentheses in -> call");
            parent=std::move(functionArgs(std::move(parent)));
            ensureAndGetToken(token==Lexer::PAREN_CLOSE, "Expected closing parentheses in -> call");
        }
        return parent;
    }

    // expr1: expr2 { ? expr2 : expr2 }*
    ASTPtr expr1(){
        return expr2(); // TODO: put in other crap
    }

    // expr2: expr3 { OR expr3 }*
    ASTPtr expr2(){
        ASTPtr curr=expr3();
        while(token==Lexer::OR){
            std::string text=tokenText;
            getToken();
            curr=ASTPtr(new ASTNode(text,std::move(curr),expr3())) ;
        }
        return curr;
    }

    // expr3: expr4 { AND expr4 }*
    ASTPtr expr3(){
        ASTPtr curr=expr4();
        while(token==Lexer::AND){
            std::string text=tokenText;
            getToken();
            curr=ASTPtr(new ASTNode(text,std::move(curr),expr4())) ;
        }
        return curr;
    }

    // expr4: expr5 { EQ|NE expr5 }*
    ASTPtr expr4(){
        ASTPtr curr=expr5();
        while(token==Lexer::EQUALS || token==Lexer::NOT_EQUALS){
            std::string text=tokenText;
            getToken();
            curr=ASTPtr(new ASTNode(text,std::move(curr),expr5())) ;
        }
        return curr;
    }

    // expr5: expr6 { <|<=|>|>= expr6 }*
    ASTPtr expr5(){
        ASTPtr curr=expr6();
        while(token==Lexer::LESS || token==Lexer::LESS_EQUAL || token==Lexer::GREATER || token==Lexer::GREATER_EQUAL){
            std::string text=tokenText;
            getToken();
            curr=ASTPtr(new ASTNode(text,std::move(curr),expr6())) ;
        }
        return curr;
    }

    // expr6: expr7 { +|- expr7}*
    ASTPtr expr6(){
        ASTPtr curr=expr7();
        while(token==Lexer::PLUS || token==Lexer::MINUS){
            std::string text=tokenText;
            getToken();
            ASTPtr newChild=expr7();
            curr=ASTPtr(new ASTNode(text,std::move(curr),std::move(newChild))) ;
        }
        return curr;
    }

    // expr7: expr8 { *|/|% expr8}*
    ASTPtr expr7(){
        ASTPtr curr=expr8();
        while(token==Lexer::TIMES || token==Lexer::DIVIDE || token==Lexer::MOD){
            std::string text=tokenText;
            getToken();
            ASTPtr newChild=expr8();
            curr=ASTPtr(new ASTNode(text,std::move(curr),std::move(newChild))) ;
        }
        return curr;
    }

    // expr8: {!|~|-|+}* expr9
    ASTPtr expr8(){
        ASTPtr parent;
        ASTNode* curr=0;
        while(token==Lexer::TWIDLE || token==Lexer::NOT || token==Lexer::MINUS || token==Lexer::PLUS){
            ASTPtr op(new ASTNode(tokenText));
            if(!parent){
                parent=std::move(op);
                curr=parent.get();
            }else{
                curr=curr->addChild(std::move(op));
            }
            getToken();
        }
        if(parent){
            curr->addChild(expr9());
            return parent;
        }else return expr9();
    }

    //expr9: {expr10 ^}* expr10
    ASTPtr expr9(){
        ASTPtr parent=expr10();
        ASTNode* curr=0;
        while(token==Lexer::POWER){
            getToken();
            ASTPtr newNode=expr10();
            if(!curr){
                parent=ASTPtr(new ASTNode("^",parent,newNode));
                curr=parent.get();
            }else{
                ASTPtr oldNode = curr->removeChild();
                curr=curr->addChild(ASTPtr(new ASTNode("^",oldNode,newNode)));
            }
        }
        return parent;
    }

    // expr10: expr11 { [ expr ] }*
    ASTPtr expr10(){
        ASTPtr curr=expr11();
        while(token==Lexer::BRACKET_OPEN){
            getToken();
            ASTPtr combined=ASTPtr(new ASTNode("bracket",std::move(curr),expr()));
            curr=std::move(combined);
            ensureAndGetToken(token==Lexer::BRACKET_CLOSE, "Did not get closing bracket to subscripting operator");
        }
        return curr;
    }

    // expr11: (expr) | NUM | IDENT | IDENT (args) | STR | '[' expr {, expr}* ']'
    ASTPtr expr11(){
        if(token==Lexer::PAREN_OPEN){
            getToken();
            ASTPtr ret=expr();
            ensureAndGetToken(token==Lexer::PAREN_CLOSE,"Expect closing parentheses after sub-expression");
            return ret;
        }else if(token==Lexer::NUM){
            std::string numStr=tokenText;
            getToken();
            return ASTPtr(new ASTNode("NUM = "+numStr));
        }else if(token==Lexer::IDENT){
            ASTPtr callNode(new ASTNode("CALL"));
            getToken();
            if(token==Lexer::PAREN_OPEN){
                getToken();
                callNode->addChild(functionArgs(nullptr));
                ensureAndGetToken(token==Lexer::PAREN_CLOSE, "Expected ending parentheses after function call arguments");
            }else{
                return ASTPtr(new ASTNode("IDENT"));
            }
            return callNode;
        }else if(token==Lexer::STRING){
            // TODO: do something with string
            return ASTPtr(new ASTNode("STRING"));
        }else if(token==Lexer::BRACKET_OPEN){
            getToken();
            ASTPtr vec(new ASTNode("vector"));
            vec->addChild(expr());
            while(token==Lexer::COMMA){
                getToken    ();
                vec->addChild(expr());
            }
            ensureAndGetToken(token==Lexer::BRACKET_CLOSE,"Expected bracket close at end of vector");
            return vec;
        }else{
            ensure(false, "At this point I expected, an identifier, function call, literal, or sub-expression.");
        }
        return 0;
    }

    ASTPtr functionArgs(ASTPtr firstArgumentForCurry){
        // TODO: fix
        ASTPtr args(new ASTNode("args"));
        if(firstArgumentForCurry) args->addChild(std::move(firstArgumentForCurry));
        while(token != Lexer::PAREN_CLOSE){
            args->addChild(expr());
            if(token != Lexer::COMMA) break;
            getToken();
        }
        return args;
    }

    ASTPtr assign(){
        ensure(token == Lexer::IDENT, "Expected identifier at begin of assignment");
        getToken();
        ensure(isAssignOrMutator(token), "Expected mutator on assignment (got '')"+tokenText);
        getToken();
        ASTPtr e=expr();
        ASTPtr assignNode(new ASTNode("assign",e));
        ensure(token == Lexer::SEMICOLON, "Expected semi-colon at end of assignment");
        getToken();
        return assignNode;
    }

    ASTPtr ifthenelse(){
        ensureAndGetToken(token == Lexer::IF, "Expected if at begin of if then else");
        ensureAndGetToken(token == Lexer::PAREN_OPEN, "Expected open parentheses after if");
        ASTPtr condition=expr();
        ensureAndGetToken(token == Lexer::PAREN_CLOSE, "Expected closing parentheses after if condition");
        ensureAndGetToken(token == Lexer::BRACE_OPEN, "Expected opening braces after if condition");
        // optionally handle assignments
        ASTPtr assignBlock(new ASTNode("assignBlock",condition));
        while(token == Lexer::IDENT){
            assignBlock->addChild(assign());
        }
        ensureAndGetToken(token == Lexer::BRACE_CLOSE, "Expected opening braces after statements");

        if(token == Lexer::ELSE){
            assignBlock->addChild(ifthenelse());
        }else{
            ASTPtr emptyNode(new ASTNode("empty"));
            assignBlock->addChild(std::move(emptyNode));
        } 
        return assignBlock;
    }

private:
    Lexer lexer;
    Lexer::Token token; // current token
    Lexer::Token lookAheadToken; // next token
    std::array<int,2> tokenPosition;
    std::array<int,2> lookAheadTokenPosition;
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