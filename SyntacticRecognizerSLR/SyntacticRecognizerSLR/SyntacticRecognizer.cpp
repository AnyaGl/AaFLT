#include "SyntacticRecognizer.h"

SyntacticRecognizer::SyntacticRecognizer(std::vector<std::vector<std::string>> const& table, std::vector<SLR1TableGenerator::Rule> const& rules)
	: m_table(table)
	, m_rules(rules)
	, m_currState(m_table.at(1).at(0))
{
	m_states.push(m_currState);
}

void SyntacticRecognizer::Recognize(std::shared_ptr<IInputSequence> const& in)
{
	while (!in->IsEnd())
	{
		auto currItem = in->GetNextItem();
		UpdateCurrentState(currItem);
	}
	if (m_currState != "ok")
	{
		throw std::runtime_error("The sequence does not belong to grammar");
	}
}

void SyntacticRecognizer::UpdateCurrentState(std::string const& item)
{
	int columnIndex = GetColumnIndex(item);
	int rowIndex = GetRowIndex();
	auto newState = m_table.at(rowIndex).at(columnIndex);

	if (IsRollUpState(newState))
	{
		RollUp(std::stoi(newState.erase(0, 1)));
	}
	else
	{
		if (!IsState(newState))
		{
			throw std::runtime_error("The sequence does not belong to grammar");
		}
		m_currState = newState;
		m_states.push(m_currState);
		m_readedItems.push(item);
	}
}

void SyntacticRecognizer::RollUp(int ruleNum)
{
	auto rightPartOfRuleSize = m_rules.at(ruleNum).rightPartOfRule.size();
	rightPartOfRuleSize = m_rules.at(ruleNum).rightPartOfRule.back() == "#" ? rightPartOfRuleSize - 1 : rightPartOfRuleSize;

	if (m_states.size() < rightPartOfRuleSize || m_readedItems.size() < rightPartOfRuleSize)
	{
		throw std::runtime_error("The sequence does not belong to grammar");
	}
	for (size_t i = 0; i < rightPartOfRuleSize; ++i)
	{
		m_states.pop();
		m_readedItems.pop();
	}
	auto newItem = m_rules.at(ruleNum).nonTerminal;
	m_currState = m_states.top();
	UpdateCurrentState(newItem);
}

int SyntacticRecognizer::GetColumnIndex(std::string const& item) const
{
	for (size_t i = 0; i < m_table.at(0).size(); ++i)
	{
		if (m_table.at(0).at(i) == item)
		{
			return i;
		}
	}
	throw std::runtime_error("Item '" + item + "' cannot be in input sequence");
}

int SyntacticRecognizer::GetRowIndex() const
{
	for (size_t i = 0; i < m_table.size(); ++i)
	{
		if (m_table.at(i).at(0) == m_currState)
		{
			return i;
		}
	}
	throw std::runtime_error("Unknown state: '" + m_currState + "'");
}

bool SyntacticRecognizer::IsRollUpState(std::string const& state)
{
	return state.at(0) == 'R';
}

bool SyntacticRecognizer::IsState(std::string const& state)
{
	return state.at(0) == 'S' || state.at(0) == 'R' || state == "ok";
}
