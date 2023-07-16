#include "interCode.h"

#define OPTIMIZE 1

extern Table table; // 符号表

interCode codeHead; // 双向链表表头
interCode codeTail; // 双向链表表尾
static int tmpCnt; // 临时变量计数
//static int varCnt; // 变量计数
static int labelCnt; // 标志计数

// 声明双向链表所需的本地函数
static void initInterCode();
static void insertInterCode(interCode code);
static void removeInterCode(interCode code);

static char* printOperand(Operand op);
static void printInterCode(interCode ic, FILE *fp);
static void printInterCodes(char* fileName);

// 声明用于申请新操作数的本地函数
static Operand new_operand(int kind, unsigned val);

//声明用于描述中间代码的本地函数
//只有single_op和double_op有多种kind，其余都仅对应一种kind
static interCode new_single_op(int kind, Operand op);
static interCode new_assign(Operand left, Operand right);
static interCode new_double_op(int kind, Operand res, Operand op1, Operand op2);
static interCode new_triple_op(Operand label, Operand op1, Operand op2, char* relop);
static interCode new_dec(Operand op, int size);
static interCode new_call(Operand op1, Operand op2);


//debug用
static void enter(char* s){printf("enter %s\n",s);}
static void leave(char* s){printf("leave %s\n",s);}

static int matchParam(char* name)
{
	SymList p = table->stack[table->stackTop].down;
	while(p != NULL)
	{
		if(p->type->kind == FUNCTION)
		{
			FieldList q = p->type->u.function->paraType;
			while(q != NULL)
			{
				if(strcmp(q->name, name) == 0)
					return 1;
				q = q->tail;
			}
		}
		p = p->down;
	}
	return 0;
}

// 实现双向链表所需的本地函数
static void initInterCode() // 使用带有附加头节点的双向链表
{
	codeHead = (interCode)malloc(sizeof(struct interCode_));
	codeHead->pre = codeHead;
	codeHead->next = codeHead;
	codeTail = codeHead;
	
	tmpCnt = 0;
	//varCnt = 0;
	labelCnt = 0;
}

static void insertInterCode(interCode code) // 插入
{
	if(code == NULL)
	{
		printf("empty pointer in function insertInterCode\n");
		return;
	}
	codeTail->next = code;
	code->pre = codeTail;
	code->next = codeHead;
	codeHead->pre = code;
	codeTail = code;
}

static void removeInterCode(interCode code) // 删除
{
	if(code == NULL)
	{
		printf("empty pointer in function removeInterCode\n");
		return;
	}
	interCode p = code->pre;
	interCode q = code->next;
	p->next = q;
	q->pre = p;
	if(code == codeTail)//移动尾指针
		codeTail = p;
	free(code);
}

static char* printOperand(Operand op) // 生成op对应的字符串
{
	//enter("printOperand");
	char* opStr = (char*)malloc(sizeof(char) * 32);
    memset(opStr, 0, sizeof(opStr));
	if(op == NULL)
		op = new_operand(TMPVAR, 0);
	switch (op->kind)
	{
		case VARIABLE:
			return op->u.name;
		case ARRAY_PARAM:
			return op->u.name;
		case CONSTANT:
            sprintf(opStr, "#%d", op->u.val);
            break;
		case ADDRESS: // 只有临时变量有机会变为地址类型
            sprintf(opStr, "t%d", op->u.var_no);
            break;
		case TMPVAR:
            sprintf(opStr, "t%d", op->u.var_no);
            break;
		case ARRAY_OP: // 只有变量才能变为数组类型
            sprintf(opStr, "&%s", op->u.name);
            break;
	}
	//leave("printOperand");
	return opStr;
}

static void printInterCode(interCode ic, FILE *fp) // 打印一条中间代码
{	
	//printf("%d\n",ic->kind);
	char line[100];
    memset(line, 0, sizeof(line));
	if(ic->kind == LABEL_IC)
	{
		sprintf(line, "LABEL label%d :\n", ic->u.single_op.op->u.var_no);
	}
	else if(ic->kind == FUNCTION_IC)
	{
		sprintf(line, "FUNCTION %s :\n", ic->u.single_op.op->u.name);
	}
	else if(ic->kind == RETURN_IC)
	{
		if(ic->u.single_op.op->kind == ADDRESS)
			sprintf(line, "RETURN *%s\n", printOperand(ic->u.single_op.op));
		else
			sprintf(line, "RETURN %s\n", printOperand(ic->u.single_op.op));
	}
	else if(ic->kind == GOTO)
	{
		sprintf(line, "GOTO label%d\n", ic->u.single_op.op->u.var_no);
	}
	else if(ic->kind == PARAM)
	{
		if(ic->u.single_op.op->kind == ADDRESS)
			sprintf(line, "PARAM *%s\n", printOperand(ic->u.single_op.op));
		else
			sprintf(line, "PARAM %s\n", printOperand(ic->u.single_op.op));
	}
	else if(ic->kind == ARG)
	{
		if(ic->u.single_op.op->kind == ADDRESS)
			sprintf(line, "ARG *%s\n", printOperand(ic->u.single_op.op));
		else
			sprintf(line, "ARG %s\n", printOperand(ic->u.single_op.op));
	}
	else if(ic->kind == READ)
	{
		if(ic->u.single_op.op->kind == ADDRESS)
			sprintf(line, "READ *%s\n", printOperand(ic->u.single_op.op));
		else
			sprintf(line, "READ %s\n", printOperand(ic->u.single_op.op));
	}
	else if(ic->kind == WRITE)
	{
		if(ic->u.single_op.op->kind == ADDRESS)
			sprintf(line, "WRITE *%s\n", printOperand(ic->u.single_op.op));
		else
			sprintf(line, "WRITE %s\n", printOperand(ic->u.single_op.op));
	}
	else if(ic->kind == ASSIGN)
	{
		//Stmt向translate_exp中传入NUll，导致赋值左侧可能是NULl，已经除去这种情况
		//赋值左侧不可能是address
		if(ic->u.assign.left->kind == ADDRESS && ic->u.assign.right->kind != ADDRESS)
			sprintf(line, "*%s := %s\n", printOperand(ic->u.assign.left), printOperand(ic->u.assign.right));
		else if(ic->u.assign.left->kind != ADDRESS && ic->u.assign.right->kind == ADDRESS)
			sprintf(line, "%s := *%s\n", printOperand(ic->u.assign.left), printOperand(ic->u.assign.right));
		else if(ic->u.assign.left->kind == ADDRESS && ic->u.assign.right->kind == ADDRESS)
			sprintf(line, "*%s := *%s\n", printOperand(ic->u.assign.left), printOperand(ic->u.assign.right));
		else
			sprintf(line, "%s := %s\n", printOperand(ic->u.assign.left), printOperand(ic->u.assign.right));
	}
	else if(ic->kind == ADD_IC)
	{
		//特殊情况为 address = array + tmp
		if(ic->u.double_op.res->kind == ADDRESS 
		&& (ic->u.double_op.op1->kind == ARRAY_OP || ic->u.double_op.op1->kind == ARRAY_PARAM)
		)
		{
			sprintf(line, "%s := %s + %s\n", printOperand(ic->u.double_op.res), printOperand(ic->u.double_op.op1), printOperand(ic->u.double_op.op2));
		}
		//其他情况，左值肯定不会是address, 右值不会出现ARRAY_PARA只有可能出现address
		else if(ic->u.double_op.op1->kind == ADDRESS && ic->u.double_op.op2->kind == ADDRESS)
			sprintf(line, "%s := *%s + *%s\n", printOperand(ic->u.double_op.res), printOperand(ic->u.double_op.op1), printOperand(ic->u.double_op.op2));
		else if(ic->u.double_op.op1->kind == ADDRESS && ic->u.double_op.op2->kind != ADDRESS)
			sprintf(line, "%s := *%s + %s\n", printOperand(ic->u.double_op.res), printOperand(ic->u.double_op.op1), printOperand(ic->u.double_op.op2));
		else if(ic->u.double_op.op1->kind != ADDRESS && ic->u.double_op.op2->kind == ADDRESS)
			sprintf(line, "%s := %s + *%s\n", printOperand(ic->u.double_op.res), printOperand(ic->u.double_op.op1), printOperand(ic->u.double_op.op2));
		else if(ic->u.double_op.op1->kind != ADDRESS && ic->u.double_op.op2->kind != ADDRESS)
			sprintf(line, "%s := %s + %s\n", printOperand(ic->u.double_op.res), printOperand(ic->u.double_op.op1), printOperand(ic->u.double_op.op2));
	}
	else if(ic->kind == SUB_IC)
	{
		//左值肯定不会是指针类型,右值不会出现ARRAY_PARA
		if(ic->u.double_op.op1->kind == ADDRESS && ic->u.double_op.op2->kind == ADDRESS)
			sprintf(line, "%s := *%s - *%s\n", printOperand(ic->u.double_op.res), printOperand(ic->u.double_op.op1), printOperand(ic->u.double_op.op2));
		else if(ic->u.double_op.op1->kind == ADDRESS && ic->u.double_op.op2->kind != ADDRESS)
			sprintf(line, "%s := *%s - %s\n", printOperand(ic->u.double_op.res), printOperand(ic->u.double_op.op1), printOperand(ic->u.double_op.op2));
		else if(ic->u.double_op.op1->kind != ADDRESS && ic->u.double_op.op2->kind == ADDRESS)
			sprintf(line, "%s := %s - *%s\n", printOperand(ic->u.double_op.res), printOperand(ic->u.double_op.op1), printOperand(ic->u.double_op.op2));
		else if(ic->u.double_op.op1->kind != ADDRESS && ic->u.double_op.op2->kind != ADDRESS)
			sprintf(line, "%s := %s - %s\n", printOperand(ic->u.double_op.res), printOperand(ic->u.double_op.op1), printOperand(ic->u.double_op.op2));
	}
	else if(ic->kind == MUL_IC)
	{
		//左值肯定不会是指针类型,右值不会出现ARRAY_PARA
		if(ic->u.double_op.op1->kind == ADDRESS && ic->u.double_op.op2->kind == ADDRESS)
			sprintf(line, "%s := *%s * *%s\n", printOperand(ic->u.double_op.res), printOperand(ic->u.double_op.op1), printOperand(ic->u.double_op.op2));
		else if(ic->u.double_op.op1->kind == ADDRESS && ic->u.double_op.op2->kind != ADDRESS)
			sprintf(line, "%s := *%s * %s\n", printOperand(ic->u.double_op.res), printOperand(ic->u.double_op.op1), printOperand(ic->u.double_op.op2));
		else if(ic->u.double_op.op1->kind != ADDRESS && ic->u.double_op.op2->kind == ADDRESS)
			sprintf(line, "%s := %s * *%s\n", printOperand(ic->u.double_op.res), printOperand(ic->u.double_op.op1), printOperand(ic->u.double_op.op2));
		else if(ic->u.double_op.op1->kind != ADDRESS && ic->u.double_op.op2->kind != ADDRESS)
			sprintf(line, "%s := %s * %s\n", printOperand(ic->u.double_op.res), printOperand(ic->u.double_op.op1), printOperand(ic->u.double_op.op2));
	}
	else if(ic->kind == DIV_IC)
	{
		//左值肯定不会是指针类型,右值不会出现ARRAY_PARA
		if(ic->u.double_op.op1->kind == ADDRESS && ic->u.double_op.op2->kind == ADDRESS)
			sprintf(line, "%s := *%s / *%s\n", printOperand(ic->u.double_op.res), printOperand(ic->u.double_op.op1), printOperand(ic->u.double_op.op2));
		else if(ic->u.double_op.op1->kind == ADDRESS && ic->u.double_op.op2->kind != ADDRESS)
			sprintf(line, "%s := *%s / %s\n", printOperand(ic->u.double_op.res), printOperand(ic->u.double_op.op1), printOperand(ic->u.double_op.op2));
		else if(ic->u.double_op.op1->kind != ADDRESS && ic->u.double_op.op2->kind == ADDRESS)
			sprintf(line, "%s := %s / *%s\n", printOperand(ic->u.double_op.res), printOperand(ic->u.double_op.op1), printOperand(ic->u.double_op.op2));
		else if(ic->u.double_op.op1->kind != ADDRESS && ic->u.double_op.op2->kind != ADDRESS)
			sprintf(line, "%s := %s / %s\n", printOperand(ic->u.double_op.res), printOperand(ic->u.double_op.op1), printOperand(ic->u.double_op.op2));
	}
	else if(ic->kind == IFGOTO)
	{
		if(ic->u.triple_op.op1->kind != ADDRESS && ic->u.triple_op.op2->kind != ADDRESS)
			sprintf(line, "IF %s %s %s GOTO label%d\n", printOperand(ic->u.triple_op.op1), ic->u.triple_op.relop, printOperand(ic->u.triple_op.op2), ic->u.triple_op.label->u.var_no);
		else if(ic->u.triple_op.op1->kind != ADDRESS && ic->u.triple_op.op2->kind == ADDRESS)
			sprintf(line, "IF %s %s *%s GOTO label%d\n", printOperand(ic->u.triple_op.op1), ic->u.triple_op.relop, printOperand(ic->u.triple_op.op2), ic->u.triple_op.label->u.var_no);
		else if(ic->u.triple_op.op1->kind == ADDRESS && ic->u.triple_op.op2->kind != ADDRESS)
			sprintf(line, "IF *%s %s %s GOTO label%d\n", printOperand(ic->u.triple_op.op1), ic->u.triple_op.relop, printOperand(ic->u.triple_op.op2), ic->u.triple_op.label->u.var_no);
		else
			sprintf(line, "IF *%s %s *%s GOTO label%d\n", printOperand(ic->u.triple_op.op1), ic->u.triple_op.relop, printOperand(ic->u.triple_op.op2), ic->u.triple_op.label->u.var_no);
		
	}
	else if(ic->kind == DEC)
	{
		sprintf(line, "DEC %s %d\n", printOperand(ic->u.dec.op), ic->u.dec.size);
	}
	else if(ic->kind == CALL)
	{
		if(ic->u.call.op != NULL && ic->u.call.op->kind == ADDRESS)
			sprintf(line, "*%s := CALL %s\n", printOperand(ic->u.call.op), ic->u.call.fun->u.name);
		else
			sprintf(line, "%s := CALL %s\n", printOperand(ic->u.call.op), ic->u.call.fun->u.name);
	}
	else
	{
		printf("undefined intercode type\n");
	}
	fputs(line, fp);
	//leave("printInterCode");
}

static void printInterCodes(char* fileName) // 向文件中写入中间代码
{
	FILE* fp = fopen(fileName, "w");
	if(fp == NULL)
	{
		printf("open file failed in printInterCodes\n");
		return;
	}
	interCode p = codeHead->next;
	//删除赋值左侧为空的结点，对应Stmt调用的translate_Exp
	while(p != codeHead)
	{
		if(p->kind == ASSIGN && p->u.assign.left == NULL)
		{
			interCode tmp = p;
			p = p->next;
			removeInterCode(tmp);
			continue;
		}
		p = p->next;
	}
	//删除结束
	p = codeHead->next;
	while(p != codeHead)
	{
		printInterCode(p, fp);
		p = p->next;
	}
	//leave("printInterCodes");
	fclose(fp);
}

// 实现用于申请新操作数的本地函数
static Operand new_operand(int kind, unsigned val)
{
	Operand operand = (Operand)malloc(sizeof(struct Operand_));
	operand->kind = kind;
	if(kind == VARIABLE)
	{
		//varCnt++;
		//operand->u.var_no = varCnt;
		operand->u.name = (char*) val;
	}
	else if(kind == TMPVAR)
	{
		tmpCnt++;
		operand->u.var_no = tmpCnt;
	}
	else if(kind == LABEL)
	{
		labelCnt++;
		operand->u.var_no = labelCnt;
	}
	else if(kind == ADDRESS || kind == CONSTANT)
	{
		operand->u.val = (int)val;
	}
	else if(kind == FUNC)
	{
		operand->u.name = (char*)val;
	}
	else if(kind == ARRAY_OP || kind == ARRAY_PARAM)
	{
		operand->u.name = (char*) val;
	}
	else
	{
		printf("unknown operand in new_operand\n");
		return NULL;
	}
	return operand;
}

//实现用于描述中间代码的本地函数
//只有单操作数和双操作数的情况需要指定code的类型，其他都只有一种情况，可以直接指定
static interCode new_single_op(int kind, Operand op)
{
	//为中间代码申请空间并初始化
	interCode code = (interCode)malloc(sizeof(struct interCode_));
	code->pre = NULL;
	code->next = NULL;
	//填充
	code->kind = kind;
	code->u.single_op.op = op;
}

static interCode new_assign(Operand left, Operand right)
{
	//为中间代码申请空间并初始化
	interCode code = (interCode)malloc(sizeof(struct interCode_));
	code->pre = NULL;
	code->next = NULL;
	//填充
	code->kind = ASSIGN;
	code->u.assign.left = left;
	code->u.assign.right = right;
}

static interCode new_double_op(int kind, Operand res, Operand op1, Operand op2)
{
	//为中间代码申请空间并初始化
	interCode code = (interCode)malloc(sizeof(struct interCode_));
	code->pre = NULL;
	code->next = NULL;
	//填充
	code->kind = kind;
	code->u.double_op.res = res;
	code->u.double_op.op1 = op1;
	code->u.double_op.op2 = op2;
}

static interCode new_triple_op(Operand label, Operand op1, Operand op2, char* relop)
{
	//为中间代码申请空间并初始化
	interCode code = (interCode)malloc(sizeof(struct interCode_));
	code->pre = NULL;
	code->next = NULL;
	//填充
	code->kind = IFGOTO;
	code->u.triple_op.label = label;
	code->u.triple_op.op1 = op1;
	code->u.triple_op.op2 = op2;
	code->u.triple_op.relop = relop;
}

static interCode new_dec(Operand op, int size)
{
	//为中间代码申请空间并初始化
	interCode code = (interCode)malloc(sizeof(struct interCode_));
	code->pre = NULL;
	code->next = NULL;
	//填充
	code->kind = DEC;
	code->u.dec.op = op;
	code->u.dec.size = size;
}

static interCode new_call(Operand ret, Operand fun)
{
	//为中间代码申请空间并初始化
	interCode code = (interCode)malloc(sizeof(struct interCode_));
	code->pre = NULL;
	code->next = NULL;
	//填充
	code->kind = CALL;
	code->u.call.op = ret;
	code->u.call.fun = fun;
}

// 声明翻译中间代码所需的本地函数(除了translate_Program)
/* High-level Definitions */
static void translate_ExtDefList(Node* root);
static void translate_ExtDef(Node* root);
//extDecList定义全局变量，实验3不涉及全局变量

/* Specifiers */
//Specifiers中的语句用于描述声明/定义结构体类型或基本类型,不需要产生中间语句

/* Declarators */
//产生中间语句为结构体或数组类型的变量分配空间,只能被Dec调用
static void translate_VarDec(Node* root);
//不再调用VarList，直接记录参数t1 t2 t3...即可
static void translate_FunDec(Node* root);
//VarList ParamDec继续调用的话，最后调用VarDec,但是参数和局部变量处理方式不同
//为了减少工作量，此处直接解析参数列表，产生形如v1 v2 v3的变量即可

/* Statements */
static void translate_CompSt(Node* root);//调用DefList和StmtList
static void translate_StmtList(Node* root);//调用Stmt
static void translate_Stmt(Node* root);//需要产生中间语句

/* Local Definitions */
static void translate_DefList(Node* root);//调用Def
static void translate_Def(Node* root);//调用DecList
static void translate_DecList(Node* root);//调用Dec
//Dec调用VarDec,还可能存在初始化语句，需要产生中间语句
static void translate_Dec(Node* root);

/* Expressions */
static void translate_Exp(Node* root, Operand place, int isLeft);
static void translate_Args(Node* root);

// Other Functions
static void translate_Cond(Node *root, Operand label_true, Operand label_false);
static void optGoto();
static void deleteUselessLabel();
static void deleteConstant();
static void mergeAssign();
static void deleteGoto();
static void optArg();
static void optReturn();
static void optTriple();
static void optWrite();
static void optRead();
static void optArray();
static void optArith();
static void optLabel();

// 实现翻译中间代码所需的本地函数
/* High-level Definitions */
void translate_Program(Node* root)
{
	assert(root != NULL);
	// 我们不处理出现了结构体的程序
	if(table->structTop > 0)
	{
		printf("Cannot translate: Code contains variables or parameters of structure type.\n");
		return;
	}
	initInterCode(); // 初始化双向链表
	//enter("ExtDefList");
	translate_ExtDefList(root->child); // 开始翻译语法树
	//leave("ExtDefList");
	if (OPTIMIZE)
	{
		//enter("optGoto");
		optGoto();
		//enter("deleteGoto");
		deleteGoto();
		//enter("deleteUselessLabel");
		deleteUselessLabel();
		//enter("optTriple")
		optTriple();
		//enter("deleteConstant");
		deleteConstant();
		//enter("mergeAssign");
		mergeAssign();
		//enter("optArg");
		optArg();
		//enter("optReturn");
		optReturn();
		optWrite();
		optRead();
		optArray();
		optArith();
		optLabel();
		deleteUselessLabel();
	}
	interCode p = codeHead->next;
	//删除赋值左侧为空的结点，对应Stmt调用的translate_Exp
	while(p != codeHead)
	{
		if(p->kind == ASSIGN && p->u.assign.left == NULL)
		{
			interCode tmp = p;
			p = p->next;
			removeInterCode(tmp);
			continue;
		}
		p = p->next;
	}
	//printInterCodes(filename); // 翻译完毕打印中间代码
}

/*void translate_Program(Node* root, char *filename)
{
	assert(root != NULL);
	// 我们不处理出现了结构体的程序
	if(table->structTop > 0)
	{
		printf("Cannot translate: Code contains variables or parameters of structure type.\n");
		return;
	}
	initInterCode(); // 初始化双向链表
	//enter("ExtDefList");
	translate_ExtDefList(root->child); // 开始翻译语法树
	//leave("ExtDefList");
	if (OPTIMIZE)
	{
		//enter("optGoto");
		optGoto();
		//enter("deleteGoto");
		deleteGoto();
		//enter("deleteUselessLabel");
		deleteUselessLabel();
		//enter("optTriple")
		optTriple();
		//enter("deleteConstant");
		deleteConstant();
		//enter("mergeAssign");
		mergeAssign();
		//enter("optArg");
		optArg();
		//enter("optReturn");
		optReturn();
		optWrite();
		optRead();
		optArray();
		optArith();
		optLabel();
		deleteUselessLabel();
	}
	printInterCodes(filename); // 翻译完毕打印中间代码
}*/

static void translate_ExtDefList(Node* root)
{
	assert(root != NULL);
	Node* p = root;
	while(p != NULL)//p = ExtDefList
	{
		p = p->child;//p = ExtDef or null
		if(p != NULL)
		{	
			translate_ExtDef(p);
			p = p->brother;//p = ExtDefList
		}
		else
			break;
	}
}

static void translate_ExtDef(Node* root)
{
	assert(root != NULL);
	Node* p = root->child->brother;
	if(strcmp(p->type, "FunDec") == 0)//ExtDef -> Specifiers FunDec CompSt
	{
		//enter("FunDec");
		translate_FunDec(p);
		//leave("FunDec");
		//enter("CompSt");
		translate_CompSt(p->brother);
		//leave("CompSt");
	}
	else if(strcmp(p->type, "SEMI") == 0 || strcmp(p->type, "ExtDecList") == 0)
		;//无需处理
	else
	{
		printf("unknown grammar in translate_ExtDef\n");
	}
}

/* Declarators */
static void translate_VarDec(Node* root)//产生中间语句为数组类型的变量分配空间,只能被Dec调用
{
	assert(root != NULL);
	//只考虑是ID的情况
	//对于非ID的情况，直接继续递归即可
	if(root->child->brother == NULL)//VarDec -> ID
	{
		char* sym_name = root->child->info.name;
		SymList sym = getSym(sym_name);
		if(sym == NULL)
		{
			printf("getSym -%s- failed in translate_VarDec\n",sym_name);
			return;
		}
		if(sym->type->kind == BASIC || sym->type->kind == FUNCTION)
		{	
			return;//普通类型和函数名，不需要dec空间
		}
		else if(sym->type->kind == STRUCTURE)
		{//预设在之前处理出现结构体的错误，所以此处不应该出现此情况
			assert(0);
		}
		else if(sym->type->kind == ARRAY)
		{//需要处理的情况--数组
			int size = 1;
			Type type = sym->type;
			while(type->kind == ARRAY)
			{
				size *= type->u.array.size;
				type = type->u.array.elem;
			}
			if(type->kind == BASIC)
				size *= 4;
			else//出现非基础类型数组
			{
				printf("array with not-basic elem in translate_VarDec\n");
				return;
			}
			Operand op = new_operand(VARIABLE, (unsigned)sym_name);
			interCode code = new_dec(op, size);
			insertInterCode(code);
		}
		else
		{
			printf("unknown type kind in translate_VarDec\n");
			return;
		}
	}
	else//VarDec -> VarDec LB INT RB
	{
		translate_VarDec(root->child);
	}
}

static void translate_FunDec(Node* root)//不再调用VarList，直接记录参数t1 t2 t3...即可
{
	assert(root != NULL);
	char *fname = root->child->info.name; // get name of function
	Operand newOp = new_operand(FUNC, (unsigned)fname);
	interCode newCode = new_single_op(FUNCTION_IC, newOp);
	insertInterCode(newCode);
	// translate VarList here
	if (strcmp(root->child->brother->brother->type, "VarList") == 0)
	{
		SymList func = getSym(fname);
		FieldList paraList = func->type->u.function->paraType;
		while (paraList != NULL)
		{
			// 参数不能有结构体, 理论上在该情况不会在这里出现
			if (paraList->type->kind == STRUCTURE)
				assert(0);
			// 需要判断参数是不是高维数组
			if (paraList->type->kind == ARRAY)
			{
				// 出现了高维数组
				if (paraList->type->u.array.elem->kind == ARRAY)
				{
					printf("Cannot translate: Code contains parameters of multi-dimensional array type.\n");
					return;
				}
				Operand newPara = new_operand(VARIABLE, (unsigned)paraList->name);
				insertInterCode(new_single_op(PARAM, newPara));
				paraList = paraList->tail;
				continue;
			}
			Operand newPara = new_operand(VARIABLE, (unsigned)paraList->name);
			insertInterCode(new_single_op(PARAM, newPara));
			paraList = paraList->tail;
		}
	}
}

/* Statements */
static void translate_CompSt(Node* root)//调用DefList和StmtList
{
	assert(root != NULL);
	Node* p = root->child->brother;
	translate_DefList(p);
	translate_StmtList(p->brother);
}

static void translate_StmtList(Node* root)//调用Stmt
{
	assert(root != NULL);
	Node* p = root;
	while(p != NULL)
	{
		p = p->child;//p = Stmt
		if(p != NULL)
		{
			translate_Stmt(p);
			p = p->brother;//p = StmtList
		}
		else
			break;
	}
}

static void translate_Stmt(Node* root)//需要产生中间语句
{
	//使用教材提供的翻译模板
	assert(root != NULL);
	Node *child = root->child;
	// Stmt -> Exp SEMI
	if (strcmp(child->type, "Exp") == 0)
	{
		//enter("Exp");
		translate_Exp(child, NULL,0);
		//leave("Exp");
	}
	// Stmt -> CompSt
	else if (strcmp(child->type, "CompSt") == 0)
	{
		//enter("CompSt");
		translate_CompSt(child);
		//leave("CompSt");
	}
	// Stmt -> RETURN Exp SEMI
	else if (strcmp(child->type, "RETURN") == 0)
	{
		//enter("RETURN");
		Operand newOp = new_operand(TMPVAR, 0);
		translate_Exp(child->brother, newOp,0);
		insertInterCode(new_single_op(RETURN_IC, newOp));
		//leave("RETURN");
	} 
	// Stmt -> IF xxxx
	else if (strcmp(child->type, "IF") == 0)
	{
		// Stmt -> IF LP Exp RP Stmt
		if (child->brother->brother->brother->brother->brother == NULL)
		{
			Operand label1 = new_operand(LABEL, 0);
			Operand label2 = new_operand(LABEL, 0);
			translate_Cond(child->brother->brother, label1, label2);

			insertInterCode(new_single_op(LABEL_IC, label1));

			translate_Stmt(child->brother->brother->brother->brother);

			insertInterCode(new_single_op(LABEL_IC, label2));
		}
		// Stmt -> IF LP Exp RP Stmt ELSE Stmt
		else
		{
			Operand label1 = new_operand(LABEL, 0);
			Operand label2 = new_operand(LABEL, 0);
			Operand label3 = new_operand(LABEL, 0);
			Node *exp = child->brother->brother;
			Node *stmt1 = exp->brother->brother;
			Node *stmt2 = stmt1->brother->brother;

			translate_Cond(exp, label1, label2);//code1

			insertInterCode(new_single_op(LABEL_IC, label1));//label1

			translate_Stmt(stmt1);//code2

			insertInterCode(new_single_op(GOTO, label3));//goto label3

			insertInterCode(new_single_op(LABEL_IC, label2));//label2

			translate_Stmt(stmt2);//code3

			insertInterCode(new_single_op(LABEL_IC, label3));//label3
		}
		
	}
	// Stmt -> WHILE LP Exp RP Stmt
	else
	{
		//enter("WHILE");
		Operand label1 = new_operand(LABEL, 0);
		Operand label2 = new_operand(LABEL, 0);
		Operand label3 = new_operand(LABEL, 0);
		Node *exp = child->brother->brother;
		Node *stmt = exp->brother->brother;

		insertInterCode(new_single_op(LABEL_IC, label1));//lable1

		translate_Cond(exp, label2, label3);//code1

		insertInterCode(new_single_op(LABEL_IC, label2));//label2

		translate_Stmt(stmt);//code2

		insertInterCode(new_single_op(GOTO, label1));//goto label1

		insertInterCode(new_single_op(LABEL_IC, label3));//label3
		//leave("WHILE");
	}
}

/* Local Definitions */
static void translate_DefList(Node* root)//调用Def
{
	assert(root != NULL);
	Node* p = root;
	while(p != NULL)
	{
		p = p->child;//p = Def
		if(p != NULL)
		{
			translate_Def(p);
			p = p->brother;//p = DefList
		}
		else
			break;
	}
}

static void translate_Def(Node* root)//调用DecList
{
	assert(root != NULL);
	Node* p = root->child->brother;
	translate_DecList(p);
}

static void translate_DecList(Node* root)//调用Dec
{
	assert(root != NULL);
	Node* p = root;
	while(p != NULL)
	{
		p = p->child;//p = Dec
		translate_Dec(p);
		p = p->brother;
		if(p == NULL)
			break;
		else
			p = p->brother;
	}
}

static void translate_Dec(Node* root)//Dec调用VarDec,还可能存在初始化语句，需要产生中间语句
{
	assert(root != NULL);
	// Dec -> VarDec
	if (root->child->brother == NULL)
		translate_VarDec(root->child);
	// Dec -> VarDec ASSIGNOP Exp
	else if (root->child->brother != NULL)
	{
		// 此种情况的VarDec不会产生形如DEC x size的中间代码
		// 此种情况的Dec产生的是形如 vx := ty的中间代码
		// C--不支持数组初始化, 所以这里的VarDec理论上只能推出ID
		if (strcmp(root->child->child->type, "ID") == 0)
		{
			Operand newVar = new_operand(VARIABLE, (unsigned)root->child->child->info.name);
			Operand newTmp = new_operand(TMPVAR, 0);
			translate_Exp(root->child->brother->brother, newTmp, 0);
			insertInterCode(new_assign(newVar, newTmp));
		}
		else
		{
			assert(0);
		}
	}
}

/* Expressions */
static void translate_Exp(Node* root, Operand place, int isLeft)
{
	assert(root != NULL);
	Node* child = root->child;
	if(strcmp(child->type, "Exp") == 0)
	{
		if(strcmp(child->brother->type, "ASSIGNOP") == 0)
		{
			//Exp -> Exp1 ASSIGNOP Exp2
			Operand left;
			if(strcmp(child->child->type, "ID") == 0)
			{
				//left -> ID;
				SymList sym = getSym(child->child->info.name);
				if(sym == NULL)
				{
					fprintf(stderr, "left = right, can`t find symbol left");
					return;
				}
				left = new_operand(VARIABLE, (unsigned)sym->name);
			}
			else
			{
				//left -> 数组
				left = new_operand(TMPVAR, 0);
				translate_Exp(child, left, 1);
			}
			Operand t1 = new_operand(TMPVAR, 0);
			translate_Exp(child->brother->brother, t1,0);			
			interCode code1 = new_assign(left, t1);
			interCode code2 = new_assign(place, left);
			insertInterCode(code1);
			insertInterCode(code2);
		}
		else if(strcmp(child->brother->type, "AND") == 0
				|| strcmp(child->brother->type, "OR") == 0
				|| strcmp(child->brother->type, "RELOP") == 0)
		{
			//Exp -> Exp AND Exp
			Operand label1 = new_operand(LABEL, 0);
			Operand label2 = new_operand(LABEL, 0);
			Operand constant = new_operand(CONSTANT, 0);
			interCode code = new_assign(place, constant);
			insertInterCode(code);
			translate_Cond(root, label1, label2);
			code = new_single_op(LABEL_IC, label1);
			insertInterCode(code);
			
			constant = new_operand(CONSTANT, 1);
			code = new_assign(place, constant);
			insertInterCode(code);
			
			code = new_single_op(LABEL_IC, label2);
			insertInterCode(code);
		}
		else if(strcmp(child->brother->type, "OR") == 0)
		{
			//Exp -> Exp OR Exp
		}
		else if(strcmp(child->brother->type, "RELOP") == 0)
		{
			//Exp -> Exp RELOP Exp
		}
		else if(strcmp(child->brother->type, "PLUS") == 0)
		{
			//Exp -> Exp PLUS Exp
			Operand t1 = new_operand(TMPVAR, 0);
			Operand t2 = new_operand(TMPVAR, 0);
			translate_Exp(child, t1,0);
			translate_Exp(child->brother->brother, t2,0);
			interCode code = new_double_op(ADD_IC, place, t1, t2);
			insertInterCode(code);
		}
		else if(strcmp(child->brother->type, "MINUS") == 0)
		{
			//Exp -> Exp MINUS Exp
			Operand t1 = new_operand(TMPVAR, 0);
			Operand t2 = new_operand(TMPVAR, 0);
			translate_Exp(child, t1,0);
			translate_Exp(child->brother->brother, t2,0);
			interCode code = new_double_op(SUB_IC, place, t1, t2);
			insertInterCode(code);
		}
		else if(strcmp(child->brother->type, "STAR") == 0)
		{
			//Exp -> Exp STAR Exp
			Operand t1 = new_operand(TMPVAR, 0);
			Operand t2 = new_operand(TMPVAR, 0);
			translate_Exp(child, t1,0);
			translate_Exp(child->brother->brother, t2,0);
			interCode code = new_double_op(MUL_IC, place, t1, t2);
			insertInterCode(code);
		}
		else if(strcmp(child->brother->type, "DIV") == 0)
		{
			//Exp -> Exp DIV Exp
			Operand t1 = new_operand(TMPVAR, 0);
			Operand t2 = new_operand(TMPVAR, 0);
			translate_Exp(child, t1,0);
			translate_Exp(child->brother->brother, t2,0);
			interCode code = new_double_op(DIV_IC, place, t1, t2);
			insertInterCode(code);
		}
		else if(strcmp(child->brother->type, "LB") == 0)
		{
			//Exp -> Exp LB Exp RB
			//enter("LB");
			while(strcmp(child->type, "ID") != 0)
				child = child->child;
			SymList sym = getSym(child->info.name);
			if(sym == NULL)
			{
				fprintf(stderr, "getSym -%s- failed at Exp[Exp]\n",child->info.name);
				return;
			}
			Type type = sym->type;
			int depth = 0;
			int* d = (int*)malloc(100 * sizeof(int));
			int* sum = (int*)malloc(100 * sizeof(int));
			while(type->kind == ARRAY)
			{
				d[depth] = type->u.array.size;
				depth++;
				type = type->u.array.elem;
			}
			sum[depth-1] = 4;
			for(int i = depth - 2; i >= 0; i--)
			{
				sum[i]= sum[i+1]*d[i+1];
			}
			
			Operand tmp = new_operand(TMPVAR, 0);
			Operand constant = new_operand(CONSTANT, 0);
			interCode code = new_assign(tmp, constant);//t = #0;
			insertInterCode(code);
			child = root->child;
			while(strcmp(child->type, "ID") != 0)
			{
				Operand tmp1 = new_operand(TMPVAR, 0);
				translate_Exp(child->brother->brother, tmp1,0);
				Operand constant1 = new_operand(CONSTANT, sum[depth-1]);
				depth--;
				Operand tmp2 = new_operand(TMPVAR, 0);
				interCode code = new_double_op(MUL_IC, tmp2, tmp1, constant1);
				insertInterCode(code);
				code = new_double_op(ADD_IC, tmp, tmp, tmp2);
				insertInterCode(code);
				child = child->child;
			}
			//place->kind = ADDRESS;
			Operand var;
			int i = matchParam(sym->name);
			if(i == 0)
				var	= new_operand(ARRAY_OP, (unsigned)sym->name);
			else if(i == 1)
				var	= new_operand(ARRAY_PARAM, (unsigned)sym->name);
			else
			{
				fprintf(stderr,"match failed\n");
			}
			if(isLeft == 1)
			{
				place->kind = ADDRESS;
				code = new_double_op(ADD_IC, place, var, tmp);
				insertInterCode(code);
			}
			else
			{
				Operand tmp2 = new_operand(TMPVAR,0);
				tmp2->kind = ADDRESS;
				code = new_double_op(ADD_IC, tmp2, var, tmp);
				insertInterCode(code);
				code = new_assign(place, tmp2);
				insertInterCode(code);
			}
			//leave("LB");
		}
		else if(strcmp(child->brother->type, "DOT") == 0)
		{
			//Exp -> Exp DOT ID
			//对于结构体的访问，不应该出现此情况
			assert(0);
		}
		else
		{
			fprintf(stderr, "unknown operator : Exp ??? Exp at 805\n");
			return;
		}
	}
	else if(strcmp(child->type, "LP") == 0)
	{
		//Exp -> LP Exp RP
		translate_Exp(child->brother, place, isLeft);
	}
	else if(strcmp(child->type, "MINUS") == 0)
	{
		//Exp -> MINUS Exp
		Operand t1 = new_operand(CONSTANT, 0);
		Operand t2 = new_operand(TMPVAR, 0);
		translate_Exp(child->brother, t2, 0);
		interCode code = new_double_op(SUB_IC, place, t1, t2);
		insertInterCode(code);
	}
	else if(strcmp(child->type, "NOT") == 0)
	{
		//Exp -> NOT Exp
		Operand label1 = new_operand(LABEL, 0);
		Operand label2 = new_operand(LABEL, 0);
		Operand constant = new_operand(CONSTANT, 0);
		interCode code = new_assign(place, constant);
		insertInterCode(code);
		translate_Cond(root, label1, label2);
		code = new_single_op(LABEL_IC, label1);
		insertInterCode(code);
			
		constant = new_operand(CONSTANT, 1);
		code = new_assign(place, constant);
		insertInterCode(code);
			
		code = new_single_op(LABEL_IC, label2);
		insertInterCode(code);
	}
	else if(strcmp(child->type, "ID") == 0)
	{
		if(child->brother == NULL)
		{
			//Exp -> ID
			SymList sym = getSym(child->info.name);
			if(sym == NULL)
			{
				fprintf(stderr, "getSym -%s- failed at Exp -> ID\n",child->info.name);
				return;
			}
			Operand variable;
			if(sym->type->kind == BASIC)
				variable = new_operand(VARIABLE, (unsigned) sym->name);
			else
			{
				int i = matchParam(sym->name);
				if(i == 0)
					variable = new_operand(ARRAY_OP,(unsigned) sym->name);
				else if(i == 1)
					variable = new_operand(VARIABLE,(unsigned) sym->name);
			}
			interCode code = new_assign(place, variable);
			insertInterCode(code);
		}
		else if(strcmp(child->brother->brother->type, "Args") == 0)
		{
			//enter("args");
			//Exp -> ID LP Args RP
			SymList sym = getSym(child->info.name);
			if(sym == NULL)
			{
				fprintf(stderr, "getSym -%s- failed at Exp -> ID LP Args RP\n",child->info.name);
				return;
			}
			Node* args = child->brother->brother->child;
			if(strcmp(sym->name, "write") == 0)
			{
				Operand t1 = new_operand(TMPVAR, 0);
				translate_Exp(args, t1, 0);
				interCode code = new_single_op(WRITE, t1);
				insertInterCode(code);
			}
			else
			{
				int len = 0;
				while(args != NULL){
					len++;
					if(args->brother == NULL)
						break;
					args = args->brother->brother->child;
				}
				
				Operand* argsList = (Operand*)malloc(len * sizeof(Operand));
				args = child->brother->brother->child;
				len = 0;
				while(args != NULL)
				{
					argsList[len] = new_operand(TMPVAR, 0);
					translate_Exp(args, argsList[len],0);
					if(argsList[len]->kind == ADDRESS)
					{
						Operand pointer = argsList[len];
						argsList[len] = new_operand(TMPVAR,0);
						insertInterCode(new_assign(argsList[len],pointer));
					}
					len++;
					if(args->brother == NULL)
						break;
					args = args->brother->brother->child;
				}
				for(int i = len - 1; i >= 0; i--)
				{
					interCode code = new_single_op(ARG, argsList[i]);
					insertInterCode(code);
				}
				Operand func = new_operand(FUNC, (unsigned) sym->name);
				interCode code = new_call(place, func);
				insertInterCode(code);
			}
			//leave("args");
		}
		else if(strcmp(child->brother->brother->type, "RP") == 0)
		{
			//Exp -> ID LP RP
			SymList sym = getSym(child->info.name);
			if(sym == NULL)
			{
				fprintf(stderr, "getSym -%s-failed at 910\n",child->info.name);
				return;
			}
			if(strcmp(sym->name, "read") == 0)
			{
				interCode code = new_single_op(READ, place);
				insertInterCode(code);
			}
			else
			{
				Operand func = new_operand(FUNC,(unsigned) sym->name);
				interCode code = new_call(place, func);
				insertInterCode(code);
			}
		}
		else
		{
			fprintf(stderr,"unknown expression : Exp -> ID LP ??? at 925\n");
			return;
		}
	}
	else if(strcmp(child->type, "INT") == 0)
	{
		//Exp -> INT
		Operand constant = new_operand(CONSTANT, child->info.intVal);
		interCode code = new_assign(place, constant);
		insertInterCode(code);
	}
	else if(strcmp(child->type, "FLOAT") == 0)
	{
		//Exp -> FLOAT
		fprintf(stderr, "lab3 has no number with type float\n");
		return;
	}
	else
	{
		fprintf(stderr, "unknown expression in translate_Exp at 944\n");
		return;
	}
}

static void translate_Args(Node* root)
{
	assert(root != NULL);
}

static void translate_Cond(Node *root, Operand label_true, Operand label_false)
{
	assert(root != NULL);
	Node* child = root->child;
	if(strcmp(child->type, "NOT") == 0)
	{
		translate_Cond(child->brother, label_false, label_true);
	}
	else if(strcmp(child->type, "Exp") == 0)
	{
		if(strcmp(child->brother->type, "AND") == 0)
		{
			Operand label1 = new_operand(LABEL, 0);
			translate_Cond(child, label1, label_false);
			insertInterCode(new_single_op(LABEL_IC, label1));
			translate_Cond(child->brother->brother, label_true, label_false);
		}
		else if(strcmp(child->brother->type, "OR") == 0)
		{
			Operand label1 = new_operand(LABEL, 0);
			translate_Cond(child, label_true, label1);
			insertInterCode(new_single_op(LABEL_IC, label1));
			translate_Cond(child->brother->brother, label_true, label_false);
		}
		else if(strcmp(child->brother->type, "RELOP") == 0)
		{
			Operand tmp1 = new_operand(TMPVAR, 0);
			Operand tmp2 = new_operand(TMPVAR, 0);
			translate_Exp(child, tmp1,0);
			translate_Exp(child->brother->brother, tmp2,0);
			char *relop = child->brother->info.name;
			insertInterCode(new_triple_op(label_true, tmp1, tmp2, relop));
			insertInterCode(new_single_op(GOTO, label_false));
		}
		else
		{
			fprintf(stderr,"unknown op in translate_Cond\n");
			return;
		}
	}
	else
	{
		Operand t1 = new_operand(TMPVAR, 0);
		translate_Exp(root, t1,0);
		Operand constant = new_operand(CONSTANT, 0);
		interCode code = new_triple_op(label_true, t1, constant, "!=");
		insertInterCode(code);
		code = new_single_op(GOTO, label_false);
		insertInterCode(code);
	}
}

static void optGoto()
{
	interCode ic = codeHead->next;
	while (ic != codeHead)
	{
		if (ic->kind == GOTO)
		{
			interCode c1 = ic;
			interCode c2 = ic->next;
			// case like:
			// GOTO labelk
			// labelk
			if (c2 != codeHead && c2->kind == LABEL_IC && c1->u.single_op.op == c2->u.single_op.op)
			{
				ic = ic->next;
				removeInterCode(c1);
			}
		}
		else if (ic->kind == IFGOTO)
		{
			interCode c1 = ic;
			if (c1->next == codeHead || c1->next->next == codeHead)
				break;
			interCode c2 = c1->next;
			interCode c3 = c2->next;
			// case like :
			// if op1 [RELOP] op2 GOTO labeln
			// GOTO labelm
			// Labeln
			// we turn it into:
			// if op1 ![RELOP] op2 GOTO labelm
			// Labeln
			if (c2->kind == GOTO && c3->kind == LABEL_IC && c1->u.triple_op.label == c3->u.single_op.op)
			{
				c1->u.triple_op.label = c2->u.single_op.op;
				removeInterCode(c2);
				if (strcmp(c1->u.triple_op.relop, "==") == 0)
				{
					c1->u.triple_op.relop = (char*)malloc(sizeof(char) * 4);
					memset(c1->u.triple_op.relop, 0, 4);
					strcpy(c1->u.triple_op.relop, "!=");
				}
				else if (strcmp(c1->u.triple_op.relop, "!=") == 0)
				{
					c1->u.triple_op.relop = (char*)malloc(sizeof(char) * 4);
					memset(c1->u.triple_op.relop, 0, 4);
					strcpy(c1->u.triple_op.relop, "==");
				}
				else if (strcmp(c1->u.triple_op.relop, ">=") == 0)
				{
					c1->u.triple_op.relop = (char*)malloc(sizeof(char) * 4);
					memset(c1->u.triple_op.relop, 0, 4);
					strcpy(c1->u.triple_op.relop, "<");
				}
				else if (strcmp(c1->u.triple_op.relop, "<=") == 0)
				{
					c1->u.triple_op.relop = (char*)malloc(sizeof(char) * 4);
					memset(c1->u.triple_op.relop, 0, 4);
					strcpy(c1->u.triple_op.relop, ">");
				}
				else if (strcmp(c1->u.triple_op.relop, ">") == 0)
				{
					c1->u.triple_op.relop = (char*)malloc(sizeof(char) * 4);
					memset(c1->u.triple_op.relop, 0, 4);
					strcpy(c1->u.triple_op.relop, "<=");
				}
				else if (strcmp(c1->u.triple_op.relop, "<") == 0)
				{
					c1->u.triple_op.relop = (char*)malloc(sizeof(char) * 4);
					memset(c1->u.triple_op.relop, 0, 4);
					strcpy(c1->u.triple_op.relop, ">=");
				}
			}
		}
		ic = ic->next;
	}
}

static void deleteUselessLabel()
{
	char labelMap[1024]; // 假设出现的标记数量总共不超过1024个
	memset(labelMap, 0, sizeof(char)*1024);
	// count useful label
	interCode ic = codeHead->next;
	while (ic != codeHead)
	{
		if (ic->kind == GOTO)
			labelMap[ic->u.single_op.op->u.var_no] = 1;
		else if (ic->kind == IFGOTO)
			labelMap[ic->u.triple_op.label->u.var_no] = 1;
		ic = ic->next;
	}
	// remove useless label
	ic = codeHead->next;
	while (ic != codeHead)
	{
		// find useless label
		if (ic->kind == LABEL_IC && labelMap[ic->u.single_op.op->u.var_no] == 0)
		{
			interCode p = ic;
			ic = ic->next;
			removeInterCode(p);
		}
		else
			ic = ic->next;
	}
	// remove duplicated label
	ic = codeHead->next;
	while (ic != codeHead)
	{
		interCode c1 = ic;
		interCode c2 = ic->next;
		if (c2 == codeHead)
			break;
		if (c1->kind == LABEL_IC && c2->kind == LABEL_IC)
		{
			int c1_no = c1->u.single_op.op->u.var_no;
			interCode p = ic;
			while (p != codeHead)
			{
				if (p->kind == GOTO && p->u.single_op.op->u.var_no == c1_no)
				{
					p->u.single_op.op->u.var_no = c2->u.single_op.op->u.var_no;
				}
				else if (p->kind == IFGOTO && p->u.triple_op.label->u.var_no == c1_no)
				{
					p->u.triple_op.label->u.var_no = c2->u.single_op.op->u.var_no;
				}
				p = p->pre;
			}
			ic = c2;
			removeInterCode(c1);
		}
		else
			ic = ic->next;	
	}
}

static void deleteConstant()
{
	interCode ic = codeHead->next;
	while (ic != codeHead)
	{
		if (ic->kind == ADD_IC || ic->kind == SUB_IC || ic->kind == MUL_IC || ic->kind == DIV_IC)
		{
			Operand res = ic->u.double_op.res;
			Operand op1 = ic->u.double_op.op1;
			Operand op2 = ic->u.double_op.op2;
			if (op1->kind == CONSTANT && op2->kind == CONSTANT)
			{
				int resInt;
				switch(ic->kind)
				{
					case ADD_IC: resInt = op1->u.val + op2->u.val; break;
					case SUB_IC: resInt = op1->u.val - op2->u.val; break;
					case MUL_IC: resInt = op1->u.val * op2->u.val; break;
					case DIV_IC: resInt = op1->u.val / op2->u.val; break;
					default: assert(0);
				}

				if (res->kind == TMPVAR)
				{
					res->kind = CONSTANT;
					res->u.val = resInt;
					interCode p = ic;
					ic = ic->next;
					removeInterCode(p);;
					continue;
				}
				else
				{
					ic->kind = ASSIGN;
					ic->u.assign.left = res;
					ic->u.assign.right = op1;
					op1->u.val = resInt;
				}
			}
		}
		ic = ic->next;
	}
}

static void mergeAssign()
{
	// 合并赋值表达式
	// 分为 t1 = x op y; v = t1;
	// 和 t1 = x; v = t1;
	interCode ic = codeHead->next;
	while (ic != codeHead)
	{
		interCode c1 = ic;
		interCode c2 = ic->next;
		if (c2 == codeHead)
			break;
		if (c2->kind == ASSIGN)
		{
			if (c1->kind == ADD_IC || c1->kind == SUB_IC || c1->kind == MUL_IC || c1->kind == DIV_IC)
			{
				if (c2->u.assign.left != NULL && c1->u.double_op.res->kind == TMPVAR && c2->u.assign.right->kind == TMPVAR)
					if (c1->u.double_op.res->u.var_no == c2->u.assign.right->u.var_no && c2->u.assign.left->kind != ADDRESS) 
					{
						c1->u.double_op.res = c2->u.assign.left;
						removeInterCode(c2);
					}
			}
			else if (c1->kind == ASSIGN)
			{
				if (c1->u.assign.left != NULL && c2->u.assign.left != NULL && c1->u.assign.left->kind == TMPVAR && c2->u.assign.right->kind == TMPVAR)
					if (c1->u.assign.left->u.var_no == c2->u.assign.right->u.var_no) 
					{
						c1->u.assign.left = c2->u.assign.left;
						removeInterCode(c2);
					} 
			}
		}
		ic = ic->next;
	}
}

static void deleteGoto()
{
	// 删除return后面的goto
	interCode ic = codeHead->next;
	while (ic != codeHead)
	{
		if (ic->kind == RETURN_IC)
		{
			interCode c1 = ic;
			interCode c2 = ic->next;
			if (c2 == codeHead)
				break;
			if (c2->kind == GOTO)
				removeInterCode(c2);
		}
		ic = ic->next;
	}
}

static void optArg()
{
	interCode ic = codeHead->next;
	interCode head = codeHead->next;
	while (ic != codeHead)
	{
		if (ic->kind == ARG && ic->u.single_op.op->kind == TMPVAR)
		{
			int temp_no =  ic->u.single_op.op->u.var_no;
			interCode p = ic->pre;
			while (p != codeHead)
			{
				if (p->kind == ASSIGN && p->u.assign.left != NULL && p->u.assign.left->kind == TMPVAR && p->u.assign.left->u.var_no == temp_no)
				{
					ic->u.single_op.op = p->u.assign.right;
					removeInterCode(p);
					break;
				}
				p = p->pre;
			}
		}
		ic = ic->next;
	}
}

static void optReturn()
{
	// 遇到类似return tx
	// 检测之前有没有出现过tx = xxx的代码
	// 直接生成return xxx
	interCode ic = codeHead->next;
	while (ic != codeHead)
	{
		if (ic->kind == RETURN_IC)
		{
			int t_no = ic->u.single_op.op->u.var_no;
			interCode p = ic->pre;
			while (p != codeHead)
			{
				if (p->kind == ASSIGN && p->u.assign.left != NULL && p->u.assign.left->kind == TMPVAR && p->u.assign.left->u.var_no == t_no)
				{
					ic->u.single_op.op = p->u.assign.right;
					removeInterCode(p);
					break;
				}
				p = p->pre;
			}
		}
		ic = ic->next;
	}
}

static void optTriple()
{
	// 实验性优化类似下面的代码
	// t1 = a
	// t2 = b
	// t3 = t1 op t2 或 if t1 relop t2 goto labeln
	// 优化策略是删去前两条语句 改为t3 = a op b 或 if a relop b goto labeln
	// ***这里进行该种的优化的前提假设是, t1和t2在t3之后的代码不会再复用*** (不一定正确)
	interCode ic = codeHead->next;
	while (ic != codeHead)
	{
		if (ic->kind == ADD_IC || ic->kind == SUB_IC || ic->kind == MUL_IC || ic->kind == DIV_IC)
		{
			if (ic->u.double_op.res->kind == TMPVAR && ic->u.double_op.op1->kind == TMPVAR && ic->u.double_op.op2->kind == TMPVAR)
			{
				int t1_no = ic->u.double_op.op1->u.var_no;
				int t2_no = ic->u.double_op.op2->u.var_no;
				interCode p = ic->pre;
				int change_num = 0;
				while (p != codeHead && change_num != 2)
				{
					if (p->kind == ASSIGN && p->u.assign.left != NULL)
					{
						if (p->u.assign.left->u.var_no == t1_no)
						{
							change_num++;
							ic->u.double_op.op1 = p->u.assign.right;
							interCode q = p;
							p = p->pre;
							removeInterCode(q);
						}
						else if (p->u.assign.left->u.var_no == t2_no)
						{
							change_num++;
							ic->u.double_op.op2 = p->u.assign.right;
							interCode q = p;
							p = p->pre;
							removeInterCode(q);
						}
						else
							p = p->pre;
					}
					else
						p = p->pre;
				}
			}
			else if (ic->u.double_op.res->kind == TMPVAR && ic->u.double_op.op1->kind == TMPVAR && ic->u.double_op.op2->kind == CONSTANT)
			{
				int t_no = ic->u.double_op.op1->u.var_no;
				interCode p = ic->pre;
				while (p != codeHead)
				{
					if (p->kind == ASSIGN && p->u.assign.left != NULL && p->u.assign.left->u.var_no == t_no)
					{
						ic->u.double_op.op1 = p->u.assign.right;
						removeInterCode(p);
						break;
					}
					p = p->pre;
				}
			}
			else if (ic->u.double_op.res->kind == TMPVAR && ic->u.double_op.op1->kind == CONSTANT && ic->u.double_op.op2->kind == TMPVAR)
			{
				int t_no = ic->u.double_op.op2->u.var_no;
				interCode p = ic->pre;
				while (p != codeHead)
				{
					if (p->kind == ASSIGN && p->u.assign.left != NULL && p->u.assign.left->u.var_no == t_no)
					{
						ic->u.double_op.op2 = p->u.assign.right;
						removeInterCode(p);
						break;
					}
					p = p->pre;
				}
			}
		}
		else if (ic->kind == IFGOTO)
		{
			if (ic->u.triple_op.op1->kind == TMPVAR && ic->u.triple_op.op2->kind == TMPVAR)
			{
				int t1_no = ic->u.triple_op.op1->u.var_no;
				int t2_no = ic->u.triple_op.op2->u.var_no;
				interCode p = ic->pre;
				int change_num = 0;
				while (p != codeHead && change_num != 2)
				{
					if (p->kind == ASSIGN && p->u.assign.left != NULL)
					{
						if (p->u.assign.left->u.var_no == t1_no)
						{
							change_num++;
							ic->u.triple_op.op1 = p->u.assign.right;
							interCode q = p;
							p = p->pre;
							removeInterCode(q);
						}
						else if (p->u.assign.left->u.var_no == t2_no)
						{
							change_num++;
							ic->u.triple_op.op2 = p->u.assign.right;
							interCode q = p;
							p = p->pre;
							removeInterCode(q);
						}
						else
							p = p->pre;
					}
					else
						p = p->pre;
				}
			}
		}
		ic = ic->next;
	}
}

static void optWrite()
{
	interCode ic = codeHead->next;
	while (ic != codeHead)
	{
		if (ic->kind == WRITE)
		{
			int t_no = ic->u.single_op.op->u.var_no;
			interCode p = ic->pre;
			while (p != codeHead)
			{
				if (p->kind == ASSIGN && p->u.assign.left != NULL && p->u.assign.left->kind == TMPVAR && p->u.assign.left->u.var_no == t_no)
				{
					ic->u.single_op.op = p->u.assign.right;
					removeInterCode(p);
					break;
				}
				p = p->pre;
			}
		}
		ic = ic->next;
	}
}

static void optRead()
{
	interCode ic = codeHead->next;
	while (ic != codeHead)
	{
		if (ic->kind == READ)
		{
			int t_no = ic->u.single_op.op->u.var_no;
			interCode p = ic->next;
			while (p != codeHead)
			{
				if (p->kind == ASSIGN && p->u.assign.left != NULL && p->u.assign.left->kind != ADDRESS && p->u.assign.right->kind == TMPVAR && p->u.assign.right->u.var_no == t_no)
				{
					ic->u.single_op.op = p->u.assign.left;
					removeInterCode(p);
					break;
				}
				p = p->next;
			}
		}
		ic = ic->next;
	}
}

static void optArray()
{
	// t2 = x op y
	// t1 = #0 + t2
	// 改为:
	// t1 = x op y
	// 观察到该情况主要出现在计算数组地址的时候
	interCode ic = codeHead->next;
	while (ic != codeHead)
	{
		if (ic->kind == ADD_IC && ic->u.double_op.op1->kind == CONSTANT && ic->u.double_op.op1->u.val == 0 && ic->u.double_op.op2->kind == TMPVAR)
		{
			int t_no = ic->u.double_op.op2->u.var_no;
			interCode p = ic->pre;
			while (p != codeHead)
			{
				if (p->kind == ADD_IC || p->kind == SUB_IC || p->kind == MUL_IC || p->kind == DIV_IC)
				{
					if (p->u.double_op.res->kind == TMPVAR && p->u.double_op.res->u.var_no == t_no)
					{
						p->u.double_op.res->u.var_no = ic->u.double_op.res->u.var_no;
						p = ic;
						ic = ic->next;
						removeInterCode(p);
						break;
					}	
				}
				p = p->pre;
			}
			if (p == codeHead)
				ic = ic->next;
		}
		else
			ic = ic->next;
	}
}

static void optArith()
{
	interCode ic = codeHead->next;
	while (ic != codeHead)
	{
		interCode c1 = ic;
		interCode c2 = ic->next;
		if (c2 != codeHead)
		{
			if ((c1->kind == ADD_IC || c1->kind == SUB_IC) && (c2->kind == ADD_IC || c2->kind == SUB_IC) &&
				(c1->u.double_op.op2->kind == CONSTANT && c2->u.double_op.op2->kind == CONSTANT))
			{
				if (c1->u.double_op.res->kind == TMPVAR && c2->u.double_op.op1->kind == TMPVAR && 
					c1->u.double_op.res->u.var_no == c2->u.double_op.op1->u.var_no)
				{
					int const1 = c1->u.double_op.op2->u.val;
					int const2 = c2->u.double_op.op2->u.val;
					c2->u.double_op.op1 = c1->u.double_op.op1;
					if (c1->kind == ADD_IC)
					{
						if (c2->kind == ADD_IC)
							c2->u.double_op.op2->u.val = const1 + const2;
						else
							c2->u.double_op.op2->u.val = const1 - const2;
					}
					else if (c1->kind == SUB_IC)
					{
						if (c2->kind == ADD_IC)
							c2->u.double_op.op2->u.val = const1 - const2;
						else
							c2->u.double_op.op2->u.val = const1 + const2;
					}
					c2->kind = c1->kind;
					ic = c2;
					removeInterCode(c1);
					continue;
				}
			}
			else if (c1->kind == MUL_IC && c2->kind == MUL_IC && c1->u.double_op.op2->kind == CONSTANT && c2->u.double_op.op2->kind == CONSTANT)
			{
				if (c1->u.double_op.res->kind == TMPVAR && c2->u.double_op.op1->kind == TMPVAR && 
					c1->u.double_op.res->u.var_no == c2->u.double_op.op1->u.var_no)
				{
					c2->u.double_op.op2->u.val = c1->u.double_op.op2->u.val * c2->u.double_op.op2->u.val;
					c2->u.double_op.op1 = c1->u.double_op.op1;
					ic = c2;
					removeInterCode(c1);
					continue;					
				}
			}
		}
		ic = ic->next;
	}

	ic = codeHead->next;
	while (ic != codeHead)
	{
		interCode c1 = ic;
		interCode c2 = ic->next;
		if (c2 != codeHead && c1->kind == ASSIGN && c2->kind == ASSIGN && 
		c1->u.assign.left != NULL && c2->u.assign.left != NULL && 
		!strcmp(c1->u.assign.left->u.name, c2->u.assign.left->u.name))
		{
			ic = c2;
			removeInterCode(c1);
		}
		else
			ic = ic->next;
	}
}

static void optLabel()
{
	interCode ic = codeHead->next;

	while (ic != codeHead)
	{
		interCode c1 = ic;
		interCode c2 = ic->next;
		if (c2 != codeHead && c1->kind == LABEL_IC && c2->kind == GOTO)
		{
			int l1 = c1->u.single_op.op->u.var_no;
			interCode p = ic->pre;
			while (p != codeHead)
			{
				if (p->kind == GOTO && p->u.single_op.op->u.var_no == l1)
				{
					p->u.single_op.op = new_operand(LABEL, 0);
					p->u.single_op.op->u.var_no = c2->u.single_op.op->u.var_no;
				}
				else if (p->kind == IFGOTO && p->u.triple_op.label->u.var_no == l1)
				{
					p->u.single_op.op = new_operand(LABEL, 0);
					p->u.triple_op.label->u.var_no = c2->u.single_op.op->u.var_no;
				}
				p = p->pre;
			}
		}
		ic = ic->next;
	}
}