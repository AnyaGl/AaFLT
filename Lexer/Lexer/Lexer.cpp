#include "Lexer.h"
#include "Constants.h"
#include <algorithm>
#include <string>
#include <utility>

CLexer::CLexer(std::istream& in, std::ostream& out)
	: m_in(in)
	, m_out(out)
{
}

CLexer::Lexeme CLexer::GetNextLexeme()
{
	auto resLexeme = m_prevLexeme;
	if (!m_prevLexeme.lexeme.empty())
	{
		m_prevLexeme.lexeme = "";
		return resLexeme;
	}
	AnalizeString();
	while ((!m_isLexemeFind && (!m_isCurrLexemFind || !m_prevLexeme.lexeme.empty())) && std::getline(m_in, m_currLine))
	{
		++m_currentLineNumber;
		m_currentCharInLine = 0;
		AnalizeString();
		
	}
	if (!m_isLexemeFind)
	{
		if (GetToken(m_lexeme.lexeme) == CLexer::Token::Error && (m_state == State::AnalizingMultiLineComment || m_state == State::AnalizingOneLineComment))
		{
			FlushLexeme(CLexer::Token::EndOfFile);
		}
		else
		{
			FlushLexeme(GetToken(m_lexeme.lexeme));
		}
	}

	if (m_prevLexeme.lexeme.empty() && !m_lexeme.lexeme.empty() && m_isCurrLexemFind)
	{
		resLexeme = m_lexeme;
		m_lexeme.lexeme = "";

		m_isCurrLexemFind = false;
	}
	else
	{
		resLexeme = m_prevLexeme;
		m_prevLexeme.lexeme = "";
		if (m_isCurrLexemFind)
		{
			m_prevLexeme = m_lexeme;
			m_lexeme.lexeme = "";
			m_isCurrLexemFind = false;
		}
	}
	m_isLexemeFind = false;
	return resLexeme;
}

void CLexer::AnalizeString()
{
	auto charNumber = 0;
	for (auto ch : m_currLine)
	{
		++m_currentCharInLine;
		++charNumber;
		switch (m_state)
		{
		case State::AnalizingCode:
			AnalizeCode(ch);
			break;
		case State::AnalizingOneLineComment:
		case State::AnalizingMultiLineComment:
			m_lexeme.lexeme += ch;
			AnalizeLexeme(ch, CLexer::Token::Comment);
			break;
		case State::AnalizingString:
			m_lexeme.lexeme += ch;
			AnalizeLexeme(ch, CLexer::Token::String);
			break;
		case State::AnalizingChar:
			m_lexeme.lexeme += ch;
			AnalizeLexeme(ch, CLexer::Token::Char);
			break;
		case State::LookingForSecondCommentLiteral:
			AnalizeSecondCommentLiteral(ch);
			break;
		case State::LookingForSecondCompLiteral:
			AnalizeSecondCompLiteral(ch);
			break;
		}
		if (m_isLexemeFind || m_isCurrLexemFind && m_prevLexeme.lexeme.empty())
		{
			if (charNumber >= m_currLine.size())
			{
				m_currLine = "";
				if (m_state == State::AnalizingMultiLineComment || m_state == State::AnalizingString)
				{
					m_lexeme.lexeme += "\n";
				}
			}
			else
			{
				m_currLine = m_currLine.substr(charNumber);
			}
			return;
		}
	}
	if (m_currLine.empty())
	{
		return;
	}
	if (m_state == State::AnalizingMultiLineComment || m_state == State::AnalizingString)
	{
		m_lexeme.lexeme += "\n";
	}
	else if (m_state == State::AnalizingOneLineComment)
	{
		FlushLexeme(CLexer::Token::Comment);
		m_state = State::AnalizingCode;
	}
	else if (m_state != State::AnalizingString)
	{
		FlushLexeme(GetToken(m_lexeme.lexeme));
		m_state = State::AnalizingCode;
	}
	m_currLine = "";
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
		m_lexeme.line = m_currentLineNumber;
		m_lexeme.position = m_currentCharInLine;
	}

	if (ch == COMMENT_START)
	{
		FlushLexeme(GetToken(m_lexeme.lexeme));
		m_state = State::LookingForSecondCommentLiteral;
		m_lexeme.lexeme += ch;
		m_lexeme.line = m_currentLineNumber;
		m_lexeme.position = m_currentCharInLine;
	}
	else if (ch == EQUALITY_START || ch == INEQUALITY_START)
	{
		FlushLexeme(GetToken(m_lexeme.lexeme));
		m_state = State::LookingForSecondCompLiteral;
		m_lexeme.lexeme += ch;
		m_lexeme.line = m_currentLineNumber;
		m_lexeme.position = m_currentCharInLine;
	}
	else if (ch == STRING_START)
	{
		FlushLexeme(GetToken(m_lexeme.lexeme));
		m_currDFAWalker = std::make_shared<DFAWalker>(STRING_FINISH_DFA);
		m_state = State::AnalizingString;
		m_lexeme.lexeme += ch;
		m_lexeme.line = m_currentLineNumber;
		m_lexeme.position = m_currentCharInLine;
	}
	else if (ch == CHAR_START)
	{
		FlushLexeme(GetToken(m_lexeme.lexeme));
		m_currDFAWalker = std::make_shared<DFAWalker>(CHAR_FINISH_DFA);
		m_state = State::AnalizingChar;
		m_lexeme.lexeme += ch;
		m_lexeme.line = m_currentLineNumber;
		m_lexeme.position = m_currentCharInLine;
	}
	else if (IsGrammarWord(OPERATION_SIGNS, currCh)
		|| IsGrammarWord(DELIMITERS, currCh)
		|| IsGrammarWord(COMPARISON, currCh)
		|| IsGrammarWord(BRACKETS, currCh))
	{
		if (currCh != "+" && currCh != "-" || !IsFloatPointNumberBeforeSign())
		{
			FlushLexeme(GetToken(m_lexeme.lexeme));
			m_lexeme = { currCh, GetToken(currCh), m_currentLineNumber, m_currentCharInLine };
			m_isCurrLexemFind = true;
		}
		else
		{
			m_lexeme.lexeme += currCh;
		}
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
		m_currLine = currCh + m_currLine;
		m_currentCharInLine--;

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
		auto resToken = token;
		switch (token)
		{
		case CLexer::Token::Identifier:
			resToken = m_lexeme.lexeme.size() > 64 ? CLexer::Token::Error : token;
			break;
		case CLexer::Token::IntNumber:
			resToken = m_lexeme.lexeme.size() > 11 ? CLexer::Token::Error : token;
			break;
		case CLexer::Token::FixedPointNumber:
			resToken = m_lexeme.lexeme.size() > 18 ? CLexer::Token::Error : token;
			break;
		}
		m_lexeme.token = resToken;
		m_prevLexeme = m_lexeme;
		m_lexeme.lexeme = "";
		m_isLexemeFind = true;
	}
}

bool CLexer::IsFloatPointNumberBeforeSign()
{
	if (m_lexeme.lexeme.empty() || m_lexeme.lexeme.back() != 'e' && m_lexeme.lexeme.back() != 'E')
	{
		return false;
	}
	m_currDFAWalker = std::make_shared<DFAWalker>(FLOAT_POINT_NUMBER_DFA);
	for (auto ch : m_lexeme.lexeme)
	{
		if (!m_currDFAWalker->GoToNextState(ch))
		{
			return false;
		}
		else if (m_currDFAWalker->IsFinishedState())
		{
			return false;
		}
	}
	return true;
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
	if (IsGrammarWord(FLOAT_POINT_NUMBER_DFA, lexeme))
	{
		return CLexer::Token::FloatPointNumber;
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
	case Token::FloatPointNumber:
		return "FloatPointNumber";
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
	case Token::EndOfFile:
		return "EndOfFile";
	default:
		break;
	}
	throw std::invalid_argument("unknown token");
}
