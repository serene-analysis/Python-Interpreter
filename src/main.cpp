#include "Evalvisitor.h"
#include "Python3Lexer.h"
#include "Python3Parser.h"
#include "antlr4-runtime.h"
#include <iostream>
using namespace antlr4;
// TODO: regenerating files in directory named "generated" is dangerous.
//       if you really need to regenerate,please ask TA for help.
int main(int argc, const char *argv[]) {
	//std::cout << "???" << std::endl;
	//auto mem = freopen("/home/enovo/Python-Interpreter/build/BigIntegerTest1.in","r",stdin);
	//freopen("/home/enovo/Python-Interpreter/build/mine.out","w",stdout);
	//if(!mem){
	//	std::cerr << "File open failed\n";
	//	return 0;
	//}
	// TODO: please don't modify the code below the construction of ifs if you want to use visitor mode
	ANTLRInputStream input(std::cin);
	Python3Lexer lexer(&input);
	CommonTokenStream tokens(&lexer);
	tokens.fill();
	Python3Parser parser(&tokens);
	tree::ParseTree *tree = parser.file_input();
	EvalVisitor visitor;
	visitor.visit(tree);
	return 0;
}
