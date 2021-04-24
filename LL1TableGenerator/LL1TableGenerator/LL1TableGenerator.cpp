#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <set>
#include <sstream>
#include <string>
#include <vector>

const std::string END_SYMBOL = "#";
const std::string EMPTY_SYMBOL = "e";

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

bool IsTerminal(std::string const& str)
{
	return str.at(0) != '<' || str.at(str.size() - 1) != '>';
}

Rule CreateRule(std::string const& input)
{
	std::stringstream ss(input);
	Rule rule;
	ss >> rule.nonTerminal;
	if (IsTerminal(rule.nonTerminal))
	{
		throw std::runtime_error("Terminal found on the left part of the rule");
	}

	std::string item;
	if (!(ss >> item && item == "-"))
	{
		throw std::runtime_error("Input format must be: <nonTerminal> - <item> [<item>, ...] / <first> [<first>]");
	}

	while (ss >> item && item != "/")
	{
		rule.rightPartOfRule.push_back(item);
	}
	if (rule.rightPartOfRule.empty())
	{
		throw std::runtime_error("Right part of the rule is empty");
	}

	while (ss >> item)
	{
		rule.firstSet.insert(item);
	}
	if (rule.firstSet.empty())
	{
		throw std::runtime_error("First set is empty");
	}

	return rule;
}
std::set<std::string> GetFirstSet(std::vector<Rule> const& rules, std::string const& nonTerminal)
{
	std::set<std::string> firstSet;
	bool wasSymbolFind = false;
	for (auto rule : rules)
	{
		if (rule.nonTerminal == nonTerminal)
		{
			firstSet.merge(rule.firstSet);
			wasSymbolFind = true;
		}
		else if (wasSymbolFind)
		{
			break;
		}
	}
	if (firstSet.empty())
	{
		throw std::runtime_error("Unknown non-terminal: " + nonTerminal);
	}
	return firstSet;
}

int GetNextRowNumber(std::vector<Rule> const& rules, bool isLastSymbolInRule, std::string const& symbol, int rowNumber)
{
	if (IsTerminal(symbol))
	{
		if (symbol == EMPTY_SYMBOL)
		{
			return -1;
		}
		return (isLastSymbolInRule) ? rowNumber + 1 : -1;
	}
	return std::find_if(rules.begin(), rules.end(), [symbol](Rule const& rule) { return rule.nonTerminal == symbol; }) - rules.begin();
}

Table CreateTable(std::vector<Rule> rules)
{
	auto startTerminal = rules.front().nonTerminal;
	std::sort(rules.begin(), rules.end(), [startTerminal](const Rule& a, const Rule& b) {
		return (a.nonTerminal == startTerminal && b.nonTerminal != startTerminal) || (b.nonTerminal != startTerminal && a.nonTerminal < b.nonTerminal);
	});

	Table table;

	int numberOfRows = rules.size();
	for (size_t i = 0; i < rules.size(); ++i)
	{
		auto rule = rules[i];
		Row row;
		row.currSymbol = rule.nonTerminal;
		row.firstSet = rule.firstSet;
		row.isEnd = false;
		row.isError = (i != rules.size() - 1) && (rule.nonTerminal != rules[i + 1].nonTerminal);
		row.needAddToStack = false;
		row.needShift = false;
		row.nextRow = numberOfRows;
		numberOfRows += rule.rightPartOfRule.size();
		table.push_back(row);
	}

	for (auto& rule : rules)
	{
		for (size_t i = 0; i < rule.rightPartOfRule.size(); ++i)
		{
			auto symbol = rule.rightPartOfRule[i];
			bool isTerminal = IsTerminal(symbol);
			Row row;
			row.currSymbol = symbol;
			row.firstSet = isTerminal ? (symbol == EMPTY_SYMBOL ? rule.firstSet : std::set<std::string>{ symbol }) : GetFirstSet(rules, symbol);
			row.isEnd = isTerminal && (row.firstSet.find(END_SYMBOL) != row.firstSet.end());
			row.isError = true;
			row.needAddToStack = !isTerminal && (i < rule.rightPartOfRule.size() - 1);
			row.needShift = isTerminal && (symbol != EMPTY_SYMBOL);
			row.nextRow = GetNextRowNumber(rules, i < rule.rightPartOfRule.size() - 1, symbol, table.size());
			table.push_back(row);
		}
	}

	return table;
}

void PrintTable(Table const& table)
{
	std::cout << "num|symbol  |first set |shift|error|next|stack|end|\n";
	for (size_t i = 0; i < table.size(); ++i)
	{
		auto row = table[i];
		std::cout << std::to_string(i) + std::string(3 - std::to_string(i).size(), ' ') + "|"
				  << row.currSymbol + std::string(std::max((size_t)8, row.currSymbol.size()) - row.currSymbol.size(), ' ') + "|";

		std::stringstream ss;
		std::copy(row.firstSet.begin(), row.firstSet.end(), std::ostream_iterator<std::string>(ss, " "));

		std::cout << ss.str() + std::string(std::max((size_t)10, ss.str().size()) - ss.str().size(), ' ') + "|"
				  << (row.needShift ? "  +  |" : "  -  |")
				  << (row.isError ? "  +  |" : "  -  |")
				  << std::to_string(row.nextRow) + std::string(4 - std::to_string(row.nextRow).size(), ' ') + "|"
				  << (row.needAddToStack ? "  +  |" : "  -  |")
				  << (row.isEnd ? " + |" : " - |")
				  << std::endl;
	}
}

int main()
{
	std::ifstream input("input.txt");
	if (!input.is_open())
	{
		std::cout << "Failed to open file\n";
	}
	std::vector<Rule> rules;
	std::string line;
	while (getline(input, line))
	{
		rules.push_back(CreateRule(line));
	}
	auto table = CreateTable(rules);

	PrintTable(table);

	return 0;
}
