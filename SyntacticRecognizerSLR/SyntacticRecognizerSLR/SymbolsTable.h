#pragma once
#include <string>
#include <vector>

class SymbolsTable
{
public:
	struct Type
	{
		std::string type;
		std::vector<std::string> inTypes;
	};

	void CreateNewBlock();
	void RemoveLastBlock();
	void AddSymbol(std::string const& name, std::string const& type, const std::vector<std::string>& inTypes = {});
	std::string GetType(std::string const& name) const;
	std::vector<std::string> GetInTypes(std::string const& name) const;
	std::string GetTableAsString() const;
	std::string CopyLastBlockToInTypesOfPrevBlock();

private:
	std::vector<std::vector<std::pair<std::string, Type>>> m_table;
};
