%{
#include <stdio.h>
#include "main.h"
#include "yacc.tab.h"

extern "C"
{
	int yywrap(void);
	int yylex(void);
	extern int yyerror(const char *s);
}

int parsing(char* name);

%}


%%

IF|ELSE|WHILE|DO|FOR|RETURN	return *yytext;
INT  return INT;
STRING|EXTERN			return *yytext;

[A-Za-z\_][A-Za-z\_0-9]*	{
if(yyleng>31)
	yyerror(yytext);
else
	{ 
		string  str(yytext); 
		NIdentifier* n = new NIdentifier();
		n->id = str;
		yylval=n;
		return IDENT;
	}
}
[0-9]+						{
	NInt* n = new NInt();
	n->value = atoi(yytext);
	yylval=n;
	printf("CONST_INT: %d\n", n->value);
	return CONST_INT;
	  }

\"[^"]*\"					{
	string  str(yytext); 
	NString* n = new NString();
	n->value = str;
	yylval=n;
	return CONST_STRING;
}

\+|\-						printf("%s:\tADD_OP\n",yytext);
\*|\/|\%					printf("%s:\tMULT_EXP\n",yytext);
"<<"|">>"					printf("%s:\tEXP\n",yytext);
"=="|"!="|"<"|">"|"<="|">="	printf("%s:\tCMP_OP\n",yytext);
"="							printf("%s:\tASMENT\n",yytext);
"("|")"|"{"|"}"|"["|"]"|","   ;
";"	return ';';



\n 		;
" "		;
.		yyerror(yytext);

%%

int parsing(char* name)
{
	yyin = fopen(name,"r");
	if(!yyin)	return 1;
	printf("Lex\tToken\n----------------------------\n");
	yylex();
	fclose(yyin);
	return 0;
}

int yywrap(void)
{

}

