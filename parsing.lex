%{
#include <stdio.h>

extern "C"
{
	int yywrap(void);
	int yylex(void);
	extern int yyerror(const char *s);
}

int parsing(char* name);

%}


%%

IF|ELSE|WHILE|DO|FOR|RETURN	printf("%s:\tCONTROL\n",yytext);
INT|STRING|EXTERN			printf("%s:\tTYPE\n",yytext);

[A-Za-z\_][A-Za-z\_0-9]*	{
if(yyleng>31)
	yyerror(yytext);
else
	printf("%s:\tIDENT\n",yytext);
}
[0-9]+						printf("%s:\tCONST-INT\n",yytext);

\"[^"]*\"					printf("%s:\tCONST-STRING\n",yytext);
\+|\-						printf("%s:\tADD_OP\n",yytext);
\*|\/|\%					printf("%s:\tMULT_EXP\n",yytext);
"<<"|">>"					printf("%s:\tEXP\n",yytext);
"=="|"!="|"<"|">"|"<="|">="	printf("%s:\tCMP_OP\n",yytext);
"="							printf("%s:\tASMENT\n",yytext);
"("|")"|"{"|"}"|"["|"]"|","|";"	printf("%s:\tPUNCT\n",yytext);



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

