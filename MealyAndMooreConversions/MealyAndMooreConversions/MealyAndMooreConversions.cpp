#include <iostream>
#include <vector>

struct Transition
{
    size_t state;
    size_t outputSignal;
};

using MealyTable = std::vector<std::vector<Transition>>;
using MooreTable = std::vector<std::vector<size_t>>;        //the zero line consists of output signals

MealyTable ReadMealyTableFrom(std::istream& stream)
{

}

MooreTable ReadMooreTableFrom(std::istream& stream)
{

}

MealyTable ConversionOfMooreToMealy(MooreTable const& mooreTable)
{

}

MooreTable ConversionOfMealyToMoore(MealyTable const& mealyTable)
{

}

void PrintMealyTable(MealyTable const& mealyTable, std::ostream& stream)
{

}

void PrintMooreTable(MooreTable const& mooreTable, std::ostream& stream)
{

}

int main()
{
    std::cout << "Hello World!\n";
}

