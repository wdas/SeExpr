#include "SeExprParse.h"
#include <cassert>
#include <vector>
#include <memory>

/// A Simple AST class for representing SeExpr, useful for editors
class ASTNode{
public:
    /// Allows adding arbitary number of items to a container holding unique_ptrs
    struct Adder{
        void sequence(){}
        template<typename T,typename... Targs> void sequence(T&& guy,Targs&&... args){
            container.emplace_back(std::move(guy));
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

    void print(std::ostream& out, int indent=1, unsigned int mask=0){
        if(indent != 1){
             out<<"   ";
            for(int i=1;i<indent-1;i++){
                if((mask&(1<<i))!=0) out<<"   ";
                else out<<"  "<<"\xe2\x94\x82";;
            }
            out<<"  ";
            if((mask & (1<<(indent-1))) != 0){
                out<<"\xe2\x94\x94";
            }else{
                out<<"\xe2\x94\x9c";
            }
            out<<"\xe2\x94\x80 ";
        }else{
            out<<"   ";
        }
        //out<<std::string(indent*4,' ')<<"|- "<<_name<<"\n";
        out<<display()<<std::endl;
        for(auto it=_children.begin(); it!=_children.end(); ++it){
            int isEnd=it+1==_children.end() ? 1 : 0;
            unsigned int newMask = mask | (int(isEnd) << (indent));
            (*it)->print(out,indent+1,newMask);
        }
    }
    virtual std::string display() const{
        return _name;
    }
private:
    std::string _name;
    //int _startOffset, _endOffset;
    std::vector<std::unique_ptr<ASTNode>> _children;
};


struct ASTPolicy{
    typedef ASTNode Base;
    typedef std::unique_ptr<ASTNode> Ptr;

#define SEEXPR_AST_SUBCLASS(name) \
    struct name : public Base { \
        template<typename...Args> name(Args&&...args):Base(#name,args...) {} \
    };
#define SEEXPR_AST_SUBCLASS_OP(name) \
    struct name : public Base { \
        template<typename...Args> name(char op,Args&&...args):Base(std::string(#name)+op ,args...) {} \
    };
    SEEXPR_AST_SUBCLASS(Module);
    SEEXPR_AST_SUBCLASS(Prototype);
    SEEXPR_AST_SUBCLASS(LocalFunction);
    SEEXPR_AST_SUBCLASS(Block);
    SEEXPR_AST_SUBCLASS(Node);
    SEEXPR_AST_SUBCLASS(IfThenElse);
    SEEXPR_AST_SUBCLASS(Vec);
    SEEXPR_AST_SUBCLASS(Cond);
    SEEXPR_AST_SUBCLASS(Compare);
    SEEXPR_AST_SUBCLASS_OP(UnaryOp);
    SEEXPR_AST_SUBCLASS_OP(BinaryOp);
    SEEXPR_AST_SUBCLASS_OP(CompareEq);
    SEEXPR_AST_SUBCLASS(Func);
    SEEXPR_AST_SUBCLASS(Subscript);

    struct Str : public Base {
        Str(const std::string& s): Base("string"),s(s){}
        std::string s;
    };

    struct Num : public Base {
        Num(double num) :Base("num"),num(num) {}
        double num;
        std::string display() const{return std::string("num")+" "+std::to_string(num);}
    };

    struct Var : public  Base {
        Var(const std::string& var):Base("var"),var(var)
        {}
        std::string var;
        std::string display() const{return std::string("var")+" '"+var+"'";}
    };

    struct Assign : public Base {
        Assign(const std::string& var, Ptr node)
            :Base("assign",std::move(node)),var(var)
        {}
        std::string display() const{return std::string("assign ")+" '"+var+"'";}
        std::string var;
    };

};


typedef std::unique_ptr<ASTNode> ASTPtr;

template<class Policy>
class SeParser{
    typedef typename Policy::Ptr NodePtr;
public:
    SeParser(const std::string& inputString)
    :lexer(inputString), lookAheadToken(Lexer::END_OF_BUFFER), lookAheadTokenText(""), lookAheadTokenPosition(std::array<int,2>{{0,0}})
    {

    }

    void getToken(){
        token=lookAheadToken;
        tokenText=lookAheadTokenText;
        tokenPosition=lookAheadTokenPosition;
        lookAheadToken=lexer.getToken();
        lookAheadTokenText=lexer.getTokenText();
        lookAheadTokenPosition=lexer.getTokenPosition();
    }

    NodePtr parse(){
        std::cerr<<"we!"<<std::endl;
        getToken(); 
        getToken();
        NodePtr tree = module();
        tree->print(std::cout);
        return tree;
    }  

    NodePtr module(){
        //std::cerr<<"we!"<<std::endl;
        //std::cerr<<"tok is "<<Lexer::getTokenName(token)<<" '"<<lexer.getTokenText()<<"'"<<std::endl;;
        NodePtr moduleTree(new typename Policy::Module());
        while(token==Lexer::DEF){
            moduleTree->addChild(declaration());
        }
        moduleTree->addChild(block());
        return moduleTree;
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
    NodePtr declaration(){
        ensureAndGetToken(token==Lexer::DEF, "Expected the word 'def' in declaration");
        if(token!=Lexer::IDENT) typeDeclare();
        ensureAndGetToken(token==Lexer::IDENT, "Expected identifier");
        ensure(token==Lexer::PAREN_OPEN, "Expected open parentheses for function declaration");
        typeList();
        ensureAndGetToken(token==Lexer::PAREN_CLOSE, "Expected close parentheses for function declaration");
        // TODO: fix
        return NodePtr(new ASTNode("blah"));
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

    NodePtr block(){
        NodePtr ret(new typename Policy::Block());
        while(1){
            if(token == Lexer::IF) 
                ret->addChild(ifthenelse());
            else if(token == Lexer::IDENT && isAssignOrMutator(lookAheadToken)) 
                ret->addChild(assign());
            else
                break; // done with the top part
        }
        ret->addChild(expr());
        //ret->print(std::cout);
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
    NodePtr expr(){
        NodePtr parent=expr1();
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
    NodePtr expr1(){
        return expr2(); // TODO: put in other crap
    }

    // expr2: expr3 { OR expr3 }*
    NodePtr expr2(){
        NodePtr curr=expr3();
        while(token==Lexer::OR){
            std::string text=tokenText;
            getToken();
            curr=NodePtr(new ASTNode(text,std::move(curr),expr3())) ;
        }
        return curr;
    }

    // expr3: expr4 { AND expr4 }*
    NodePtr expr3(){
        NodePtr curr=expr4();
        while(token==Lexer::AND){
            std::string text=tokenText;
            getToken();
            curr=NodePtr(new typename Policy::BinaryOp('&',std::move(curr),expr4())) ;
        }
        return curr;
    }

    // expr4: expr5 { EQ|NE expr5 }*
    NodePtr expr4(){
        NodePtr curr=expr5();
        while(token==Lexer::EQUALS || token==Lexer::NOT_EQUALS){
            std::string text=tokenText;
            getToken();
            curr=NodePtr(new typename Policy::BinaryOp('|',std::move(curr),expr5())) ;
        }
        return curr;
    }

    // expr5: expr6 { <|<=|>|>= expr6 }*
    NodePtr expr5(){
        NodePtr curr=expr6();
        while(1){
            char op=' ';
            if(token == Lexer::LESS) op='<';
            else if(token == Lexer::LESS_EQUAL) op='l';
            else if(token == Lexer::GREATER) op='>';
            else if(token == Lexer::GREATER_EQUAL) op='g';
            else break;
            std::string text=tokenText;
            getToken();
            curr=NodePtr(new typename Policy::BinaryOp(op,std::move(curr),expr6())) ;
        }
        return curr;
    }

    // expr6: expr7 { +|- expr7}*
    NodePtr expr6(){
        NodePtr curr=expr7();
        while(token==Lexer::PLUS || token==Lexer::MINUS){
            char code=token==Lexer::PLUS ? '+' : '-';
            std::string text=tokenText;
            getToken();
            NodePtr newChild=expr7();
            curr=NodePtr(new typename Policy::BinaryOp(code,std::move(curr),std::move(newChild))) ;
        }
        return curr;
    }

    // expr7: expr8 { *|/|% expr8}*
    NodePtr expr7(){
        NodePtr curr=expr8();
        while(token==Lexer::TIMES || token==Lexer::DIVIDE || token==Lexer::MOD){
            char code=token==Lexer::TIMES ? '*' : (token == Lexer::DIVIDE ? '/' : '%');
            std::string text=tokenText;
            getToken();
            NodePtr newChild=expr8();
            curr=NodePtr(new typename Policy::BinaryOp(code,std::move(curr),std::move(newChild))) ;
        }
        return curr;
    }

    // expr8: {!|~|-|+}* expr9
    NodePtr expr8(){
        NodePtr parent;
        typename Policy::Base* curr=0;
        while(1){
            char op=' ';
            if(token==Lexer::TWIDLE) op='~';
            else if(token==Lexer::NOT) op='!';
            else if(token==Lexer::MINUS) op='-';
            else if(token==Lexer::PLUS) op='+';
            else break;
            NodePtr unaryOp(new typename Policy::UnaryOp(op));
            if(!parent){
                parent=std::move(unaryOp);
                curr=parent.get();
            }else{
                curr=curr->addChild(std::move(unaryOp));
            }
            getToken();
        }
        if(parent){
            curr->addChild(expr9());
            return parent;
        }else return expr9();
    }

    //expr9: {expr10 ^}* expr10
    NodePtr expr9(){
        NodePtr parent=expr10();
        typename Policy::Base* curr=0;
        while(token==Lexer::POWER){
            getToken();
            NodePtr newNode=expr10();
            if(!curr){
                parent=NodePtr(new typename Policy::BinaryOp('^',parent,newNode));
                curr=parent.get();
            }else{
                NodePtr oldNode = curr->removeChild();
                curr=curr->addChild(NodePtr(new typename Policy::BinaryOp('^',oldNode,newNode)));
            }
        }
        return parent;
    }

    // expr10: expr11 { [ expr ] }*
    NodePtr expr10(){
        NodePtr curr=expr11();
        while(token==Lexer::BRACKET_OPEN){
            getToken();
            NodePtr combined=NodePtr(new typename Policy::Subscript(std::move(curr),expr()));
            curr=std::move(combined);
            ensureAndGetToken(token==Lexer::BRACKET_CLOSE, "Did not get closing bracket to subscripting operator");
        }
        return curr;
    }

    // expr11: (expr) | NUM | IDENT | IDENT (args) | STR | '[' expr {, expr}* ']'
    NodePtr expr11(){
        if(token==Lexer::PAREN_OPEN){
            getToken();
            NodePtr ret=expr();
            ensureAndGetToken(token==Lexer::PAREN_CLOSE,"Expect closing parentheses after sub-expression");
            return ret;
        }else if(token==Lexer::NUM){
            std::string numStr=tokenText;
            getToken();
            return NodePtr(new typename Policy::Num(atof(numStr.c_str())));
        }else if(token==Lexer::IDENT){
            NodePtr callNode(new ASTNode("CALL"));
            std::string identName = tokenText;
            getToken();
            if(token==Lexer::PAREN_OPEN){
                getToken();
                callNode->addChild(functionArgs(nullptr));
                ensureAndGetToken(token==Lexer::PAREN_CLOSE, "Expected ending parentheses after function call arguments");
            }else{
                return NodePtr(new typename Policy::Var(identName));
            }
            return callNode;
        }else if(token==Lexer::STRING){
            // TODO: do something with string
            return NodePtr(new typename Policy::Str(tokenText));
        }else if(token==Lexer::BRACKET_OPEN){
            getToken();
            NodePtr vec(new typename Policy::Vec());
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

    NodePtr functionArgs(NodePtr firstArgumentForCurry){
        // TODO: fix
        NodePtr args(new typename Policy::Node());
        if(firstArgumentForCurry) args->addChild(std::move(firstArgumentForCurry));
        while(token != Lexer::PAREN_CLOSE){
            args->addChild(expr());
            if(token != Lexer::COMMA) break;
            getToken();
        }
        return args;
    }

    NodePtr assign(){
        ensure(token == Lexer::IDENT, "Expected identifier at begin of assignment");
        std::string varName=tokenText;
        getToken();
        ensure(isAssignOrMutator(token), "Expected mutator on assignment (got '')"+tokenText);
        getToken();
        NodePtr e=expr();

        NodePtr assignNode(new typename Policy::Assign(varName,std::move(e)))   ;
        ensure(token == Lexer::SEMICOLON, "Expected semi-colon at end of assignment");
        getToken();
        return assignNode;
    }

    ASTPtr assignBlock(){
        ASTPtr assigns(new typename Policy::Node());
        ensureAndGetToken(token == Lexer::BRACE_OPEN, "Expected opening braces after if condition");
        while(token == Lexer::IDENT){
            assigns->addChild(assign());
        }
        ensureAndGetToken(token == Lexer::BRACE_CLOSE, "Expected opening braces after statements");
        return assigns;
    }

    ASTPtr ifthenelse(){
        ensureAndGetToken(token == Lexer::IF, "Expected if at begin of if then else");
        ensureAndGetToken(token == Lexer::PAREN_OPEN, "Expected open parentheses after if");
        ASTPtr condition=expr();
        ensureAndGetToken(token == Lexer::PAREN_CLOSE, "Expected closing parentheses after if condition");
        // optionally handle assignments
        ASTPtr ifthenelseNode(new ASTNode("ifthenelse",condition));
        ifthenelseNode->addChild(assignBlock());
        if(token == Lexer::ELSE){
            getToken();
            if(token == Lexer::IF) ifthenelseNode->addChild(ifthenelse());
            else ifthenelseNode->addChild(assignBlock());
        }else{
            ASTPtr emptyNode(new ASTNode("empty"));
            ifthenelseNode->addChild(std::move(emptyNode));
        } 
        return ifthenelseNode;
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

//#ifdef TEST_PARSER

int main(int argc,char*argv[])
{
    std::ifstream ifs(argv[1]);
    std::string content((std::istreambuf_iterator<char>(ifs)),
        std::istreambuf_iterator<char>());
    std::cerr<<"PARSING! '"<<content<<"'"<<std::endl;;
    SeParser<ASTPolicy> parser(content);
    try{
        parser.parse();
    }catch(const ParseError& e){
        std::cerr<<"parse error: "<<e._errorStr<<std::endl;
    }
    return 0;
}
//#endif
