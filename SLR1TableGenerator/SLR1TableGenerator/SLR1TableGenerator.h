#pragma once
#include <fstream>
#include <map>
#include <queue>
#include <unordered_set>
#include <vector>
#include <set>

class SLR1TableGenerator
{
public:
	struct Node
	{
		std::string item;
		int ruleNum;
		int itemNum;
	};

	struct NodeHash
	{
		size_t operator()(SLR1TableGenerator::Node const& node) const
		{
			return std::hash<std::string>{}(node.item);
		}
	};

	using NodesSet = std::unordered_set<Node, NodeHash>;

	struct Rule
	{
		std::string nonTerminal;
		std::vector<std::string> rightPartOfRule;
		NodesSet firstSet;
	};

	using Column = std::vector<NodesSet>;
	using Table = std::map<std::string, Column>;

	void ReadRulesFromFile(std::istream& in);
	Table GenerateTable();
	void PrintTable() const;

private:
	NodesSet GetFirstSet(std::string const& item);
	void ProcessNode(Node const& node);
	bool IsLastItemInRule(Node const& node) const;
	std::set<std::string> GetNextItems(std::string const& item, std::set<std::string>& processedNonTerminals) const;
	void AppendRolledUpItemsToTable(std::set<std::string> const& items, int ruleNum);
	void AppendNodesToTable(NodesSet const& nodes);
	std::set<std::string> FindRollUpItems(std::string const& item) const;

	static Rule CreateRule(std::string const& input);
	static bool IsTerminal(std::string const& item);
	static void PrintNodes(NodesSet const& nodes);
	static bool IsRolledUp(NodesSet const& nodes);

	std::vector<Rule> m_rules;
	Table m_table;
	std::queue<NodesSet> m_itemsForProcessing;
	std::map<std::string, std::vector<std::string>> m_nextItems;
	std::vector<NodesSet> m_processedItems;
	std::string m_startNonTerminal;
};

inline bool operator==(SLR1TableGenerator::Node const& lhs, SLR1TableGenerator::Node const& rhs)
{
	return lhs.item == rhs.item && lhs.ruleNum == rhs.ruleNum && lhs.itemNum == rhs.itemNum;
}

inline bool operator!=(SLR1TableGenerator::Node const& lhs, SLR1TableGenerator::Node const& rhs)
{
	return !(lhs == rhs);
}
