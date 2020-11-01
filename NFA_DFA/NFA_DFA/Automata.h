#pragma once
#include <boost/numeric/ublas/matrix.hpp>
#include <fstream>
#include <map>
using namespace boost::numeric::ublas;

class CAutomata
{
public:
	using ConversionTableNFA = matrix<std::vector<unsigned>>;
	using ConversionTableDFA = matrix<int>; // -1 is empty conversion

	CAutomata(std::istream& in);

	ConversionTableDFA ToDFA();

	std::string ToGraph();

private:
	void AddNewStatesToTable(CAutomata::ConversionTableDFA& table, std::map<unsigned, std::vector<unsigned>> const& newStates, unsigned& newState);
	void UpdateNewStates(std::map<unsigned, std::vector<unsigned>> const& newStates);
	bool IsNewState(std::map<unsigned, std::vector<unsigned>> const& newStates, std::vector<unsigned> const& vector);
	std::vector<unsigned> GetNewConversion(std::vector<unsigned> const& vector, unsigned j);
	unsigned GetState(std::vector<unsigned> const& vector);

	std::map<unsigned, std::vector<unsigned>> m_newStates;
	ConversionTableNFA m_NFATable;
	ConversionTableDFA m_DFATable;
};
