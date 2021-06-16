#include "SLR1TableGenerator.h"
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

int main()
{
	std::ifstream input("input.txt");
	if (!input.is_open())
	{
		std::cout << "Failed to open file\n";
	}
	SLR1TableGenerator generator;
	generator.ReadRulesFromFile(input);
	generator.GenerateTable();
	generator.PrintTable();
	std::cout << "\n\n";
	generator.PrintSimplifiedTable();
}
