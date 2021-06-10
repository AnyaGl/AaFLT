#include "SLR1TableGenerator.h"
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
//
//const std::string STATE = "S";
//const std::string ROLL_UP = "R";
//
//struct Rule
//{
//	std::string nonTerminal;
//	std::vector<std::string> rightPartOfRule;
//	std::set<Node> firstSet;
//};
//
//struct Node
//{
//	std::string item;
//	int ruleNum;
//	int itemNum;
//};
//
//using Table = std::vector<std::vector<std::set<Node>>>;
//
//bool IsTerminal(std::string const& str)
//{
//	return str.at(0) != '<' || str.at(str.size() - 1) != '>';
//}
//
//Rule CreateRule(std::string const& input)
//{
//	std::stringstream ss(input);
//	Rule rule;
//	ss >> rule.nonTerminal;
//	if (IsTerminal(rule.nonTerminal))
//	{
//		throw std::runtime_error("Terminal found on the left part of the rule");
//	}
//
//	std::string item;
//	if (!(ss >> item && item == "-"))
//	{
//		throw std::runtime_error("Input format must be: <nonTerminal> - <item> [<item>, ...] / <first> [<first>]");
//	}
//
//	while (ss >> item && item != "/")
//	{
//		rule.rightPartOfRule.push_back(item);
//	}
//	if (rule.rightPartOfRule.empty())
//	{
//		throw std::runtime_error("Right part of the rule is empty");
//	}
//
//	while (ss >> item)
//	{
//		int ruleNum, itemNum;
//		ss >> ruleNum >> itemNum;
//		rule.firstSet.insert({ item, ruleNum, itemNum });
//	}
//	if (rule.firstSet.empty())
//	{
//		throw std::runtime_error("First set is empty");
//	}
//
//	return rule;
//}
//
//Table CreateTable(std::vector<Rule> rules)
//{
//	Table table;
//
//	std::map<std::string, std::vector<std::string>> changes;
//	std::map<std::string, std::vector<std::string>> nextItems;
//	std::set<std::string> allItems;
//
//	for (auto rule : rules)
//	{
//		allItems.insert(rule.nonTerminal);
//		allItems.insert(rule.rightPartOfRule[0]);
//		for (auto i = 1; i < rule.rightPartOfRule.size(); ++i)
//		{
//			auto prevItem = rule.rightPartOfRule[i - 1];
//			auto currItem = rule.rightPartOfRule[i];
//			nextItems[prevItem].push_back(currItem);
//			allItems.insert(currItem);
//		}
//	}
//
//	std::vector<std::string> firstLine;
//	for (auto item : allItems)
//	{
//		firstLine.push_back(item);
//	}
//
//	ProcessNode(firstLine, table, rules);
//
//	return table;
//}
//
//std::set<Node> ProcessNode(std::vector<std::string> const& firstLine, Table& table, std::vector<Rule> const& rules)
//{
//	auto firstRule = rules[0];
//
//	Node node{ firstRule.rightPartOfRule.at(0), 0, 0 };
//	std::set<Node> nodes;
//	nodes.insert(node);
//	if (IsTerminal(node.item))
//	{
//		nodes.emplace(GetFirstSet(rules, node.item));
//	}
//
//	table.push_back(std::vector<std::set<Node>>{});
//	for (auto& setItem : nodes)
//	{
//		AppendNodeToTable(firstLine, table, setItem);
//	}
//}
//
//bool IsTerminal(std::string const& str)
//{
//	return str.at(0) != '<' || str.at(str.size() - 1) != '>';
//}
//
//std::set<Node> GetFirstSet(std::vector<Rule> const& rules, std::string const& item)
//{
//	std::set<Node> set;
//	for (auto& rule : rules)
//	{
//		if (rule.nonTerminal == item)
//		{
//			set.emplace(rule.firstSet);
//		}
//	}
//	return set;
//}
//
//void AppendNodeToTable(std::vector<std::string> const& firstLine, Table& table, Node const& node)
//{
//	for (auto& firstLineItem : firstLine)
//	{
//		table.back().push_back(std::set<Node>{});
//		if (firstLineItem == node.item)
//		{
//			table.back().back().insert(node);
//		}
//	}
//}

int main()
{
	std::ifstream input("input.txt");
	if (!input.is_open())
	{
		std::cout << "Failed to open file\n";
	}
	SLR1TableGenerator generator;
	generator.ReadRulesFromFile(input);
	generator.GenerateTable();
	generator.PrintTable();
}
