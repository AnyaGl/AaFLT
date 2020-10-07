#include <iostream>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include "MooreMachine.h"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include "MealyMachine.h"
//#include <gvc.h>
//
//int main()
//{
//	using namespace std;
//	using namespace boost;
//
//	/* define the graph type
//		  listS: selects the STL list container to store
//				 the OutEdge list
//		  vecS: selects the STL vector container to store
//				the vertices
//		  directedS: selects directed edges
//	*/
//	typedef adjacency_list< listS, vecS, directedS > digraph;
//
//	// instantiate a digraph object with 8 vertices
//	digraph g(8);
//
//	// add some edges
//	add_edge(0, 1, g);
//	add_edge(1, 5, g);
//	add_edge(5, 6, g);
//	add_edge(2, 3, g);
//	add_edge(2, 4, g);
//	add_edge(3, 5, g);
//	add_edge(4, 5, g);
//	add_edge(5, 7, g);
//
//	std::string fileName = "output.dot";
//	std::ofstream file(fileName);
//
//	// represent graph in DOT format and send to cout
//	write_graphviz(file, g);
//
//	return 0;
//}
using namespace boost::numeric::ublas;

int main()
{
	std::string fileName = "input1.txt";
	std::ifstream file(fileName);
	if (!file)
	{
		std::cout << "Failed to open input file" << std::endl;
	}
	unsigned machineType;
	file >> machineType;
	if (machineType == 1)
	{
		CMealyMachine mealyMachine(file);
		mealyMachine.Minimize();
	}
	else if (machineType == 2)
	{
		CMooreMachine mooreMachine(file);
		mooreMachine.Minimize();
	}
	return 0;
}


