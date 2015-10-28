#ifndef MAIN_HPP
#define MAIN_HPP

#include <iostream>
#include <string>
#include <vector>
using namespace std;

enum instrucationType {T_COMPOUND, T_EXPRESSION, T_ITERATION, T_SELECT, T_JUMP};
enum variableType {T_INT, T_STRING};
enum unaryOP { T_NEGATIVE};
enum binaryOP {T_PLUS, T_MINUS, T_MULTI, T_DIV, T_MODULE, T_SHIFTLEFT, T_SHIFTRIGHT};
enum comparisonOP { T_EGAL, T_SUP, T_INF, T_INFEQUAL, T_SUPEQUAL, T_DIFF};

class Node
{
public:
	virtual void code()
	{
		cout<<"Node\n";
	}

};

#define YYSTYPE Node*

class NExpression : public Node
{
public:
	virtual void code()
	{
		cout<<"NExpression\n";
	}
};

class NInstruction : public Node
{
public:
	instrucationType type;
	vector<NInstruction> insrtctionList;
	virtual void code()
	{

	}
};

class NInt : public NExpression
{
public:
	int value;
	virtual void code()
	{
		cout<<"NInt: "<<value<<endl;
	}
};

class NString : public NExpression
{
public:
	string value;
	virtual void code()
	{
		cout<<"NString: "<<value<<endl;
	}
};

class NIdentifier : public NExpression
{
public:
	string id;
	virtual void code()
	{
		cout<<"NIdentifier: "<<id<<endl;
	}
};

class _Variable
{
public:
	variableType type;
	NIdentifier id;
};

class NVariableDeclaration : public NInstruction
{
public:
	vector<_Variable> variableList;
};

class NFunctionDeclaration : public NInstruction
{
public:
	NIdentifier funcationName;
	variableType returnType;
	vector<_Variable> paraList;
};

class NUnaryOp : public NExpression
{
public:
	unaryOP operation;
	NExpression childExp;
};

class NBinaryOp : public NExpression
{
public:
	binaryOP operation;
	NExpression leftExp;
	NExpression rightExp;
};

class NFunctionCall : public NExpression
{
public:
	NIdentifier funcationName;
	vector<NIdentifier> argumentList;
};

class NAssign : public Node
{
public:
	NIdentifier id;
	NExpression exp;
};


class NCondition : public Node
{
public:
	comparisonOP operation;
	NExpression leftExp;
	NExpression rightExp;
};

#endif
