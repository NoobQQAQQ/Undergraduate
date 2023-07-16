#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "syntaxTree.h"
//#include "syntax.tab.h"

Node* createNode(char* type, int isSyntax, Info *info, int lineno)
{
	Node* node = (Node*)malloc(sizeof(Node));
	node->child = NULL;
	node->brother = NULL;
	node->lineno = lineno;
	node->isSyntax = isSyntax;
	//node->isRight = 1;
	strcpy(node->type, type);

	if (strcmp(type, "ID") == 0 || strcmp(type, "TYPE") == 0 || strcmp(type,"RELOP") == 0)
		strcpy(node->info.name, info->name);
	else if (strcmp(type, "INT") == 0)
		node->info.intVal = info->intVal;
	else if (strcmp(type, "FLOAT") == 0)
		node->info.floatVal = info->floatVal;

	return node;
}

void addChild(Node* root, Node* child)
{
	if (root == NULL)
		return;
	if (child == NULL)
		return;
	
	if (root->child == NULL)
		root->child = child;
	else
	{
		Node* p = root->child;
		while(p->brother != NULL)
			p = p->brother;
		p->brother = child;
	}
	
	//if (strcmp(child->type, "Error") == 0)
	//	strcpy(root->type, "Error");
}

void addBrother(Node* root, Node* brother)
{
	Node* p = root;
	while (p->brother != NULL)
		p = p->brother;
	p->brother = brother;
}

void showTree(Node* root, int height)
{
	int i = 0; 
	if (root == NULL)
		return;
	//if(strcmp(root->type, "Error") == 0)
	//	return;
	// 统一处理缩进
	/*for(int i = 0; i < height; i++)
			printf("  ");

	if (root->isSyntax == 1 && root->lineno > -1)
		printf("%s (%d)\n", root->type, root->lineno);
	else if (root->isSyntax == 0)
	{
		printf("%s", root->type);

		if (strcmp(root->type, "ID") == 0)
			printf(": %s", root->info.name);
		else if(strcmp(root->type, "TYPE") == 0)
			printf(": %s", root->info.name);
		else if(strcmp(root->type, "INT") == 0)
			printf(": %d", root->info.intVal);
		else if(strcmp(root->type, "FLOAT") == 0)
			printf(": %f", root->info.floatVal);
		
		printf("\n");
	}*/
	
	if(root->isSyntax == 1 && root->lineno > -1){
		for(i = 0; i < height; i++){
			printf("  ");
		}
		printf("%s (%d)\n",root->type, root->lineno);
	}
	else if(root->isSyntax == 0){
		for(i = 0; i < height; i++){
			printf("  ");
		}
		printf("%s",root->type);
		if(strcmp(root->type,"ID") == 0){
			printf(": %s",root->info.name);
		}
		else if(strcmp(root->type,"TYPE") == 0){
			printf(": %s",root->info.name);
		}
		else if(strcmp(root->type,"INT") == 0){
			printf(": %d",root->info.intVal);
		}
		else if(strcmp(root->type,"FLOAT") == 0){
			printf(": %f",root->info.floatVal);
		}
		printf("\n");
	}
	showTree(root->child, height + 1);
	showTree(root->brother, height);
}

void deleteTree(Node* root)
{
	if (root == NULL)
		return;
	if(root->child != NULL)
		deleteTree(root->child);
	if(root->brother != NULL)
		deleteTree(root->brother);
	free(root);
	root = NULL;
}