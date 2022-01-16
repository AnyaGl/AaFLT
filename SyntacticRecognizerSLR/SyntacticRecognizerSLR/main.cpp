#include "CharSequence.h"
#include "SLR1TableGenerator.h"
#include "SyntacticRecognizer.h"
#include <fstream>
#include <iostream>
#include "StringSequence.h"
#include "SLRRulesCreator.h"
#include "LexerAdapter.h"

int WithLexer()
{
	std::ifstream input("grammar.txt");
	if (!input.is_open())
	{
		std::cout << "Failed to open file\n";
		return 1;
	}
	SLRRulesCreator creator(input);
	std::cout << "Read rules from file...\n";

	std::fstream file("rules.txt");
	if (!file.is_open())
	{
		std::cout << "Failed to open file\n";
		return 1;
	}
	creator.PrintRules(file);

	file.close();
	std::ifstream rulesF("rules.txt");

	std::cout << "Generate SLR table...\n";
	SLR1TableGenerator generator;
	generator.ReadRulesFromFile(rulesF);
	generator.GenerateTable();
	auto table = generator.GetSimplifiedTable();
	auto rules = generator.GetRules();

	std::ofstream tableF("table.txt");

	std::cout << "Recognize program...\n";
	SyntacticRecognizer recognizer(table, rules);
	std::ifstream prog("program.txt");
	if (!prog.is_open())
	{
		std::cout << "Failed to open file\n";
		return 1;
	}
	recognizer.Recognize(std::make_shared<LexerAdapter>(prog));

	std::cout << "\nDo you want to print tracing? y/n\n";
	std::string answer;
	std::cin >> answer;
	if (answer == "y" || answer == "Y")
	{
		recognizer.PrintTracing();
	}
}

int WithoutLexer()
{
	std::ifstream input("input.txt");
	if (!input.is_open())
	{
		std::cout << "Failed to open file\n";
		return 1;
	}
	SLRRulesCreator creator(input);

	std::fstream file("rules.txt");
	if (!file.is_open())
	{
		std::cout << "Failed to open file\n";
		return 1;

	}
	creator.PrintRules(file);

	file.close();
	std::ifstream rulesF("rules.txt");

	SLR1TableGenerator generator;
	generator.ReadRulesFromFile(rulesF);
	generator.GenerateTable();
	auto table = generator.GetSimplifiedTable();
	auto rules = generator.GetRules();

	generator.PrintTable();
	std::cout << "-------------" << std::endl;
	generator.PrintSimplifiedTable();
	std::cout << std::endl;

	SyntacticRecognizer recognizer(table, rules);
	//recognizer.Recognize(std::make_shared<StringSequence>("( ( a + a ) + a )"));
	//recognizer.Recognize(std::make_shared<StringSequence>("- id + - ( num * - num ) + id + id + - id"));
	recognizer.Recognize(std::make_shared<StringSequence>("b"));

	std::cout << "\nDo you want to print tracing? y/n\n";
	std::string answer;
	std::cin >> answer;
	if (answer == "y" || answer == "Y")
	{
		recognizer.PrintTracing();
	}
}

int main()
{
	int x = 1;
	if (x == 1)
	{
		return WithLexer();
	}
	return WithoutLexer();
}