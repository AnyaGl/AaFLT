#pragma once
#include "DFAWalker.h"
#include <iostream>
#include <sstream>
#include <vector>

class CLexer
{
public:
	enum class Token
	{
		Keyword,
		Identifier,
		IntNumber,
		FixedPointNumber,
		BinaryNumber,
		OctalNumber,
		HexNumber,
		String,
		Char,
		OperationSign,
		Delimiter,
		Comment,
		Comparison,
		Bracket,
		Error
	};

	struct Lexeme
	{
		std::string lexeme;
		Token token;
		int line = 0;
		int position = 0;
	};

	CLexer(std::istream& in, std::ostream& out);

	void PrintLexemeWithTokens() const;

	void Analize();

	Token GetToken(std::string const& lexeme);

	std::vector<Lexeme> GetLexemesWithTokens() const;

private:
	enum class State
	{
		AnalizingCode,
		AnalizingOneLineComment,
		AnalizingMultiLineComment,
		AnalizingString,
		AnalizingChar,
		LookingForSecondCommentLiteral,
		LookingForSecondCompLiteral,
	};

	bool IsGrammarWord(const DFA& dfa, const std::string& word);
	bool IsGrammarWord(const std::vector<std::string>& words, const std::string& word);

	void AnalizeCode(char ch);
	void AnalizeLexeme(char ch, Token token);
	void AnalizeSecondCommentLiteral(char ch);
	void AnalizeSecondCompLiteral(char ch);

	void FlushLexeme(Token token);

	std::string TokenToString(Token token) const;

	std::istream& m_in;
	std::ostream& m_out;
	Lexeme m_lexeme;
	int m_currentLine = 0;
	int m_currentCharInLine = 0;
	std::vector<Lexeme> m_resultLexemes;
	State m_state = State::AnalizingCode;
	std::shared_ptr<DFAWalker> m_currDFAWalker = nullptr;
};
