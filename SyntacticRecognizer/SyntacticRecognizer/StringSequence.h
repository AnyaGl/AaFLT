#pragma once
#include "IInputSequence.h"
#include <sstream>

class StringSequence : public IInputSequence
{
public:
	StringSequence(std::string str);

	std::string GetNextItem() override;
	bool IsEnd() override;

private:
	std::stringstream m_stream;
};
