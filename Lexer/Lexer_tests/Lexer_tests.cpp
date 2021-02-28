#define CATCH_CONFIG_MAIN
#include "../Lexer/Lexer.h"
#include "catch2/catch.hpp"
#include <sstream>

std::vector<CLexer::Lexeme> GetLexemes(std::string input)
{
	std::stringstream in(std::move(input));
	std::stringstream out;
	CLexer lexer(in, out);
	lexer.Analize();
	return lexer.GetLexemesWithTokens();
}

bool operator==(const CLexer::Lexeme& lhs, const CLexer::Lexeme& rhs)
{
	return std::tie(lhs.lexeme, lhs.token, lhs.line, lhs.position) == 
		std::tie(rhs.lexeme, rhs.token, rhs.line, rhs.position);
}

TEST_CASE("Analize string")
{
	auto lexemes = GetLexemes("\"123\"");
	CHECK(lexemes.size() == 1);
	CHECK(lexemes.at(0) == CLexer::Lexeme{ "\"123\"", CLexer::Token::String, 1, 1 });

	lexemes = GetLexemes(" \"123\"");
	CHECK(lexemes.size() == 1);
	CHECK(lexemes.at(0) == CLexer::Lexeme{ "\"123\"", CLexer::Token::String, 1, 2 });

	lexemes = GetLexemes(" \"12  \\\" '3\"");
	CHECK(lexemes.size() == 1);
	CHECK(lexemes.at(0) == CLexer::Lexeme{ "\"12  \\\" '3\"", CLexer::Token::String, 1, 2 });

	lexemes = GetLexemes(" \"12  \"\" '3");
	CHECK(lexemes.size() == 2);
	CHECK(lexemes.at(0) == CLexer::Lexeme{ "\"12  \"", CLexer::Token::String, 1, 2 });
	CHECK(lexemes.at(1) == CLexer::Lexeme{ "\" '3", CLexer::Token::Error, 1, 8 });
}

TEST_CASE("Analize char")
{
	auto lexemes = GetLexemes("'1'");
	CHECK(lexemes.size() == 1);
	CHECK(lexemes.at(0) == CLexer::Lexeme{ "'1'", CLexer::Token::Char, 1, 1 });

	lexemes = GetLexemes(" '1'");
	CHECK(lexemes.size() == 1);
	CHECK(lexemes.at(0) == CLexer::Lexeme{ "'1'", CLexer::Token::Char, 1, 2 });

	lexemes = GetLexemes(" '\\''");
	CHECK(lexemes.size() == 1);
	CHECK(lexemes.at(0) == CLexer::Lexeme{ "'\\''", CLexer::Token::Char, 1, 2 });

	lexemes = GetLexemes(" '\\1'");
	CHECK(lexemes.size() == 2);
	CHECK(lexemes.at(0) == CLexer::Lexeme{ "'\\1", CLexer::Token::Error, 1, 2 });
	CHECK(lexemes.at(1) == CLexer::Lexeme{ "'", CLexer::Token::Error, 1, 5 });
}

TEST_CASE("Analize one line comment")
{
	auto lexemes = GetLexemes("something //on;e 'l'ine/* comm*/e\"nnn\"t\nsomething");
	CHECK(lexemes.size() == 3);
	CHECK(lexemes.at(0) == CLexer::Lexeme{ "something", CLexer::Token::Identifier, 1, 1 });
	CHECK(lexemes.at(1) == CLexer::Lexeme{ "//on;e 'l'ine/* comm*/e\"nnn\"t", CLexer::Token::Comment, 1, 11 });
	CHECK(lexemes.at(2) == CLexer::Lexeme{ "something", CLexer::Token::Identifier, 2, 1 });
}

TEST_CASE("Analize multi line comment")
{
	auto lexemes = GetLexemes("something /* mu\"lt\"i\n l//ine \nc;omm'e'nt*/\nsomething");
	CHECK(lexemes.size() == 3);
	CHECK(lexemes.at(0) == CLexer::Lexeme{ "something", CLexer::Token::Identifier, 1, 1 });
	CHECK(lexemes.at(1) == CLexer::Lexeme{ "/* mu\"lt\"i\n l//ine \nc;omm'e'nt*/", CLexer::Token::Comment, 1, 11 });
	CHECK(lexemes.at(2) == CLexer::Lexeme{ "something", CLexer::Token::Identifier, 4, 1 });
}

TEST_CASE("Analize code")
{
	auto lexemes = GetLexemes("int iden123 = 123;\nif (iden123 == iden1)\n{\niden1 = iden123;\n}\nelse if (iden1 != iden123)\n{\niden123 = iden1;\n}");
	/*	
int iden123 = 123;
int iden1 = 1;
if (iden123 == iden1)
{
	iden1 = iden123;
}
else if (iden1 != iden123)
{
	iden123 = iden1;
}
	*/
	CHECK(lexemes.size() == 30);
	CHECK(lexemes.at(0) == CLexer::Lexeme{ "int", CLexer::Token::Keyword, 1, 1 });
	CHECK(lexemes.at(1) == CLexer::Lexeme{ "iden123", CLexer::Token::Identifier, 1, 5 });
	CHECK(lexemes.at(2) == CLexer::Lexeme{ "=", CLexer::Token::OperationSign, 1, 13 });
	CHECK(lexemes.at(3) == CLexer::Lexeme{ "123", CLexer::Token::IntNumber, 1, 15 });
	CHECK(lexemes.at(4) == CLexer::Lexeme{ ";", CLexer::Token::Delimiter, 1, 18 });

	CHECK(lexemes.at(5) == CLexer::Lexeme{ "if", CLexer::Token::Keyword, 2, 1 });
	CHECK(lexemes.at(6) == CLexer::Lexeme{ "(", CLexer::Token::Bracket, 2, 4 });
	CHECK(lexemes.at(8) == CLexer::Lexeme{ "==", CLexer::Token::Comparison, 2, 13 });

	CHECK(lexemes.at(11) == CLexer::Lexeme{ "{", CLexer::Token::Bracket, 3, 1 });

	CHECK(lexemes.at(21) == CLexer::Lexeme{ "!=", CLexer::Token::Comparison, 6, 16 });
}

TEST_CASE("Analize intNumber")
{
	auto lexemes = GetLexemes("123");
	CHECK(lexemes.size() == 1);
	CHECK(lexemes.at(0) == CLexer::Lexeme{ "123", CLexer::Token::IntNumber, 1, 1 });
}

TEST_CASE("Analize fixed point number")
{
	auto lexemes = GetLexemes("12.345");
	CHECK(lexemes.size() == 1);
	CHECK(lexemes.at(0) == CLexer::Lexeme{ "12.345", CLexer::Token::FixedPointNumber, 1, 1 });

	lexemes = GetLexemes("12.");
	CHECK(lexemes.size() == 1);
	CHECK(lexemes.at(0) == CLexer::Lexeme{ "12.", CLexer::Token::FixedPointNumber, 1, 1 });

	lexemes = GetLexemes(".97");
	CHECK(lexemes.size() == 1);
	CHECK(lexemes.at(0) == CLexer::Lexeme{ ".97", CLexer::Token::FixedPointNumber, 1, 1 });
}

TEST_CASE("Analize octal number")
{
	auto lexemes = GetLexemes("0123");
	CHECK(lexemes.size() == 1);
	CHECK(lexemes.at(0) == CLexer::Lexeme{ "0123", CLexer::Token::OctalNumber, 1, 1 });

	lexemes = GetLexemes("0128");
	CHECK(lexemes.size() == 1);
	CHECK(lexemes.at(0) == CLexer::Lexeme{ "0128", CLexer::Token::Error, 1, 1 });
}

TEST_CASE("Analize hex number")
{
	auto lexemes = GetLexemes("0xAA1");
	CHECK(lexemes.size() == 1);
	CHECK(lexemes.at(0) == CLexer::Lexeme{ "0xAA1", CLexer::Token::HexNumber, 1, 1 });

	lexemes = GetLexemes("0xG");
	CHECK(lexemes.size() == 1);
	CHECK(lexemes.at(0) == CLexer::Lexeme{ "0xG", CLexer::Token::Error, 1, 1 });
}