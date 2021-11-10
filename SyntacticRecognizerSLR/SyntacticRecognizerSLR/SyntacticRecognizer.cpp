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
		std::cout << e.what() << std::endl
				  << "Error in " + std::to_string(in->GetCurrentLine()) + " line on " + std::to_string(in->GetCurrentPosition()) + " position" << std::endl;
	}
}

void SyntacticRecognizer::PrintTracing() const
{
	std::cout << m_tracing.str();
}

void SyntacticRecognizer::TryRecognize(std::shared_ptr<IInputSequence> const& in)
{
	while (!in->IsEnd() || !m_nextItems.empty() && !(m_currState == "ok" && m_nextItems.front() == "#"))
	{
		std::string currItem;
		if (m_nextItems.empty())
		{
			currItem = in->GetNextItem();
			m_isNewItem = true;
		}
		else
		{
			currItem = m_nextItems.front();
			m_nextItems.pop();
		}
		UpdateCurrentState(currItem, in);
	}
	if (m_currState != "ok")
	{
		throw std::runtime_error("The sequence does not belong to grammar.");
	}
}

void SyntacticRecognizer::UpdateCurrentState(std::string const& item, std::shared_ptr<IInputSequence> const& in)
{
	SaveState(item, in);
	int columnIndex = GetColumnIndex(item);
	int rowIndex = GetRowIndex();
	auto newState = m_table.at(rowIndex).at(columnIndex);

	if (IsRollUpState(newState))
	{
		RollUp(std::stoi(newState.erase(0, 1)), item);
	}
	else
	{
		if (!IsState(newState))
		{
			throw std::runtime_error("The sequence does not belong to grammar");
		}
		m_currState = newState;
		m_states.push_back(m_currState);
		UpdateSymbolsTable(item, in);
		m_readedItems.push_back(item);
	}
}

void SyntacticRecognizer::UpdateSymbolsTable(std::string const& item, std::shared_ptr<IInputSequence> const& in)
{

	if (!m_readedItems.empty() && m_readedItems.back() == "<Type>")
	{
		m_symbols.AddSymbol(in->GetLexeme(0), in->GetLexeme(1));
	}
	else if (item == "(")
	{
		m_symbols.CreateNewBlock();
	}
	else if (item == "{" && in->GetLexeme(1) != ")" /*&& m_isNewItem*/)
	{
		m_symbols.CreateNewBlock();
	}
	else if (item != "{" && in->GetLexeme(1) == ")" && m_isNewItem)
	{
		m_symbols.RemoveLastBlock();
		m_isNewItem = false;
	}
	else if (item == "}")
	{
		m_symbols.RemoveLastBlock();
	}
}

void SyntacticRecognizer::RollUp(int ruleNum, std::string const& item)
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
	m_nextItems.push(newItem);
	m_nextItems.push(item);
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

void SyntacticRecognizer::SaveState(std::string const& item, std::shared_ptr<IInputSequence> const& in)
{
	m_tracing << "Current item: " << item << " (" << in->GetLexeme(0) << ")  Current state: " << m_currState << "\n";
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
	m_tracing << "\nSymbols table:\n"
			  << m_symbols.GetTableAsString();
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
