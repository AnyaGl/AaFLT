#include "SyntaticRecognizer.h"
#include <stdexcept>
#include <sstream>

SyntaticRecognizer::SyntaticRecognizer(LL1TableGenerator::Table const& table)
	: m_table(table)
{
}

void SyntaticRecognizer::Recognize(std::shared_ptr<IInputSequence> const& input)
{
	m_currItem = input->GetNextItem();
	m_currRow = GetStartRow();

	while (!IsEnd(input) && !input->IsEnd())
	{
		std::set<std::string> expectedItems;
		while (!ItemFoundInFirstSet(m_currItem, m_currRow))
		{
			expectedItems.merge(m_table.at(m_currRow).firstSet);
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

int SyntaticRecognizer::GoToNextRow(std::shared_ptr<IInputSequence> const& input)
{
	m_currItem = m_table.at(m_currRow).needShift ? input->GetNextItem() : m_currItem;
	if (m_table.at(m_currRow).needAddToStack)
	{
		m_returnStack.push(m_currRow + 1);	
	}
	m_currRow = m_table.at(m_currRow).nextRow;
	if (m_currRow == -1)
	{
		m_currRow = m_returnStack.top();
		m_returnStack.pop();
	}
}

int SyntaticRecognizer::GetStartRow()
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

bool SyntaticRecognizer::ItemFoundInFirstSet(std::string const& item, int row)
{
	return m_table.at(row).firstSet.count(item) != 0;
}

bool SyntaticRecognizer::IsEnd(std::shared_ptr<IInputSequence> const& input)
{
	return input->IsEnd() && m_returnStack.empty() && m_table.at(m_currRow).isEnd;
}