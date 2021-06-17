#pragma once
#include "IInputSequence.h"
#include "LL1TableGenerator.h"
#include <memory>
#include <sstream>

class SyntacticRecognizer
{
public:
	SyntacticRecognizer(LL1TableGenerator::Table const& table);

	void Recognize(std::shared_ptr<IInputSequence> const& input);
	void PrintTracing() const;

private:
	void TryRecognize(std::shared_ptr<IInputSequence> const& input);
	bool ItemFoundInFirstSet(std::string const& item, int row);
	void GoToNextRow(std::shared_ptr<IInputSequence> const& input);
	int GetStartRow();
	bool IsEnd(std::shared_ptr<IInputSequence> const& input);
	void SaveState();

	LL1TableGenerator::Table m_table;
	int m_currRow = 0;
	std::string m_currItem;
	std::vector<int> m_returnStack;
	std::stringstream m_tracing;
};
