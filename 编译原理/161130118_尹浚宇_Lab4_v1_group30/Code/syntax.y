%locations
//%define parse.error verbose
%define parse.lac none
%{
	#include "lex.yy.c"
	#include <stdio.h>
	/* #define YYDEBUG 1 */
	extern void yyerror(char const* s);
    extern int yylineno;
    extern int nr_Error;
    extern Node *root;
%}

/* declared types */
%union{
	struct syntaxTreeNode* node;
	//double type_double;
}

/* declared tokens */
%token <node> INT
%token <node> FLOAT
%token <node> ID
%token <node> SEMI COMMA
%token <node> ASSIGNOP RELOP PLUS SUB STAR DIV AND OR DOT NOT MINUS
%token <node> TYPE
%token <node> LP RP LB RB LC RC
%token <node> STRUCT RETURN IF ELSE WHILE

%type <node> Program ExtDefList ExtDef ExtDecList
%type <node> Specifier StructSpecifier OptTag Tag
%type <node> VarDec  FunDec VarList ParamDec
%type <node> CompSt StmtList Stmt
%type <node> DefList Def DecList Dec
%type <node> Exp Args
%type <node> error

/*%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE*/
/*!!!!!!!unknown process!!!!!!!!*/
%nonassoc LOWER_THAN_COMMA
%nonassoc COMMA
%nonassoc LOWER_THAN_ASSIGNOP

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS SUB
%left STAR DIV
%right NOT MINUS
%left LP RP LB RB DOT

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
/* High-level Definitions */ 
Program : ExtDefList {
			$$ = createNode("Program", 1, NULL, @$.first_line);
			addChild($$,$1);
			root = $$;
		}
	;
ExtDefList : /* empty */ {
			$$ = createNode("ExtDefList", 1, NULL, -1);
		}
	|	ExtDef ExtDefList {
			$$ = createNode("ExtDefList", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
		}
	;
ExtDef : Specifier ExtDecList SEMI {
			$$ = createNode("ExtDef", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
		}
	|	Specifier SEMI {
			$$ = createNode("ExtDef", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
		}
	|	Specifier FunDec CompSt {
			$$ = createNode("ExtDef", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
		}
	|	error SEMI {
			$$ = NULL;
			//printf("*****ExtDef error*****\n");
            nr_Error++;
		}
	;
ExtDecList : VarDec {
			$$ = createNode("ExtDecList", 1, NULL, @$.first_line);
			addChild($$,$1);
		}
	|	VarDec COMMA ExtDecList {
			$$ = createNode("ExtDecList", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
		}
	;

/* Specifiers */
Specifier : TYPE {
			$$ = createNode("Specifier", 1, NULL, @$.first_line);
			addChild($$,$1);
		}
	|	StructSpecifier {
			$$ = createNode("Specifier", 1, NULL, @$.first_line);
			addChild($$,$1);
		}
	;
StructSpecifier : STRUCT OptTag LC DefList RC {
			$$ = createNode("StructSpecifier", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
			addChild($$,$4);
			addChild($$,$5);
		}
	|	STRUCT Tag {
			$$ = createNode("StructSpecifier", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
		}
	;
OptTag : /* empty */ {
			$$ = createNode("OptTag", 1, NULL, -1);
		}
	|	ID {
			$$ = createNode("OptTag", 1, NULL, @$.first_line);
			addChild($$,$1);
		}
	;
Tag : ID {
			$$ = createNode("Tag", 1, NULL, @$.first_line);
			addChild($$,$1);
		}
	;

/* Declarators */
VarDec : ID {
			$$ = createNode("VarDec", 1, NULL, @$.first_line);
			addChild($$,$1);
		}
	|	VarDec LB INT RB {
			$$ = createNode("VarDec", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
			addChild($$,$4);
		}
	|	VarDec LB error RB {
			$$ = NULL;
			nr_Error++;
		}
	;
FunDec : ID LP VarList RP {
			$$ = createNode("FunDec", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
			addChild($$,$4);
		}
	|	ID LP RP {
			$$ = createNode("FunDec", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
		}
	|	ID LP error RP {
			$$ = NULL;
			nr_Error++;
		}
	;
VarList : ParamDec COMMA VarList {
			$$ = createNode("VarList", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
		}
	|	ParamDec {
			$$ = createNode("VarList", 1, NULL, @$.first_line);
			addChild($$,$1);
		}
	;
ParamDec : Specifier VarDec {
			$$ = createNode("ParamDec", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
		}
	;

/* Statements */
CompSt : LC DefList StmtList RC {
			$$ = createNode("CompSt", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
			addChild($$,$4);
			/*printf("%s-%s-%s-%s\n",$$->child->type, $$->child->brother->type,$$->child->brother->brother->type,$$->child->brother->brother->brother->type);*/
		}
	|	LC error RC {/***********************************************************/
			$$ = NULL;
			//printf("*****CompSt error*****\n");
            nr_Error++;
		}
	;
StmtList : /* empty */ {
			$$ = createNode("StmtList", 1, NULL, -1);
		}
	|	Stmt StmtList {
			$$ = createNode("StmtList", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
		}
	;
Stmt : Exp SEMI {
			$$ = createNode("Stmt", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
		}
	|	CompSt {
			$$ = createNode("Stmt", 1, NULL, @$.first_line);
			addChild($$,$1);
		}
	|	RETURN Exp SEMI {
			$$ = createNode("Stmt", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
		}
	|	IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {
			$$ = createNode("Stmt", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
			addChild($$,$4);
			addChild($$,$5);
		}
	|	IF LP Exp RP Stmt ELSE Stmt {
			$$ = createNode("Stmt", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
			addChild($$,$4);
			addChild($$,$5);
			addChild($$,$6);
			addChild($$,$7);
		}
	|	WHILE LP Exp RP Stmt {
			$$ = createNode("Stmt", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
			addChild($$,$4);
			addChild($$,$5);
		}
	|	error SEMI {
			$$ = NULL;
			//$$ = createNode("Error", 1, NULL, @$.first_line);
			//printf("*****Stmt SEMI error*****\n");
            nr_Error++;
		}
	|	IF LP error RP Stmt %prec LOWER_THAN_ELSE {
			$$ = NULL;
			nr_Error++;
		}
	|	IF LP error RP Stmt ELSE Stmt {
			$$ = NULL;
			nr_Error++;
		}
	|	WHILE LP error RP Stmt {
			$$ = NULL;
			nr_Error++;
		}
	|	error RP {
			//$$ = createNode("Error", 1, NULL, @$.first_line);
			//printf("*****Stmt RP error*****\n");
            $$ = NULL;
			nr_Error++;
		}
	;

/* Local Definitions */
DefList : /* empty */ {
			$$ = createNode("DefList", 1, NULL, -1);
		}
	|	Def DefList {
			$$ = createNode("DefList", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
		}
	;
Def : Specifier DecList SEMI {
			$$ = createNode("Def", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
		}
	|	error SEMI {
			//$$ = createNode("Error", 1, NULL, @$.first_line);
			//printf("*****Def error*****\n");
            $$ = NULL;
			nr_Error++;
		}
	;
DecList : Dec %prec LOWER_THAN_COMMA {
			$$ = createNode("DecList", 1, NULL, @$.first_line);
			addChild($$,$1);
		}
	|	Dec COMMA DecList {
			$$ = createNode("DecList", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
		}
	;
Dec : VarDec %prec LOWER_THAN_ASSIGNOP {
			$$ = createNode("Dec", 1, NULL, @$.first_line);
			addChild($$,$1);
		}
	|	VarDec ASSIGNOP Exp {
			$$ = createNode("Dec", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
		}
	;

/* Expressions */
Exp : Exp ASSIGNOP Exp {
			$$ = createNode("Exp", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
		}
	|	Exp AND Exp {
			$$ = createNode("Exp", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
		}
	|	Exp OR Exp {
			$$ = createNode("Exp", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
		}
	|	Exp RELOP Exp {
			$$ = createNode("Exp", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
		}
	|	Exp PLUS Exp {
			$$ = createNode("Exp", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
		}
	|	Exp SUB Exp {
			$$ = createNode("Exp", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
		}
	|	Exp STAR Exp {
			$$ = createNode("Exp", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
		}
	|	Exp DIV Exp {
			$$ = createNode("Exp", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
		}
	|	LP Exp RP {
			$$ = createNode("Exp", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
		}
	|	SUB Exp %prec MINUS {
			$$ = createNode("Exp", 1, NULL, @$.first_line);
			addChild($$, $1);
			addChild($$, $2);
		}
	|	NOT Exp {
			$$ = createNode("Exp", 1, NULL, @$.first_line);
			addChild($$, $1);
			addChild($$, $2);
		}
	|	ID LP Args RP{
			$$ = createNode("Exp", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
			addChild($$,$4);
		}
	|	ID LP RP{
			$$ = createNode("Exp", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
		}
	|	Exp LB Exp RB{
			$$ = createNode("Exp", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
			addChild($$,$4);
		}
	|	Exp DOT ID {
			$$ = createNode("Exp", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
		}
	|	ID {
			$$ = createNode("Exp", 1, NULL, @$.first_line);
			addChild($$,$1);
		}
	|	INT {
			$$ = createNode("Exp", 1, NULL, @$.first_line);
			addChild($$,$1);
		}
	|	FLOAT {
			$$ = createNode("Exp", 1, NULL, @$.first_line);
			addChild($$,$1);
		}
	|	LP error RP {
			$$ = NULL;
			nr_Error++;
		}
	|	ID LP error RP {
			$$ = NULL;
			nr_Error++;
		}
	|	Exp LB error RB {
			$$ = NULL;
			nr_Error++;
		}
	|	error RP {
			//$$ = NULL;
			//printf("*****Exp RP error*****\n");
            $$ = NULL;
			nr_Error++;
		}
	
	;
Args : Exp COMMA Args {
			$$ = createNode("Args", 1, NULL, @$.first_line);
			addChild($$,$1);
			addChild($$,$2);
			addChild($$,$3);
		}
	|	Exp {
			$$ = createNode("Args", 1, NULL, @$.first_line);
			addChild($$,$1);
		}
	;
%%
void yyerror(char const* s){
	fprintf(stderr,"Error type B at Line %d: %s.\n", yylineno, s);
}