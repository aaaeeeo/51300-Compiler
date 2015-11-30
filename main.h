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
extern int noMax;
extern int outMax;
extern int skipMax;
extern int loopMax;
extern bool isStrncpy;
extern bool isStrncat;
extern string cfun;
extern int save_cstr(string value);
extern string itos(int i);

class Node
{
public:
	
    Node(){}
	
    virtual void code()
    {
        cout<<"Node\n";
    }

    virtual int code_opt()
    {
        return 0;
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
    virtual string getRef()
    {
        return "";
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
			labelLoop = loopMax;
			cout<<"\tloop"<<labelLoop<<":"<<endl;
            labelLoop++;
            loopMax = labelLoop;
			
			instructionList.at(0)->code_opt();
			instructionList.at(1)->code();
			
			labelSkip = skipMax;
			cout<<"skip"<<labelSkip<<endl;
            labelSkip++;
            skipMax = labelSkip;

            labelLoop--;
			cout<<"\tjmp "<<"loop"<<labelLoop<<endl;
			labelSkip--;
			cout<<"\tskip"<<labelSkip<<":"<<endl;
		}//T_DOITERATION        
        else if(type==5){
			labelLoop = loopMax;
			cout<<"\tloop"<<labelLoop<<":"<<endl;
            labelLoop++;
            loopMax = labelLoop;
			
			if(instructionList.at(0)->code_opt()==-1)
                return ;
			labelSkip = skipMax;
			cout<<"skip"<<labelSkip<<endl;
            labelSkip++;
            skipMax = labelSkip;
			
			instructionList.at(1)->code();
            labelLoop--;
			cout<<"\tjmp "<<"loop"<<labelLoop<<endl;
			labelSkip--;
			cout<<"\tskip"<<labelSkip<<":"<<endl;
		}//T_WHILEITERATION         
        else if(type==6){
            instructionList.at(0)->code();
            labelLoop = loopMax;
			cout<<"\tloop"<<labelLoop<<":"<<endl;
			labelLoop++;
            loopMax = labelLoop;

			if(instructionList.at(1)->code_opt()==-1)
                return ;
            labelSkip = skipMax;
			cout<<"skip"<<labelSkip<<endl;
            labelSkip++;
            skipMax = labelSkip;
			
			instructionList.at(3)->code();
			instructionList.at(2)->code();
            labelLoop--;			
			cout<<"\tjmp "<<"loop"<<labelLoop<<endl;
			labelSkip--;
			cout<<"\tskip"<<labelSkip<<":"<<endl;
		}//T_FORITERATION          
        else if(type==7){
			if(instructionList.at(0)->code_opt()==-1)
                return ;
            labelNo = noMax;
			cout<<"no"<<labelNo<<endl;
            labelNo++;
            noMax = labelNo;

			if(instructionList.size()==3){
				instructionList.at(1)->code();

                labelOut = outMax;
				cout<<"\tjmp "<<"out"<<labelOut<<endl;
                labelOut++;
                outMax = labelOut;

                labelNo--;
                cout<<"\tno"<<labelNo<<":"<<endl;
                instructionList.at(2)->code();
                labelOut--;
                cout<<"\tout"<<labelOut<<":"<<endl;
			}else{
                instructionList.at(1)->code();
                labelNo--;
                cout<<"\tno"<<labelNo<<":"<<endl;
            }		
		}//T_SELECT
        else if(type==8){
		    
			printList();
            //cout<<instructionList.at(0)->getNodeType()<<instructionList.at(0)->getInt()<<endl;
            if(instructionList.at(0)->getInt()==1)
            {
                Node* n=instructionList.at(0);
                cout<<"\tpushl $128\n";
                cout<<"\tpushl "<<n->getRef()<<endl;
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
    virtual string getRef()
    {
        string temp="$"+itos(value);
        return temp;
    }
    virtual string getString()
    {
        string temp="";
        temp+=value;
        return temp;
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
        int num = save_cstr(this->value);
        cout<<"\tpushl $.s"<<num<<endl;
    }
    virtual int getInt()
    {
        return 1;
    }
    virtual string getNodeType()
    {
        return "NString";
    }
    virtual string getString()
    {
        return value;
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
        //cerr<<id<<" "<<offset<<endl;
        if(offset==1)
            cout<<"\tmovl "<<id<<", %eax\n";
        else if(type==T_STRING){
            cout<<"\tleal "<<this->getRef()<<", %eax"<<endl;
            cout<<"\tpushl %eax"<<endl;
        }
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
    virtual string getRef()
    {
        //cout<<"offset"<<offset<<endl;
        if(offset==1 || offset==-1)
            return id;
        else
        {
            string temp="";
            temp+=itos(offset);
            temp+="(%ebp)"; 
            return temp;
        }
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
        //cout<<"_Variable: name: "<<id<<"type: "<<( type==0 ? "int" : "string" )<<endl;
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
        cout<<"\tmovl %eax, %ebx\n\tmovl $0, %eax\n\tsubl %ebx, %eax\n";
    }
    virtual Node* getNode()
    {
        return childExp;
    }
    virtual string getNodeType()
    {
        return "NUnaryOp";
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
    bool isconst=false;
    string combine;

    NBinaryOp(binaryOP operation, Node* leftExp, Node* rightExp):
    operation(operation), leftExp(leftExp), rightExp(rightExp)
    {
        if(leftExp->getInt()==1 || rightExp->getInt()==1)
            type=T_STRING;
        if((leftExp->getNodeType()=="NString" && rightExp->getNodeType()=="NString")||
            (leftExp->getNodeType()=="NString" && rightExp->getNodeType()=="NInt")||
            (leftExp->getNodeType()=="NInt" && rightExp->getNodeType()=="NString"))
        {
            isconst=true;
            combine = leftExp->getString()+rightExp->getString();
        }
    }
    virtual void code()
    {
        if((leftExp->getNodeType()=="NString" && rightExp->getNodeType()=="NString")||
            (leftExp->getNodeType()=="NString" && rightExp->getNodeType()=="NInt")||
            (leftExp->getNodeType()=="NInt" && rightExp->getNodeType()=="NString")){
            type=T_STRING;

            if(operation==0){
                //isconst=true;
                //string combine = leftExp->getString()+rightExp->getString();
                int num = save_cstr(combine);
                cout<<"\tpushl $.s"<<num<<endl;              
            }
        }else if(leftExp->getInt()==1 || rightExp->getInt()==1){//string variable and string const                                                       //2 string variables
            type=T_STRING;


            if(leftExp->getInt()==0)
            {
                int num = save_cstr(leftExp->getString());
                cout<<"\tpushl $.s"<<num<<endl;
            }
            else
                leftExp->code();

            cout<<"\tpush $.stracc"<<endl;
            cout<<"\tcall strncpy"<<endl;
            isStrncpy = true;
            cout<<"\tmovb $0, 127(%eax)"<<endl;
            if(isStrncpy){
                cout<<"\taddl $12, %esp"<<endl;
            }
            isStrncpy = false;

            if(rightExp->getInt()==0)
            {
                int num = save_cstr(rightExp->getString());
                cout<<"\tpushl $.s"<<num<<endl;
            }
            else
                rightExp->code();

            cout<<"\tpush $.stracc"<<endl;
            cout<<"\tcall strcat"<<endl;
            isStrncat = true;
            cout<<"\tmovb $0, 127(%eax)"<<endl;
            if(isStrncat){
                cout<<"\taddl $8, %esp"<<endl;
            }
            isStrncat = false;
        }
        else if(leftExp->getNodeType()=="NInt" && rightExp->getNodeType()=="NInt")
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
                cout<<"\tmovl %ebx, %ecx\n";
                cout<<"\tsall %cl, %eax\n";
            }
            if(operation==6)
            {
                cout<<"\tmovl %ebx, %ecx\n";
                cout<<"\tsarl %cl, %eax\n";
            }
        }

    }
    virtual Node* getNode()
    {
        return leftExp;
    }
    virtual string getNodeType()
    {
        return (isconst==true ? "NString" : "NBinaryOp");
    }
    virtual int getInt()
    {
        return type;
    }
    virtual string getString()
    {
        return combine;
    }

};

#define needcpy (*it)->getNodeType()=="NString" || ((*it)->getNodeType()=="NFunctionCall" && (*it)->getInt()==1) || ((*it)->getNodeType()=="NBinaryOp" && (*it)->getInt()==1)
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
		int count=0; //number of all parameters
        int scount=0;//number of string parameters
        int snum=0; //current string parameter index
        int num=0; //current parameter index 
        for( auto it = argumentList.begin(); it != argumentList.end(); it++)
        {
            //cerr<<count<<": "<<(*it)->getNodeType()<<(*it)->getInt()<<endl;
            count++;
            if( needcpy )
            {
                scount++;
            }
        } 
        if(scount!=0)
        {
            cout<<"\tsubl $"<<scount*128<<", %esp\n";

            for( auto it = argumentList.begin(); it != argumentList.end(); it++)
            {
                if(scount==0)
                    break;
    
                if( needcpy )
                {
                    string src;
                    if((*it)->getNodeType()=="NString")
                    {
                        cout<<"\tpushl $128\n";
                        (*it)->code();
                    }
                    else if((*it)->getNodeType()=="NFunctionCall" || (*it)->getNodeType()=="NBinaryOp")
                    {
                        (*it)->code();
                        src="$.stracc";
                        cout<<"\tpushl $128\n";
                        cout<<"\tpushl "<<src<<endl;
                    }
                    cout<<"\tleal "<<8+snum*128<<"(%esp),%eax\n";
                    string dst="%eax";
                    
                    
                    cout<<"\tpushl "<<dst<<endl;
                    cout<<"\tcall strncpy\n";
                    if(dst!="%eax")
                        cout<<"\tmovl "<<dst<<", %eax\n";
                    cout<<"\tmovb $0,127(%eax)\n\taddl $12, %esp\n";
    
                    snum++;
                }
            } 
        }
        num=0;snum=0;
		for( auto it= argumentList.rbegin(); it!=argumentList.rend(); it++)
        {
            if( !(needcpy) )
            {
                if((*it)->getNodeType()=="NBinaryOp" || (*it)->getNodeType()=="NUnaryOp" ||
                    ( (*it)->getNodeType()=="NFunctionCall" && (*it)->getInt()==0 ) )
                {
                    (*it)->code();
                    cout<<"\tpushl %eax\n";
                }
                else if((*it)->getNodeType()=="NIdentifier" && (*it)->getInt()==1)
                {
                    cout<<"\tleal "<<(*it)->getRef()<<", %eax\n";
                    cout<<"\tpushl %eax\n";
                }
                else
                {
                    cout<<"\tpushl "<<(*it)->getRef()<<"\n";
                }
            }
            else
            {
                snum++;
                cout<<"\tleal "<<4*num+(scount-snum)*128<<"(%esp),%eax\n";
                cout<<"\tpush %eax\n";
            }
            num++;
            
        } 
        cout<<"\tcall "<<funcationName->getString()<<endl;
        variableType type = (variableType)funcationName->getInt();
        if(type==3)
        {
            cout<<"\tpushl $128\n";
            cout<<"\tpushl %eax\n";
            cout<<"\tpushl $.stracc\n";
            cout<<"\tcall strncpy\n";
            cout<<"\tmovl $.stracc, %eax\n";
            cout<<"\tmovb $0,127(%eax)\n\taddl $12, %esp\n";
        }
		cout<<"\taddl $"<<4*count+128*scount<<", %esp"<<endl;
    }
    virtual Node* getNode()
    {
        return funcationName;
    }
    virtual int getInt()
    {
        return funcationName->getInt()-2;
    }
    virtual string getNodeType()
    {
        return "NFunctionCall";
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
        if(id->getInt()==1){
            cout<<"\tpushl $128"<<endl;
            exp->code();

            if(exp->getNodeType()!="NString")
            {
                cout<<"\tpushl $128"<<endl;
                cout<<"\tpushl $.stracc"<<endl;
            }

            cout<<"\tleal "<<id->getRef()<<", %eax"<<endl;
            cout<<"\tpushl %eax"<<endl;
            cout<<"\tcall strncpy"<<endl;
            isStrncpy = true;
            cout<<"\tmovb $0, 127(%eax)"<<endl;
            if(isStrncpy){
               cout<<"\taddl $12, %esp"<<endl;
            }
            isStrncpy = false;
            
        }else{
            exp->code();
            int offset= id->getOffset();
            if(offset==1 || offset==-1)
                cout<<"\tmovl %eax, "<<id->getString()<<"\n";
            else
                cout<<"\tmovl %eax, "<<offset<<"(%ebp)\n";
        }
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
    virtual int code_opt()
    {	
        if(operation==0)
        {
            if(leftExp->getNodeType()=="NString" && rightExp->getNodeType()=="NString")
            {
                //cerr<<leftExp->getString()<<" vs "<<rightExp->getString()<<endl;
                if(leftExp->getString()!=rightExp->getString())
                    return -1;
            }
        }
        if(operation==5)
        {
            if(leftExp->getNodeType()=="NString" && rightExp->getNodeType()=="NString")
            {
                //cerr<<leftExp->getString()<<" vs "<<rightExp->getString()<<endl;
                if(leftExp->getString()==rightExp->getString())
                    return -1;
            }
        }
		leftExp->code();
		cout<<"\tmovl %eax, %esi"<<endl;
		
		rightExp->code();
        cout<<"\tmovl %eax, %edi"<<endl;
		
		cout<<"\tcmpl %edi, %esi"<<endl;

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

        return 0;
		
    }
};

#endif
