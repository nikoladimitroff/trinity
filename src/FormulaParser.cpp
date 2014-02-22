#include <iostream>
#include <mutex>
#include <string>
#include <stack>
#include <queue>
#include <stdlib.h>
#include <math.h>
#include "FormulaParser.h"
#include "TreeNode.h"
using namespace std;

static bool RequiresTwoArguments(const string& token)
{
	return token=="+"||
		token=="-" ||
		token=="*" ||
		token=="/" ||
		token=="%" ||
		token=="^" ||
		token=="max"||
		token=="min";
}

static bool IsOperator(const string& token) {
	return token == "+" ||
		token == "-" ||
		token == "*" ||
		token == "/" ||
		token == "%" ||
		token == "^" ||
		token == "%";
}

static bool IsFunction(const string& token){
	return  token=="min" ||
            token=="max" ||
            token=="sin" ||
            token=="cos" ||
            token=="tan" ||
            token=="cotan" ||
            token=="asin" ||
            token=="floor" ||
            token=="ceil" ||
            token=="sqrt";
}

double FormulaParser::RPNParse(double x, double y, double z, TreeNode* root)
{
	if(root->HasValue())
	{
		if(root->Parameter == "x")
			return x;
		else if(root->Parameter == "y")
			return y;
		else if(root->Parameter == "z")
			return z;
		else
			return root->Value;
	}

	double left, right;
	bool hasRight = root->Right != nullptr;
	bool hasLeft = root->Left != nullptr;

	if(hasLeft)
		left = RPNParse(x, y, z, root->Left);

	if(hasRight)
		right = RPNParse(x, y, z, root->Right);

	switch (root->Op)
	{
	case TreeOperator::Plus:
		return left + right;
	case TreeOperator::Minus:
		return left - right;
    case TreeOperator::Divide:
        return left / right;
    case TreeOperator::Mult:
        return left * right;
    case TreeOperator::Pow:
        return pow(left,right);
    case TreeOperator::Mod:
        return (int)left % (int)right;
    case TreeOperator::Max:
        return max(left,right);
    case TreeOperator::Min:
        return min(left,right);
    case TreeOperator::Sin:
        return sin(right);
    case TreeOperator::Cos:
        return cos(right);
    case TreeOperator::Tan:
        return tan(right);
    case TreeOperator::Cotan:
        return 1/tan(right);
    case TreeOperator::Asin:
        return asin(right);
    case TreeOperator::Floor:
        return floor(right);
    case TreeOperator::Ceil:
        return ceil(right);
    case TreeOperator::Sqrt:
        return sqrt(right);
	default:
		throw new exception();
	}
}

static void AddToQueue(queue<string>& q, string el)
{
	if(el != "")
		q.push(el);
}

static bool IsLeftPare(const string& token)
{
	return token == "(";
}

static bool IsRightPare(const string& token)
{
	return token == ")";
}

static int PrecedenceOf(const string& token) {
    if (token == "%") return -1;
	if (token == "+" || token == "-") return 0;
	if (token == "*" || token == "/" || token == "%") return 1;
	if (token == "^") return 2;
	return -1;
}

static bool PrecedenceCmp(const string& tokenOne, const string& tokenTwo)
{
	int PrecedenceOne = PrecedenceOf(tokenOne);
	int PrecedenceTwo = PrecedenceOf(tokenTwo);
	if(PrecedenceOne==2 && PrecedenceTwo==2)
	{
		return false;
	}
	return (PrecedenceOf(tokenOne)>=PrecedenceOf(tokenTwo));
}

enum TokenType{
	Function,
	LeftPare,
	RightPare,
	Operator,
	Number
};

static TokenType switchHelper(const string& token)
{
	if(IsFunction(token))return Function;
	if(IsLeftPare(token))return LeftPare;
	if(IsRightPare(token))return RightPare;
	if(IsOperator(token))return Operator;
	return Number;
}

static vector<string> ShuntingYard(queue<string> tokens)
{
	string result="";
	stack<string> operatorsStack;
	queue<string> outputQueue;
	int tokensSize = tokens.size();
	for(int i = 0; i<tokensSize;++i)
	{
		string currentToken=tokens.front();
		tokens.pop();
		switch (switchHelper(currentToken))
		{
		case Function:
			{
				operatorsStack.push(currentToken);
				break;
			}
		case LeftPare:
			operatorsStack.push(currentToken);
			break;
		case RightPare:
			{
				if(!operatorsStack.empty())
				{
					while(operatorsStack.top()!="(")
					{
						outputQueue.push(operatorsStack.top());
						operatorsStack.pop();
						if(operatorsStack.empty())break;
					}
					operatorsStack.pop();

					if(!operatorsStack.empty())
					{
						if(IsFunction(operatorsStack.top()))
						{
							outputQueue.push(operatorsStack.top());
							operatorsStack.pop();
						}
					}
				}
				break;
			}
		case Operator:
			{
				while(!operatorsStack.empty()
					&& PrecedenceCmp(operatorsStack.top(),currentToken))
				{
					outputQueue.push(operatorsStack.top());
					operatorsStack.pop();
				}

				operatorsStack.push(currentToken);
				break;
			}
		case Number: outputQueue.push(currentToken);
		}
	}

	while(!operatorsStack.empty()){
		if(operatorsStack.top()!="(")outputQueue.push(operatorsStack.top());
		operatorsStack.pop();
	}
	vector<string> vec;

	while(!outputQueue.empty())
    {
        vec.push_back(outputQueue.front());
        outputQueue.pop();
    }
	return vec;
}


namespace FormulaParser{
queue<string> tokenize(string input){
        queue<string> tokens;
        int tBegin=0;
        for(unsigned int i=0;i<input.length();++i)
        {
            if(input[i]==' '||input[i]==',')
            {
                AddToQueue(tokens, input.substr(tBegin,i-tBegin));
                tBegin = i+1;
            }

            if(input[i]=='('||input[i]==')')
            {
                AddToQueue(tokens, input.substr(tBegin,i-tBegin));
                AddToQueue(tokens, (input.substr(i,1)));
                tBegin = i+1;
            }
        }

        if(tBegin != input.length())
        {
            string s = input.substr(tBegin,input.length()-tBegin);
            AddToQueue(tokens, s);
        }
        return tokens;
    }
}

TreeNode* FormulaParser::GenerateTree(string input)
{
    return Treenify(ShuntingYard(FormulaParser::tokenize(input)));
}
