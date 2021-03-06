%{
#include "main.h"
using namespace std;



extern "C"
{
	int yyerror(const char *s);
	extern int yylex(void);
	extern int yylineno;
}
extern string section;

// +offset.5 for int para
// -offset.5 for string para
// +offset for int
// -offset for string
// +1 for global int
// -1 for global string
// 2 for int function 
// 3 for string function
vector< unordered_map<string, double>* > symbolTable;
unordered_map<string, double>* temp_tb;
Node* root;
int coffset=0;
double poffset=0.5;
bool err=false;


double check_str(string id)
{
	for(int i=symbolTable.size()-1; i>=0; i--)
	{
		unordered_map<string, double>* table = symbolTable.at(i) ;
		if(table->find(id)!=table->end()) 
        {
        	return (*table)[id];
        }
        //cout<<"not found in "<<i<<endl;
	}
	return -2;
}

void check_ID(Node* node)
{
	double type;
	type = check_str(node->getString());
	if(type==-2)
	{
		string temp;
		temp = "No declaration for " + node->getString();
		yyerror(temp.c_str());
	}
	else
	{
		if(type==2||type==3)
			node->setInt(type);
		else
		{
			if(type>0)
				node->setInt(0);
			else
				node->setInt(1);

			if(type<0)
				type=-type;
			if((int)(type*10)%10 == 0)
				type=-type;
			else
				type+=8;
			node->setOffset(type);
		}
	}
}

void save_symbol(unordered_map<string, double>* table, string name, int type)
{
	string temp = "Redeclaration for " + name;
	if(table->find(name)!=table->end()) 
        yyerror(temp.c_str());
	else
	{
		if(type==0)
		{
			coffset+=4;
			(*table)[name] = coffset;
		}
		else if(type==1)
		{
			coffset+=128;
			(*table)[name] = -coffset;
		}
		else
			(*table)[name] = type;
	}
}

void save_symbol_gb(unordered_map<string, double>* table, string name, int type)
{
	string temp = "Redeclaration for " + name;
	if(table->find(name)!=table->end()) 
        yyerror(temp.c_str());
	else
	{
		if(type==0)
		{
			(*table)[name] = +1;
		}
		else if(type==1)
		{
			coffset+=128;
			(*table)[name] = -1;
		}
		else
			(*table)[name] = type;
	}
}

void save_symbol_pa(unordered_map<string, double>* table, string name, int type)
{
	string temp = "Redeclaration for " + name;
	if(table->find(name)!=table->end()) 
        yyerror(temp.c_str());
	else
	{
		if(type==0)
		{
			(*table)[name] = poffset;
			poffset+=4;
			//(*table)["0"] = size;
			
		}
		else if(type==1)
		{
			(*table)[name] = -poffset;
			//cout<<"save str pa: "<<size<<endl; 
			poffset+=128;
			//(*table)["0"] = size;
		}
		else
			(*table)[name] = type;
	}
}

void init_table(unordered_map<string, double>* table)
{
	//(*table)["0"]=0;
}

void print_table( unordered_map<string, double>* tb)
{
	//cout<<"------------Symbol Table--------------"<<endl;
	for ( auto it = tb->begin(); it != tb->end(); it++ )
    {	cerr << it->first << "\t" ;
    	double type= it->second;
    	cerr<<type<<endl;
    }
}

void code_table_var( unordered_map<string, double>* tb)
{
	//cout<<"------------Symbol Table--------------"<<endl;
	for ( auto it = tb->begin(); it != tb->end(); it++ )
    {	if(it->first!="0")
		{
    	int type= it->second;
    	if(type==2||type==3)
    		;
    	else if(type>0)
    		cout<<"\t.comm "<<it->first<<",4,4\n";
    	else if(type<0)
    		cout<<"\t.comm "<<it->first<<",128,4\n";
    	}
    }
}

void check_type( Node* a, Node *b)
{
	/*
	while( dynamic_cast <NIdentifier*> (a) == NULL && 
		dynamic_cast <NInt*> (a) == NULL && 
		dynamic_cast <NString*> (a) == NULL)
		a=a->getNode();
	while( dynamic_cast <NIdentifier*> (b) == NULL && 
		dynamic_cast <NInt*> (b) == NULL && 
		dynamic_cast <NString*> (b) == NULL)
		b=b->getNode();
	*/
	//cerr<<a->getNodeType()<<a->getInt()<<"   "<<b->getNodeType()<<b->getInt()<<endl;
	if(a->getInt() != b->getInt())
	{
		string str="Type dismatch with " ;
		str+=(a->getInt()==0 ? "int" : "string");
		str+=" and ";
		str+=(b->getInt()==0 ? "int" : "string");
		yyerror(str.c_str());
	}
}

void save_offset(Node* ins,unordered_map<string, double>* table)
{
	//cout<<"!!!!"<<(*table)["0"]<<endl<<endl;
	ins->setOffset(coffset);
}

%}

%token<u_i> INT
%token<u_i> STRING
%token<u_node> CONST_INT
%token<u_node> CONST_STRING

%token<u_node> IDENT

%token EXTERN

%token IF
%token ELSE
%token WHILE
%token DO
%token RETURN
%token FOR
%token THEN

%token ASSIGN
%token<u_i> INF
%token<u_i> EGAL
%token<u_i> SUP
%token<u_i> INFEQUAL
%token<u_i> SUPEQUAL
%token<u_i> DIFF

%token<u_i> PLUS
%token<u_i> MINUS
%token<u_i> MULTI
%token<u_i> DIV

%token SHIFTRIGHT
%token SHIFTLEFT
%token<u_i> MODULO

%union {
    int u_i;
    Node *u_node;
    vector<Node*>* u_list;
}

%type<u_node> instruction instruction1 compound_instruction iteration_instruction iteration_instruction1 expression_instruction select_instruction select_instruction1 jump_instruction
%type<u_node> expression expression_additive expression_multiplicative unary_expression expression_postfixee primary_expression declarator function_declarator parameter_declaration
%type<u_node> program external_declaration declaration function_definition assignment  cond_instruction condition
%type<u_list> argument_expression_list declarator_list instruction_list parameter_list declaration_list
%type<u_i> comparison_operator type
%%


program :  
external_declaration { vector<Node*>* vec = new vector<Node*>; vec->push_back($1); $$ = new NInstruction(T_PROGRAM, *vec); root = $$; }
| program external_declaration {( (NInstruction*)$1)->instructionList.push_back($2); }
;


external_declaration :  
declaration 	// Declaration Global
{
	$$ = $1;

	vector<Node*> list = $1->getList();
	int type = $1->getInt();
	unordered_map<string, double>* table = symbolTable.at(1);
	for(int i=0; i<list.size(); i++)
	{
		string name = list.at(i)->getString();
		
		if(list.at(i)->getInt()==1)
		{
			type+=2;
		}
		//cout<<"GLB ID:"<<name<<","<<type<<endl;
		save_symbol_gb(table,name,type);
	}
}			
| EXTERN declaration // Set Extern attribute	
{
	$$ = $2;

	vector<Node*> list = $2->getList();
	int type = $2->getInt();
	unordered_map<string, double>* table = symbolTable.at(0);
	for(int i=0; i<list.size(); i++)
	{
		string name = list.at(i)->getString();
		
		if(list.at(i)->getInt()==1)
		{
			type+=2;
		}
		//cout<<"extern ID:"<<name<<","<<type<<endl;
		save_symbol_gb(table,name,type);
	}
}		
| function_definition { 
	$$ = $1; 

	//cerr<<endl<<"-------Extern Symbol Table--------"<<endl;
	//print_table(symbolTable.at(0));
	//cerr<<endl<<"-------Global Symbol Table--------"<<endl;
	//print_table(symbolTable.at(1));
	 }
;

function_definition :  
type function_declarator {
	//store function
	unordered_map<string, double>* table = symbolTable.back();
	string funname = $2->getString();
	(*table)[funname]=($1+2);

	coffset=0;

	//store parameters
	temp_tb = new unordered_map<string,double>;
	vector<Node*> list = $2->getList();

	poffset=0.5;
	for(int i=0; i<list.size(); i++)
	{
		string name = list.at(i)->getString();
		int type = list.at(i)->getInt();
		
		//cout<<"para ID:"<<name<<","<<type<<endl;
		save_symbol_pa(temp_tb,name,type);
		
	} 
}
	compound_instruction { 
	vector<Node*>* vec1 = new vector<Node*>; 
	vec1->push_back($2); 	
	Node* dec = new NDeclaration(variableType($1), *vec1);
	vector<Node*>* vec2 = new vector<Node*>;
	vec2->push_back(dec);
	vec2->push_back($4);
	$$ = new NInstruction(T_FUNCTION, *vec2); 


	}// generate code function
  
;

/*
decl_glb_fct :
// Get function name _ Create a spot to store the function _ set attributes
{
	

}
;*/

declaration :  
type declarator_list ';' { $$ = new NDeclaration(variableType($1), *$2); }
;

type :  
INT  { $$ = T_INT;} 					// set INT
| STRING  { $$ = T_STRING; }				// set String
;

declarator_list :  
declarator { $$ = new vector<Node*>; $$->push_back($1);}				// Propagate code
| declarator_list ',' declarator { $1->push_back($3); $$ = $1; }	
;

declaration_list :  
declaration 				// Set locals
{
	$$ = new vector<Node*>; $$->push_back($1);	
	
	vector<Node*> list = $1->getList();
	int type = $1->getInt();
	unordered_map<string, double>* table = symbolTable.back();
	for(int i=0; i<list.size(); i++)
	{
		string name = list.at(i)->getString();
		
		if(list.at(i)->getInt()==1)
		{
			type+=2;
		}
		//cout<<"LOCAL ID:"<<name<<","<<type<<endl;
		save_symbol(table,name,type);
	}
}
| declaration_list declaration  	// Set locals
{
	$1->push_back($2); $$ = $1;

	vector<Node*> list = $2->getList();
	int type = $2->getInt();
	unordered_map<string, double>* table = symbolTable.back();
	for(int i=0; i<list.size(); i++)
	{
		string name = list.at(i)->getString();
		
		if(list.at(i)->getInt()==1)
		{
			type+=2;
		}
		//cout<<"LOCAL ID:"<<name<<","<<type<<endl;
		save_symbol(table,name,type);
	}
}
;

declarator :  
IDENT  { $$ = new NVarDeclaration($1, 0);}					// Create Variable
| function_declarator { $$ = $1; }		        // Create Function
;

function_declarator :  
IDENT '(' ')' { $$ = new NVarDeclaration($1, 1); }				// Create function name
| IDENT '(' parameter_list ')' {$$ = new NVarDeclaration($1, 1, *$3); } 	// Create partial function 
;

parameter_list :  
parameter_declaration { vector<Node*>* vec = new vector<Node*>; vec->push_back($1); $$ = vec; }			
| parameter_list ',' parameter_declaration { $1->push_back($3); $$ = $1; }// Insert parameters
;

parameter_declaration :  
type IDENT   { $$ = new _Variable( variableType($1), $2); }// Type declaration
;

instruction :  
';' {}  
| compound_instruction {$$=$1;}
| expression_instruction  {$$=$1;}
| iteration_instruction  {$$=$1;}
| select_instruction  {$$=$1;}
| jump_instruction {$$=$1;}
;

instruction1 :  
';' {}
| compound_instruction {$$=$1;}
| expression_instruction  {$$=$1;}
| iteration_instruction1  {$$=$1;}
| select_instruction1  {$$=$1;}
| jump_instruction {$$=$1;}
;

expression_instruction :              
expression ';' { vector<Node*>* vec = new vector<Node*>; vec->push_back($1); $$ = new NInstruction(T_EXPRESSION, *vec); }  
| assignment ';' { vector<Node*>* vec = new vector<Node*>; vec->push_back($1); $$ = new NInstruction(T_EXPRESSION, *vec); }
;

assignment :  
IDENT ASSIGN expression  { 
	$$ = new NAssign($1, $3); 

	check_ID($1);

	check_type($1, $3);
}
;

compound_instruction :  
block_start declaration_list instruction_list block_end { 
	vector<Node*>* vec = new vector<Node*>; 
	vec->insert(vec->end(), $2->begin(), $2->end());
	vec->insert(vec->end(), $3->begin(), $3->end());
	$$ = new NInstruction(T_COMPOUND, *vec); 
	save_offset($$,symbolTable.back());
	delete( symbolTable.back() );
	symbolTable.pop_back();
	}//{$$=$3;}
| block_start declaration_list block_end { $$ = new NInstruction(T_COMPOUND, *$2);
	save_offset($$,symbolTable.back());
	delete( symbolTable.back() );
	symbolTable.pop_back();
}
| block_start instruction_list block_end { $$ = new NInstruction(T_COMPOUND, *$2);
	save_offset($$,symbolTable.back());
	delete( symbolTable.back() );
	symbolTable.pop_back();
}
| block_start block_end  { $$ = new NInstruction(T_COMPOUND); 
	save_offset($$,symbolTable.back());
	delete( symbolTable.back() );
	symbolTable.pop_back();
}
;


block_start :  
'{'  // Init your hash table _ symbol table
{
	unordered_map<string, double>* table = new unordered_map<string, double>;
	init_table(table);


	if(temp_tb!=NULL && temp_tb->size()>0)
	{
		for ( auto it = temp_tb->begin(); it != temp_tb->end(); it++ )
    	{
    		(*table)[it->first] = it->second;	
    	}
	}
	

	symbolTable.push_back( table );

	if(temp_tb!=NULL)
		temp_tb->clear();

}

;

block_end :  
'}' // Empty hash table
{
	//cerr<<endl<<"-------Local Symbol Table--------"<<endl;
	//print_table(symbolTable.back());
}
;

instruction_list :  
instruction  { $$=new vector<Node*>; $$->push_back($1); }
| instruction_list instruction  { $1->push_back($2); $$ = $1; }
;

select_instruction :  
cond_instruction instruction { 
	vector<Node*>* vec = new vector<Node*>;
	vec->push_back($1);
	vec->push_back($2);
	$$ = new NInstruction(T_SELECT, *vec); }
| cond_instruction instruction1 ELSE instruction { 
	vector<Node*>* vec = new vector<Node*>;
	vec->push_back($1);
	vec->push_back($2);
	vec->push_back($4);
	$$ = new NInstruction(T_SELECT, *vec); }
;

select_instruction1 :   
cond_instruction instruction1 ELSE instruction1 { 
	vector<Node*>* vec = new vector<Node*>;
	vec->push_back($1);
	vec->push_back($2);
	vec->push_back($4);
	$$ = new NInstruction(T_SELECT, *vec); }
;

cond_instruction :  
IF '(' condition ')' {$$=$3;} 
;

iteration_instruction :  
WHILE '(' condition ')' instruction {
	vector<Node*>* vec = new vector<Node*>; 
	vec->push_back($3); 
	vec->push_back($5); 
    $$ = new NInstruction(T_WHILEITERATION, *vec); }
| DO instruction WHILE '(' condition ')' { 
	vector<Node*>* vec = new vector<Node*>; 
	vec->push_back($2); 
	vec->push_back($5);
	$$ = new NInstruction(T_DOITERATION, *vec); }
| FOR '(' assignment ';' condition ';' assignment ')' instruction { 
	vector<Node*>* vec = new vector<Node*>; 
	vec->push_back($3); 
	vec->push_back($5); 
	vec->push_back($7); 
	vec->push_back($9);
	$$ = new NInstruction(T_FORITERATION, *vec);}
;

iteration_instruction1 :  
WHILE '(' condition ')' instruction1  {
	vector<Node*>* vec = new vector<Node*>; 
	vec->push_back($3); 
	vec->push_back($5); 
        $$ = new NInstruction(T_WHILEITERATION, *vec); }
| DO instruction WHILE '(' condition ')'  { 
	vector<Node*>* vec = new vector<Node*>; 
	vec->push_back($2); 
	vec->push_back($5);
	$$ = new NInstruction(T_DOITERATION, *vec); }
| FOR '(' assignment ';' condition ';' assignment ')' instruction1  { 
	vector<Node*>* vec = new vector<Node*>; 
	vec->push_back($3); 
	vec->push_back($5); 
	vec->push_back($7); 
	vec->push_back($9);
	$$ = new NInstruction(T_FORITERATION, *vec);}
;

jump_instruction:  
RETURN expression ';' { vector<Node*>* vec = new vector<Node*>; vec->push_back($2); $$ = new NInstruction(T_JUMP, *vec); }
;

condition :  
expression comparison_operator expression { $$ = new NCondition( comparisonOP($2), $1, $3); }
;

comparison_operator :  
EGAL   { $$ = T_EGAL;}
| DIFF { $$ = T_DIFF;} 
| INF  { $$ = T_INF;}
| SUP  { $$ = T_SUP;}
| INFEQUAL { $$ = T_INFEQUAL;}
| SUPEQUAL { $$ = T_SUPEQUAL;} 
;

expression :  
expression_additive { 
	$$=$1; 
	//$$->code();
}
| expression SHIFTLEFT expression_additive { $$ = new NBinaryOp(T_SHIFTLEFT, $1, $3); check_type($1,$3);}
| expression SHIFTRIGHT expression_additive { $$ = new NBinaryOp(T_SHIFTRIGHT, $1, $3); check_type($1,$3);}
;

expression_additive :  
expression_multiplicative { $$=$1;}
| expression_additive PLUS expression_multiplicative { $$ = new NBinaryOp(T_PLUS, $1, $3); }
| expression_additive MINUS expression_multiplicative { $$ = new NBinaryOp(T_MINUS, $1, $3); check_type($1,$3);}
;

expression_multiplicative :  
unary_expression { $$=$1;}
| expression_multiplicative MULTI unary_expression { $$ = new NBinaryOp(T_MULTI, $1, $3); check_type($1,$3);}
| expression_multiplicative DIV unary_expression { $$ = new NBinaryOp(T_DIV, $1, $3); check_type($1,$3);}
| expression_multiplicative MODULO unary_expression { $$ = new NBinaryOp(T_MODULE, $1, $3); check_type($1,$3);}
;

unary_expression:  
expression_postfixee { $$=$1;}
| MINUS unary_expression { $$=new NUnaryOp(T_NEGATIVE, $2); }
;

expression_postfixee :  
primary_expression {$$=$1;}
| IDENT '(' argument_expression_list')' { 
	$$=new NFunctionCall($1, *$3); 

	check_ID($1);
}
| IDENT '(' ')' { 
	$$=new NFunctionCall($1); 

	check_ID($1);
}
;

argument_expression_list:  
expression 	{ $$=new vector<Node*>; $$->push_back($1); }
| argument_expression_list',' expression { $1->push_back($3); $$=$1; }
;

primary_expression :  
IDENT  
{ 
	check_ID($1);
	
	$$=$1; 
}
| CONST_INT		{ $$=$1; }
| CONST_STRING  { 
	$$=$1; 
	string value= $$->getString();

}
| '(' expression ')'	{ $$=$2; }
;



%%

int yyerror(const char* msg)
{
	fprintf(stderr,"\033[31m\033[47mERROR:\033[0m \033[31mline %d: %s\033[0m\n",yylineno,msg);
	//cerr<<"ERROR: line "<<yylineno<<": "<<msg<<endl;
	err=true;
}

int main()
{
	fprintf(stderr,"\033[2J\033[35m\n------------------------------COMPILER------------------------------\n  \
		   Author: Zuoming Li, Xueyin Wang\n\n\033[0m");
	fprintf(stderr, "\033[34mParsing...\033[0m\n");
	unordered_map<string, double>* table_glb = new unordered_map<string, double>;
	unordered_map<string, double>* table_ertern = new unordered_map<string, double>;
	init_table(table_glb);
	init_table(table_ertern);
	symbolTable.push_back(table_ertern);
	symbolTable.push_back(table_glb);
 
	if(yyparse()==0)
	{
		fprintf(stderr, "\n\033[34mGenerating code...\033[0m\n");

		root->code();
		code_table_var(symbolTable.at(0));
		code_table_var(symbolTable.at(1));
		cout<<"\n\t.comm .stracc,256,4\n\n";
		cout<<section;

		if(!err)
			fprintf(stderr, "\033[32mCode generated without error.\033[0m\n");
		else
			fprintf(stderr, "\033[31mCode generated with error.\033[0m\n");
	}
	fprintf(stderr,"\033[35m\n--------------------------------------------------------------------\n\033[0m");

}
