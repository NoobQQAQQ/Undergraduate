#include <stdio.h>
#include <stdlib.h>
#include "syntax.tab.h"
#include "syntaxTree.h"
#include "symTable.h"
#include "semantic.h"
#include "interCode.h"
#include "objectCode.h"

extern FILE* yyin;
extern int yylineno;
extern void yyrestart (FILE*);
extern int yyparse (void);
//extern int yydebug;

extern int nr_Error;
extern int s_error; // number of semantic errors
Node *root;

int main(int argc, char **argv)
{
	if (argc <= 1) return -1;

	FILE* f = fopen(argv[1], "r");
	if(!f)
    {
		perror(argv[1]);
		return -1;
	}
    root = NULL;
	
    yylineno = 1;
	yyrestart(f);
	//yydebug = 1;
	yyparse();
	
    if (!nr_Error)
	{
		//initTable();
		Program(root);
        //showTree(root, 0);
	}
	if (!s_error)
	{
		translate_Program(root);
		make_objectCodes(argv[2]);
	}

    deleteTree(root);
	return 0;
}
