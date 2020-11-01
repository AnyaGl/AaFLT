#include "DotWriter.h"

CDotWriter::CDotWriter()
{
	m_out += "digraph G{\n";
}

void CDotWriter::PrintEdge(int from, int to, const std::string& label)
{
	m_out += std::to_string(from) + "->" + std::to_string(to) + " [label=\"" + label + " \"];\n";
}

void CDotWriter::PrintVertex(int index, const std::string& label, StateType type)
{
	m_out += std::to_string(index) + " [";
	m_out += "shape=\"" + GetShape(type) + "\"";
	m_out += "label=\"" + label + "\"];\n";
}

std::string CDotWriter::Get() const
{
	return m_out + "}\n";
}

std::string CDotWriter::GetShape(StateType type) const
{
	switch (type)
	{
	case StateType::Initial:
		return "circle";
	case StateType::Nonterminal:
		return "box";
	case StateType::Terminal:
		return "doublecircle";
	}

	return std::string();
}