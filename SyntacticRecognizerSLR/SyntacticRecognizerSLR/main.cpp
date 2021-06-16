#include "CharSequence.h"
#include "SLR1TableGenerator.h"
#include "SyntacticRecognizer.h"
#include <fstream>
#include <iostream>

int main()
{
	std::ifstream file("input.txt");
	if (!file.is_open())
	{
		std::cout << "Failed open file";
		return 1;
	}
	SLR1TableGenerator generator;
	generator.ReadRulesFromFile(file);
	generator.GenerateTable();
	auto table = generator.GetSimplifiedTable();
	auto rules = generator.GetRules();
	generator.PrintTable();
	std::cout << "-------------" << std::endl;
	generator.PrintSimplifiedTable();
	SyntacticRecognizer recognizer(table, rules);
	recognizer.Recognize(std::make_shared<CharSequence>("baaac"));
}
