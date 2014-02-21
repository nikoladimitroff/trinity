#include <iostream>
#include <string>
#include <stack>
#include <queue>
#include <stdlib.h>
#include <math.h>

#include "FormulaParser.h"
using namespace std;

static void AddToQueue(queue<string>& q, string el)
{
	if(el != "")
		q.push(el);
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
	;
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

static queue<string> ShuntingYard(queue<string> tokens)
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
	return outputQueue;
}

static bool RequiresTwoArguments(string token)
{
	return token=="+"||
		token=="-" ||
		token=="*" ||
		token=="/" ||
		token=="%" ||
		token=="^" ||
		token=="max" ||
		token=="min";
}
namespace FormulaParser
{
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
queue<string> FormulaParser::GenerateTree(string input)
{
    return ShuntingYard(tokenize(input));
}

double FormulaParser::RPNParse(double x, double y, double z, queue<string> surfaceEquation) //TODO:NIKOLA tuka malko ne moa si spomnq kak si bqhme napravili Vector v trinity
    {
    stack<double> values;
    while(!surfaceEquation.empty())
    {
        string currentToken = surfaceEquation.front();
        surfaceEquation.pop();
        if(!IsOperator(currentToken)&&!IsFunction(currentToken))
        {
            if(currentToken=="x") values.push(x);
            else if(currentToken=="y") values.push(y);
            else if(currentToken=="z") values.push(z);
            else values.push(atof(currentToken.c_str()));
        }
        else
        {
            double temp=0;
            double argumentOne = values.top();
            values.pop();
            if(RequiresTwoArguments(currentToken))
            {
                double argumentTwo = values.top();
                values.pop();
                if(currentToken=="+")temp = argumentOne+argumentTwo;
                if(currentToken=="-")temp = argumentTwo-argumentOne;
                if(currentToken=="*")temp = argumentOne*argumentTwo;
                if(currentToken=="/")temp = argumentTwo/argumentOne;
                if(currentToken=="^")temp = pow(argumentTwo,argumentOne);
                if(currentToken=="%")temp = (int)argumentOne%(int)argumentTwo;
                if(currentToken=="max")temp = max(argumentOne,argumentTwo);
                if(currentToken=="min")temp = min(argumentOne,argumentTwo);
            }
            else
            {
                if(currentToken=="sin")temp = sin(argumentOne);
                if(currentToken=="cos")temp = cos(argumentOne);
                if(currentToken=="tan") temp = tan(argumentOne);
                if(currentToken=="cotan")temp = 1/tan(argumentOne);
                if(currentToken=="asin")temp = asin(argumentOne);
                if(currentToken=="floor")temp = floor(argumentOne);
                if(currentToken=="ceil")temp = ceil(argumentOne);
                if(currentToken=="sqrt")temp = sqrt(argumentOne);
            }
            values.push(temp);
        }
    }
    return values.top();
}
