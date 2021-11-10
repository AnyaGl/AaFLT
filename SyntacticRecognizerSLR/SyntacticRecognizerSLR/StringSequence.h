#pragma once
#include "IInputSequence.h"
#include <sstream>

class StringSequence : public IInputSequence
{
public:
	StringSequence(std::string str);

	std::string GetNextItem() override;
	bool IsEnd() override;
	int GetCurrentLine() const override;
	int GetCurrentPosition() const override;
	std::string GetLexeme(int index) const override;

private:
	std::stringstream m_stream;
	int m_index = 0;
};
