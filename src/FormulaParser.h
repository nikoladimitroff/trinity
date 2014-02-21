#ifndef FORMULA_PARSER_GUARD
#define FORMULA_PARSER_GUARD

#include <string>
#include <queue>
namespace FormulaParser
{
    std::queue<std::string> GenerateTree(std::string input);
    double RPNParse(double x, double y, double z, std::queue<std::string> surfaceEquation);
}

#endif // FORMULA_PARSER_GUARD
