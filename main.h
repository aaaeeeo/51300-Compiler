#ifndef MAIN_HPP
#define MAIN_HPP

#include <iostream>
#include <string>
#include <vector>
#include <hash_map>
using namespace std;

enum instrucationType {T_COMPOUND, T_EXPRESSION, T_DOITERATION, T_WHILEITERATION, T_FORITERATION, T_SELECT, T_JUMP};
enum variableType {T_INT, T_STRING};
enum unaryOP { T_NEGATIVE};
enum binaryOP {T_PLUS, T_MINUS, T_MULTI, T_DIV, T_MODULE, T_SHIFTLEFT, T_SHIFTRIGHT};
enum comparisonOP { T_EGAL, T_SUP, T_INF, T_INFEQUAL, T_SUPEQUAL, T_DIFF};


class Node
{
public:
    Node(){}
    virtual void code()
    {
        cout<<"Node\n";
    }
    virtual string getString()
    {}
    virtual int getInt()
    {}
    virtual vector<Node*> getList()
    {}

};


class NExpression : public Node
{
public:
    NExpression(){}
    virtual void code()
    {
        cout<<"NExpression\n";
    }
};

class NInstruction : public Node
{
public:
    instrucationType type;
    vector<Node*> insrtctionList;

    NInstruction(instrucationType type, vector<Node*> insrtctionList): type(type), insrtctionList(insrtctionList)
    {}
    NInstruction(instrucationType type): type(type)
    {}
    virtual void code()
    {
        cout<<"NInstruction\n";
    }
};

class NInt : public NExpression
{
public:
    int value;

    NInt(int value): value(value) {}
    virtual void code()
    {
        cout<<"NInt: "<<value<<endl;
    }
};

class NString : public NExpression
{
public:
    string value;

    NString(string value): value(value) {}
    virtual void code()
    {
        cout<<"NString: "<<value<<endl;
    }
};

class NIdentifier : public NExpression
{
public:
    string id;
    variableType type;

    NIdentifier(string id): id(id) {}
    virtual void code()
    {
        cout<<"NIdentifier: "<<id<<endl;
    }
    virtual string getString()
    {
    	return id;
    }
    virtual int getInt()
    {
    	return type;
    }
};

class _Variable : public Node
{
public:
    variableType type;
    NIdentifier id;
};

class NDeclaration : public NInstruction
{
public:
	variableType type;
    vector<Node*> declList; //NVarDeclaration

    NVariableDeclaration(variableType type, vector<Node*> declList):
    type(type), declList(declList) {}
    virtual void code()
    {
        cout<<"NVariableDeclaration"<<endl;
    }
    virtual vector<Node*> getList()
    {
    	return declList;
    }
    virtual getInt()
    {
    	return type;
    }
};

class NVarDeclaration : public NInstruction
{
public:
    Node* name;
    bool isfun;
    vector<Node*> paraList; //_Variable

    NFunctionDeclaration(NIdentifier name, bool isfun, vector<Node*> paraList):
    name(name),isfun(isfun),paraList(paraList) {}
    virtual void code()
    {
        cout<<"NFunctionDeclaration"<<endl;
    }
    virtual void getInt()
    {
    	return isfun;
    }
};

class NUnaryOp : public NExpression
{
public:
    unaryOP operation;
    Node* childExp;

    NUnaryOp(unaryOP operation, Node* childExp):
    operation(operation), childExp(childExp)
    {}
    virtual void code()
    {
        cout<<"NUnaryOp"<<endl;
    }
};

class NBinaryOp : public NExpression
{
public:
    binaryOP operation;
    Node* leftExp;
    Node* rightExp;

    NBinaryOp(binaryOP operation, Node* leftExp, Node* rightExp):
    operation(operation), leftExp(leftExp), rightExp(rightExp)
    {}
    virtual void code()
    {
        cout<<"**NBinaryOp**\n";
        leftExp->code();
        rightExp->code();
    }
};

class NFunctionCall : public NExpression
{
public:
    Node* funcationName;
    vector<Node*> argumentList;

    NFunctionCall(Node* name, vector<Node*> argumentList):
    funcationName(name),argumentList(argumentList) {}

    NFunctionCall(Node* name):
    funcationName(name) {}
    virtual void code()
    {
        cout<<"NFunctionCall"<<endl;
    }
};

class NAssign : public Node
{
public:
    Node* id;
    Node* exp;

    NAssign(Node* id, Node* exp):id(id), exp(exp) {}
    virtual void code()
    {
        cout<<"NAssign"<<endl;
    }
};


class NCondition : public Node
{
public:
    comparisonOP operation;
    Node* leftExp;
    Node* rightExp;

    NCondition(){}
    virtual void code()
    {
        cout<<"NCondition"<<endl;
    }
};

#endif
