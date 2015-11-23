#ifndef MAIN_HPP
#define MAIN_HPP

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

enum instructionType {T_PROGRAM, T_FUNCTION, T_COMPOUND, T_EXPRESSION, T_DOITERATION, T_WHILEITERATION, T_FORITERATION, T_SELECT, T_JUMP};
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
    virtual Node* getNode()
    {}
    virtual void setInt(int)
    {}
    virtual string getNodeType()
    {}
    virtual int getValue()
    {}
     virtual void setOffset(int o)
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
    instructionType type;
    vector<Node*> instructionList;

    NInstruction(instructionType type, vector<Node*> instructionList): type(type), instructionList(instructionList)
    {}
    NInstruction(instructionType type): type(type)
    {}
    NInstruction()
    {}
    virtual void code()
    {
        cout<<"NInstruction: type:";
        if(type==0)
            cout<<"T_PROGRAM"<<endl;
        if(type==1)
            cout<<"T_FUNCTION"<<endl;
        if(type==2)
            cout<<"T_COMPOUND"<<endl;
        if(type==3)
            cout<<"T_EXPRESSION"<<endl;
        if(type==4)
            cout<<"T_DOITERATION"<<endl;
        if(type==5)
            cout<<"T_WHILEITERATION"<<endl;
        if(type==6)
            cout<<"T_FORITERATION"<<endl;
        if(type==7)
            cout<<"T_SELECT"<<endl;
        if(type==8)
            cout<<"T_JUMP"<<endl;
        for( auto it = instructionList.begin(); it != instructionList.end(); it++)
        {
            (*it)->code();
        }
    }
};

class NInt : public NExpression
{
public:
    int value;

    NInt(int value): value(value) {}
    virtual void code()
    {
        //cout<<"NInt: "<<value<<endl;
        cout<<"movl $"<<value<<", %eax"<<endl;
    }
    virtual int getInt()
    {
        return 0;
    }
    virtual string getNodeType()
    {
        return "NInt";
    }
    virtual int getValue()
    {
        return value;
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
    virtual int getInt()
    {
        return 1;
    }
    virtual string getNodeType()
    {
        return "NString";
    }
};

class NIdentifier : public NExpression
{
public:
    string id;
    variableType type;
    int offset;

    NIdentifier(string id): id(id) {}
    virtual void code()
    {
        //cout<<"NIdentifier: "<<id;

        cout<<"movl -"<<offset<<"(%ebp), %eax\n";
    }
    virtual string getString()
    {
    	return id;
    }
    virtual int getInt()
    {
    	return type;
    }
    virtual void setInt(int t)
    {
        type=(variableType)t;
    }
    virtual void setOffset(int o)
    {
        if(o<0)
            o=-o;
        offset=o;
    }
};

class _Variable : public Node
{
public:
    variableType type;
    Node* id;   //NIdentifier
    _Variable(variableType type, Node* id):
    type(type), id(id)
    {}
    virtual void code()
    {
        cout<<"_Variable: name: "<<id<<"type: "<<( type==0 ? "int" : "string" )<<endl;
    }
    virtual string getString()
    {
        return id->getString();
    }
    virtual int getInt()
    {
        return type;
    }

};

class NDeclaration : public NInstruction
{
public:
	variableType type;
    vector<Node*> declList; //NVarDeclaration

    NDeclaration(variableType type, vector<Node*> declList):
    type(type), declList(declList) {}

    virtual void code()
    {
        cout<<"NDeclaration: type:"<<( type==0 ? "int" : "string" )<<endl;
        for( auto it = declList.begin(); it != declList.end(); it++)
        {
            (*it)->code();
        }
    }
    virtual vector<Node*> getList()
    {
    	return declList;
    }
    virtual int getInt()
    {
    	return type;
    }
};

class NVarDeclaration : public NInstruction
{
public:
    Node* name; //NIdentifier
    bool isfun;
    vector<Node*> paraList; //_Variable

    NVarDeclaration(Node* name, bool isfun, vector<Node*> paraList):
    name(name),isfun(isfun),paraList(paraList) {}
    NVarDeclaration(Node* name, bool isfun):
    name(name),isfun(isfun) {}

    virtual void code()
    {
        cout<<"NVarDeclaration: "<<( isfun==1 ? "function " : "" )<<"name: "<<name->getString()<<endl;
    }
    virtual int getInt()
    {
    	return isfun;
    }
    virtual string getString()
    {
        return ((NIdentifier*) name)->getString();
    }
    virtual vector<Node*> getList()
    {
        return paraList;
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
        cout<<"NUnaryOp: - ";
        childExp->code();
    }
    virtual Node* getNode()
    {
        return childExp;
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
        cout<<"\n\nNBinaryOp: ";

        if(operation==0)
            cout<<"T_PLUS";
        if(operation==1)
            cout<<"T_MINUS";
        if(operation==2)
            cout<<"T_MULTI";
        if(operation==3)
            cout<<"T_DIV";
        if(operation==4)
            cout<<"T_MODULE";
        if(operation==5)
            cout<<"T_SHIFTLEFT";
        if(operation==6)
            cout<<"T_SHIFTRIGHT";
        cout<<"\n";

        if(leftExp->getNodeType()=="NInt" && rightExp->getNodeType()=="NInt")
        {
            int re;
            if(operation==0)
                re=leftExp->getValue()+rightExp->getValue();
            if(operation==1)
                re=leftExp->getValue()-rightExp->getValue();
            if(operation==2)
                re=leftExp->getValue()*rightExp->getValue();
            if(operation==3)
                re=leftExp->getValue()/rightExp->getValue();
            if(operation==4)
                re=leftExp->getValue()%rightExp->getValue();
            if(operation==5)
                re=leftExp->getValue()<<rightExp->getValue();
            if(operation==6)
                re=leftExp->getValue()>>rightExp->getValue();
            cout<<"movl $"<<re<<", %eax"<<endl;
        }
        else
        {
            leftExp->code();
            cout<<"push %eax\n";
            rightExp->code();
            cout<<"push %eax\n";
            cout<<"pop %ebx\npop %eax\n";
            if(operation==0)
                cout<<"addl %ebx, %eax"<<endl;
            if(operation==1)
                cout<<"subl %ebx, %eax"<<endl;
            if(operation==2)
                cout<<"imull %ebx, %eax"<<endl;
            if(operation==3)
            {
                cout<<"cltd\n";
                cout<<"idivl %ebx"<<endl;
            }
            if(operation==4)
            {
                cout<<"cltd\n";
                cout<<"idivl %ebx"<<endl;
                cout<<"movl %edx,%eax\n";
            }
            if(operation==5)
            {
                cout<<"sall %bl, %ea\n";
            }
            if(operation==6)
            {
                cout<<"sarl %bl, %ea\n";
            }
        }

    }
    virtual Node* getNode()
    {
        return leftExp;
    }
    virtual string getNodeType()
    {
        return "NBinaryOp";
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
        cout<<"NFunctionCall: name:"<<funcationName->getString()<<endl;
    }
    virtual Node* getNode()
    {
        return funcationName;
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
        cout<<"NAssign: "<<id->getString()<<" = ";
        exp->code();

    }
};


class NCondition : public Node
{
public:
    comparisonOP operation;
    Node* leftExp;
    Node* rightExp;

    NCondition( comparisonOP operation, Node* leftExp, Node* rightExp ):
    operation(operation), leftExp(leftExp), rightExp(rightExp)
    {}
    virtual void code()
    {
        cout<<"NCondition: ";
        leftExp->code();
        cout<<" ";

        if(operation==0)
            cout<<"T_EGAL";
        if(operation==1)
            cout<<"T_SUP";
        if(operation==2)
            cout<<"T_INF";
        if(operation==3)
            cout<<"T_INFEQUAL";
        if(operation==4)
            cout<<"T_SUPEQUAL";
        if(operation==5)
            cout<<"T_DIFF";

        cout<<" ";
        rightExp->code();
        cout<<endl;
    }
};

#endif
