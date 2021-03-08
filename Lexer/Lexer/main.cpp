#include <iostream>
#include <fstream>
#include "Lexer.h"


std::string TokenToString(CLexer::Token token)
{
	switch (token)
	{
	case CLexer::Token::Keyword:
		return "Keyword";
	case CLexer::Token::Identifier:
		return "Identifier";
	case CLexer::Token::IntNumber:
		return "IntNumber";
	case CLexer::Token::FixedPointNumber:
		return "FixedPointNumber";
	case CLexer::Token::FloatPointNumber:
		return "FloatPointNumber";
	case CLexer::Token::BinaryNumber:
		return "BinaryNumber";
	case CLexer::Token::OctalNumber:
		return "OctalNumber";
	case CLexer::Token::HexNumber:
		return "HexNumber";
	case CLexer::Token::String:
		return "String";
	case CLexer::Token::Char:
		return "Char";
	case CLexer::Token::OperationSign:
		return "OperationSign";
	case CLexer::Token::Delimiter:
		return "Delimiter";
	case CLexer::Token::Comparison:
		return "Comparison";
	case CLexer::Token::Comment:
		return "Comment";
	case CLexer::Token::Bracket:
		return "Bracket";
	case CLexer::Token::Error:
		return "Error";
	case CLexer::Token::EndOfFile:
		return "EndOfFile";
	default:
		break;
	}
	throw std::invalid_argument("unknown token");
}


int main()
{
	std::ifstream fs("input.txt");
	CLexer lexer(fs, std::cout);
	auto lexeme = lexer.GetNextLexeme();
	while (!lexeme.lexeme.empty())
	{
		std::cout << lexeme.lexeme << ": " << TokenToString(lexeme.token) << "\tline: " << lexeme.line << " pos: " << lexeme.position << std::endl;

		lexeme = lexer.GetNextLexeme();
	}
}