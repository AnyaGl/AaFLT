#pragma once
#include <ostream>
#include <string>

enum StateType
{
	Initial,
	Nonterminal,
	Terminal
};

class CDotWriter
{
public:
	CDotWriter();

	void PrintEdge(int from, int to, const std::string& label);
	void PrintVertex(int index, const std::string& label, StateType type);
	std::string Get() const;

private:
	std::string GetShape(StateType type) const;

	std::string m_out;
};