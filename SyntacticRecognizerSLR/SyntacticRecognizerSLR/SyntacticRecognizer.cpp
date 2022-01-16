#include "SyntacticRecognizer.h"
#include "DotWriter.h"
#include <iostream>

namespace
{

static bool rollup = false;

std::string ValueToType(std::string const& type)
{
	if (type == "IntNumber" || type == "BinaryNumber" || type == "OctalNumber" || type == "HexNumber")
	{
		return "int";
	}
	if (type == "FixedPointNumber" || type == "FloatPointNumber")
	{
		return "double";
	}
	if (type == "String")
	{
		return "string";
	}
	if (type == "Char")
	{
		return "char";
	}
	if (type == "true" || type == "false")
	{
		return "bool";
	}
	throw;
}

std::string GetType(const std::string& type1, const std::string& type2, int line)
{
	if (type1 == type2)
	{
		return type1;
	}
	if (type1 == "double" && type2 == "int")
	{
		return type1;
	}
	if (type2 == "double" && type1 == "int")
	{
		return type2;
	}
	std::string s = "types are not compatible at line " + std::to_string(line);
	throw std::exception(s.c_str());
}

std::string GetType(const std::unique_ptr<SyntacticRecognizer::Node>& node1, const std::unique_ptr<SyntacticRecognizer::Node>& node2)
{
	return GetType(node1->type, node2->type, node1->line);
}

std::string GetType(std::unique_ptr<SyntacticRecognizer::Node>& node)
{
	if (!node->type.empty())
	{
		return node->type;
	}
	if (node->token == "<Assign>" && node->children.size() == 2)
	{
		GetType(node->children.front());
		GetType(node->children.back());
		node->type = GetType(node->children.front(), node->children.back());
	}
	else if (node->token == "<Operation>" && node->children.size() == 1)
	{
		node->type = GetType(node->children.back());
	}
	else if ((node->token == "<E>" || node->token == "<T>") && node->children.size() == 1)
	{
		node->type = GetType(node->children.back());
	}
	else if ((node->token == "<E>" || node->token == "<T>") && node->children.size() == 3)
	{
		GetType(node->children.front());
		GetType(node->children.back());
		node->type = GetType(node->children.front(), node->children.back());
		if (node->children.at(1)->token == "Comparison")
		{
			node->type = "bool";
		}
	}
	else if (node->token == "<F>" && node->children.size() == 2 && node->children.back()->token == "<ParamsList>")
	{
		GetType(node->children.back());
		auto inTypes1 = node->children.front()->inTypes;
		auto inTypes2 = node->children.back()->inTypes;
		if (inTypes1.size() != inTypes2.size())
		{
			throw std::exception("types are not compatible");
		}
		for (auto i = 0; i < inTypes1.size(); ++i)
		{
			GetType(inTypes1.at(i), inTypes2.at(i), node->line);
		}
		node->type = node->children.front()->type;
	}
	else if (node->token == "<F>" && node->children.size() == 2 && node->children.front()->lexeme == "-")
	{
		auto type = GetType(node->children.back());
		GetType("int", type, node->line);
		node->type = type;
	}
	else if (node->token == "<F>" && node->children.size() == 2)
	{
		node->type = GetType(node->children.back());
	}
	else if (node->token == "<F>" && node->children.size() == 3 && node->children.at(1)->token == "<E>")
	{
		node->type = GetType(node->children.at(1));
	}
	else if (node->token == "<F>" && node->children.size() == 1)
	{
		node->type = GetType(node->children.back());
	}
	else if (node->token == "<F>" && node->children.front()->token == "Identifier")
	{
		node->type = GetType(node->children.front());
	}
	else if (node->token == "<Value>")
	{
		node->type = ValueToType(node->children.front()->token);
		node->children.front()->type = node->type;
	}
	else if (node->token == "<Statement>" && node->children.size() == 2 && node->children.front()->token == "return")
	{
		node->type = GetType(node->children.back());
	}
	else if (node->token == "<StatementList>")
	{
		auto type1 = GetType(node->children.front());
		auto type2 = GetType(node->children.back());
		node->type = type1.empty() ? type2 : type1;
	}
	else if (node->token == "<Func>" && node->children.size() >= 2)
	{
		GetType(node->children.front());
		GetType(node->children.back());
		node->type = GetType(node->children.front(), node->children.back());
	}
	else if (node->token == "<ParamsList>" && node->children.size() == 2)
	{
		GetType(node->children.back());
		node->inTypes = { GetType(node->children.front()) };
		node->inTypes.insert(node->inTypes.end(), node->children.back()->inTypes.begin(), node->children.back()->inTypes.end());
	}
	else if (node->token == "<ParamsList>" && node->children.size() == 1)
	{
		node->inTypes = { GetType(node->children.front()) };
	}
	else if (node->token == "<Loop>" && node->children.front()->token == "while")
	{
		GetType("bool", GetType(node->children.at(1)), node->line);
	}
	else if (node->token == "<Loop>" && node->children.front()->token == "for")
	{
		GetType("bool", GetType(node->children.at(2)), node->line);
	}
	else if (node->token == "<If>")
	{
		GetType("bool", GetType(node->children.at(1)), node->line);
	}
	return node->type;
}
void ClearTree2(std::unique_ptr<SyntacticRecognizer::Node>& parent)
{
	int pos = 0;
	for (auto it = parent->children.begin(); it != parent->children.end();)
	{
		auto& node = *it;
		if (node->token[0] == '<' && node->token != "<First>")
		{
			int n = node->children.size();
			parent->children.insert(parent->children.begin() + pos, std::make_move_iterator(node->children.begin()), std::make_move_iterator(node->children.end()));
			pos += n;

			it = parent->children.erase(parent->children.begin() + pos);
		}
		else
		{
			++it;
			++pos;
		}
	}

	for (auto it = parent->children.begin(); it != parent->children.end(); ++it)
	{
		ClearTree2(*it);
	}
}

void ClearTreeAfterCheckTypes(std::unique_ptr<SyntacticRecognizer::Node>& parent)
{
	for (auto it = parent->children.begin(); it != parent->children.end(); ++it)
	{
		auto& node = *it;
		while (node->children.size() == 1)
		{
			node = std::move(node->children.front());
		}
		it->swap(node);
		ClearTreeAfterCheckTypes(*it);
	}
}

bool IsType(const std::string& str)
{
	return str == "int" || str == "double" || str == "char" || str == "string" || str == "bool";
}

} // namespace

SyntacticRecognizer::SyntacticRecognizer(std::vector<std::vector<std::string>> const& table, std::vector<SLR1TableGenerator::Rule> const& rules)
	: m_table(table)
	, m_rules(rules)
	, m_currState(m_table.at(1).at(0))
{
	m_states.push_back(m_currState);
}

void SyntacticRecognizer::Recognize(std::shared_ptr<IInputSequence> const& in)
{
	try
	{
		TryRecognize(in);
		std::cout << "ok" << std::endl;
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
		//<< "Error in " + std::to_string(in->GetCurrentLine()) + " line on " + std::to_string(in->GetCurrentPosition()) + " position" << std::endl;
	}
}

void SyntacticRecognizer::PrintTracing() const
{
	std::cout << m_tracing.str();
}

void SyntacticRecognizer::TryRecognize(std::shared_ptr<IInputSequence> const& in)
{
	while (!in->IsEnd() || !m_nextItems.empty() && !(m_currState == "ok" && m_nextItems.front() == "#"))
	{
		std::string currItem;
		if (m_nextItems.empty())
		{
			currItem = in->GetNextItem();
			m_isNewItem = true;
		}
		else
		{
			currItem = m_nextItems.front();
			m_nextItems.pop();
		}
		UpdateCurrentState(currItem, in);
	}
	ClearTree(m_tree);
	CheckTypes(m_tree);
	ClearTreeAfterCheckTypes(m_tree.front());
	if (m_currState != "ok")
	{
		throw std::runtime_error("The sequence does not belong to grammar.");
	}
	SaveTreeToGraph();
}

void SyntacticRecognizer::UpdateCurrentState(std::string const& item, std::shared_ptr<IInputSequence> const& in)
{
	SaveState(item, in);
	int columnIndex = GetColumnIndex(item);
	int rowIndex = GetRowIndex();
	auto newState = m_table.at(rowIndex).at(columnIndex);

	if (IsRollUpState(newState))
	{
		RollUp(std::stoi(newState.erase(0, 1)), item, in);
	}
	else
	{
		if (!IsState(newState))
		{
			throw std::runtime_error("The sequence does not belong to grammar");
		}
		UpdateSymbolsTable(item, in);
		CreateNewNode(item, in);
		m_currState = newState;
		m_states.push_back(m_currState);
		m_readedItems.push_back(item);
	}
}

void SyntacticRecognizer::UpdateSymbolsTable(std::string const& item, std::shared_ptr<IInputSequence> const& in)
{

	if (!m_readedItems.empty() && m_readedItems.back() == "<Type>")
	{
		m_symbols.AddSymbol(in->GetLexeme(0), in->GetLexeme(1));
	}
	else if (!m_readedItems.empty() && m_readedItems.back() == "<TypedParamsList>")
	{
		auto idName = m_symbols.CopyLastBlockToInTypesOfPrevBlock();
		if (m_tree.size() == 4 && m_tree.at(1)->lexeme == idName)
		{
			m_tree.at(1)->inTypes = m_symbols.GetInTypes(idName);
		}
	}
	else if (item == "(")
	{
		m_symbols.CreateNewBlock();
	}
	else if (item == "{" && in->GetLexeme(1) != ")")
	{
		m_symbols.CreateNewBlock();
	}
	else if (item != "{" && in->GetLexeme(1) == ")" && m_isNewItem)
	{
		m_symbols.RemoveLastBlock();
		m_isNewItem = false;
	}
	else if (item == "}")
	{
		m_symbols.RemoveLastBlock();
	}
}

void SyntacticRecognizer::RollUp(int ruleNum, std::string const& item, std::shared_ptr<IInputSequence> const& in)
{
	auto rightPartOfRuleSize = m_rules.at(ruleNum).rightPartOfRule.size();
	rightPartOfRuleSize = m_rules.at(ruleNum).rightPartOfRule.back() == "#" ? rightPartOfRuleSize - 1 : rightPartOfRuleSize;

	if (m_states.size() < rightPartOfRuleSize || m_readedItems.size() < rightPartOfRuleSize)
	{
		throw std::runtime_error("The sequence does not belong to grammar");
	}

	auto node = std::make_unique<Node>();
	node->number = m_nodeCounter++;
	for (size_t i = 0; i < rightPartOfRuleSize; ++i)
	{
		m_states.pop_back();
		m_readedItems.pop_back();

		node->line = m_tree.back()->line;
		node->children.insert(node->children.begin(), std::move(m_tree.back()));
		m_tree.pop_back();
	}
	auto newItem = m_rules.at(ruleNum).nonTerminal;
	m_currState = m_states.back();
	m_nextItems.push(newItem);
	m_nextItems.push(item);
	node->token = newItem;
	m_tree.push_back(std::move(node));

	rollup = true;
}

int SyntacticRecognizer::GetColumnIndex(std::string const& item) const
{
	for (size_t i = 0; i < m_table.at(0).size(); ++i)
	{
		if (m_table.at(0).at(i) == item)
		{
			return i;
		}
	}
	throw std::runtime_error("Item '" + item + "' cannot be in input sequence");
}

int SyntacticRecognizer::GetRowIndex() const
{
	for (size_t i = 0; i < m_table.size(); ++i)
	{
		if (m_table.at(i).at(0) == m_currState)
		{
			return i;
		}
	}
	throw std::runtime_error("Unknown state: '" + m_currState + "'");
}

void SyntacticRecognizer::SaveState(std::string const& item, std::shared_ptr<IInputSequence> const& in)
{
	m_tracing << "Current item: " << item << " (" << in->GetLexeme(0) << ")  Current state: " << m_currState << "\n";
	m_tracing << "States: ";
	for (auto state : m_states)
	{
		m_tracing << state << "  ";
	}
	m_tracing << "\nReaded items: ";
	for (auto item : m_readedItems)
	{
		m_tracing << item << "  ";
	}
	m_tracing << "\nSymbols table:\n"
			  << m_symbols.GetTableAsString();
	m_tracing << "\n\n";
}

void SyntacticRecognizer::CreateNewNode(std::string const& item, std::shared_ptr<IInputSequence> const& in)
{
	if (!rollup)
	{
		auto node = std::make_unique<Node>();
		node->token = item;
		node->number = m_nodeCounter++;
		node->line = in->GetCurrentLine();
		node->pos = in->GetCurrentPosition();
		if (item.at(0) != '<' || item.at(item.size() - 1) != '>')
		{
			node->lexeme = in->GetLexeme(0);
			if (item == "Identifier")
			{
				node->type = m_symbols.GetType(node->lexeme);
				node->inTypes = m_symbols.GetInTypes(node->lexeme);
			}
		}
		m_tree.push_back(std::move(node));
	}
	rollup = false;
}

void SyntacticRecognizer::ClearTree(std::vector<std::unique_ptr<Node>>& tree)
{
	for (auto it = tree.begin(); it != tree.end();)
	{
		auto& node = *it;
		if (node->token == "String" || node->token == "Char")
		{
			auto x = node->lexeme.substr(1, node->lexeme.size() - 2);
			node->lexeme = x;
		}
		if (node->token == "<Type>" || node->token == "(" || node->token == ")" || node->token == "="
			|| node->token == "{" || node->token == "}" || node->token == ";" || node->token == ",")
		{
			it = tree.erase(it);
		}
		else
		{
			ClearTree(node->children);
			++it;
		}
	}
}

void SyntacticRecognizer::CheckTypes(std::vector<std::unique_ptr<Node>>& tree)
{
	for (auto& node : tree)
	{
		if (node->token == "<Assign>" || node->token == "<Statement>" || node->token == "<StatementList>" || node->token == "<Func>"
			|| node->token == "<E>" || node->token == "<T>" || node->token == "<F>"
			|| node->token == "<Loop>" || node->token == "<If>")
		{
			GetType(node);
		}
		CheckTypes(node->children);
	}
}

void WriteNodesToGraph(const std::vector<std::unique_ptr<SyntacticRecognizer::Node>>& tree, CDotWriter& writer, int parent)
{
	for (auto it = tree.begin(); it != tree.end(); ++it)
	{
		auto& node = *it;
		writer.PrintEdge(parent, node->number, "");

		std::string inTypes;
		for (auto i = 0; i < node->inTypes.size(); ++i)
		{
			if (i == 0)
			{
				inTypes += "(";
			}
			inTypes += node->inTypes.at(i);
			if (i != node->inTypes.size() - 1)
			{
				inTypes += ", ";
			}
			else
			{
				inTypes += ")";
			}
		}

		auto label = node->lexeme;
		if (node->token[0] == '<')
		{
			label = node->token;
			if (node->token == "<E>" || node->token == "<T>" || node->token == "<F>" || node->token == "<Func>")
			{
				label += ":" + node->type;
			}
			else if (node->token == "<ParamsList>")
			{
				label += inTypes;
			}
		}
		else if (node->token == "Identifier" || node->token == "IntNumber"
			|| node->token == "FixedPointNumber" || node->token == "FloatPointNumber"
			|| node->token == "BinaryNumber" || node->token == "OctalNumber"
			|| node->token == "HexNumber" || node->token == "String" || node->token == "Char")
		{
			label = node->lexeme + ":" + node->type + inTypes;
			if (node->token == "Identifier")
			{
				label = "Identifier " + label;
			}
		}
		//label += " {" + std::to_string(node->line) + ", " + std::to_string(node->pos) + "}";
		writer.PrintVertex(node->number, label, StateType::Nonterminal);

		WriteNodesToGraph(node->children, writer, node->number);
	}
}

void SyntacticRecognizer::SaveTreeToGraph()
{
	std::ofstream out("graph2.dot");
	CDotWriter writer(out);
	for (auto it = m_tree.begin(); it != m_tree.end(); ++it)
	{
		auto& node = *it;
		writer.PrintVertex(node->number, node->token, StateType::Initial);
		WriteNodesToGraph(node->children, writer, node->number);
	}
}

bool SyntacticRecognizer::IsRollUpState(std::string const& state)
{
	return state.at(0) == 'R';
}

bool SyntacticRecognizer::IsState(std::string const& state)
{
	return state.at(0) == 'S' || state.at(0) == 'R' || state == "ok";
}
