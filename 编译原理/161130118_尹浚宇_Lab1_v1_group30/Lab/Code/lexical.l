%option yylineno
%{
	#include "syntax.tab.h"
	#include "syntaxTree.h"
	int yycolumn = 1;
	int nr_Error = 0;
	#define YY_USER_ACTION \
		yylloc.first_line = yylloc.last_line = yylineno; \
		yylloc.first_column = yycolumn; \
		yylloc.last_column = yycolumn + yyleng - 1; \
		yycolumn += yyleng;
%}

digit [0-9]
letter [_a-zA-Z]
int (0|[1-9][0-9]*)
float ([0-9]+\.[0-9]+)
id [_a-zA-Z][_a-zA-Z0-9]*
blank [ \t\r\v\f]+
semi ;
comma ,
assignop =
relop >|<|>=|<=|==|!=
plus \+
sub \-
star \*
div \/
and &&
or \|\|
dot \.
not !
type int|float
lp \(
rp \)
lb \[
rb \]
lc \{
rc \}
struct struct
return return
if if
else else
while while
comment "//"(.*)[$\n<<EOF>>]|"/*"([^\*]|(\*)*[^\*\/])*(\*)*"*/"

%%
{comment} {
	/* printf("comments will be ignored\n"); */
	}
{struct} {
	/*printf("**%s**\n",yytext); */
	yylval.node = createNode("STRUCT",0,NULL,yylineno);
	return STRUCT;
	}
{return} {
	/* printf("return"); */
	yylval.node = createNode("RETURN",0,NULL,yylineno);
	return RETURN;
	}
{if} {
	/* printf("if"); */
	yylval.node = createNode("IF",0,NULL,yylineno);
	return IF;
	}
{else} {
	/* printf("else"); */
	yylval.node = createNode("ELSE",0,NULL,yylineno);
	return ELSE;
	}
{while} {
	/* printf("while"); */
	yylval.node = createNode("WHILE",0,NULL,yylineno);
	return WHILE;
	}
{type} {
	/* printf("**TYPE: %s**\n",yytext); */
	Info info;
	strcpy(info.name,yytext);
	yylval.node = createNode("TYPE",0,&info,yylineno);
	return TYPE;
	}
{lp} {
	/* printf("**%s**\n",yytext); */
	yylval.node = createNode("LP",0,NULL,yylineno);
	return LP;
	}
{rp} {
	/* printf("**%s**\n",yytext); */
	yylval.node = createNode("RP",0,NULL,yylineno);
	return RP;
	}
{lb} {
	/* printf("**%s**\n",yytext); */
	yylval.node = createNode("LB",0,NULL,yylineno);
	return LB;
	}
{rb} {
	/* printf("**%s**\n",yytext); */
	yylval.node = createNode("RB",0,NULL,yylineno);
	return RB;
	}
{lc} {
	/* printf("**%s**\n",yytext); */
	yylval.node = createNode("LC",0,NULL,yylineno);
	return LC;
	}
{rc} {
	/* printf("**%s**\n",yytext); */
	yylval.node = createNode("RC",0,NULL,yylineno);
	return RC;
	}
{assignop} {
	/* printf("**%s**\n",yytext); */
	yylval.node = createNode("ASSIGNOP",0,NULL,yylineno);
	return ASSIGNOP;
	}
{relop} {
	/* printf("**%s**\n",yytext); */
	Info info;
	strcpy(info.name,yytext);
	yylval.node = createNode("RELOP",0,&info,yylineno); 
	return RELOP;
	}
{plus} {
	/* printf("+"); */
	yylval.node = createNode("PLUS",0,NULL,yylineno);
	return PLUS;
	}
{sub} {
	/* printf("**%s**\n",yytext); */
	yylval.node = createNode("MINUS",0,NULL,yylineno);
	return SUB;
	}
{star} {
	/* printf("**%s**\n",yytext); */
	yylval.node = createNode("STAR",0,NULL,yylineno);
	return STAR;
	}
{div} {
	/* printf("**%s**\n",yytext); */
	yylval.node = createNode("DIV",0,NULL,yylineno);
	return DIV;
	}
{and} {
	/* printf("**%s**\n",yytext); */
	yylval.node = createNode("AND",0,NULL,yylineno);
	return AND;
	}
{or} {
	/* printf("**%s**\n",yytext); */
	yylval.node = createNode("OR",0,NULL,yylineno);
	return OR;
	}
{dot} {
	/* printf("**%s**\n",yytext); */
	yylval.node = createNode("DOT",0,NULL,yylineno);
	return DOT;
	}
{not} {
	/* printf("**%s**\n",yytext); */
	yylval.node = createNode("NOT",0,NULL,yylineno);
	return NOT;
	}
{semi} {
	/* printf("**%s**\n",yytext); */
	yylval.node = createNode("SEMI",0,NULL,yylineno);
	return SEMI;
	}
{comma} {
	/* printf("**%s**\n",yytext); */
	yylval.node = createNode("COMMA",0,NULL,yylineno);
	return COMMA;
	}
{id} {
	/* printf("**id: %s**\n",yytext); */
	Info info;
	strcpy(info.name, yytext);
	yylval.node = createNode("ID",0,&info,yylineno);
	return ID;
	}
{int} {
	/* printf("int:%d",atoi(yytext)); */
	Info info;
	info.intVal = atoi(yytext);
	yylval.node = createNode("INT",0,&info,yylineno);
	return INT;
	}
{float} {
	/* printf("float:%f",atof(yytext)); */
	Info info;
	info.floatVal = (float)(atof(yytext)); 
	yylval.node = createNode("FLOAT",0,&info,yylineno);
	return FLOAT;
	}
\n {
	/* printf("\n"); */
	yycolumn = 1;
	}
{blank} { /* printf("WhiteSpace\n"); */ }
.   {
	//exit(0);
    nr_Error++;
	fprintf(stderr,"Error type A at Line %d: Mysterious character \"%s\".\n", yylineno, yytext);
	}
%%