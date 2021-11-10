#pragma once
#include "IInputSequence.h"
#include "SLR1TableGenerator.h"
#include "SymbolsTable.h"
#include <optional>
#include <queue>
#include <sstream>

class SyntacticRecognizer
{
public:
	SyntacticRecognizer(std::vector<std::vector<std::string>> const& table, std::vector<SLR1TableGenerator::Rule> const& rules);

	void Recognize(std::shared_ptr<IInputSequence> const& in);
	void PrintTracing() const;

private:
	void TryRecognize(std::shared_ptr<IInputSequence> const& in);
	void UpdateCurrentState(std::string const& item, std::shared_ptr<IInputSequence> const& in);
	void UpdateSymbolsTable(std::string const& item, std::shared_ptr<IInputSequence> const& in);
	void RollUp(int ruleNum, std::string const& item);
	int GetColumnIndex(std::string const& item) const;
	int GetRowIndex() const;
	void SaveState(std::string const& item, std::shared_ptr<IInputSequence> const& in);

	static bool IsRollUpState(std::string const& state);
	static bool IsState(std::string const& state);

	std::vector<std::vector<std::string>> m_table;
	std::vector<SLR1TableGenerator::Rule> m_rules;
	std::vector<std::string> m_readedItems;
	std::vector<std::string> m_states;
	std::string m_currState;
	std::stringstream m_tracing;
	std::queue<std::string> m_nextItems;
	SymbolsTable m_symbols;
	bool m_isNewItem = false;
	
};
