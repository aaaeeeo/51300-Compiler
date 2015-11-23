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

%option yylineno

%%

^\#[^\n]*\n 	{
	
}

IF|if		return IF;
ELSE|else	return ELSE;
WHILE|while     return WHILE;
DO|do           return DO;
FOR|for		return FOR;
RETURN|return   return RETURN;
INT|int  	return INT;
STRING|string   return STRING;
EXTERN|extern   return EXTERN;

[A-Za-z\_][A-Za-z\_0-9]*	{
if(yyleng>31)
{
	string str="identifier too long: ";
	str+=yytext;
	yyerror(str.c_str());
}
else
	{ 
		string  str(yytext); 
		NIdentifier* n = new NIdentifier(str);
		yylval.u_node=n;
		//printf("ID: %s\n", n->id.c_str());
		return IDENT;
	}
}
[0-9]+						{
	NInt* n = new NInt(atoi(yytext));
	yylval.u_node=n;
	//printf("CONST_INT: %d\n", n->value);
	return CONST_INT;
	  }

\"[^"]*\"					{
	string  str(yytext); 
	NString* n = new NString(str);
	yylval.u_node=n;
	return CONST_STRING;
}

\+	return PLUS;
\-	return MINUS;						
\*	return MULTI;
\/	return DIV;
"%"	return MODULO;				
"<<"	return SHIFTLEFT;
">>"	return SHIFTRIGHT;			
"=="	return EGAL;
"!="	return DIFF;
"<"	return INF;
">"	return SUP;
"<="	return INFEQUAL;
">="	return SUPEQUAL;
"="	return ASSIGN;					
"("	return '(';
")"	return ')';
"{"	return '{';
"}"	return '}';
"["	return '[';
"]"	return ']';
","   	return ',';
";"	return ';';



\n 		;
\t 		;
" "		;
.		{
	string str="unknown token: ";
	str+=yytext;
	yyerror(str.c_str());
}

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

