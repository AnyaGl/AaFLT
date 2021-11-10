#include "SLR1TableGenerator.h"
#include <functional>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>

const std::string ROLL_UP = "RollUp";

namespace
{

std::string GetNodesSetAsString(SLR1TableGenerator::NodesSet const& nodes)
{
	std::stringstream ss;
	for (auto& node : nodes)
	{
		ss << node.item;

		if (node.ruleNum != -1)
		{
			ss << node.ruleNum;
		}
		if (node.itemNum != -1)
		{
			ss << "." << node.itemNum;
		}
		ss << " ";
	}
	return ss.str();
}
} // namespace

void SLR1TableGenerator::ReadRulesFromFile(std::istream& in)
{
	std::string line;
	while (getline(in, line))
	{
		m_rules.push_back(CreateRule(line));
	}

	m_startNonTerminal = m_rules.at(0).nonTerminal;

	if (NeedAddNewRule())
	{
		AddNewRule();
	}
	if (m_rules.front().rightPartOfRule.back() != "#")
	{
		m_rules.front().rightPartOfRule.push_back("#");
	}
}

void SLR1TableGenerator::GenerateTable()
{
	for (auto rule : m_rules)
	{
		m_table.emplace(rule.nonTerminal, Column{});
		m_table.emplace(rule.rightPartOfRule.at(0), Column{});

		for (size_t i = 1; i < rule.rightPartOfRule.size(); ++i)
		{
			auto prevItem = rule.rightPartOfRule.at(i - 1);
			auto currItem = rule.rightPartOfRule.at(i);
			m_nextItems[prevItem].push_back(currItem);

			m_table.emplace(currItem, Column{});
		}
	}

	ProcessFirstNode();

	while (!m_itemsForProcessing.empty())
	{
		for (auto& column : m_table)
		{
			column.second.push_back({});
		}

		m_processedItems.push_back(m_itemsForProcessing.back());
		m_itemsForProcessing.pop_back();
		for (auto node : m_processedItems.back())
		{
			ProcessNode(node);
		}

		UpdateItemsForProcess();
	}
}

SLR1TableGenerator::NodesSet SLR1TableGenerator::GetFirstSet(std::string const& item) const
{
	NodesSet set;
	for (auto rule : m_rules)
	{
		if (rule.nonTerminal == item)
		{
			set.merge(rule.firstSet);
		}
	}
	return set;
}

void SLR1TableGenerator::ProcessNode(Node const& node)
{
	NodesSet nodes;
	if (IsLastItemInRule(node))
	{
		std::set<std::string> processedNonTerminals;
		auto nextItems = GetNextItems(m_rules.at(node.ruleNum).nonTerminal, processedNonTerminals);
		AppendRolledUpItemsToTable(nextItems, node.ruleNum);
		return;
	}
	auto nextNode = m_rules.at(node.ruleNum).rightPartOfRule.at(node.itemNum + 1);
	nodes.insert(Node{ nextNode, node.ruleNum, node.itemNum + 1 });
	if (!IsTerminal(nextNode))
	{
		nodes.merge(GetFirstSet(nextNode));
	}
	AppendNodesToTable(nodes);
}

void SLR1TableGenerator::ProcessFirstNode()
{
	for (auto& column : m_table)
	{
		column.second.push_back({});
	}
	m_processedItems.push_back({ Node{ m_startNonTerminal, -1, -1 } });

	auto nextNode = Node{ m_rules.at(0).rightPartOfRule.at(0), 0, 0 };
	NodesSet nodes;
	nodes.insert(nextNode);
	auto set = m_rules.at(0).firstSet;
	nodes.merge(set);
	if (!IsTerminal(nextNode.item))
	{
		nodes.merge(GetFirstSet(nextNode.item));
	}
	AppendNodesToTable(nodes);
	UpdateItemsForProcess();
	m_table[m_startNonTerminal].at(0) = { Node{ "ok", -1, -1 } };
}

bool SLR1TableGenerator::IsLastItemInRule(Node const& node) const
{
	return m_rules.at(node.ruleNum).rightPartOfRule.size() - 1 == node.itemNum || m_rules.at(node.ruleNum).rightPartOfRule.at(node.itemNum + 1) == "#";
}

std::set<std::string> SLR1TableGenerator::GetNextItems(std::string const& item, std::set<std::string>& processedNonTerminals) const
{
	processedNonTerminals.insert(item);
	std::set<std::string> nextItems;
	if (item == m_startNonTerminal)
	{
		nextItems.insert("#");
	}
	auto rollUpItems = FindRollUpItems(item);
	for (auto& rollUpItem : rollUpItems)
	{
		if (processedNonTerminals.find(rollUpItem) == processedNonTerminals.end())
		{
			nextItems.merge(GetNextItems(rollUpItem, processedNonTerminals));
		}
	}
	if (m_nextItems.find(item) == m_nextItems.end())
	{
		return nextItems;
	}
	auto items = m_nextItems.at(item);
	for (auto nextItem : items)
	{
		if (!IsTerminal(nextItem))
		{
			auto firstSet = GetFirstSet(nextItem);
			for (auto item : firstSet)
			{
				nextItems.insert(item.item);
			}
			nextItems.insert(nextItem);
		}
		else
		{
			nextItems.insert(nextItem);
		}
	}

	return nextItems;
}

void SLR1TableGenerator::AppendRolledUpItemsToTable(std::set<std::string> const& items, int ruleNum)
{
	for (auto item : items)
	{
		m_table.at(item).back().insert(Node{ ROLL_UP, ruleNum, -1 });
	}
}

void SLR1TableGenerator::AppendNodesToTable(NodesSet const& nodes)
{
	for (auto node : nodes)
	{
		if (node.item != "e")
		{
			m_table.at(node.item).back().insert(node);

		}
	}
}

std::set<std::string> SLR1TableGenerator::FindRollUpItems(std::string const& item) const
{
	std::set<std::string> set;
	for (auto& rule : m_rules)
	{
		if (rule.rightPartOfRule.back() == item)
		{
			set.insert(rule.nonTerminal);
		}
	}
	return set;
}

void SLR1TableGenerator::UpdateItemsForProcess()
{
	for (auto& column : m_table)
	{
		auto item = column.second.back();
		if (item.size() > 0 && std::find(m_itemsForProcessing.begin(), m_itemsForProcessing.end(), item) == m_itemsForProcessing.end() && std::find(m_processedItems.begin(), m_processedItems.end(), item) == m_processedItems.end() && !IsRolledUp(item))
		{
			m_itemsForProcessing.push_back(item);
		}
	}
}

bool SLR1TableGenerator::NeedAddNewRule() const
{
	for (auto item : m_rules.at(0).rightPartOfRule)
	{
		if (m_startNonTerminal == item)
		{
			return true;
		}
	}
	for (size_t i = 1; i < m_rules.size(); ++i)
	{
		for (auto item : m_rules.at(i).rightPartOfRule)
		{
			if (m_startNonTerminal == item)
			{
				return true;
			}
		}
		if (m_startNonTerminal == m_rules.at(i).nonTerminal)
		{
			return true;
		}
	}

	return false;
}

void SLR1TableGenerator::AddNewRule()
{
	for (size_t i = 0; i < m_rules.size(); ++i)
	{
		NodesSet firstSet;
		for (auto& item : m_rules.at(i).firstSet)
		{
			firstSet.insert(Node{ item.item, item.ruleNum + 1, item.itemNum });
		}
		m_rules.at(i).firstSet = firstSet;
	}

	NodesSet firstSet;
	firstSet.insert(Node{ m_startNonTerminal, 0, 0 });
	firstSet.merge(GetFirstSet(m_startNonTerminal));

	Rule rule;
	rule.nonTerminal = "<First>";
	rule.rightPartOfRule = { m_startNonTerminal };
	m_startNonTerminal = rule.nonTerminal;
	rule.firstSet = firstSet;

	m_rules.insert(m_rules.begin(), rule);
}

void SLR1TableGenerator::PrintTable(std::ostream& out) const
{
	std::map<int, int> spacesCount;
	spacesCount[0] = m_processedItems.at(0).size();
	for (auto item : m_processedItems)
	{
		auto str = GetNodesSetAsString(item);
		if (str.size() > spacesCount[0])
		{
			spacesCount[0] = str.size();
		}
	}
	auto counter = 1;
	for (auto column : m_table)
	{
		spacesCount[counter] = column.first.size();
		for (auto item : column.second)
		{
			auto str = GetNodesSetAsString(item);
			if (str.size() > spacesCount[counter])
			{
				spacesCount[counter] = str.size();
			}
		}
		counter++;
	}

	out << std::string(spacesCount[0], ' ') << "|";
	counter = 1;
	for (auto column : m_table)
	{
		out << column.first << std::string(spacesCount[counter] - column.first.size(), ' ') << "|";
		counter++;
	}
	out << std::endl;

	auto columnSize = m_table.at(m_startNonTerminal).size();
	for (size_t i = 0; i < columnSize; ++i)
	{
		PrintNodes(out, m_processedItems.at(i), spacesCount[0]);
		out << "|";
		counter = 1;
		for (auto column : m_table)
		{
			PrintNodes(out, column.second.at(i), spacesCount[counter]);
			out << "|";
			counter++;
		}
		out << std::endl;
	}
}

std::vector<std::vector<std::string>> SLR1TableGenerator::GetSimplifiedTable() const
{
	std::vector<std::vector<std::string>> table;
	auto verticalSize = m_processedItems.size();
	int counter = 0;
	std::map<std::string, NodesSet> changes = { { "ok", { Node{ "ok", -1, -1 } } } };

	auto getChange = [this, &changes](NodesSet const& set) {
		std::optional<std::string> result;
		for (auto& change : changes)
		{
			if (change.second == set)
			{
				result = change.first;
				return result;
			}
		}
		return result;
	};

	table.push_back({});
	table.back().push_back("  ");
	for (size_t i = 0; i < verticalSize; ++i)
	{
		table.push_back({});
		auto change = getChange(m_processedItems.at(i));
		if (!change)
		{
			change = "S" + std::to_string(counter++);
			changes[*change] = m_processedItems.at(i);
		}
		table.back().push_back(*change);
	}

	for (auto& column : m_table)
	{
		table.front().push_back(column.first);

		for (size_t i = 0; i < verticalSize; ++i)
		{
			auto item = column.second.at(i);

			auto change = getChange(item);
			if (item.size() < 1)
			{
				change = "  ";
			}
			if (!change)
			{
				if (IsRolledUp(item))
				{
					change = "R" + std::to_string(item.begin()->ruleNum);
				}
				else
				{
					change = "S" + std::to_string(counter++);
				}
				changes[*change] = item;
			}
			table.at(i + 1).push_back(*change);
		}
	}
	std::ofstream change("changes.txt");

	for (auto ch : changes)
	{
		change << ch.first << "  -  ";
		PrintNodes(change, ch.second, 100);
		change << std::endl;
	}

	return table;
}

void SLR1TableGenerator::PrintSimplifiedTable() const
{
	auto table = GetSimplifiedTable();

	for (auto line : table)
	{
		for (auto elem : line)
		{
			std::cout << elem << "   |";
		}
		std::cout << std::endl;
	}
}

std::vector<SLR1TableGenerator::Rule> SLR1TableGenerator::GetRules() const
{
	return m_rules;
}

SLR1TableGenerator::Rule SLR1TableGenerator::CreateRule(std::string const& input)
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

	while (ss >> item && item != "|")
	{
		rule.rightPartOfRule.push_back(item);
	}
	if (rule.rightPartOfRule.empty())
	{
		throw std::runtime_error("Right part of the rule is empty");
	}

	while (ss >> item)
	{
		int ruleNum, itemNum;
		ss >> ruleNum >> itemNum;
		rule.firstSet.insert({ item, ruleNum, itemNum });
	}
	if (rule.firstSet.empty())
	{
		throw std::runtime_error("First set is empty");
	}

	return rule;
}

bool SLR1TableGenerator::IsTerminal(std::string const& item)
{
	return item.at(0) != '<' || item.at(item.size() - 1) != '>';
}

void SLR1TableGenerator::PrintNodes(std::ostream& out, NodesSet const& nodes, int maxLength)
{
	auto str = GetNodesSetAsString(nodes);
	out << str
		<< std::string(maxLength - str.size(), ' ');
}

bool SLR1TableGenerator::IsRolledUp(NodesSet const& nodes)
{
	for (auto& node : nodes)
	{
		if (node.item == ROLL_UP)
		{
			if (nodes.size() != 1)
			{
				throw std::runtime_error("Rolled up item cannot contains more than one node");
			}
			return true;
		}
	}
	return false;
}
