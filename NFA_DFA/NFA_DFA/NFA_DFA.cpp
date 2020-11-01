#include "Automata.h"
#include <iostream>

void SaveImage(CAutomata& automata, std::string fileName)
{
	const std::string& dotName = fileName + ".dot";
	const std::string& imageName = fileName + ".png";

	std::ofstream dotFile(dotName);
	dotFile << automata.ToGraph();
	dotFile.close();
	std::string command = "dot -Tpng -o " + imageName + " " + dotName;
	system(command.c_str());
}

void PrintDFA(CAutomata::ConversionTableDFA const& dfa)
{
	for (unsigned i = 0; i < dfa.size1(); i++)
	{
		for (unsigned j = 0; j < dfa.size2(); j++)
		{
			std::cout << dfa(i, j) << " ";
		}
		std::cout << std::endl;
	}
}

int main()
{
	std::string fileName = "input.txt";
	std::ifstream file(fileName);
	if (!file)
	{
		std::cout << "Failed to open input file" << std::endl;
	}
	CAutomata automata(file);
	auto dfa = automata.ToDFA();
	PrintDFA(dfa);
	SaveImage(automata, "image");
}
