#pragma once
#include "IInputSequence.h"
#include "SLR1TableGenerator.h"
#include <optional>
#include <stack>

class SyntacticRecognizer
{
public:
	SyntacticRecognizer(std::vector<std::vector<std::string>> const& table, std::vector<SLR1TableGenerator::Rule> const& rules);

	void Recognize(std::shared_ptr<IInputSequence> const& in);

private:
	void UpdateCurrentState(std::string const& item);
	void RollUp(int ruleNum);
	int GetColumnIndex(std::string const& item) const;
	int GetRowIndex() const;

	static bool IsRollUpState(std::string const& state);
	static bool IsState(std::string const& state);

	std::vector<std::vector<std::string>> m_table;
	std::vector<SLR1TableGenerator::Rule> m_rules;
	std::stack<std::string> m_readedItems;
	std::stack<std::string> m_states;
	std::string m_currState;
};
