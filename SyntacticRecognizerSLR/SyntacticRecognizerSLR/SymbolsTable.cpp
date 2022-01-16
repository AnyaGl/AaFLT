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

void SymbolsTable::AddSymbol(std::string const& name, std::string const& type, const std::vector<std::string>& inTypes)
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
	m_table.back().push_back({ name, { type, inTypes } });
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
				return pair.second.type;
			}
		}
	}
	std::string err = "unknown variable: " + name;
	throw std::exception(err.c_str());
}

std::vector<std::string> SymbolsTable::GetInTypes(std::string const& name) const
{
	for (auto it = m_table.rbegin(); it != m_table.rend(); ++it)
	{
		auto block = *it;
		for (auto pair : block)
		{
			if (pair.first == name)
			{
				return pair.second.inTypes;
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
			ss << pair.first << " - " << pair.second.type << "(";
			for (auto i = 0; i < pair.second.inTypes.size(); ++i)
			{
				ss << pair.second.inTypes.at(i);
				if (i != pair.second.inTypes.size() - 1)
				{
					ss << ", ";
				}
			}
			ss << ")\n";
		}
	}
	return ss.str();
}

std::string SymbolsTable::CopyLastBlockToInTypesOfPrevBlock()
{
	std::vector<std::string> inTypes;
	if (m_table.size() >= 2)
	{
		for (auto s : m_table.back())
		{
			inTypes.push_back(s.second.type);
		}
		m_table.at(m_table.size() - 2).back().second.inTypes = inTypes;
	}
	return m_table.at(m_table.size() - 2).back().first;
}
