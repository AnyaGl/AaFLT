#include "SyntacticRecognizer.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

SyntacticRecognizer::SyntacticRecognizer(LL1TableGenerator::Table const& table)
	: m_table(table)
{
}

void SyntacticRecognizer::Recognize(std::shared_ptr<IInputSequence> const& input)
{
	try
	{
		TryRecognize(input);
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

void SyntacticRecognizer::TryRecognize(std::shared_ptr<IInputSequence> const& input)
{
	m_currItem = input->GetNextItem();
	m_currRow = GetStartRow();

	while (!IsEnd(input))
	{
		SaveState();
		std::set<std::string> expectedItems;
		while (!ItemFoundInFirstSet(m_currItem, m_currRow))
		{
			auto firstSet = m_table.at(m_currRow).firstSet;
			expectedItems.merge(firstSet);
			if (m_table.at(m_currRow).isError)
			{
				std::ostringstream ssExpectedItems;
				std::copy(expectedItems.begin(), expectedItems.end(),
					std::ostream_iterator<std::string>(ssExpectedItems, ", "));
				throw std::runtime_error("Expected one of: " + ssExpectedItems.str() + "\nActual: " + m_currItem);
			}
			m_currRow++;
		}
		GoToNextRow(input);
	}

	if (!IsEnd(input))
	{
		throw std::runtime_error("Unfinished input sequence");
	}
}

void SyntacticRecognizer::GoToNextRow(std::shared_ptr<IInputSequence> const& input)
{
	m_currItem = m_table.at(m_currRow).needShift ? input->GetNextItem() : m_currItem;
	if (m_table.at(m_currRow).needAddToStack)
	{
		m_returnStack.push_back(m_currRow + 1);
	}
	m_currRow = m_table.at(m_currRow).nextRow;
	if (m_currRow == -1)
	{
		m_currRow = m_returnStack.back();
		m_returnStack.pop_back();
	}
}

int SyntacticRecognizer::GetStartRow()
{
	auto rulesCount = m_table.at(0).nextRow;
	while (m_currRow < rulesCount)
	{
		if (ItemFoundInFirstSet(m_currItem, m_currRow))
		{
			return m_currRow;
		}
		m_currRow++;
	}
	throw std::runtime_error("Input sequence cannot start with " + m_currItem);
}

bool SyntacticRecognizer::ItemFoundInFirstSet(std::string const& item, int row)
{
	return m_table.at(row).firstSet.count(item) != 0;
}

bool SyntacticRecognizer::IsEnd(std::shared_ptr<IInputSequence> const& input)
{
	return input->IsEnd() && m_returnStack.empty() && m_table.at(m_currRow).isEnd;
}

void SyntacticRecognizer::SaveState()
{
	m_tracing << "Row: " << m_currRow;
	m_tracing << "\nItem: " << m_currItem;
	m_tracing << "\nStack: ";
	for (auto row : m_returnStack)
	{
		m_tracing << row << "  ";
	}
	m_tracing << "\n\n";
}
