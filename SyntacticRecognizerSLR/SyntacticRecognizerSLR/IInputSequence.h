#pragma once
#include <string>

class IInputSequence
{
public:
	virtual std::string GetNextItem() = 0;
	virtual bool IsEnd() = 0;
	virtual int GetCurrentLine() const = 0;
	virtual int GetCurrentPosition() const = 0;
	virtual std::string GetLexeme(int index) const = 0;
};