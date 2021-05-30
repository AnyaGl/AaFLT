#pragma once
#include <algorithm>
//#include <fstream>
//#include <iterator>
#include <set>
#include <string>
#include <vector>

class LL1TableGenerator
{
public:
	struct Rule
	{
		std::string nonTerminal;
		std::vector<std::string> rightPartOfRule;
		std::set<std::string> firstSet;
	};

	struct Row
	{
		std::string currSymbol;
		std::set<std::string> firstSet;
		bool needShift = false;
		bool isError = false;
		int nextRow = -1;
		bool needAddToStack = false;
		bool isEnd = false;
	};

	using Table = std::vector<Row>;

	Table CreateTableFromFile(std::istream& file);
	Table CreateTable(std::vector<Rule> rules);

private:
	bool IsTerminal(std::string const& str);
	Rule CreateRule(std::string const& input);
	std::set<std::string> GetFirstSet(std::vector<Rule> const& rules, std::string const& nonTerminal);
	int GetNextRowNumber(std::vector<Rule> const& rules, bool isLastSymbolInRule, std::string const& symbol, int rowNumber);

};
