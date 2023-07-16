#ifndef INTERCODE_H
#define INTERCODE_H

#include "symTable.h"
#include "syntaxTree.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

typedef struct Operand_* Operand;
typedef struct interCode_* interCode;
struct Operand_ {
	enum { VARIABLE, CONSTANT, ADDRESS, TMPVAR, LABEL, FUNC, ARRAY_OP, ARRAY_PARAM } kind;
	union {
		int var_no; // tmpvar label
		int val; // constant address
		char *name;//func variable array_op arrar_param
	} u;
};

struct interCode_ {
	enum { 	
		LABEL_IC, FUNCTION_IC, RETURN_IC, GOTO, PARAM, ARG, READ, WRITE,
		ASSIGN,
		ADD_IC, SUB_IC, MUL_IC, DIV_IC,
		IFGOTO,
		DEC,
		CALL
	} kind;
	union 
	{
		struct { Operand op; } single_op;//LABEL FUNCTION GOTO RETURN ARG PARAM READ WRITE
		struct { Operand left, right; } assign;//ASSIGN
		struct { Operand res, op1, op2; } double_op;//ADD SUB MUL DIV
		struct { Operand label, op1, op2; char* relop; } triple_op;//IFGOTO
		struct { Operand op; int size; } dec;//DEC
		struct { Operand op, fun; } call;//CALL
	} u;
	interCode pre;
	interCode next;
};

void translate_Program(Node* root); // for lab4
// void translate_Program(Node* root, char* filename);

#endif