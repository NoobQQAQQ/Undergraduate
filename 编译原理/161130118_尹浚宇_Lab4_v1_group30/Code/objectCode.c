#include "objectCode.h"

extern interCode codeHead, codeTail; //引用lab3的中间代码双向链表

static Data dataTable = NULL; //存储lab4数据的哈希表
static Reg regs = NULL;//$2 = eax 通用的是$3-$25

static FILE* fp = NULL;

/*指明当前栈指针和帧指针的距离
*访问栈中数据需要使用帧指针+偏移量的方式
*/
static int offset = 0;
static int oldOffset = 0;

static int paraCnt = 0;

static int exchangeCnt = BEGIN;

//初始化哈希表和寄存器表
static void init();
//哈希表的函数
static unsigned int hash_(char* name);
static void putData(Data data);//将数据存放在哈希表中
static Data getData(char* name);//从哈希表寻找数据
static int getReg(char* name);

static int memoryToRegs(char* name, int i);
static void regsToMemory(int i);
//寄存器分配函数，使用返回的编号为变量分配寄存器
static int regMalloc();
//获取保存name变量的寄存器的编号
static int getReg(char* name);

//处理中间代码,生成目标代码
static void make_objectCode(interCode code);
static char* printOperand(Operand op);
//为每一种中间代码定义生成目标代码的函数
static void make_LABEL(interCode code);
static void make_FUNCTION(interCode code);
static void make_RETURN(interCode code);
static void make_GOTO(interCode code);
static void make_PARAM(interCode code);
static void make_ARG(interCode code);
static void make_READ(interCode code);
static void make_WRITE(interCode code);
static void make_ASSIGN(interCode code);
static void make_ADD(interCode code);
static void make_SUB(interCode code);
static void make_MUL(interCode code);
static void make_DIV(interCode code);
static void make_IFGOTO(interCode code);
static void make_DEC(interCode code);
static void make_CALL(interCode code);

void make_objectCodes(char* fileName)
{
	init();
	fp = fopen(fileName, "w");
    fputs(".data\n", fp);
    fputs("_prompt: .asciiz \"Enter an integer:\"\n", fp);
    fputs("_ret: .asciiz \"\\n\"\n", fp);
    fputs(".globl main\n", fp);
    fputs(".text\n", fp);
    
    fputs("read:\n", fp);
    fputs("\tli $v0, 4\n", fp);
    fputs("\tla $a0, _prompt\n", fp);
    fputs("\tsyscall\n", fp);
    fputs("\tli $v0, 5\n", fp);
    fputs("\tsyscall\n", fp);
    fputs("\tjr $ra\n", fp);

    fputs("\nwrite:\n", fp);
    fputs("\tli $v0, 1\n", fp);
    fputs("\tsyscall\n", fp);
    fputs("\tli $v0, 4\n", fp);
    fputs("\tla $a0, _ret\n", fp);
    fputs("\tsyscall\n", fp);
    fputs("\tmove $v0, $0\n", fp);
    fputs("\tjr $ra\n", fp);

	interCode p = codeHead->next;
	while (p != codeHead)
	{
		make_objectCode(p);
		p = p->next;
	}
	fclose(fp);
}

static void make_objectCode(interCode code)
{
	if(code == NULL)
		return;
	if(code->kind == LABEL_IC)
	{
		//fprintf(stderr, "enter LABEL\n");
		make_LABEL(code);
		//fprintf(stderr, "leave LABEL\n");
	}
	else if(code->kind == FUNCTION_IC)
	{
		//fprintf(stderr, "enter FUNCTION\n");
		make_FUNCTION(code);
		//fprintf(stderr, "leave FUNCTION\n");
	}
	else if(code->kind == RETURN_IC)
	{
		//fprintf(stderr, "enter RETURN\n");
		make_RETURN(code);
		//fprintf(stderr, "leave RETURN\n");
	}
	else if(code->kind == GOTO)
	{
		//fprintf(stderr, "enter GOTO\n");
		make_GOTO(code);
		//fprintf(stderr, "leave GOTO\n");
	}
	else if(code->kind == PARAM)
	{
		//fprintf(stderr, "enter PARAM\n");
		make_PARAM(code);
		//fprintf(stderr, "leave PARAM\n");
	}
	else if(code->kind == ARG)
	{
		//fprintf(stderr, "enter ARG\n");
		make_ARG(code);
		//fprintf(stderr, "leave ARG\n");
	}
	else if(code->kind == READ)
	{
		//fprintf(stderr, "enter READ\n");
		make_READ(code);
		//fprintf(stderr, "leave READ\n");
	}
	else if(code->kind == WRITE)
	{
		//fprintf(stderr, "enter WRIRE\n");
		make_WRITE(code);
		//fprintf(stderr, "leave WRIRE\n");
	}
	else if(code->kind == ASSIGN)
	{
		//fprintf(stderr, "enter ASSIGN\n");
		make_ASSIGN(code);
		//fprintf(stderr, "leave ASSIGN\n");
	}
	else if(code->kind == ADD_IC)
	{
		//fprintf(stderr, "enter ADD\n");
		make_ADD(code);
		//fprintf(stderr, "leave ADD\n");
	}
	else if(code->kind == SUB_IC)
	{
		//fprintf(stderr, "enter SUB\n");
		make_SUB(code);
		//fprintf(stderr, "leave SUB\n");
	}
	else if(code->kind == MUL_IC)
	{
		//fprintf(stderr, "enter MUL\n");
		make_MUL(code);
		//fprintf(stderr, "leave MUL\n");
	}
	else if(code->kind == DIV_IC)
	{
		//fprintf(stderr, "enter DIV\n");
		make_DIV(code);
		//fprintf(stderr, "leave DIV\n");
	}
	else if(code->kind == IFGOTO)
	{
		//fprintf(stderr, "enter IFGOTO\n");
		make_IFGOTO(code);
		//fprintf(stderr, "leave IFGOTO\n");
	}
	else if(code->kind == DEC)
	{
		//fprintf(stderr, "enter DEC\n");
		make_DEC(code);
		///fprintf(stderr, "leave DEC\n");
	}
	else if(code->kind == CALL)
	{
		//fprintf(stderr, "enter CALL\n");
		make_CALL(code);
		//fprintf(stderr, "leave CALL\n");
	}
	else
	{
		fprintf(stderr, "unknown interCode in function make_objectCode\n");
	}
}

static char* printOperand(Operand op)
{
	//enter("printOperand");
	char* opStr = (char*)malloc(sizeof(char) * 64);
    memset(opStr, 0, sizeof(opStr));
	switch (op->kind)
	{
		case VARIABLE:
			return op->u.name;
		case ADDRESS: // 只有临时变量有机会变为地址类型
            sprintf(opStr, "t%d", op->u.var_no);
            break;
		case TMPVAR:
            sprintf(opStr, "t%d", op->u.var_no);
            break;
		case ARRAY_OP: // 只有变量才能变为数组类型
            sprintf(opStr, "%s", op->u.name);
            break;
		case CONSTANT:
			sprintf(opStr, "#%d", op->u.val);
			break;
	}
	//leave("printOperand");
	return opStr;
}

static void init()
{
	dataTable = (Data)malloc(sizeof(struct Data_)*HASH_SIZE);
	for(int i = 0; i < HASHSIZE; i++)
	{
		dataTable[i].name = NULL;
		dataTable[i].offset = 0;
		dataTable[i].next = NULL;
	}
	regs = (Reg)malloc(sizeof(struct Reg_)*32);
	for(int i = 0; i < 32; i++)
	{
		regs[i].name = NULL;
	}
}

static unsigned int hash_(char* name)//课本给出的哈希函数
{
	unsigned int val = 0, i;
	for (; *name; ++name)
	{
		val = (val << 2) + *name;
		if (i = val & ~HASHSIZE)
			val = (val ^ (i >> 12))&HASHSIZE;
	}
	return val;
}

static void putData(Data data)
{
	if(data == NULL) 
		return;
	unsigned index = hash_(data->name);
	Data p = dataTable[index].next;
	dataTable[index].next = data;
	data->next = p;
}

static Data getData(char* name)
{
	if(name == NULL)
		return NULL;
	unsigned index = hash_(name);
	Data p = dataTable[index].next;
	while(p != NULL)
	{
		if(strcmp(p->name, name) == 0)
			return p;
		p = p->next;
	}
	return p;
}

static int memoryToRegs(char* name, int i)
{
	char cmd[256];
	memset(cmd, 0, 256);
	Data data1 = getData(name);
	if(data1 == NULL)//新的变量，不在内存也不在寄存器
	{//装入内存
		offset -= 4;
		sprintf(cmd, "\taddi $sp, $sp, -4\n");
		fputs(cmd, fp);
		data1 = (Data)malloc(sizeof(struct Data_));
		data1->name = name;
		data1->offset = offset;
		data1->next = NULL;
		putData(data1);
	}
	if(regs[i].name == NULL)//装入空寄存器
	{
		sprintf(cmd, "\tlw $%d, %d($fp)\n", i, data1->offset);
		fputs(cmd, fp);
	}
	else//替换装入
	{
		//Data data2 = getData(regs[i].name);
		//sprintf(cmd, "\tsw $%d, %d($fp)\n", i, data2->offset);
		//fputs(cmd, fp);
		sprintf(cmd, "\tlw $%d, %d($fp)\n", i, data1->offset);
		fputs(cmd, fp);
	}
	regs[i].name = name;
	return i;
}

static void regsToMemory(int i)
{
	if(regs[i].name == NULL)
		return;
	char cmd[256];
	memset(cmd, 0, 256);
	Data data1 = getData(regs[i].name);
	if(data1 == NULL)
	{
		offset -= 4;
		sprintf(cmd, "\taddi $sp, $sp, -4\n");
		fputs(cmd, fp);
		data1 = (Data)malloc(sizeof(struct Data_));
		data1->name = regs[i].name;
		data1->offset = offset;
		data1->next = NULL;
		putData(data1);
	}
	sprintf(cmd, "\tsw $%d, %d($fp)\n", i, data1->offset);
	fputs(cmd, fp);
	regs[i].name = NULL;
}

static int regMalloc()
{
	for(int i = BEGIN; i<=END; i++)
	{
		if(regs[i].name == NULL)
			return i;
	}
	int res = exchangeCnt;
	exchangeCnt++;
	if(exchangeCnt == END+1)
		exchangeCnt = BEGIN;
	return res;
}

static int getReg(char* name)
{
	for(int i = BEGIN; i <= END; i++)
	{
		if (regs[i].name == NULL)
			continue;
		if(strcmp(name, regs[i].name) == 0)
			return i;
	}
	return -1;
}

//make_xxx函数都没有检测参数是否为空指针, 需要在调用前检查
static void make_LABEL(interCode code) // LABEL xxx -> xxx:
{
	char cmd[64];
	memset(cmd, 0, 64);
	sprintf(cmd, "label%d:\n", code->u.single_op.op->u.var_no);
	fputs(cmd, fp);
}

static void make_FUNCTION(interCode code)
{
	/*处理流程:
	*esp-4 : addi $sp, $sp, -4
	*保存ebp : sw $fp, 0($sp)
	*ebp = esp : move $fp, $sp
	* 课本样例的做法没有维护栈指针,相当于此函数不会产生目标代码
	*/
	char cmd[128];
	memset(cmd, 0, 128);
	sprintf(cmd, "\n%s:\n", code->u.single_op.op->u.name);
	fputs(cmd, fp);
	sprintf(cmd, "\taddi $sp, $sp, -4\n");
	fputs(cmd, fp);
	sprintf(cmd, "\tsw $fp, 0($sp)\n");
	fputs(cmd, fp);
	sprintf(cmd, "\tmove $fp, $sp\n");
	fputs(cmd, fp);
	oldOffset = offset;
	offset = 0;
	paraCnt = 0;
}

static void make_RETURN(interCode code)
{
	/*处理流程:
	*保存返回值  : move $v0, reg(x)
	*esp = ebp : move $sp, $fp
	*ebp = old ebp : move %fp, 0($sp)
	*esp+4 : addi $sp, $sp, 4
	*跳转 jr $ra
	*个人理解 : 保存返回值和修改栈帧指针的顺序可以调换
	*课本样例的做法,没有修改帧指针的部分
	*/
	char cmd[256];
	memset(cmd, 0, 256);
	//寻找返回值所在位置
	if(code->u.single_op.op->kind == CONSTANT)
	{
		sprintf(cmd, "\tli $v0, %d\n", code->u.single_op.op->u.val);
		fputs(cmd, fp);
	}
	else
	{
		char* name = printOperand(code->u.single_op.op);
		Data data = getData(name);
		sprintf(cmd, "\tlw $v0, %d($fp)\n", data->offset);
		fputs(cmd, fp);
	}
	sprintf(cmd, "\tmove $sp, $fp\n");
	fputs(cmd, fp);
	sprintf(cmd, "\tlw $fp, 0($sp)\n");
	fputs(cmd, fp);
	sprintf(cmd, "\taddi $sp, $sp, 4\n");
	fputs(cmd, fp);
	sprintf(cmd, "\tjr $ra\n");
	fputs(cmd, fp);
	//offset = oldOffset;
}

static void make_GOTO(interCode code) // GOTO xxx -> j xxx
{
	char cmd[64];
	memset(cmd, 0, 64);
	sprintf(cmd, "\tj label%d\n", code->u.single_op.op->u.var_no);
	fputs(cmd, fp);
}

static void make_PARAM(interCode code)
{
	Data data = (Data)malloc(sizeof(struct Data_));
	data->name = printOperand(code->u.single_op.op);
	data->offset = 8 + 4 * paraCnt;
	data->next = NULL;
	paraCnt++;
	putData(data);
	if(code->next->kind != PARAM)
		paraCnt = 0;
}

static void make_ARG(interCode code)
{
	char cmd[256];
	memset(cmd, 0, 256);
	int i = 5;
	if(code->u.single_op.op->kind != CONSTANT)
	{
		char* argName = printOperand(code->u.single_op.op);
		memoryToRegs(argName, i);
	}
	else
	{
		sprintf(cmd, "\tli $%d, %d\n", i, code->u.single_op.op->u.val);
		fputs(cmd, fp);
	}
	offset -= 4;
	sprintf(cmd, "\taddi $sp, $fp, %d\n", offset);
	fputs(cmd, fp);
	if (code->u.single_op.op->kind != ADDRESS)
	{
		sprintf(cmd, "\tsw $%d, 0($sp)\n", i);
		fputs(cmd, fp);
	}
	else
	{
		sprintf(cmd, "\tlw $%d, 0($%d)\n", i, i);
		fputs(cmd, fp);
		sprintf(cmd, "\tsw $%d, 0($sp)\n", i);
		fputs(cmd, fp);
	}	
}

static void make_READ(interCode code)
{
	/*处理流程 : READ xxx
	*栈指针-4 : addi $sp, $sp, -4
	*保存$ra到栈 : sw $ra, 0($sp) 
	*跳转至read : jal read
	*从栈中恢复$ra : lw $ra, 0($sp)
	*栈指针+4 : addi $sp, $sp, 4
	*将函数返回值赋值给xxx : move xxx, $v0
	*/
	char cmd[256];
	memset(cmd, 0, 256);
	sprintf(cmd, "\taddi $sp, $sp, -4\n");
	fputs(cmd, fp);
	offset -= 4;
	sprintf(cmd, "\tsw $ra, 0($sp)\n");
	fputs(cmd, fp);
	sprintf(cmd, "\tjal read\n");
	fputs(cmd, fp);
	sprintf(cmd, "\tlw $ra, 0($sp)\n");
	fputs(cmd, fp);
	sprintf(cmd, "\taddi $sp, $sp, 4\n");
	fputs(cmd, fp);
	offset +=  4;
	
	int i = 5;
	char* name = printOperand(code->u.single_op.op);
	memoryToRegs(name, i);
	sprintf(cmd, "\tmove $%d, $v0\n", i);
	fputs(cmd, fp);
	regsToMemory(i);
}

static void make_WRITE(interCode code)
{
	/*处理流程 : WRIRE xxx
	*参数写入$a0
	*栈指针-4 : addi $sp, $sp, -4
	*保存$ra到栈 : sw $ra, 0($sp) 
	*跳转至read : jal read
	*从栈中恢复$ra : lw $ra, 0($sp)
	*栈指针+4 : addi $sp, $sp, 4
	*/
	char cmd[256];
	memset(cmd, 0, 256);
	int i = 5;
	if(code->u.single_op.op->kind == CONSTANT)
	{
		sprintf(cmd, "\tli $a0, %d\n", code->u.single_op.op->u.val);
		fputs(cmd, fp);
	}
	else
	{
		char* name = printOperand(code->u.single_op.op);
		memoryToRegs(name, i);
		if (code->u.single_op.op->kind != ADDRESS)
		{
			sprintf(cmd, "\tmove $a0, $%d\n", i);
			fputs(cmd, fp);
		}
		else
		{
			sprintf(cmd, "\tlw $a0, 0($%d)\n", i);
			fputs(cmd, fp);
		}	
	}
	sprintf(cmd, "\taddi $sp, $sp, -4\n");
	fputs(cmd, fp);
	offset -= 4;
	sprintf(cmd, "\tsw $ra, 0($sp)\n");
	fputs(cmd, fp);
	sprintf(cmd, "\tjal write\n");
	fputs(cmd, fp);
	sprintf(cmd, "\tlw $ra, 0($sp)\n");
	fputs(cmd, fp);
	sprintf(cmd, "\taddi $sp, $sp, 4\n");
	offset += 4;
	fputs(cmd, fp);
}

static void make_ASSIGN(interCode code)
{
	char cmd[256];
	memset(cmd, 0, 256);
	char* name1 = printOperand(code->u.assign.left);
	int i = 5, j = 6;
	memoryToRegs(name1, i);
	if(code->u.assign.right->kind != CONSTANT)
	{
		char* name2 = printOperand(code->u.assign.right);
		memoryToRegs(name2, j);
	}
	else
	{
		sprintf(cmd, "\tli $%d, %d\n", j, code->u.assign.right->u.val);
		fputs(cmd, fp);
	}
	//先确定寄存器
	if(code->u.assign.left->kind == ADDRESS && code->u.assign.right->kind == ADDRESS)
	{//对op1 op2解引用
		sprintf(cmd, "\tlw $%d, 0($%d)\n", j, j);
		fputs(cmd, fp);
		sprintf(cmd, "\tsw $%d, 0($%d)\n", j, i);
		fputs(cmd, fp);
	}
	else if(code->u.assign.left->kind == ADDRESS && code->u.assign.right->kind != ADDRESS)
	{
		sprintf(cmd, "\tsw $%d, 0($%d)\n", j, i);
		fputs(cmd, fp);
	}
	else if(code->u.assign.left->kind != ADDRESS && code->u.assign.right->kind == ADDRESS)
	{//对op2解引用
		sprintf(cmd, "\tlw $%d, 0($%d)\n", i, j);
		fputs(cmd, fp);
		regsToMemory(i);
	}
	else if(code->u.assign.left->kind != ADDRESS && code->u.assign.right->kind != ADDRESS)
	{//不需要解引用
		//op2是常数
		sprintf(cmd, "\tmove $%d, $%d\n", i, j);
		fputs(cmd, fp);
		regsToMemory(i);
	}
}

static void make_ADD(interCode code)
{
	char cmd[256];
	memset(cmd, 0, 256);
	//address = arr + xxx; arr是数组名,需要解引用; lab4中只有简单参数
	char* name = printOperand(code->u.double_op.res);
	int i = 5, j = 6, k = 7;
	memoryToRegs(name, 5);
	if(code->u.double_op.res->kind == ADDRESS && (code->u.double_op.op1->kind == ARRAY_OP || code->u.double_op.op1->kind == ARRAY_PARAM))
	{
		char* name1 = printOperand(code->u.double_op.op1);
		Data arr = getData(name1);
		sprintf(cmd, "\taddi $%d, $fp, %d\n", j, arr->offset);
		fputs(cmd, fp);
		if(code->u.double_op.op2->kind == CONSTANT)
		{
			sprintf(cmd, "\taddi $%d, $%d, %d\n", i, j, code->u.double_op.op2->u.val);
			fputs(cmd, fp);
		}
		else
		{
			char* name2 = printOperand(code->u.double_op.op2);
			memoryToRegs(name2, k);
			sprintf(cmd, "\tadd $%d, $%d, $%d\n", i, j, k);
			fputs(cmd, fp);
		}
		regsToMemory(i);
	}
	//其他情况，左边肯定不会是address, 右边可能出现address
	else if(code->u.double_op.op1->kind == ADDRESS && code->u.double_op.op2->kind == ADDRESS)
	{//对op1 op2解引用
		char* name1 = printOperand(code->u.double_op.op1);
		char* name2 = printOperand(code->u.double_op.op2);
		memoryToRegs(name1, j);
		memoryToRegs(name2, k);
		sprintf(cmd, "\tlw $%d, 0($%d)\n", j, j);
		fputs(cmd, fp);
		sprintf(cmd, "\tlw $%d, 0($%d)\n", k, k);
		fputs(cmd, fp);
		sprintf(cmd, "\tadd $%d, $%d, $%d\n", i, j, k);
		fputs(cmd, fp);
		regsToMemory(i);
	}
	else if(code->u.double_op.op1->kind == ADDRESS && code->u.double_op.op2->kind != ADDRESS)
	{//对op1解引用
		char* name1 = printOperand(code->u.double_op.op1);
		memoryToRegs(name1, j);
		sprintf(cmd, "\tlw $%d, 0($%d)\n", j, j);
		fputs(cmd, fp);
		if(code->u.double_op.op2->kind == CONSTANT)
		{
			sprintf(cmd, "\taddi $%d, $%d, %d\n", i, j, code->u.double_op.op2->u.val);
			fputs(cmd, fp);
		}
		else
		{
			char* name2 = printOperand(code->u.double_op.op2);
			memoryToRegs(name2, k);
			sprintf(cmd, "\tadd $%d, $%d, $%d\n", i, j, k);
			fputs(cmd, fp);
		}
		regsToMemory(i);
	}
	else if(code->u.double_op.op1->kind != ADDRESS && code->u.double_op.op2->kind == ADDRESS)
	{//对op2解引用
		char* name2 = printOperand(code->u.double_op.op2);
		memoryToRegs(name2, k);
		sprintf(cmd, "\tlw $%d, 0($%d)\n", k, k);
		fputs(cmd, fp);
		if(code->u.double_op.op1->kind == CONSTANT)
		{
			sprintf(cmd, "\taddi $%d, $%d, %d\n", i, k, code->u.double_op.op1->u.val);
			fputs(cmd, fp);
		}
		else
		{
			char* name1 = printOperand(code->u.double_op.op1);
			memoryToRegs(name1, j);
			sprintf(cmd, "\tadd $%d, $%d, $%d\n", i, j, k);
			fputs(cmd, fp);
		}
		regsToMemory(i);
	}
	else if(code->u.double_op.op1->kind != ADDRESS && code->u.double_op.op2->kind != ADDRESS)
	{//不需要解引用
		if(code->u.double_op.op1->kind == CONSTANT)
		{
			sprintf(cmd, "\tli $%d, %d\n", j, code->u.double_op.op1->u.val);
			fputs(cmd, fp);
		}
		else
		{
			char* name1 = printOperand(code->u.double_op.op1);
			memoryToRegs(name1, j);
		}
		if(code->u.double_op.op2->kind == CONSTANT)
		{
			sprintf(cmd, "\tli $%d, %d\n", k, code->u.double_op.op2->u.val);
			fputs(cmd, fp);
		}
		else
		{
			char* name2 = printOperand(code->u.double_op.op2);
			memoryToRegs(name2, k);
		}
		sprintf(cmd, "\tadd $%d, $%d, $%d\n", i, j, k);
		fputs(cmd, fp);
		regsToMemory(i);
	}
}

static void make_SUB(interCode code)
{
	char cmd[256];
	memset(cmd, 0, 256);
	int i = 5, j = 6, k = 7;
	char* name = printOperand(code->u.double_op.res);
	memoryToRegs(name, i);
	//左边是普通变量,右边可能出现address
	if(code->u.double_op.op1->kind == ADDRESS && code->u.double_op.op2->kind == ADDRESS)
	{//对op1 op2解引用
		char* name1 = printOperand(code->u.double_op.op1);
		char* name2 = printOperand(code->u.double_op.op2);
		memoryToRegs(name1, j);
		memoryToRegs(name2, k);
		sprintf(cmd, "\tlw $%d, 0($%d)\n", j, j);
		fputs(cmd, fp);
		sprintf(cmd, "\tlw $%d, 0($%d)\n", k, k);
		fputs(cmd, fp);
		sprintf(cmd, "\tsub $%d, $%d, $%d\n", i, j, k);
		fputs(cmd, fp);
		regsToMemory(i);
	}
	else if(code->u.double_op.op1->kind == ADDRESS && code->u.double_op.op2->kind != ADDRESS)
	{//对op1解引用
		char* name1 = printOperand(code->u.double_op.op1);
		memoryToRegs(name1, j);
		sprintf(cmd, "\tlw $%d, 0($%d)\n", j, j);
		fputs(cmd, fp);
		if(code->u.double_op.op2->kind == CONSTANT)
		{
			sprintf(cmd, "\taddi $%d, $%d, %d\n", i, j, -code->u.double_op.op2->u.val);
			fputs(cmd, fp);
		}
		else
		{
			char* name2 = printOperand(code->u.double_op.op2);
			memoryToRegs(name2, k);
			sprintf(cmd, "\tsub $%d, $%d, $%d\n", i, j, k);
			fputs(cmd, fp);
		}
		regsToMemory(i);
	}
	else if(code->u.double_op.op1->kind != ADDRESS && code->u.double_op.op2->kind == ADDRESS)
	{//对op2解引用
		char* name2 = printOperand(code->u.double_op.op2);
		memoryToRegs(name2, k);
		sprintf(cmd, "\tlw $%d, 0($%d)\n", k, k);
		fputs(cmd, fp);
		char* name1 = printOperand(code->u.double_op.op1);
		memoryToRegs(name1, j);
		sprintf(cmd, "\tsub $%d, $%d, $%d\n", i, j, k);
		fputs(cmd, fp);
		regsToMemory(i);
	}
	else if(code->u.double_op.op1->kind != ADDRESS && code->u.double_op.op2->kind != ADDRESS)
	{//不需要解引用
		if(code->u.double_op.op1->kind == CONSTANT)
		{
			sprintf(cmd, "\tli $%d, %d\n", j, code->u.double_op.op1->u.val);
			fputs(cmd, fp);
		}
		else
		{
			char* name1 = printOperand(code->u.double_op.op1);
			memoryToRegs(name1, j);
		}
		if(code->u.double_op.op2->kind == CONSTANT)
		{
			sprintf(cmd, "\tli $%d, %d\n", k, code->u.double_op.op2->u.val);
			fputs(cmd, fp);
		}
		else
		{
			char* name2 = printOperand(code->u.double_op.op2);
			memoryToRegs(name2, k);
		}
		sprintf(cmd, "\tsub $%d, $%d, $%d\n", i, j, k);
		fputs(cmd, fp);
		regsToMemory(i);
	}
}

static void make_MUL(interCode code)
{
	char cmd[256];
	memset(cmd, 0, 256);
	int i = 5, j = 6, k = 7;
	char* name = printOperand(code->u.double_op.res);
	memoryToRegs(name, i);
	//左边是普通变量,右边可能出现address
	if(code->u.double_op.op1->kind == ADDRESS && code->u.double_op.op2->kind == ADDRESS)
	{//对op1 op2解引用
		char* name1 = printOperand(code->u.double_op.op1);
		char* name2 = printOperand(code->u.double_op.op2);
		memoryToRegs(name1, j);
		memoryToRegs(name2, k);
		sprintf(cmd, "\tlw $%d, 0($%d)\n", j, j);
		fputs(cmd, fp);
		sprintf(cmd, "\tlw $%d, 0($%d)\n", k, k);
		fputs(cmd, fp);
		sprintf(cmd, "\tmul $%d, $%d, $%d\n", i, j, k);
		fputs(cmd, fp);
		regsToMemory(i);
	}
	else if(code->u.double_op.op1->kind == ADDRESS && code->u.double_op.op2->kind != ADDRESS)
	{//对op1解引用
		char* name1 = printOperand(code->u.double_op.op1);
		memoryToRegs(name1, j);
		sprintf(cmd, "\tlw $%d, 0($%d)\n", j, j);
		fputs(cmd, fp);
		if(code->u.double_op.op2->kind == CONSTANT)
		{
			sprintf(cmd, "\tli $%d, %d\n", k, code->u.double_op.op2->u.val);
			fputs(cmd, fp);
		}
		else
		{
			char* name2 = printOperand(code->u.double_op.op2);
			memoryToRegs(name2, k);
		}
		sprintf(cmd, "\tmul $%d, $%d, $%d\n", i, j, k);
		fputs(cmd, fp);
		regsToMemory(i);
	}
	else if(code->u.double_op.op1->kind != ADDRESS && code->u.double_op.op2->kind == ADDRESS)
	{//对op2解引用
		char* name2 = printOperand(code->u.double_op.op2);
		memoryToRegs(name2, k);
		sprintf(cmd, "\tlw $%d, 0($%d)\n", k, k);
		fputs(cmd, fp);
		if(code->u.double_op.op1->kind == CONSTANT)
		{
			sprintf(cmd, "\tli $%d, %d\n", j, code->u.double_op.op1->u.val);
			fputs(cmd, fp);
		}
		else
		{
			char* name1 = printOperand(code->u.double_op.op1);
			memoryToRegs(name1, j);
		}
		sprintf(cmd, "\tmul $%d, $%d, $%d\n", i, j, k);
		fputs(cmd, fp);
		regsToMemory(i);
	}
	else if(code->u.double_op.op1->kind != ADDRESS && code->u.double_op.op2->kind != ADDRESS)
	{//不需要解引用
		if(code->u.double_op.op1->kind == CONSTANT)
		{
			sprintf(cmd, "\tli $%d, %d\n", j, code->u.double_op.op1->u.val);
			fputs(cmd, fp);
		}
		else
		{
			char* name1 = printOperand(code->u.double_op.op1);
			memoryToRegs(name1, j);
		}
		if(code->u.double_op.op2->kind == CONSTANT)
		{
			sprintf(cmd, "\tli $%d, %d\n", k, code->u.double_op.op2->u.val);
			fputs(cmd, fp);
		}
		else
		{
			char* name2 = printOperand(code->u.double_op.op2);
			memoryToRegs(name2, k);
		}
		sprintf(cmd, "\tmul $%d, $%d, $%d\n", i, j, k);
		fputs(cmd, fp);
		regsToMemory(i);
	}
	
}

static void make_DIV(interCode code)
{
	char cmd[256];
	memset(cmd, 0, 256);
	int i = 5, j = 6, k = 7;
	char* name = printOperand(code->u.double_op.res);
	memoryToRegs(name, i);
	//左边是普通变量,右边可能出现address
	if(code->u.double_op.op1->kind == ADDRESS && code->u.double_op.op2->kind == ADDRESS)
	{//对op1 op2解引用
		char* name1 = printOperand(code->u.double_op.op1);
		char* name2 = printOperand(code->u.double_op.op2);
		memoryToRegs(name1, j);
		memoryToRegs(name2, k);
		sprintf(cmd, "\tlw $%d, 0($%d)\n", j, j);
		fputs(cmd, fp);
		sprintf(cmd, "\tlw $%d, 0($%d)\n", k, k);
		fputs(cmd, fp);
		sprintf(cmd, "\tdiv $%d, $%d\n", j, k);
		fputs(cmd, fp);
		sprintf(cmd, "\tmflo $%d\n", i);
		fputs(cmd, fp);
		regsToMemory(i);
	}
	else if(code->u.double_op.op1->kind == ADDRESS && code->u.double_op.op2->kind != ADDRESS)
	{//对op1解引用
		char* name1 = printOperand(code->u.double_op.op1);
		memoryToRegs(name1, j);
		sprintf(cmd, "\tlw $%d, 0($%d)\n", j, j);
		fputs(cmd, fp);
		if(code->u.double_op.op2->kind == CONSTANT)
		{
			sprintf(cmd, "\tli $%d, %d\n", k, code->u.double_op.op2->u.val);
			fputs(cmd, fp);
		}
		else
		{
			char* name2 = printOperand(code->u.double_op.op2);
			memoryToRegs(name2, k);
		}
		sprintf(cmd, "\tdiv $%d, $%d\n", j, k);
		fputs(cmd, fp);
		sprintf(cmd, "\tmflo $%d\n", i);
		fputs(cmd, fp);
		regsToMemory(i);
	}
	else if(code->u.double_op.op1->kind != ADDRESS && code->u.double_op.op2->kind == ADDRESS)
	{//对op2解引用
		char* name2 = printOperand(code->u.double_op.op2);
		memoryToRegs(name2, k);
		sprintf(cmd, "\tlw $%d, 0($%d)\n", k, k);
		fputs(cmd, fp);
		if(code->u.double_op.op1->kind == CONSTANT)
		{
			sprintf(cmd, "\tli $%d, %d\n", j, code->u.double_op.op1->u.val);
			fputs(cmd, fp);
		}
		else
		{
			char* name1 = printOperand(code->u.double_op.op1);
			memoryToRegs(name1, j);
		}
		sprintf(cmd, "\tdiv $%d, $%d\n", j, k);
		fputs(cmd, fp);
		sprintf(cmd, "\tmflo $%d\n", i);
		fputs(cmd, fp);
		regsToMemory(i);
	}
	else if(code->u.double_op.op1->kind != ADDRESS && code->u.double_op.op2->kind != ADDRESS)
	{//不需要解引用
		if(code->u.double_op.op1->kind == CONSTANT)
		{
			sprintf(cmd, "\tli $%d, %d\n", j, code->u.double_op.op1->u.val);
			fputs(cmd, fp);
		}
		else
		{
			char* name1 = printOperand(code->u.double_op.op1);
			memoryToRegs(name1, j);
		}
		if(code->u.double_op.op2->kind == CONSTANT)
		{
			sprintf(cmd, "\tli $%d, %d\n", k, code->u.double_op.op2->u.val);
			fputs(cmd, fp);
		}
		else
		{
			char* name2 = printOperand(code->u.double_op.op2);
			memoryToRegs(name2, k);
		}
		sprintf(cmd, "\tdiv $%d, $%d\n", j, k);
		fputs(cmd, fp);
		sprintf(cmd, "\tmflo $%d\n", i);
		fputs(cmd, fp);
		regsToMemory(i);
	}
}

static void make_IFGOTO(interCode code)
{//使用5 6号寄存器
	char cmd[256];
	memset(cmd, 0, 256);
	int i = 5, j = 6;
	if(code->u.triple_op.op1->kind == ADDRESS && code->u.triple_op.op2->kind == ADDRESS)
	{//对op1 op2解引用
		char* name1 = printOperand(code->u.triple_op.op1);
		char* name2 = printOperand(code->u.triple_op.op2);
		memoryToRegs(name1, 5);
		memoryToRegs(name2, 6);
		sprintf(cmd, "\tlw $5, 0($5)\n");
		fputs(cmd, fp);
		sprintf(cmd, "\tlw $6, 0($6)\n");
		fputs(cmd, fp);
	}
	else if(code->u.triple_op.op1->kind == ADDRESS && code->u.triple_op.op2->kind != ADDRESS)
	{//对op1解引用
		char* name1 = printOperand(code->u.triple_op.op1);
		memoryToRegs(name1, 5);
		sprintf(cmd, "\tlw $5, 0($5)\n");
		fputs(cmd, fp);
		if(code->u.triple_op.op2->kind == CONSTANT)
		{
			sprintf(cmd, "\tli $6, %d\n", code->u.triple_op.op2->u.val);
			fputs(cmd, fp);
		}
		else
		{
			char* name2 = printOperand(code->u.triple_op.op2);
			memoryToRegs(name2, 6);
		}
	}
	else if(code->u.triple_op.op1->kind != ADDRESS && code->u.triple_op.op2->kind == ADDRESS)
	{//对op2解引用
		char* name2 = printOperand(code->u.triple_op.op2);
		memoryToRegs(name2, 6);
		sprintf(cmd, "\tlw $6, 0($6)\n");
		fputs(cmd, fp);
		if(code->u.triple_op.op1->kind == CONSTANT)
		{
			sprintf(cmd, "\tli $5, %d\n", code->u.triple_op.op1->u.val);
			fputs(cmd, fp);
		}
		else
		{
			char* name1 = printOperand(code->u.triple_op.op1);
			memoryToRegs(name1, 5);
		}
	}
	else if(code->u.triple_op.op1->kind != ADDRESS && code->u.triple_op.op2->kind != ADDRESS)
	{//不需要解引用
		if(code->u.triple_op.op1->kind == CONSTANT)
		{
			sprintf(cmd, "\tli $5, %d\n", code->u.triple_op.op1->u.val);
			fputs(cmd, fp);
		}
		else
		{
			char* name1 = printOperand(code->u.triple_op.op1);
			memoryToRegs(name1, 5);
		}
		if(code->u.triple_op.op2->kind == CONSTANT)
		{
			sprintf(cmd, "\tli $6, %d\n", code->u.triple_op.op2->u.val);
			fputs(cmd, fp);
		}
		else
		{
			char* name2 = printOperand(code->u.triple_op.op2);
			memoryToRegs(name2, 6);
		}
	}
	//通过上述处理得到存储操作数的寄存器编码, 之后解析relop
	if(strcmp(code->u.triple_op.relop, "==") == 0)
	{//beq $1, $2, label
		sprintf(cmd, "\tbeq $5, $6, label%d\n", code->u.triple_op.label->u.var_no);
		fputs(cmd, fp);
	}
	else if(strcmp(code->u.triple_op.relop, "!=") == 0)
	{//bne $1, $2, label
		sprintf(cmd, "\tbne $5, $6, label%d\n", code->u.triple_op.label->u.var_no);
		fputs(cmd, fp);
	}
	else if(strcmp(code->u.triple_op.relop, ">") == 0)
	{//bgt $1, $2, label
		sprintf(cmd, "\tbgt $5, $6, label%d\n", code->u.triple_op.label->u.var_no);
		fputs(cmd, fp);
	}
	else if(strcmp(code->u.triple_op.relop, ">=") == 0)
	{//bge $1, $2, label
		sprintf(cmd, "\tbge $5, $6, label%d\n", code->u.triple_op.label->u.var_no);
		fputs(cmd, fp);
	}
	else if(strcmp(code->u.triple_op.relop, "<") == 0)
	{//blt $1, $2, label
		sprintf(cmd, "\tblt $5, $6, label%d\n", code->u.triple_op.label->u.var_no);
		fputs(cmd, fp);
	}
	else if(strcmp(code->u.triple_op.relop, "<=") == 0)
	{//ble $1, $2, label
		sprintf(cmd, "\tble $5, $6, label%d\n", code->u.triple_op.label->u.var_no);
		fputs(cmd, fp);
	}
	if(code->u.triple_op.op1->kind != CONSTANT)
		regsToMemory(i);
	if(code->u.triple_op.op2->kind != CONSTANT)
		regsToMemory(j);
}

static void make_DEC(interCode code)
{
	char *name = printOperand(code->u.dec.op);
	int size = code->u.dec.size;
	char cmd[256];
	memset(cmd, 0, 256);
	offset -= size;
	Data data = (Data)malloc(sizeof(struct Data_));
	data->name = name;
	data->offset = offset;
	data->next = NULL;
	putData(data);
	sprintf(cmd, "\taddi $sp, $sp, -%d\n", size);
	fputs(cmd, fp);
}

static void make_CALL(interCode code)
{
	/*处理流程: 
	*栈指针-4 : addi $sp, $sp, -4
	*保存$ra到栈 : sw $ra, 0($sp);
	*跳转 : jal xxx (猜测jal指令会自己填充$ra)
	*从栈中读取$ra : lw $ra, 0($sp)
	*栈指针+4 : addi $s, $sp, 4
	*返回值赋值给变量 : move $xxx, $v0 
	*			(这一句不是必需的,产生这句的原因是:b = call xxx, a = b,这句话用于给a赋值)
	*/
	char cmd[256];
	memset(cmd, 0, 256);
	sprintf(cmd, "\taddi $sp, $sp, -4\n");
	fputs(cmd, fp);
	offset -= 4;
	sprintf(cmd, "\tsw $ra, 0($sp)\n");
	fputs(cmd, fp);
	sprintf(cmd, "\tjal %s\n", code->u.call.fun->u.name);
	fputs(cmd, fp);
	for (int i = 5; i <= 25; i++)
		regs[i].name = NULL;
	sprintf(cmd, "\tlw $ra, 0($sp)\n");
	fputs(cmd, fp);
	sprintf(cmd, "\taddi $sp, $sp, 4\n");
	fputs(cmd, fp);
    offset += 4;
	if (code->u.call.op != NULL)
	{
		char* name = printOperand(code->u.call.op);
		int i = 5;
		memoryToRegs(name, 5);
		sprintf(cmd, "\tmove $%d, $v0\n", i);
		fputs(cmd, fp);
		regsToMemory(i);
	}
}











