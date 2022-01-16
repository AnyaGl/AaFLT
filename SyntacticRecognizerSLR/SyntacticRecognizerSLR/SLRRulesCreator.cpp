#include "SLRRulesCreator.h"
#include <sstream>
#include <string>

namespace
{
bool IsTerminal(std::string const& str)
{
	return str.at(0) != '<' || str.at(str.size() - 1) != '>';
}
} // namespace

SLRRulesCreator::SLRRulesCreator(std::istream& file)
{
	CreateRulesFromFile(file);

	GenerateFirstSets();
}

void SLRRulesCreator::PrintRules(std::ostream& out)
{
	for (auto rule : m_rules)
	{
		out << rule.nonTerminal << " - ";
		std::copy(rule.rightPartOfRule.begin(), rule.rightPartOfRule.end(), std::ostream_iterator<std::string>(out, " "));
		out << " | ";
		for (auto item : rule.firstSet)
		{
			out << item.item << " " << item.ruleNum << " " << item.itemNum << " ";
		}
		out << std::endl;
	}
}

SLR1TableGenerator::Rule SLRRulesCreator::CreateRule(std::string const& input)
{
	std::stringstream ss(input);
	SLR1TableGenerator::Rule rule;
	ss >> rule.nonTerminal;
	if (IsTerminal(rule.nonTerminal))
	{
		throw std::runtime_error("Terminal found on the left part of the rule");
	}

	std::string item;
	if (!(ss >> item && item == "-"))
	{
		throw std::runtime_error("Input format must be: <nonTerminal> - <item> [<item>, ...] | <first> [<first>]");
	}

	while (ss >> item)
	{
		rule.rightPartOfRule.push_back(item);
	}
	if (rule.rightPartOfRule.empty())
	{
		throw std::runtime_error("Right part of the rule is empty");
	}

	return rule;
}

void SLRRulesCreator::CreateRulesFromFile(std::istream& file)
{
	Rules rules;
	std::string line;
	while (getline(file, line))
	{
		if (!line.empty())
		{
			rules.push_back(CreateRule(line));
		}
	}
	Rules resultRules;

	int i = 0;
	while (i < 10)
	{
		i++;
		for (auto rule : rules)
		{
			if (rule.rightPartOfRule.front() == "e")
			{
				Rules resultRules0 = resultRules;
				for (auto r : resultRules)
				{
					if (std::find_if(r.rightPartOfRule.begin(), r.rightPartOfRule.end(), [rule](auto x) { return x == rule.nonTerminal; }) == r.rightPartOfRule.end())
					{
						continue;
					}
					SLR1TableGenerator::Rule newRule;
					newRule.nonTerminal = r.nonTerminal;
					for (auto x : r.rightPartOfRule)
					{
						if (x != rule.nonTerminal)
						{
							newRule.rightPartOfRule.push_back(x);
						}
					}
					if (newRule.rightPartOfRule.size() == 0)
					{
						continue;
					}
					bool find = false;
					for (auto r : resultRules)
					{
						if (r.nonTerminal == newRule.nonTerminal && r.firstSet == newRule.firstSet && r.rightPartOfRule == newRule.rightPartOfRule)
						{
							find = true;
							break;
						}
					}
					if (!find)
					{
						resultRules0.push_back(newRule);
					}
				}
				resultRules = resultRules0;
			}
			else
			{
				bool find = false;
				for (auto r : resultRules)
				{
					if (r.nonTerminal == rule.nonTerminal && r.firstSet == rule.firstSet && r.rightPartOfRule == rule.rightPartOfRule)
					{
						find = true;
						break;
					}
				}
				if (!find)
				{
					resultRules.push_back(rule);
				}
			}
		}
	}
	m_rules = resultRules;
	//m_rules = rules;
}

void SLRRulesCreator::GenerateFirstSets()
{
	for (int i = 0; i < m_rules.size(); ++i)
	{
		if (IsTerminal(m_rules.at(i).rightPartOfRule.front()))
		{
			m_rules.at(i).firstSet.insert(SLR1TableGenerator::Node{ m_rules.at(i).rightPartOfRule.front(), i, 0 });
		}
	}

	std::map<std::string, SLR1TableGenerator::NodesSet> firstSets;
	for (auto rule : m_rules)
	{
		firstSets[rule.nonTerminal].merge(rule.firstSet);
	}

	bool changed = true;
	while (changed)
	{
		changed = false;
		for (int i = 0; i < m_rules.size(); ++i)
		{
			if (!IsTerminal(m_rules.at(i).rightPartOfRule.front()))
			{
				auto firstSet = firstSets[m_rules.at(i).rightPartOfRule.front()];
				firstSet.insert({ m_rules.at(i).rightPartOfRule.front(), i, 0 });
				m_rules.at(i).firstSet.merge(firstSet);

				bool needAdd = false;
				for (auto item : m_rules.at(i).firstSet)
				{
					if (firstSets[m_rules.at(i).nonTerminal].count(item) == 0)
					{
						needAdd = true;
						break;
					}
				}
				if (needAdd)
				{
					auto set = m_rules.at(i).firstSet;
					firstSets[m_rules.at(i).nonTerminal].merge(set);
					changed = true;
				}
			}
		}
	}
}
