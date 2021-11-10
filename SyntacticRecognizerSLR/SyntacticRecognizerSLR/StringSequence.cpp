#include "StringSequence.h"

StringSequence::StringSequence(std::string str)
{
	if (str.at(str.size() - 1) != '#')
	{
		str += " #";
	}
	m_stream = std::stringstream(str);
}

std::string StringSequence::GetNextItem()
{
	if (!IsEnd())
	{
		std::string item;
		m_stream >> item;
		++m_index;
		return item;
	}
	throw std::runtime_error("Input sequence ended");
}

bool StringSequence::IsEnd()
{
	return m_stream.eof();
}

int StringSequence::GetCurrentLine() const
{
	return 0;
}

int StringSequence::GetCurrentPosition() const
{
	return m_index;
}

std::string StringSequence::GetLexeme(int index) const
{
	return std::string();
}