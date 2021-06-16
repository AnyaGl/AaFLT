#pragma once
#include <string>

class IInputSequence
{
public:
	virtual std::string GetNextItem() = 0;
	virtual bool IsEnd() = 0;
};