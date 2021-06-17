#include "CharSequence.h"
#include <stdexcept>

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
	throw std::runtime_error("Input sequence ended");
}

bool CharSequence::IsEnd()
{
	return m_index >= m_str.size();
}
