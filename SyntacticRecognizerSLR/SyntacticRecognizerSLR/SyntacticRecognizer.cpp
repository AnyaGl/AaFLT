#include "SyntacticRecognizer.h"
#include <iostream>

SyntacticRecognizer::SyntacticRecognizer(std::vector<std::vector<std::string>> const& table, std::vector<SLR1TableGenerator::Rule> const& rules)
	: m_table(table)
	, m_rules(rules)
	, m_currState(m_table.at(1).at(0))
{
	m_states.push_back(m_currState);
}

void SyntacticRecognizer::Recognize(std::shared_ptr<IInputSequence> const& in)
{
	try
	{
		TryRecognize(in);
		std::cout << "ok" << std::endl;
	}
	catch (std::exception& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
	}
}

void SyntacticRecognizer::PrintTracing() const
{
	std::cout << m_tracing.str();
}

void SyntacticRecognizer::TryRecognize(std::shared_ptr<IInputSequence> const& in)
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
	SaveState(item);
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
		m_states.push_back(m_currState);
		m_readedItems.push_back(item);
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
		m_states.pop_back();
		m_readedItems.pop_back();
	}
	auto newItem = m_rules.at(ruleNum).nonTerminal;
	m_currState = m_states.back();
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

void SyntacticRecognizer::SaveState(std::string const& item)
{
	m_tracing << "Current item: " << item << "   Current state: " << m_currState << "\n";
	m_tracing << "States: ";
	for (auto state : m_states)
	{
		m_tracing << state << "  ";
	}
	m_tracing << "\nReaded items: ";
	for (auto item : m_readedItems)
	{
		m_tracing << item << "  ";
	}
	m_tracing << "\n\n";
}

bool SyntacticRecognizer::IsRollUpState(std::string const& state)
{
	return state.at(0) == 'R';
}

bool SyntacticRecognizer::IsState(std::string const& state)
{
	return state.at(0) == 'S' || state.at(0) == 'R' || state == "ok";
}
