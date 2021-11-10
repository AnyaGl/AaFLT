#include "CharSequence.h"

CharSequence::CharSequence(std::string const& str)
	: m_str(str)
{
	if (m_str.at(m_str.size() - 1) != '#')
	{
		m_str += "#";
	}
}

std::string CharSequence::GetNextItem()
{
	if (!IsEnd())
	{
		return std::string(1, m_str.at(m_index++));
	}
	return std::string();
}

bool CharSequence::IsEnd()
{
	return m_index >= m_str.size();
}

int CharSequence::GetCurrentLine() const
{
	return 0;
}

int CharSequence::GetCurrentPosition() const
{
	return m_index;
}

std::string CharSequence::GetLexeme(int index) const
{
	return std::string();
}
