#ifndef SEMTABLE_H
#define SEMTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define NAME_SIZE 64
#define STACK_SIZE 1024
#define HASH_SIZE 0x3fff
#define STRUCT_SIZE 0x3fff


typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct Structure_* Structure;
typedef struct Function_* Function;
struct Type_
{
	enum { BASIC, ARRAY, STRUCTURE, FUNCTION } kind;
	union
	{
		enum { VOID_TYPE, INT_TYPE, FLOAT_TYPE } basic;
		struct { Type elem; int size; } array;
		Structure structure;
		Function function;
	} u;
};
struct FieldList_ //用于描述结构体内部类型和函数形参类型
{
	char* name;
	Type type;
	FieldList tail;
};
struct Structure_ //结构体类型
{
	char* name;//结构体名字
	FieldList content;
};
struct Function_ //函数类型
{
	char* name;
	Type retType;
	FieldList paraType;
};

typedef struct SymNode SymNode;
typedef struct SymNode* SymList;
typedef struct Table_* Table;

struct SymNode //十字链表的节点
{
	char name[NAME_SIZE];
	Type type;
	SymList down;
	SymList right;
};

struct Table_ //符号表
{
	SymList stack;//横向的栈
	SymList hashTable;//纵向的哈希表
	int stackTop;//栈顶
	Structure* structTable;
	int structTop;
};

unsigned int hash_pjw(char* name);//哈希函数
void initTable();//初始化符号表
void push();//遇到花括号时，执行一次，
void pop();//结束一层花括号时，执行一次
int putSym(SymList info);//插入符号
SymList getSym(char* name);//寻找符号

int putStruct(Structure structure);//保存新发现的结构体定义
Structure getStruct(char* name);//获取名称为name的结构体

int isTypeSame(Type a, Type b);//比较两个类型是否相同

#endif