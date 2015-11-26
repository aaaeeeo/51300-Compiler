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

extern int labelNo;
extern int labelOut;
extern int labelSkip;
extern int labelLoop;
extern string cfun;

class Node
{
public:
	
    Node(){}
	
    virtual void code()
    {
        cout<<"Node\n";
    }
	
    virtual string getString()
    {
        return "";
    }
    virtual int getInt()
    {
        return -1;
    }
    virtual vector<Node*> getList()
    {}
    virtual Node* getNode()
    {
        return NULL;
    }
    virtual void setInt(int)
    {}
    virtual string getNodeType()
    {
        return "";
    }
    virtual int getValue()
    {
        return -1;
    }
    virtual void setOffset(int o)
    {}
    virtual int getOffset()
    {
        return -1;
    }

};

class NExpression : public Node
{
public:
    NExpression(){}
    virtual void code()
    {
        //cout<<"NExpression\n";
    }
};

//==============================================
//               Instruction
//==============================================
class NInstruction : public Node
{
public:
    instructionType type;
    vector<Node*> instructionList;
    int offset;

    NInstruction(instructionType type, vector<Node*> instructionList): type(type), instructionList(instructionList)
    {}
    NInstruction(instructionType type): type(type)
    {}
    NInstruction()
    {}
    virtual void code()
    {
        if(type==0){
		    printList();
		}//T_PROGRAM
        else if(type==1){
            string name = instructionList.at(0)->getList().at(0)->getString();
            cfun = name;
            int offset = instructionList.at(1)->getOffset();
            cout<<".globl "<<name<<"\n";
            cout<<"\t.type "<<name<<",@function\n";
            cout<<name<<":\n";
            cout<<"\tenter $"<<offset<<",$0\n";
		    printList();
            cout<<"."<<name<<"_ret:\n\tleave\n\tret\n\n";

		}//T_FUNCTION
        else if(type==2)
        {
		    printList();
		}//T_COMPOUND
        else if(type==3){
			printList();
		}//T_EXPRESSION          
        else if(type==4){
			labelLoop++;
			cout<<"\tloop"<<labelLoop<<":"<<endl;
			
			instructionList.at(0)->code();
			instructionList.at(1)->code();
			
			labelSkip++;
			cout<<"skip"<<labelSkip<<endl;
			cout<<"\tjmp "<<"loop"<<labelLoop<<endl;
			
			cout<<"\tskip"<<labelSkip<<":"<<endl;
		}//T_DOITERATION        
        else if(type==5){
			labelLoop++;
			cout<<"\tloop"<<labelLoop<<":"<<endl;
			
			instructionList.at(0)->code();
			labelSkip++;
			cout<<"skip"<<labelSkip<<endl;
			
			instructionList.at(1)->code();
			cout<<"\tjmp "<<"loop"<<labelLoop<<endl;
			
			cout<<"\tskip"<<labelSkip<<":"<<endl;
		}//T_WHILEITERATION         
        else if(type==6){
			labelLoop++;
			cout<<"\tloop"<<labelLoop<<":"<<endl;
			
			instructionList.at(0)->code();
			instructionList.at(1)->code();
			labelSkip++;
			cout<<"skip"<<labelSkip<<endl;
			
			instructionList.at(3)->code();
			instructionList.at(2)->code();			
			cout<<"\tjmp "<<"loop"<<labelLoop<<endl;
			
			cout<<"\tskip"<<labelSkip<<":"<<endl;
		}//T_FORITERATION          
        else if(type==7){
			instructionList.at(0)->code();
			labelNo++;
			cout<<"no"<<labelNo<<endl;
			labelOut++;
			if(instructionList.size()==3){
				instructionList.at(2)->code();
				cout<<"\tjmp "<<"out"<<labelOut<<endl;
			}
			cout<<"\tno"<<labelNo<<":"<<endl;
			instructionList.at(1)->code();
			cout<<"\tout"<<labelOut<<":"<<endl;
		}//T_SELECT
        else if(type==8){
		    
			printList();
            if(instructionList.at(0)->getInt()==1)
            {
                cout<<"\tmovl $.stracc,%eax\n";
            }
            cout<<"\tjmp ."<<cfun<<"_ret\n";
		}//T_JUMP           
    }
	
	void printList(){
	    for( auto it = instructionList.begin(); it != instructionList.end(); it++){
            (*it)->code();
        }  
	}
    void setOffset(int o)
    {
        offset=o;
    }
    int getOffset()
    {
        return offset;
    }
};

//===========================================
//                  Int
//===========================================
class NInt : public NExpression
{
public:
    int value;

    NInt(int value): value(value) {}
    virtual void code()
    {
        cout<<"\tmovl $"<<value<<", %eax"<<endl;
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

//===========================================
//                 String
//===========================================
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

//========================================
//              Identifier
//========================================
class NIdentifier : public NExpression
{
public:
    string id;
    variableType type;
    int offset;

    NIdentifier(string id): id(id) {}
    virtual void code()
    {
        if(offset==1 || offset==-1)
            cout<<"\tmovl "<<id<<", %eax\n";
        else
            cout<<"\tmovl "<<offset<<"(%ebp), %eax\n";
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
        offset=o;
    }
    virtual int getOffset()
    {
        return offset;
    }
    virtual string getNodeType()
    {
        return "NIdentifier";
    }
};

//========================================
//            Single Variable
//========================================
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

//===========================================
//  int a,b,c; int foo(int a, int b, int c)
//===========================================
class NDeclaration : public NInstruction
{
public:
	variableType type;
    vector<Node*> declList; //NVarDeclaration

    NDeclaration(variableType type, vector<Node*> declList):
    type(type), declList(declList) {}

    virtual void code()
    {
        //cout<<"NDeclaration: type:"<<( type==0 ? "int" : "string" )<<endl;
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

//==========================================
//     a,b,c; foo(int a, int b, int c) 
//==========================================
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
        //cout<<"NVarDeclaration: "<<( isfun==1 ? "function " : "" )<<"name: "<<name->getString()<<endl;
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

//=========================================
//             Unary Operation  done
//=========================================
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
        childExp->code();
        cout<<"\tmovl %eax, %ebx\nmovl $0, %eax\nsubl %ebx, %eax\n";
    }
    virtual Node* getNode()
    {
        return childExp;
    }
};

//===========================================
//            Binary operation  done
//===========================================
class NBinaryOp : public NExpression
{
public:
    binaryOP operation;
    Node* leftExp;
    Node* rightExp;
    variableType type;

    NBinaryOp(binaryOP operation, Node* leftExp, Node* rightExp):
    operation(operation), leftExp(leftExp), rightExp(rightExp)
    {}
    virtual void code()
    {

        if(leftExp->getNodeType()=="NInt" && rightExp->getNodeType()=="NInt")
        {
            type= T_INT;
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
            cout<<"\tmovl $"<<re<<", %eax"<<endl;
        }
        else if( leftExp->getInt()==0 && rightExp->getInt()==0 )
        {
            type=T_INT;

            leftExp->code();
            cout<<"\tpush %eax\n";
            rightExp->code();
            cout<<"\tpush %eax\n";
            cout<<"\tpop %ebx\n\tpop %eax\n";
            if(operation==0)
                cout<<"\taddl %ebx, %eax"<<endl;
            if(operation==1)
                cout<<"\tsubl %ebx, %eax"<<endl;
            if(operation==2)
                cout<<"\timull %ebx, %eax"<<endl;
            if(operation==3)
            {
                cout<<"\tcltd\n";
                cout<<"\tidivl %ebx"<<endl;
            }
            if(operation==4)
            {
                cout<<"\tcltd\n";
                cout<<"\tidivl %ebx"<<endl;
                cout<<"\tmovl %edx,%eax\n";
            }
            if(operation==5)
            {
                cout<<"\tsall %bl, %ea\n";
            }
            if(operation==6)
            {
                cout<<"\tsarl %bl, %ea\n";
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
    virtual int getInt()
    {
        return type;
    }

};

//===============================================
//               Function Call
//===============================================
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
		int count=0;
		for( auto it = argumentList.begin(); it != argumentList.end(); it++){
			count++;
            cout<<"\tpushl "<<(*it)->getOffset()<<"(%ebp)"<<endl;
        } 
        cout<<"\tcall "<<funcationName->getString()<<endl;
		cout<<"\taddl $"<<4*count<<", %esp"<<endl;
    }
    virtual Node* getNode()
    {
        return funcationName;
    }
};

//================================================
//                 Assignment   done
//================================================
class NAssign : public Node
{
public:
    Node* id;
    Node* exp;

    NAssign(Node* id, Node* exp):id(id), exp(exp) {}
    virtual void code()
    {
        exp->code();
        int offset= id->getOffset();
        if(offset==1 || offset==-1)
            cout<<"\tmovl %eax, "<<id->getString()<<"\n";
        else
            cout<<"\tmovl %eax, "<<offset<<"(%ebp)\n";
    }
};

//===============================================
//                Condition
//===============================================
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
		leftExp->code();
		cout<<"\tmovl %eax, %esi"<<endl;
		
		rightExp->code();
        cout<<"\tmovl %eax, %edi"<<endl;
		
		cout<<"\tcmpl %esi, %edi"<<endl;

        if(operation==0)
            cout<<"\tjne ";  //equal
        if(operation==1)
            cout<<"\tjle ";  //greater than
        if(operation==2)
            cout<<"\tjge ";  //less than
        if(operation==3)
            cout<<"\tjg ";  //less or equal 
        if(operation==4)
            cout<<"\tjl ";  //greater or equal
        if(operation==5)
            cout<<"\tje ";  //not equal
		
		//labelIn++;
		//cout<<"In"<<labelIn<<endl;
    }
};

#endif
