#include "LL1TableGenerator.h"
#include <sstream>
#include <iostream>

const std::string END_SYMBOL = "#";
const std::string EMPTY_SYMBOL = "e";


bool LL1TableGenerator::IsTerminal(std::string const& str)
{
	return str.at(0) != '<' || str.at(str.size() - 1) != '>';
}

LL1TableGenerator::Rule LL1TableGenerator::CreateRule(std::string const& input)
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

std::set<std::string> LL1TableGenerator::GetFirstSet(std::vector<Rule> const& rules, std::string const& nonTerminal)
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

int LL1TableGenerator::GetNextRowNumber(std::vector<Rule> const& rules, bool isLastSymbolInRule, std::string const& symbol, int rowNumber)
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

LL1TableGenerator::Table LL1TableGenerator::CreateTableFromFile(std::istream& file)
{
	std::vector<Rule> rules;
	std::string line;
	while (getline(file, line))
	{
		rules.push_back(CreateRule(line));
	}
	return CreateTable(rules);
}

LL1TableGenerator::Table LL1TableGenerator::CreateTable(std::vector<Rule> rules)
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

