%{

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX 1000
#define OUTFILE "code"

int preprocessing(char * name);
char df[MAX][MAX];
char rp[MAX][MAX];
int count;
int i,j,flag;


///////C string replace function found on the internet///////////
int ReplaceStr(char *sSrc, char *sMatchStr, char *sReplaceStr)
{
        int  StringLen;
        char caNewString[MAX];
        char *FindPos = strstr(sSrc, sMatchStr);
        if( (!FindPos) || (!sMatchStr) )
                return -1;
        while( FindPos )
        {
                memset(caNewString, 0, sizeof(caNewString));
                StringLen = FindPos - sSrc;
                strncpy(caNewString, sSrc, StringLen);
                strcat(caNewString, sReplaceStr);
                strcat(caNewString, FindPos + strlen(sMatchStr));
                strcpy(sSrc, caNewString);
                FindPos = strstr(sSrc, sMatchStr);

        }
        return 0;
}
////////////////////////////////////////////////////////////////

%}

spaces (" ")+
identifier [a-zA-z/_][a-zA-z0-9/_]*
text [^" "\n\t][^\n]*
parameter [[a-zA-z0-9/_]+
def "#define"

%start DEFINE DEFINE2 DEFINE3 COMMENT


%%

"//"[^\n]*	;

"/*"					{ BEGIN COMMENT;}
<COMMENT>[^*\n]*		;
<COMMENT>"*"+[^/\n]*	;
<COMMENT>"*"+"/"		{BEGIN 0; }	
<COMMENT>\n 			fprintf(yyout, "\n");


{def}{spaces}			{ BEGIN DEFINE; }
<DEFINE>{identifier}	{
	strcpy(df[count], yytext) ;
	BEGIN DEFINE2;
}
<DEFINE2>{spaces}		{ BEGIN DEFINE3; }
<DEFINE3>{text}			{
	strcpy(rp[count], yytext) ;
	count++;
	BEGIN 0;
}
{identifier}			{
	flag=0;
	for(i=0; i<count; i++)
	{
		if(strcmp(yytext, df[i])==0) {
    		fprintf(yyout,"%s",rp[i]);
    		flag=1;
    		break;
    	}
    }
    if(flag==0) ECHO;
}



%%

int preprocessing(char * name)
{
	count=0;
	yyin=fopen(name,"r");
	if(!yyin)	return 1;
	yyout=fopen(OUTFILE, "w");
	yylex();
	fclose(yyin);
	fclose(yyout);
	return 0;
}

int main()
{
	count=0;

	yyout=fopen(OUTFILE, "w");
	yylex();
	fclose(yyout);
	return 0;
}