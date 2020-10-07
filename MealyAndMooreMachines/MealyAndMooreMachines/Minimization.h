#pragma once
#include <boost/numeric/ublas/matrix.hpp>
using namespace boost::numeric::ublas;

class Minimization
{
public:
	Minimization(matrix<unsigned> const& table, std::vector<std::vector<unsigned>> const& classes)
		: m_table(table)
		, m_classes(classes)
	{
	}

	std::vector<std::vector<unsigned>> Minimize()
	{
		matrix<unsigned> table = m_table;

		for (unsigned i = 0; i < m_table.size1(); ++i)
		{
			for (unsigned j = 0; j < m_table.size2(); ++j)
			{
				table(i, j) = GetNewState(m_classes, m_table(i, j));
			}
		}

		std::vector<std::vector<unsigned>> classes = m_classes;
		std::vector<std::vector<unsigned>> classes1;
		while (classes != classes1)
		{
			auto k = 0;
			if (classes1.size() != 0)
			{
				classes = classes1;
				classes1 = {};
			}

			for (unsigned i = 0; i < table.size2(); ++i)
			{
				if (WasColumnAddedToClass(i, classes1))
				{
					continue;
				}
				classes1.push_back({});
				classes1[k].push_back(i);
				for (unsigned j = i + 1; j < table.size2(); ++j)
				{
					if (AreColumnsEqual(i, j, table) && FromOneClass(i, j, classes))
					{
						classes1[k].push_back(j);
					}
				}
				++k;
			}
			for (unsigned i = 0; i < table.size1(); i++)
			{
				for (unsigned j = 0; j < table.size2(); j++)
				{
					table(i, j) = GetNewState(classes1, m_table(i, j));
				}
			}
		}
		return classes;
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

	bool FromOneClass(unsigned k, unsigned j, std::vector<std::vector<unsigned>> classes)
	{
		for (unsigned i = 0; i < classes.size(); ++i)
		{
			auto line = classes[i];
			if (std::find(line.begin(), line.end(), k) != line.end() && std::find(line.begin(), line.end(), j) != line.end())
			{
				return true;
			}
		}
		return false;
	}

	unsigned GetNewState(std::vector<std::vector<unsigned>> classes, unsigned state)
	{
		for (unsigned i = 0; i < classes.size(); ++i)
		{
			auto line = classes[i];
			if (std::find(line.begin(), line.end(), state) != line.end())
			{
				return i;
			}
		}
		throw std::runtime_error("Unknown state");
	}

private:
	matrix<unsigned> m_table;
	std::vector<std::vector<unsigned>> m_classes;
};