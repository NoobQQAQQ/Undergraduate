#include "symTable.h"

Table table;

unsigned int hash_pjw(char* name)//课本给出的哈希函数
{
	unsigned int val = 0, i;
	for (; *name; ++name)
	{
		val = (val << 2) + *name;
		if (i = val & ~HASH_SIZE)
			val = (val ^ (i >> 12))&HASH_SIZE;
	}
	return val;
}
void initTable()//初始化符号表
{
	table = (Table)malloc(sizeof(struct Table_));
	int i = 0;
	//申请栈、哈希表、结构体表，并初始化
	table->stack = (SymList)malloc(sizeof(struct SymNode) * STACK_SIZE);
	table->hashTable = (SymList)malloc(sizeof(struct SymNode) * HASH_SIZE);
	table->structTable = (Structure*)malloc(sizeof(Structure) * STRUCT_SIZE);
	for (i = 0; i < STACK_SIZE; i++)
	{
		table->stack[i].down = NULL;
		table->stack[i].right = NULL;
		table->stack[i].type = NULL;
	}
	for (i = 0; i < HASH_SIZE; i++)
	{
		table->hashTable[i].down = NULL;
		table->hashTable[i].right = NULL;
		table->hashTable[i].type = NULL;
	}
	for (i = 0; i < STRUCT_SIZE; i++)
		table->structTable[i] = NULL;

	//初始化栈顶计数器和结构体表计数器
	table->stackTop = 0;
	table->structTop = 0;
}
void push()
{
	table->stackTop++;
	if(table->stackTop >= STACK_SIZE){
		printf("stack overflow\n");
		assert(0);
	}
}
void pop()
{
	SymList p = table->stack[table->stackTop].down;
	table->stack[table->stackTop].down = NULL;
	//删除栈顶指向的一条链表
	while (p != NULL)
	{
		//先将其在哈希表中除去
		char* name = p->name;
		unsigned int index = hash_pjw(name);
		table->hashTable[index].right = p->right;
		//删除结点
		SymList tmp = p;
		p = p->down;
		free(tmp);
	}
	//更新top
	table->stackTop--;
}
int putSym(SymList sym)//0代表有重复，1代表插入成功
{
	//先检查是否存在重复的符号
	SymList i = table->stack[table->stackTop].down;
	while (i != NULL)
	{
		if (strcmp(i->name, sym->name) == 0)
			return 0;
		i = i->down;
	}
	//继续搜索结构体表
	for (int i = 0; i < table->structTop; i++)
	{
		Structure p = table->structTable[i];
		if (strcmp(p->name, sym->name) == 0)
			return 0;
	}
	//先横向插在哈希表中
	char* name = sym->name;
	unsigned int index = hash_pjw(name);
	SymList p = table->hashTable[index].right;
	sym->down = NULL;
	sym->right = p;
	table->hashTable[index].right = sym;
	//纵向插入，与栈连接
	sym->down = table->stack[table->stackTop].down;
	table->stack[table->stackTop].down = sym;

	return 1;
}
SymList getSym(char* name)
{
	unsigned int index = hash_pjw(name);
	SymList p = table->hashTable[index].right;
	while(p != NULL)
	{
		if (strcmp(p->name, name) == 0)
			return p;
		p = p->right;
	}
	return NULL;
}

int putStruct(Structure structure)//直接将指针加入指针数组即可
{
	if (getStruct(structure->name) != NULL)
		return 0;//重复
	SymList i = table->stack[table->stackTop].down;
	while (i != NULL)
	{
		if (strcmp(i->name, structure->name) == 0)
			return 0;//和sym名重复
		i = i->down;
	}
	table->structTable[table->structTop] = structure;
	table->structTop++;
	if(table->structTop >= STRUCT_SIZE)
	{
		printf("structTable overflow\n");
		assert(0);
	}
	return 1;//添加成功
}
Structure getStruct(char* name)//遍历数组寻找结构体的定义
{
	int i = 0;
	for (i = 0; i < table->structTop; i++)
	{
		Structure p = table->structTable[i];
		if (strcmp(p->name, name) == 0)
			return p;
	}
	return NULL;
}

int isTypeSame(Type a, Type b)//比较两个类型是否相同
{
	if (a == NULL || b == NULL)//空指针无法比较类型是否相同
	{
		return 0;
		//printf("nullptr in isTypeSame!(1)\n");
		//assert(0);
	}
	if (a->kind != b->kind)
		return 0;
	if (a->kind == BASIC)//基础类型的比较
	{
		if (a->u.basic == b->u.basic)
			return 1;
		else
			return 0;
	}
	else if (a->kind == ARRAY)//数组的比较
	{
		//return isTypeSame(a->u.array.elem, b->u.array.elem);
		//使用非递归提高效率
		Type p = a->u.array.elem, q = b->u.array.elem;
		//只有数组类型会引起递归的比较
		while (p !=NULL && q != NULL && p->kind == q->kind && p->kind == ARRAY)
		{
			p = p->u.array.elem;
			q = q->u.array.elem;
		}
		//进行一次比较就可以得知结果
		if (p == NULL || q == NULL)//正常构造的结构体不会出现此情况
		{
			return 0;
			//printf("nullptr in isTypeSame!(2)\n");
			//assert(0);
		}
		//以下是排除数组类型后的类型比较
		if (p->kind != q->kind)
			return 0;
		if (p->kind == BASIC)
		{
			if (p->u.basic == q->u.basic)
				return 1;
			else
				return 0;
		}
		if (p->kind == STRUCTURE)
		{
			if (strcmp(p->u.structure->name, q->u.structure->name) == 0)
				return 1;
			else
				return 0;
		}
		if (p->kind == FUNCTION)//数组不可能是函数的数组
		{
			return 0;
			//printf("funtion array!\n");
			//assert(0);
		}
	}
	else if (a->kind == STRUCTURE)//结构体比较名字
	{
		if (strcmp(a->u.structure->name, b->u.structure->name) == 0)
			return 1;
		else
			return 0;
	}
	else if (a->kind == FUNCTION)//函数类型的比较
	{
		//非递归进行比较
		//比较函数名
		if (strcmp(a->u.function->name, b->u.function->name) != 0)//函数名不同则不同
			return 0;
		//比较返回值类型
		Type p = a->u.function->retType, q = b->u.function->retType;
		int isRetSame = isTypeSame(p, q);//此处只会调用一次，不会多次递归
		if (isRetSame == 0)
			return 0;
		//比较参数类型
		FieldList para1 = a->u.function->paraType, para2 = b->u.function->paraType;
		int isParaSame = 1;
		while (para1 != NULL && para2 != NULL)//循环比较每一个参数
		{
			isParaSame = isParaSame & isTypeSame(para1->type, para2->type);
			para1 = para1->tail;
			para2 = para2->tail;
		}
		if (!(para1 == NULL && para2 == NULL))//此时参数个数不一致，参数不同
			isParaSame = 0;
		return isParaSame;//此时已知函数名和返回值类型都相同
	}
	else
	{
		return 0;
		//printf("undefined kind!\n");
		//assert(0);
	}
}