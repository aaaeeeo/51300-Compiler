#include <string>
#include <sstream>
#include <iostream>
using namespace std;

int labelNo = 0;
int labelOut = 0;
int labelLoop = 0;
int labelSkip = 0;
int cstrNo = -1;
extern bool isStrncpy = false;
extern bool isStrncat = false;

int save_cstr(string value);
string itos(int i);
string code_strncpy(string src, string dst);

string cfun="!non!";



string section=".section\t.rodata\n";

int save_cstr(string value)
{
	cstrNo++;
	section+=".s";
	section+=itos(cstrNo);
	section+=":\t.string ";
	section+=value;
	section+="\n";
	return cstrNo;
}


string itos(int i){ // 将int 转换成string
    stringstream s;
    s << i;
    return s.str();
}

string code_strncpy(string src, string dst)
{
	cout<<"pushl $128\n";
	cout<<"cout src\n";
	cout<<"pushl "<<src<<endl;
	cout<<"cout dst\n";
	cout<<"pushl "<<dst<<endl;
	cout<<"call strncpy\n";
	cout<<"movl "<<dst<<", %eax\nmovb $0,127(%eax)\naddl $12, %esp\n";
}