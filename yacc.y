%{
#include "main.h"

extern "C"
{
	int yyerror(const char *s);
	extern int yylex(void);
}

vector< hash_map<string, int[3]>* > symbolTable;

int cheackID(string id)
{
	for(int i=symbolTable.size()-1; i>=0; i--)
	{
		hash_map<string, int[3]> table( *(symbolTable.back()) );
		if(table.find(name)!=table.end()) 
        {
        	return table[name][0];
        }
	}
	return -1;
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
%type<u_node> expression expression_additive expression_multiplicative unary_expression expression_postfixee primary_expression
%type<u_node> program external_declaration declaration function_definition assignment  cond_instruction condition
%type<u_list> argument_expression_list declarator_list instruction_list
%type<u_i> comparison_operator 
%%


program :  
external_declaration 
| program external_declaration 
;

external_declaration :  
declaration 	// Declaration Global
{
	vector<Node*> list = $1->getList();
	variableType type = $1->getInt();
	hash_map<string, int[3]> table();
	for(int i=0; i<list.size(); i++)
	{
		string name = list.at(i)->getString();
		int *ar = new int[3];
		ar[0]= type;
		ar[1]= list.at(i)->getInt();
		ar[2]= 0;

		if(table.find(name)!=table.end()) 
        	yyerror("Redeclaration!");
		else
			table[name]=ar;
	}
	symbolTable.push_back(&table);
}			
| EXTERN declaration // Set Extern attribute	
{

}		
| function_definition 
;

function_definition :  
type function_declarator decl_glb_fct compound_instruction // generate code function
  
;

decl_glb_fct :
// Get function name _ Create a spot to store the function _ set attributes
;

declaration :  
type declarator_list ';' 
{
	

}
;

type :  
INT 					// set INT
| STRING 				// set String
;

declarator_list :  
declarator 				// Propagate code
| declarator_list ',' declarator 	
;

declaration_list :  
declaration 				// Set locals
{
	vector<Node*> list = $1->getList();
	variableType type = $1->getInt();
	hash_map<string, int[3]> table = *(symbolTable.back()) );
	for(int i=0; i<list.size(); i++)
	{
		string name = list.at(i)->getString();
		int *ar = new int[3];
		ar[0]= type;
		ar[1]= list.at(i)->getInt();
		ar[2]= 0;

		if(table.find(name)!=table.end()) 
        	yyerror("Redeclaration!");
		else
			table[name]=ar;
	}
}
| declaration_list declaration  	// Set locals
;

declarator :  
IDENT 					// Create Variable
| function_declarator 		        // Create Function
;

function_declarator :  
IDENT '(' ')' 				// Create function name
| IDENT '(' parameter_list ')'  	// Create partial function 
;

parameter_list :  
parameter_declaration 			
| parameter_list ',' parameter_declaration // Insert parameters
;

parameter_declaration :  
type IDENT 				// Type declaration
;

instruction :  
';'  
| compound_instruction {$$=$1;}
| expression_instruction  {$$=$1;}
| iteration_instruction  {$$=$1;}
| select_instruction  {$$=$1;}
| jump_instruction {$$=$1;}
;

instruction1 :  
';'  
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
IDENT ASSIGN expression  { $$ = new NAssign($1, $3);}
;

compound_instruction :  
block_start declaration_list instruction_list block_end //{$$=$3;}
| block_start declaration_list block_end 
| block_start instruction_list block_end {vector<Node*>* vec = new vector<Node*>; vec->push_back($2); $$ = new NInstruction(T_COMPOUND, *vec);}
| block_start block_end  { $$ = new NInstruction(T_COMPOUND); }
;


block_start :  
'{'  // Init your hash table _ symbol table
{
	symbolTable.push_back( new hash_map<string, int[3]> );
}

;

block_end :  
'}' // Empty hash table
{
	delete( symbolTable.back() );
	symbolTable.pop_back();
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
	$$ = new NInstruction(T_FORITERATION, *vec);}
;

jump_instruction:  
RETURN expression ';' { vector<Node*>* vec = new vector<Node*>; vec->push_back($2); $$ = new NInstruction(T_JUMP, *vec); }
;

condition :  
expression comparison_operator expression { $$ = new NCondition($2, $1, $3); }
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
expression_additive { $$=$1; $$->code();}
| expression SHIFTLEFT expression_additive { $$ = new NBinaryOp(T_SHIFTLEFT, $1, $3); }
| expression SHIFTRIGHT expression_additive { $$ = new NBinaryOp(T_SHIFTRIGHT, $1, $3); }
;

expression_additive :  
expression_multiplicative { $$=$1;}
| expression_additive PLUS expression_multiplicative { $$ = new NBinaryOp(T_PLUS, $1, $3); }
| expression_additive MINUS expression_multiplicative { $$ = new NBinaryOp(T_MINUS, $1, $3); }
;

expression_multiplicative :  
unary_expression { $$=$1;}
| expression_multiplicative MULTI unary_expression { $$ = new NBinaryOp(T_MULTI, $1, $3); }
| expression_multiplicative DIV unary_expression { $$ = new NBinaryOp(T_DIV, $1, $3); }
| expression_multiplicative MODULO unary_expression { $$ = new NBinaryOp(T_MODULE, $1, $3); }
;

unary_expression:  
expression_postfixee { $$=$1;}
| MINUS unary_expression { $$=new NUnaryOp(T_NEGATIVE, $2); }
;

expression_postfixee :  
primary_expression {$$=$1;}
| IDENT '(' argument_expression_list')' { $$=new NFunctionCall($1, *$3); }
| IDENT '(' ')' { $$=new NFunctionCall($1); }
;

argument_expression_list:  
expression 	{ $$=new vector<Node*>; $$->push_back($1); }
| argument_expression_list',' expression { $1->push_back($3); $$=$1; }
;

primary_expression :  
IDENT  
{ 
	int type;
	type = cheackID($1->getString());
	if(type==-1)
		yyerror("No declaration");
	$$=$1; 
}
| CONST_INT		{ $$=$1; }
| CONST_STRING  { $$=$1; }
| '(' expression ')'	{ $$=$2; }
;



%%

int yyerror(const char* msg)
{
	cout<<"ERROR: "<<msg<<endl;
}

int main()
{
	yyparse();
}
