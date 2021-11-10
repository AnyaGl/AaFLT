#pragma once
#include "SLR1TableGenerator.h"
#include <fstream>
#include <iostream>

class SLRRulesCreator
{
public:
	using Rules = std::vector<SLR1TableGenerator::Rule>;

	SLRRulesCreator(std::istream& file);

	void PrintRules(std::ostream& out = std::cout);

private:
	SLR1TableGenerator::Rule CreateRule(std::string const& input);
	void CreateRulesFromFile(std::istream& file);

	void GenerateFirstSets();

	Rules m_rules;
};
