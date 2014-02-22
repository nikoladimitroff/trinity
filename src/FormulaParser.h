#ifndef FORMULA_PARSER_GUARD
#define FORMULA_PARSER_GUARD

#include <string>
#include <vector>

class TreeNode;

namespace FormulaParser
{
    TreeNode* GenerateTree(std::string input);
    double RPNParse(double x, double y, double z, TreeNode* surfaceEquation);
}

#endif // FORMULA_PARSER_GUARD
