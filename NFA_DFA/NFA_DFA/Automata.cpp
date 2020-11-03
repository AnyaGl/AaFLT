#include "Automata.h"
#include "DotWriter.h"
#include <map>
#include <string>

CAutomata::CAutomata(std::istream& in)
{
	unsigned numberOfStates;
	unsigned numberOfInputs;
	in >> numberOfInputs >> numberOfStates;

	std::string clear;
	getline(in, clear);

	m_NFATable = ConversionTableNFA(numberOfStates, numberOfInputs);

	for (unsigned i = 0; i < m_NFATable.size1(); i++)
	{
		for (unsigned j = 0; j < m_NFATable.size2(); j++)
		{
			std::string line;
			std::string number;
			getline(in, line);

			std::function<void(std::string&)> AddConversion = [&](std::string& number) {
				if (!number.empty())
				{
					m_NFATable(i, j).push_back(stoi(number));
					number = "";
				}
			};

			for (auto ch : line)
			{
				if (ch == '_')
				{
					continue;
				}
				if (isdigit(ch))
				{
					number += ch;
				}
				if (ch == ',')
				{
					AddConversion(number);
				}
				if (ch == ' ')
				{
					AddConversion(number);
					++j;
				}
			}
			AddConversion(number);
			sort(m_NFATable(i, j).begin(), m_NFATable(i, j).end());
		}
	}
}

CAutomata::ConversionTableDFA CAutomata::ToDFA()
{
	if (m_DFATable.size1() != 0)
	{
		return m_DFATable;
	}
	ConversionTableDFA table(m_NFATable.size1(), m_NFATable.size2());
	std::map<unsigned, std::vector<unsigned>> newStates;
	unsigned newState = unsigned(m_NFATable.size1());
	for (unsigned i = 0; i < m_NFATable.size1(); i++)
	{
		for (unsigned j = 0; j < m_NFATable.size2(); j++)
		{
			if (m_NFATable(i, j).size() > 1)
			{
				if (IsNewState(newStates, m_NFATable(i, j)))
				{
					newStates[newState] = m_NFATable(i, j);
					table(i, j) = newState;
					++newState;
				}
				else
				{
					table(i, j) = GetState(m_NFATable(i, j));
				}
			}
			else if (m_NFATable(i, j).size() == 1)
			{
				table(i, j) = m_NFATable(i, j)[0];
			}
			else
			{
				table(i, j) = -1;
			}
		}
		UpdateNewStates(newStates);
		AddNewStatesToTable(table, newStates, newState);
		newStates.clear();
	}
	m_DFATable = table;
	return table;
}

std::string CAutomata::DFAToGraph()
{
	auto table = m_DFATable;
	CDotWriter writer;
	for (unsigned i = 0; i < table.size1(); i++)
	{
		writer.PrintVertex(i, std::to_string(i), Initial);

		for (unsigned j = 0; j < table.size2(); j++)
		{
			if (table(i, j) != -1)
			{
				writer.PrintEdge(i, table(i, j), std::to_string(j));
			}			
		}
	}

	return writer.Get();
}

void CAutomata::AddNewStatesToTable(CAutomata::ConversionTableDFA& table, std::map<unsigned, std::vector<unsigned>> const& states, unsigned& newState)
{
	std::map<unsigned, std::vector<unsigned>> newStates;
	table.resize(table.size1() + states.size(), table.size2());
	for (auto state : states)
	{
		for (unsigned j = 0; j < table.size2(); j++)
		{
			auto conversion = GetNewConversion(state.second, j);
			if (conversion.size() > 1)
			{
				if (IsNewState(newStates, conversion))
				{
					newStates[newState] = conversion;
					table(state.first, j) = newState;
					++newState;
				}
				else
				{
					table(state.first, j) = GetState(conversion);
				}
			}
			else if (conversion.size() == 1)
			{
				table(state.first, j) = conversion[0];
			}
			else
			{
				table(state.first, j) = -1;
			}
		}
		UpdateNewStates(newStates);
		AddNewStatesToTable(table, newStates, newState);
		newStates.clear();
	}
}

void CAutomata::UpdateNewStates(std::map<unsigned, std::vector<unsigned>> const& newStates)
{
	for (auto state : newStates)
	{
		m_newStates[state.first] = state.second;
	}
}

bool CAutomata::IsNewState(std::map<unsigned, std::vector<unsigned>> const& newStates, std::vector<unsigned> const& vector)
{
	for (auto state : m_newStates)
	{
		if (state.second == vector)
		{
			return false;
		}
	}
	for (auto state : newStates)
	{
		if (state.second == vector)
		{
			return false;
		}
	}
	return true;
}

unsigned CAutomata::GetState(std::vector<unsigned> const& vector)
{
	for (auto state : m_newStates)
	{
		if (state.second == vector)
		{
			return state.first;
		}
	}
	throw std::runtime_error("State does not exist");
}

std::vector<unsigned> CAutomata::GetNewConversion(std::vector<unsigned> const& vector, unsigned j)
{
	std::vector<unsigned> result;
	for (unsigned i = 0; i < vector.size(); i++)
	{
		for (auto elem : m_NFATable(vector.at(i), j))
		{
			if (find(result.begin(), result.end(), elem) == result.end())
			{
				result.push_back(elem);
			}
		}
	}
	sort(result.begin(), result.end());
	return result;
}

namespace
{
std::string ToString(std::vector<unsigned> const& vector)
{
	if (vector.empty())
	{
		return "_";
	}
	std::string result;
	for (auto elem : vector)
	{
		result += std::to_string(elem);
		result += ",";
	}
	result.erase(result.size() - 1); // delete last ','
	return result;
}
} // namespace
