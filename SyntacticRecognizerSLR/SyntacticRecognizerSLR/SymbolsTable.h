#pragma once
#include <string>
#include <vector>

class SymbolsTable
{
public:
	void CreateNewBlock();
	void RemoveLastBlock();
	void AddSymbol(std::string const& name, std::string const& type);
	std::string GetType(std::string const& name) const;
	std::string GetTableAsString() const;

private:
	std::vector<std::vector<std::pair<std::string, std::string>>> m_table;
};
