#include <iostream>
#include <fstream>
#include "Lexer.h"

int main()
{
	std::ifstream fs("input.txt");
	CLexer lexer(fs);
	lexer.Analize();
	lexer.PrintLexemeWithTokens();
}