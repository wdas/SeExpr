#include "SeExprParse.h"


class SeParser{
public:
	SeParser(const std::string& inputString)
	:lexer(inputString.c_str())
	{

	}

	void parse(){
		lexer.getToken(); // prime it

		module();
	}

	void module(){
		while(token==Lexer::DEF) declaration();
		block();
	}

	void assert(bool value){
		if(value)
			throw ParseError("Parse error");
	}

	void declaration(){
		assert(token==Lexer::DEF);
		lexer.getToken();
		if(token!=Lexer::IDENT) typeDeclare();
		assert(token==Lexer::IDENT);
		lexer.getToken();
		assert(token==Lexer::PAREN_OPEN);
		lexer.getToken();
		typeList();
		lexer.getToken();
		assert(token==Lexer::PAREN_CLOSE);
		lexer.getToken();
	}

	void typeDeclare(){
		assert(token==Lexer::STRING || token==Lexer::FLOAT);
		lexer.getToken();
		assert(token==Lexer::BRACKET_OPEN);
		lexer.getToken();
		assert(token==Lexer::NUM);
		lexer.getToken();
		assert(token==Lexer::BRACKET_CLOSE);
		lexer.getToken();
	}

	void typeList(){

	}

	void block(){

	}

private:
	Lexer lexer;
	Lexer::Token token; // current token
};


int main(int argc,char*argv[])
{
	std::ifstream ifs(argv[1]);
	std::string content((std::istreambuf_iterator<char>(ifs)),
		std::istreambuf_iterator<char>());
	SeParser parser(content.c_str());
	parser.parse();
	return 0;
}