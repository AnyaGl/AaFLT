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
		return item;
	}
	throw std::runtime_error("Input sequence ended");
}

bool StringSequence::IsEnd()
{
	return m_stream.eof();
}
