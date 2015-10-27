#include <iostream>
#include <string>
#include <vector>
using namespace std;

enum instrucationType {COMPOUND, EXPRESSION, ITERATION, SELECT, JUMP};
enum variableType {INT, STRING};
enum unaryOP {NEGATIVE};
enum binaryOP {PLUS, MINUS, MULTI, DIV, MODULE, SHIFTLEFT, SHIFTRIGHT};
enum comparisonOP {EGAL, SUP, INF, INFEQUAL, SUPEQUAL, DIFF};

class Node
{
public:
	virtual void code();

};

class NExpression : Node
{

};

class NInstruction : Node
{
public:
	instrucationType type;
	vector<NInstruction> insrtctionList;
};

class NInt : NExpression
{
public:
	int value;
};

class NString : NExpression
{
public:
	string value;
};

class NIdentifier : NExpression
{
public:
	string id;
};

class _Variable
{
public:
	variableType type;
	NIdentifier id;
};

class NVariableDeclaration : NInstruction
{
public:
	vector<_Variable> variableList;
};

class NFunctionDeclaration : NInstruction
{
public:
	NIdentifier funcationName;
	variableType returnType;
	vector<_Variable> paraList;
};

class NUnaryOp : NExpression
{
public:
	unaryOP operation;
	NExpression childExp;
};

class NBinaryOp : NExpression
{
public:
	binaryOP operation;
	NExpression leftExp;
	NExpression rightExp;
};

class NFunctionCall : NExpression
{
public:
	NIdentifier funcationName;
	vector<NIdentifier> argumentList;
};

class NAssign : Node
{
public:
	NIdentifier id;
	NExpression exp;
};


class NCondition : Node
{
public:
	comparisonOP operation;
	NExpression leftExp;
	NExpression rightExp;
};