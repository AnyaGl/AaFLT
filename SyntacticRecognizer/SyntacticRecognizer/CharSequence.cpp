#include "CharSequence.h"

CharSequence::CharSequence(std::string const& str)
	: m_str(str)
{
}

std::string CharSequence::GetNextItem()
{
	if (!IsEnd())
	{
		return std::string(1, m_str.at(m_index));
	}
	return std::string();
}

bool CharSequence::IsEnd()
{
	return m_index < m_str.size();
}
