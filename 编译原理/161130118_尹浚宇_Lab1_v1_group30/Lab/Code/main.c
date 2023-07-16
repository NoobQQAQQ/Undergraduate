#include <stdio.h>
#include <stdlib.h>
#include "syntax.tab.h"
#include "syntaxTree.h"

extern FILE* yyin;
extern int yylineno;
extern void yyrestart (FILE*);
extern int yyparse (void);
//extern int yydebug;

extern int nr_Error;
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

	yyrestart(f);
    yylineno = 1;
	//yydebug = 1;
	yyparse();

    if (!nr_Error)
        showTree(root, 0);

    deleteTree(root);
	return 0;
}