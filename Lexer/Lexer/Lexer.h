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
		FloatPointNumber,
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
		Error,
		EndOfFile
	};

	struct Lexeme
	{
		std::string lexeme;
		Token token;
		int line = 0;
		int position = 0;
	};

	CLexer(std::istream& in, std::ostream& out);

	Token GetToken(std::string const& lexeme);

	Lexeme GetNextLexeme();

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

	void AnalizeString();

	bool IsGrammarWord(const DFA& dfa, const std::string& word);
	bool IsGrammarWord(const std::vector<std::string>& words, const std::string& word);

	void AnalizeCode(char ch);
	void AnalizeLexeme(char ch, Token token);
	void AnalizeSecondCommentLiteral(char ch);
	void AnalizeSecondCompLiteral(char ch);

	void FlushLexeme(Token token);

	bool IsFloatPointNumberBeforeSign();

	std::string TokenToString(Token token) const;

	std::istream& m_in;
	std::ostream& m_out;
	Lexeme m_lexeme;
	Lexeme m_prevLexeme;
	std::string m_currLine;
	int m_currentLineNumber = 0;
	int m_currentCharInLine = 0;
	bool m_isLexemeFind = false;
	bool m_isCurrLexemFind = false;
	State m_state = State::AnalizingCode;
	std::shared_ptr<DFAWalker> m_currDFAWalker = nullptr;
};
