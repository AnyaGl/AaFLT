#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>

int main()
{
	std::map<std::string, std::vector<std::string>> grammar;
	std::map<char, std::vector<std::string>> terminals;
	int n;
	std::string word;
	std::ifstream file;
	file.open("input.txt");

	if (!file.is_open())
	{
		return 1;
	}

	file >> word;
	file >> n;
	std::string arrow;
	std::string line;
	char x;
	std::getline(file, line);
	for (int i = 0; i < n; i++)
	{
		std::getline(file, line);
		std::stringstream ss(line);

		std::string nonTerminal;
		ss >> nonTerminal;
		ss >> arrow;

		grammar.emplace(nonTerminal, std::vector<std::string>{});
		auto& rightValues = grammar.at(nonTerminal);
		std::string rightValue;
		while (ss >> x)
		{
			if (x >= 'A' && x <= 'Z')
			{
				rightValue += x;
			}
			else if (x >= 'a' && x <= 'z')
			{
				terminals.emplace(x, std::vector<std::string>{});
				terminals.at(x).push_back(nonTerminal);
			}

		}
		if (!rightValue.empty())
		{ 
			rightValues.push_back(rightValue);
		}
	}

	for (auto pair : grammar)
	{
		std::cout << pair.first << "=>";
		for (auto value : pair.second)
		{
			std::cout << value << ", ";
		}
		std::cout << std::endl;
	}	
	for (auto pair : terminals)
	{
		std::cout << pair.first << "=>";
		for (auto value : pair.second)
		{
			std::cout << value << ", ";
		}
		std::cout << std::endl;
	}



	std::vector<std::vector<std::vector<std::string>>> stairs;
	
	stairs.push_back({});
	stairs.push_back({});
	stairs.back().push_back({});
	for (auto ch : word)
	{
		stairs.back().push_back(terminals.at(ch));
	}
	int jMax = word.size();
	for (int i = 2; i <= word.size(); ++i)
	{
		stairs.push_back({});
		stairs.back().push_back({});
		for (int j = 1; j < jMax; ++j)
		{
			stairs.back().push_back({});
			for (int k = 1; k < i; ++k)
			{
				std::string chain;
				auto cell1 = stairs.at(i - k).at(j);
				auto cell2 = stairs.at(k).at(j + i - k);
				for (auto x : cell1)
				{
					for (auto y : cell2)
					{
						for (auto pair : grammar)
						{
							for (auto chain : pair.second)
							{
								if (chain == x + y)
								{
									if (!std::any_of(stairs.back().back().begin(), stairs.back().back().end(), [pair](const std::string& str) {return str == pair.first;}))
									{
										stairs.back().back().push_back(pair.first);
									}
								}
							}
						}
					}
				}
			}
		}
		--jMax;

	}
	for (auto x : stairs)
	{
		for (auto y : x)
		{
			for (auto z : y)
			{
				std::cout << z << ",";
			}
			std::cout << " | ";
		}
		std::cout << std::endl;
	}

	return 0;
}