

#include <Expression.h>

#include <Vec.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

using namespace SeExpr2;
int main(){
	Expression e("1+2");

	if(!e.isValid()){
		throw std::runtime_error(e.parseError());
	}
	e.debugPrintParseTree();
	e.debugPrintLLVM();
	const double* val=e.evalFP();
	std::cout<<"val is "<<val[0]<<std::endl;


	return 0;
}

