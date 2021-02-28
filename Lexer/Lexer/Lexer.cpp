#include "Lexer.h"
#include "Constants.h"
#include <algorithm>
#include <string>
#include <utility>

CLexer::CLexer(std::istream& stream)
	: m_stream(stream)
{
}

void CLexer::PrintLexemeWithTokens() const
{
	std::cout << "--- lexeme: token ---" << std::endl
			  << std::endl;
	for (auto resultLexeme : m_resultLexemes)
	{
		std::cout << resultLexeme.lexeme << ": " << TokenToString(resultLexeme.token) << " line: " << resultLexeme.line << " pos: " << resultLexeme.position << std::endl;
	}
}

void CLexer::Analize()
{
	std::string line;
	while (std::getline(m_stream, line))
	{
		++m_currentLine;
		m_currentCharInLine = 0;
		for (auto ch : line)
		{
			++m_currentCharInLine;
			switch (m_state)
			{
			case State::AnalizingCode:
				AnalizeCode(ch);
				break;
			case State::AnalizingOneLineComment:
			case State::AnalizingMultiLineComment:
				AnalizeLexeme(ch, CLexer::Token::Comment);
				break;
			case State::AnalizingString:
				AnalizeLexeme(ch, CLexer::Token::String);
				break;
			case State::AnalizingChar:
				AnalizeLexeme(ch, CLexer::Token::Char);
				break;
			case State::LookingForSecondCommentLiteral:
				AnalizeSecondCommentLiteral(ch);
				break;
			case State::LookingForSecondCompLiteral:
				AnalizeSecondCompLiteral(ch);
				break;
			}
		}
		if (m_state == State::AnalizingMultiLineComment)
		{
			m_lexeme.lexeme += "\n";
		}
		else if (m_state == State::AnalizingOneLineComment)
		{
			FlushLexeme(CLexer::Token::Comment);
			m_state = State::AnalizingCode;
			m_lexeme.line = m_currentLine + 1;
		}
		else
		{
			FlushLexeme(GetToken(m_lexeme.lexeme));
			m_state = State::AnalizingCode;
		}
	}
	FlushLexeme(GetToken(m_lexeme.lexeme));
}

bool CLexer::IsGrammarWord(const DFA& dfa, const std::string& word)
{
	DFAWalker dfaWalker(dfa);
	return dfaWalker.IsGrammarWord(word);
}

bool CLexer::IsGrammarWord(const std::vector<std::string>& words, const std::string& word)
{
	return std::find(words.begin(), words.end(), word) != words.end();
}

void CLexer::AnalizeCode(char ch)
{
	auto currCh = std::string(1, ch);
	if (m_lexeme.lexeme.empty())
	{
		m_lexeme.line = m_currentLine;
		m_lexeme.position = m_currentCharInLine;
	}

	if (ch == COMMENT_START)
	{
		m_state = State::LookingForSecondCommentLiteral;
		m_lexeme.lexeme += ch;
	}
	else if (ch == EQUALITY_START || ch == INEQUALITY_START)
	{
		m_state = State::LookingForSecondCompLiteral;
		m_lexeme.lexeme += ch;
	}
	else if (ch == STRING_START)
	{
		m_currDFAWalker = std::make_shared<DFAWalker>(STRING_FINISH_DFA);
		m_state = State::AnalizingString;
		m_lexeme.lexeme += ch;
	}
	else if (ch == CHAR_START)
	{
		m_currDFAWalker = std::make_shared<DFAWalker>(CHAR_FINISH_DFA);
		m_state = State::AnalizingChar;
		m_lexeme.lexeme += ch;
	}
	else if (IsGrammarWord(OPERATION_SIGNS, currCh)
		|| IsGrammarWord(DELIMITERS, currCh)
		|| IsGrammarWord(COMPARISON, currCh)
		|| IsGrammarWord(BRACKETS, currCh))
	{
		FlushLexeme(GetToken(m_lexeme.lexeme));
		m_resultLexemes.push_back({ currCh, GetToken(currCh), m_currentLine, m_currentCharInLine });
	}
	else if (IsGrammarWord(SPACES, currCh))
	{
		FlushLexeme(GetToken(m_lexeme.lexeme));
	}
	else
	{
		m_lexeme.lexeme += ch;
	}
}

void CLexer::AnalizeLexeme(char ch, Token token)
{
	m_lexeme.lexeme += ch;
	if (!m_currDFAWalker->GoToNextState(ch))
	{
		FlushLexeme(CLexer::Token::Error);
		m_state = State::AnalizingCode;
		m_currDFAWalker = nullptr;
	}
	else if (m_currDFAWalker->IsFinishedState())
	{
		FlushLexeme(token);
		m_state = State::AnalizingCode;
		m_currDFAWalker = nullptr;
	}
}

void CLexer::AnalizeSecondCommentLiteral(char ch)
{
	if (ch == ONE_LINE_COMMENT_START)
	{
		m_currDFAWalker = std::make_shared<DFAWalker>(ONE_LINE_COMMENT_DFA);
		m_state = State::AnalizingOneLineComment;
		m_lexeme.lexeme += ch;
	}
	else if (ch == MULTILINE_COMMENT_START)
	{
		m_currDFAWalker = std::make_shared<DFAWalker>(COMMENT_FINISH_DFA);
		m_state = State::AnalizingMultiLineComment;
		m_lexeme.lexeme += ch;
	}
	else
	{
		FlushLexeme(GetToken(m_lexeme.lexeme));

		auto currCh = std::string(1, ch);
		m_resultLexemes.push_back({ currCh, GetToken(currCh), m_currentLine, m_currentCharInLine });

		m_state = State::AnalizingCode;
	}
}

void CLexer::AnalizeSecondCompLiteral(char ch)
{
	if (ch == '=')
	{
		m_lexeme.lexeme += ch;
		FlushLexeme(GetToken(m_lexeme.lexeme));
	}
	else
	{
		FlushLexeme(GetToken(m_lexeme.lexeme));
		AnalizeCode(ch);
	}
	m_state = State::AnalizingCode;
}

void CLexer::FlushLexeme(Token token)
{
	if (!m_lexeme.lexeme.empty())
	{
		m_lexeme.token = token;
		m_resultLexemes.push_back(m_lexeme);
		m_lexeme.lexeme = "";
		m_lexeme.line = m_currentLine;
		m_lexeme.position = m_currentCharInLine;
	}
}

CLexer::Token CLexer::GetToken(std::string const& lexeme)
{
	if (IsGrammarWord(KEYWORDS, lexeme))
	{
		return CLexer::Token::Keyword;
	}
	if (IsGrammarWord(IDENTIFIER_DFA, lexeme))
	{
		return CLexer::Token::Identifier;
	}
	if (IsGrammarWord(INT_NUMBER_DFA, lexeme))
	{
		return CLexer::Token::IntNumber;
	}
	if (IsGrammarWord(FIXED_POINT_NUMBER_DFA, lexeme))
	{
		return CLexer::Token::FixedPointNumber;
	}
	if (IsGrammarWord(FLOATING_POINT_NUMBER_DFA, lexeme))
	{
		return CLexer::Token::FloatingPointNumber;
	}
	if (IsGrammarWord(BINARY_NUMBER_DFA, lexeme))
	{
		return CLexer::Token::BinaryNumber;
	}
	if (IsGrammarWord(OCTAL_NUMBER_DFA, lexeme))
	{
		return CLexer::Token::OctalNumber;
	}
	if (IsGrammarWord(HEX_NUMBER_DFA, lexeme))
	{
		return CLexer::Token::HexNumber;
	}
	if (IsGrammarWord(OPERATION_SIGNS, lexeme))
	{
		return CLexer::Token::OperationSign;
	}
	if (IsGrammarWord(DELIMITERS, lexeme))
	{
		return CLexer::Token::Delimiter;
	}
	if (IsGrammarWord(COMPARISON, lexeme))
	{
		return CLexer::Token::Comparison;
	}
	if (IsGrammarWord(BRACKETS, lexeme))
	{
		return CLexer::Token::Bracket;
	}
	return CLexer::Token::Error;
}

std::string CLexer::TokenToString(Token token) const
{
	switch (token)
	{
	case Token::Keyword:
		return "Keyword";
	case Token::Identifier:
		return "Identifier";
	case Token::IntNumber:
		return "IntNumber";
	case Token::FixedPointNumber:
		return "FixedPointNumber";
	case Token::FloatingPointNumber:
		return "FloatingPointNumber";
	case Token::BinaryNumber:
		return "BinaryNumber";
	case Token::OctalNumber:
		return "OctalNumber";
	case Token::HexNumber:
		return "HexNumber";
	case Token::String:
		return "String";
	case Token::Char:
		return "Char";
	case Token::OperationSign:
		return "OperationSign";
	case Token::Delimiter:
		return "Delimiter";
	case Token::Comparison:
		return "Comparison";
	case Token::Comment:
		return "Comment";
	case Token::Bracket:
		return "Bracket";
	case Token::Error:
		return "Error";
	default:
		break;
	}
	throw std::invalid_argument("unknown token");
}
