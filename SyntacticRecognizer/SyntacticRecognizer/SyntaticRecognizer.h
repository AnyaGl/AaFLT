#pragma once
#include "IInputSequence.h"
#include "LL1TableGenerator.h"
#include <memory>
#include <stack>

class SyntaticRecognizer
{
public:
	SyntaticRecognizer(LL1TableGenerator::Table const& table);

	void Recognize(std::shared_ptr<IInputSequence> const& input);

	int GoToNextRow(std::shared_ptr<IInputSequence> const& input);

private:
	bool ItemFoundInFirstSet(std::string const& item, int row);
	int GetStartRow();
	bool IsEnd(std::shared_ptr<IInputSequence> const& input);

	LL1TableGenerator::Table m_table;
	int m_currRow = 0;
	std::string m_currItem;
	std::stack<int> m_returnStack;
};
