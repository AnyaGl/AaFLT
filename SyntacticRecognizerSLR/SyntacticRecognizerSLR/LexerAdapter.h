#pragma once
#include "../../Lexer/Lexer/Lexer.h"
#include "IInputSequence.h"
#include <iostream>

class LexerAdapter : public IInputSequence
{
public:
	LexerAdapter(std::istream& in, std::ostream& out = std::cout);

	std::string GetNextItem() override;
	bool IsEnd() override;
	int GetCurrentLine() const override;
	int GetCurrentPosition() const override;
	std::string GetLexeme(int index) const override;

private:
	CLexer m_lexer;
	bool m_isEnd = false;
	CLexer::Lexeme m_lexeme;
	std::vector<CLexer::Lexeme> m_lexemas;
};
