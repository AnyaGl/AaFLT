#include "LL1TableGenerator.h"
#include <fstream>
#include <iostream>
#include <sstream>


void PrintTable(LL1TableGenerator::Table const& table)
{
	std::cout << "num|symbol  |first set |shift|error|next|stack|end|\n";
	for (size_t i = 0; i < table.size(); ++i)
	{
		auto row = table[i];
		std::cout << std::to_string(i) + std::string(3 - std::to_string(i).size(), ' ') + "|"
				  << row.currSymbol + std::string(std::max((size_t)8, row.currSymbol.size()) - row.currSymbol.size(), ' ') + "|";

		std::stringstream ss;
		std::copy(row.firstSet.begin(), row.firstSet.end(), std::ostream_iterator<std::string>(ss, " "));

		std::cout << ss.str() + std::string(std::max((size_t)10, ss.str().size()) - ss.str().size(), ' ') + "|"
				  << (row.needShift ? "  +  |" : "  -  |")
				  << (row.isError ? "  +  |" : "  -  |")
				  << std::to_string(row.nextRow) + std::string(4 - std::to_string(row.nextRow).size(), ' ') + "|"
				  << (row.needAddToStack ? "  +  |" : "  -  |")
				  << (row.isEnd ? " + |" : " - |")
				  << std::endl;
	}
}

int main()
{
	std::ifstream input("input.txt");
	if (!input.is_open())
	{
		std::cout << "Failed to open file\n";
	}
	LL1TableGenerator tableGenerator;
	auto table = tableGenerator.CreateTableFromFile(input);

	PrintTable(table);

	return 0;
}
