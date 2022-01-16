#include "LexerAdapter.h"

namespace
{
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
} // namespace

LexerAdapter::LexerAdapter(std::istream& in, std::ostream& out)
	: m_lexer(in, out)
{
}

std::string LexerAdapter::GetNextItem()
{
	m_lexeme = m_lexer.GetNextLexeme();
	m_lexemas.push_back(m_lexeme);

	m_isEnd = m_lexeme.lexeme.empty();
	if (m_isEnd)
	{
		//m_lexemas.back().lexeme = "#";
		return "#";
	}
	std::string result;
	switch (m_lexeme.token)
	{
	case CLexer::Token::Keyword:
	case CLexer::Token::Delimiter:
	case CLexer::Token::Bracket:
	case CLexer::Token::OperationSign:
		result = m_lexeme.lexeme;
		break;
	case CLexer::Token::Comment:
		result = GetNextItem();
		break;
	default:
		result = TokenToString(m_lexeme.token);
		break;
	}
	return result;
}

bool LexerAdapter::IsEnd()
{
	return m_isEnd;
}

int LexerAdapter::GetCurrentLine() const
{
	return m_lexeme.line;
}

int LexerAdapter::GetCurrentPosition() const
{
	return m_lexeme.position;
}

std::string LexerAdapter::GetLexeme(int indexFromTop) const
{
	try
	{
		return m_lexemas.at(m_lexemas.size() - 1 - indexFromTop).lexeme;
	}
	catch(...)
	{
		return "";
	}
}


