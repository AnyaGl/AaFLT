#pragma once
#include "IInputSequence.h"

class CharSequence : public IInputSequence
{
public:
	CharSequence(std::string const& str);

	std::string GetNextItem() override;
	bool IsEnd() override;

private:
	std::string m_str;
	int m_index = 0;
};
