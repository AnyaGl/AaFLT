#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include "Minimization.h"
using namespace boost::numeric::ublas;

class CMooreMachine
{
public:
	using OutputSignals = std::vector<unsigned>;
	using ConversionTable = matrix<unsigned>;

	CMooreMachine(std::ifstream& file)
	{
		unsigned numberOfStates;
		unsigned numberOfInputs;

		file >> numberOfInputs >> m_numberOfOutputs >> numberOfStates;

		for (unsigned i = 0; i < numberOfStates; i++)
		{
			unsigned signal;
			file >> signal;
			if (signal < 1)
			{
				throw std::runtime_error("output signal must be grater than 0");
			}
			m_outputSignals.push_back(signal);
		}
		m_table = ConversionTable(numberOfInputs, numberOfStates);
		for (unsigned i = 0; i < m_table.size1(); i++)
		{
			for (unsigned j = 0; j < m_table.size2(); j++)
			{
				unsigned conversion;
				file >> conversion;
				m_table(i, j) = conversion;
			}
		}
	}
	void Minimize()
	{
		std::vector<std::vector<unsigned>> classes;
		for (unsigned i = 0; i < m_numberOfOutputs; ++i)
		{
			classes.push_back({});
		}

		for (unsigned i = 0; i < m_outputSignals.size(); ++i)
		{
			classes[m_outputSignals[i] - 1].push_back(i);
		}

		Minimization minimiz(m_table, classes);
		classes = minimiz.Minimize();
		ConversionTable result = ConversionTable(m_table.size1(), classes.size());
		for (unsigned i = 0; i < result.size1(); ++i)
		{
			for (unsigned j = 0; j < result.size2(); ++j)
			{
				result(i, j) = minimiz.GetNewState(classes, m_table(i, classes[j].front()));
				if (classes[j].size() > 1)
				{
					for (unsigned l = 1; l < classes[i].size(); ++l)
					{
						m_outputSignals[classes[i][l]] = 0;
					}
				}
			}
		}

		m_outputSignals.erase(std::remove_if(m_outputSignals.begin(),
			m_outputSignals.end(),
			[](unsigned x) {return x == 0; }),
			m_outputSignals.end());

		for (unsigned i = 0; i < m_outputSignals.size(); ++i)
		{
			std::cout << m_outputSignals[i] << " ";
		}
		std::cout << std::endl;

		std::cout << result << std::endl;
	}

private:
	ConversionTable m_table;
	OutputSignals m_outputSignals;
	unsigned m_numberOfOutputs;
};


