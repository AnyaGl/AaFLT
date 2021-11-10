#pragma once
#include <functional>
#include <string>
#include <vector>
#include <stdexcept>

struct DFA
{
	DFA(std::vector<std::vector<int>> table, std::vector<int> terminals, std::vector<std::function<bool(char)>> transitionÑonditions)
		: table(table)
		, terminals(terminals)
		, transitionÑonditions(transitionÑonditions)
	{
		if (table.at(0).size() != transitionÑonditions.size())
		{
			throw std::runtime_error("Êîëè÷åñòâî ïåğåõîäîâ íå ñîâïàäàåò ñ êîëè÷åñòâîì óñëîâèé ıòèõ ïåğåõîäîâ");
		}
	}

	std::vector<std::vector<int>> table;
	std::vector<int> terminals;
	std::vector<std::function<bool(char)>> transitionÑonditions;
};

class DFAWalker
{
public:
	DFAWalker(const DFA& dfa);

	bool IsGrammarWord(std::string const& word);

	bool GoToNextState(char ch);

	bool IsFinishedState();

private:
	int m_state = 0;
	DFA m_dfa;
};
