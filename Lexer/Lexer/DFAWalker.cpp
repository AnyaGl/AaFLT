#include "DFAWalker.h"
#include <stdexcept>

DFAWalker::DFAWalker(const DFA& dfa)
	: m_dfa(dfa)
{
}

bool DFAWalker::IsGrammarWord(std::string const& word)
{
	m_state = 0;
	for (auto ch : word)
	{
		if (!GoToNextState(ch))
		{
			return false;
		}
	}
	return IsFinishedState();
}

bool DFAWalker::GoToNextState(char ch)
{
	for (size_t i = 0; i < m_dfa.transitionÑonditions.size(); i++)
	{
		if (m_dfa.transitionÑonditions[i](ch))
		{
			int newState = m_dfa.table.at(m_state).at(i);
			if (newState == -1)
			{
				break;
			}
			m_state = newState;
			return true;
		}
	}
	return false;
}

bool DFAWalker::IsFinishedState()
{
	return std::find(m_dfa.terminals.begin(), m_dfa.terminals.end(), m_state) != m_dfa.terminals.end();
}
