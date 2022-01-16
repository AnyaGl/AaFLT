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
	CDotWriter(std::ostream& out);

	void PrintEdge(int from, int to, const std::string& label);
	void PrintVertex(int index, const std::string& label, StateType type);

	~CDotWriter();

private:
	std::string GetShape(StateType type) const;

	std::ostream& m_out;
};