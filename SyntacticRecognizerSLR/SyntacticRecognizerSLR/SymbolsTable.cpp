#include "SymbolsTable.h"
#include <sstream>

void SymbolsTable::CreateNewBlock()
{
	m_table.push_back({});
}

void SymbolsTable::RemoveLastBlock()
{
	if (!m_table.empty())
	{
		m_table.pop_back();
	}
}

void SymbolsTable::AddSymbol(std::string const& name, std::string const& type)
{
	if (m_table.empty())
	{
		CreateNewBlock();
	}

	for (auto pair : m_table.back())
	{
		if (pair.first == name)
		{
			throw std::exception("symbol already exist");
		}
	}
	m_table.back().push_back({name, type});
}

std::string SymbolsTable::GetType(std::string const& name) const
{
	for (auto it = m_table.rbegin(); it != m_table.rend(); ++it)
	{
		auto block = *it;
		for (auto pair : block)
		{
			if (pair.first == name)
			{
				return pair.second;
			}
		}
	}
	std::string err = "unknown variable: " + name;
	throw std::exception(err.c_str());
}

std::string SymbolsTable::GetTableAsString() const
{
	std::stringstream ss;
	for (auto block : m_table)
	{
		ss << "----------------\n";
		for (auto pair : block)
		{
			ss << pair.first << " - " << pair.second << "\n";
		}
	}
	return ss.str();
}
