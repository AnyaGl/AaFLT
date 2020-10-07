#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include "Minimization.h"
using namespace boost::numeric::ublas;

class CMealyMachine
{
public:
	using ConversionTable = matrix<unsigned>;

	CMealyMachine(std::ifstream& file)
	{
		unsigned numberOfStates;
		unsigned numberOfInputs;

		file >> numberOfInputs >> m_numberOfOutputs >> numberOfStates;

		m_table = ConversionTable(numberOfInputs, numberOfStates);
		m_outputSignals = ConversionTable(numberOfInputs, numberOfStates);
		for (unsigned i = 0; i < m_table.size1(); i++)
		{
			for (unsigned j = 0; j < m_table.size2(); j++)
			{
				unsigned signal;
				unsigned conversion;
				file >> conversion >> signal;
				m_table(i, j) = conversion;
				m_outputSignals(i, j) = signal;
			}
		}
	}

	void Minimize()
	{
		std::vector<std::vector<unsigned>> classes;
		int k = 0;
		for (unsigned i = 0; i < m_outputSignals.size2(); ++i)
		{
			if (WasColumnAddedToClass(i, classes))
			{
				continue;
			}
			classes.push_back({});
			classes[k].push_back(i);
			for (unsigned j = i + 1; j < m_outputSignals.size2(); ++j)
			{
				if (AreColumnsEqual(i, j, m_outputSignals))
				{
					classes[k].push_back(j);
				}
			}
			++k;
		}


		Minimization minimiz(m_table, classes);
		classes = minimiz.Minimize();
		ConversionTable result = ConversionTable(m_table.size1(), classes.size());


		for (unsigned i = 0; i < result.size1(); ++i)
		{
			for (unsigned j = 0; j < result.size2(); ++j)
			{
				result(i, j) = minimiz.GetNewState(classes, m_table(i, classes[j].front()));

			}
		}

		std::cout << result << std::endl;
	}

	bool WasColumnAddedToClass(unsigned k, std::vector<std::vector<unsigned>> const& classes)
	{
		for (unsigned i = 0; i < classes.size(); ++i)
		{
			for (unsigned j = 0; j < classes[i].size(); ++j)
			{
				if (classes[i][j] == k)
				{
					return true;
				}
			}
		}
		return false;
	}

	bool AreColumnsEqual(unsigned i, unsigned j, matrix<unsigned> const& table)
	{
		for (unsigned l = 0; l < table.size1(); ++l)
		{
			if (table(l, i) != table(l, j))
			{
				return false;
			}
		}
		return true;
	}

private:
	ConversionTable m_table;
	ConversionTable m_outputSignals;
	unsigned m_numberOfOutputs;

};