#include <string>
using namespace std;

int labelNo=0;
int labelOut=0;
int labelLoop=0;
int labelSkip=0;
int save_cstr(string value);

string cfun="!non!";


int cstrNo=0;
string section=".section\t.rodata\n";

int save_cstr(string value)
{
	cstrNo++;
	section+=".s";
	section+=cstrNo;
	section+=":\t.string \"";
	section+=value;
	section+="\"\n";
	return cstrNo;
}

string code_strncpy(string src, string dst)
{
	cout<<"pushl $128\n";
	cout<<"pushl "<<src<<endl;
	cout<<"pushl "<<dst<<endl;
	cout<<"call strncpy\n";
	cout<<"movl "<<dst<<", %eax\nmovb $0,127(%eax)\naddl $12, %esp\n";
	
}