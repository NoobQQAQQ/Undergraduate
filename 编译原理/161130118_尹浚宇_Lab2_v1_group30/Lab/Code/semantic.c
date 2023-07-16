 #include "semantic.h"

extern Table table;
int s_error = 0; // number of semantic errors
static char anony[3] = "00"; // 用于给匿名结构体命名
// 用于打印错误信息
static void printError(int no, int lineno, char *name)
{
    printf("Error type %d at Line %d: ", no, lineno);
	switch (no)
    {
		case 1 : printf("Undefined variable \"%s\"", name); break;
		case 2 : printf("Undefined function \"%s\"", name); break;
		case 3 : printf("Redefined variable OR duplicate variable name \"%s\"", name); break;
		case 4 : printf("Redefined function \"%s\"", name); break;
		case 5 : printf("Type mismatched for assignment"); break;
		case 6 : printf("The left-hand side of an assignment must be a variable"); break;
		case 7 : printf("Type mismathced for operands OR mismatched type of expression"); break;
		case 8 : printf("Type mismathced for return"); break;
		case 9 : printf("Actual args mismathced for function call"); break;
		case 10: printf("Illegal usage for \"[ ]\""); break;
		case 11: printf("Illegal usage for \"( )\""); break;
		case 12: printf("Non-integer Exp in \"[ ]\""); break;
		case 13: printf("Illegal usage for \".\""); break;
		case 14: printf("Undefined field in struct"); break;
		case 15: printf("Redefined field in struct OR initialization in struct field"); break;
		case 16: printf("Redefined struct OR duplicate struct name \"%s\"", name); break;
		case 17: printf("Undefined struct \"%s\"", name); break;
	}
	printf(".\n");
}

// 声明语义分析所需的本地函数
// High-level Definitions (except Program because it has been declared in .h)
static void ExtDefList(Node *root);
static void ExtDef(Node *root);
static void ExtDecList(Type type, Node *root);

// Specifier
static Type Specifier(Node *root);
static Type StructSpecifier(Node *root);

// Declarators
static FieldList VarDec(Type type, Node *root, int isStruct);
static SymList FunDec(Type type, Node *root);
static FieldList VarList(Node *root);
static FieldList ParamDec(Node *root);

// Statements
static void CompSt(Type type, Node *root);
static void StmtList(Type type, Node *root);
static void Stmt(Type type, Node *root);

// Local Definitions
static FieldList DefList(Node *root, int isStruct);
static FieldList Def(Node *root, int isStruct);
static FieldList DecList(Type type, Node *root, int isStruct);
static FieldList Dec(Type type, Node *root, int isStruct);

// Expressions
static Type Exp(Node *root);
static int Args(Node *root, FieldList paramList);

static void debug(char* str)
{
	printf("%s",str);
}
// 实现语义分析所需要的所有函数
void Program(Node *root)
{
	assert(root != NULL);
    initTable(); // initial SymTable here
	// Program -> ExtDefList
	ExtDefList(root->child);
}

static void ExtDefList(Node *root)
{
	assert(root != NULL);
    // ExtDefList -> epsilon
	if (root->child == NULL)
		return;
	// ExtDefList -> ExtDef ExtDefList
	ExtDef(root->child);
	ExtDefList(root->child->brother);
}

static void ExtDef(Node *root)
{
    assert(root != NULL);
    Node *specifier = root->child;
    Type type = Specifier(specifier);//分析第一个分量
    root = specifier->brother;//root代表着第二个分量
    if(root == NULL)
	{
		debug("wrong in ExtDef : null root\n");
		//assert(0);
	}
    // ExtDef -> Specifier ExtDecList SEMI
    if (!strcmp(root->type, "ExtDecList"))
    {
		ExtDecList(type, root);
	}
    // ExtDef -> Specifier SEMI
    else if (!strcmp(root->type, "SEMI"))
    {
		return;
	}
    // ExtDef -> Specifier FunDec CompSt
    else if (!strcmp(root->type, "FunDec"))//函数定义
    {
        //push();
        SymList f = FunDec(type, root);
        assert(f != NULL);
        //CompSt(type, root->brother);
        //pop();     
        if (!putSym(f))
        {
            printError(4, root->lineno, f->name);
            s_error++;
        }
		else{
				//printf("---%s---\n",f->name);
		}
		push();
		CompSt(type, root->brother);
		pop();
    }
}

static void ExtDecList(Type type, Node *root)
{
    assert(root != NULL);
    Node *child = root->child;//child是VarDec
	int isStruct = 0;
	if(type->kind == STRUCTURE)
		isStruct = 1;
    VarDec(type, child, isStruct); 
    // ExtDecList -> VarDec COMMA ExtDecList
    if (child->brother != NULL)//child->brother是COMMA
        ExtDecList(type, child->brother->brother);
}

static Type Specifier(Node *root)
{
    assert(root != NULL);
    Node *child = root->child;
    Type type;

    // Specifier -> TYPE
    if (strcmp(child->type, "TYPE") == 0)//当前树结点存储的是词法单元TYPE，来源于正则表达式int|float
    {
        type = (Type)malloc(sizeof(struct Type_));
        type->kind = BASIC;
        if(!strcmp(child->info.name, "int"))
			type->u.basic = INT_TYPE;
		else
			type->u.basic = FLOAT_TYPE;
    }
    // Specifier -> StructSpecifier
    else if(strcmp(child->type, "StructSpecifier") == 0)
    {    
		type = StructSpecifier(child);
		//debug("---struct---\n");
	}
	else
	{
		debug("wrong in Specifier : unknown child!\n");
		//assert(0);
	}
    return type;
}

static Type StructSpecifier(Node *root)
{
    assert(root != NULL);
	if(strcmp(root->child->type,"STRUCT") != 0)
	{
		debug("wrong in StructSpecifier : ? STRUCT ?\n");
		//asseert(0);
	}
    root = root->child->brother;//root是OptTag或Tag
    // StructSpecifier -> STRUCT OptTag LC DefList RC
    if (!strcmp(root->type, "OptTag"))//结构体定义
    {
        Type type = (Type)malloc(sizeof(struct Type_));
		Structure newStr = (Structure)malloc(sizeof(struct Structure_));
        type->kind = STRUCTURE;
        type->u.structure = newStr;
        // OptTag -> epsilon
        if (root->child == NULL)//匿名结构体
        {
            newStr->name = (char*)malloc(sizeof(char) * 3);
            strcpy(newStr->name, anony);
            anony[1]++;
        }
        // OptTag -> ID
        else if(strcmp(root->child->type, "ID")==0)
            newStr->name = root->child->info.name;
		else
		{
			debug("wrong in StructSpecifier :  unknown OptTag\n");
		//assert(0);
		}
        // 将结构体定义插入结构体表中
        Node *temp = root;    
        root = root->brother->brother;//root是DefList，跨过了LC{
        push();
        newStr->content = DefList(root, 1);//得到结构体内部各变量组成的链表
        pop();
        SymList symChecker = getSym(newStr->name);
		Structure strChecker = getStruct(newStr->name);
		if (symChecker == NULL && strChecker == NULL) 
        {
			putStruct(newStr);
			return type;
		}
        else 
        {
			printError(16, temp->lineno, newStr->name);
			s_error++;
			return NULL;
		}
    }
    // StructSpecifier -> STRUCT Tag
    else if (!strcmp(root->type, "Tag"))//root是Tag，Tag是一个结构体的名字
    {
        Structure strChecker = getStruct(root->child->info.name);//root->child 是 ID
        if (strChecker == NULL)
        {
            printError(17, root->lineno, root->child->info.name);
            s_error++;
            return NULL;
        }
        else
        {
            Type type = (Type)malloc(sizeof(struct Type_));
			type->kind = STRUCTURE;
			type->u.structure = strChecker;
			return type;
        }        
    }
	else
	{	
		debug("wrong in StructSpecifier : not OptTag or Tag\n");
		//assert(0);
	}
}

static FieldList VarDec(Type type, Node *root, int isStruct)
{
    assert(root != NULL);
    Node *child = root->child;
    
    // VarDec -> ID
    if (!strcmp(child->type, "ID"))//发现一个新符号
    {
        SymList newSym = (SymList)malloc(sizeof(SymNode));
        FieldList newVar = (FieldList)malloc(sizeof(struct FieldList_));
		Type newType = (Type)malloc(sizeof(struct Type_));
		if(type != NULL)
			memcpy(newType, type, sizeof(struct Type_));
		else
		{
			newType->kind = BASIC;
			newType->u.basic = VOID_TYPE;
		}
        strcpy(newSym->name, child->info.name);
		//newSym->name = child->child->info.name;
        newSym->type = newType;
        newVar->name = child->info.name;
        newVar->type = newType;
		newVar->tail = NULL;

        if (!putSym(newSym))
        {
            if (isStruct)
			{
				//debug("VarDec\n");
                printError(15, child->lineno, newSym->name);
			}
            else
                printError(3, child->lineno, newSym->name);
            s_error++;
            return NULL;
        }
        return newVar;
    }
    // VarDec -> VarDec LB INT RB
    else if (!strcmp(child->type, "VarDec"))
    {
		/*int dimension = child->brother->brother->info.intVal;
		Type elem = malloc(sizeof(Type));
        elem->kind = ARRAY;//填充elem
        elem->u.array.elem = type;//填充elem
		elem->u.array.size = dimension;
		FieldList newVar = VarDec(type, child, isStruct);
		return newVar;*/
        FieldList newVar = VarDec(type, child, isStruct);
		if(newVar == NULL) 
			return NULL;
        int dimension = child->brother->brother->info.intVal;//维度INT
        if (newVar->type->kind != ARRAY)//一维数组
        {
            Type elem = malloc(sizeof(Type));
            elem->kind = newVar->type->kind;//填充elem
            elem->u = newVar->type->u;//填充elem

            newVar->type->kind = ARRAY;
            newVar->type->u.array.elem = elem;
            newVar->type->u.array.size = dimension;
        }
        else//多维数组
        {
            Type elem = malloc(sizeof(Type));
			elem->kind = ARRAY;
            elem->u.array.size = newVar->type->u.array.size;
			elem->u.array.elem = newVar->type->u.array.elem;
        
            newVar->type->u.array.size = dimension;
            newVar->type->u.array.elem = elem;
        }
        return newVar;
    }
	else
	{
		debug("wrong in VarDec : not ID or VarDec\n");
		//assert(0);
	}
}

static SymList FunDec(Type type, Node *root)
{
    assert(root != NULL);
    Node *child = root->child;
    // create new function symbol here
    SymList newSym = (SymList)malloc(sizeof(SymNode));
    strcpy(newSym->name, child->info.name);
	//newSym->name = child->info.name;
    newSym->type = (Type)malloc(sizeof(struct Type_));
    newSym->type->kind = FUNCTION;
    newSym->type->u.function = (Function)malloc(sizeof(struct Function_));
    newSym->type->u.function->name = child->info.name;
    newSym->type->u.function->retType = type;

    child = child->brother->brother;//child = VarList 或 RP
    // FunDec -> ID LP RP
    if (!strcmp(child->type, "RP"))
        newSym->type->u.function->paraType = NULL;
    // FunDec -> ID LP VarList RP
    else if(!strcmp(child->type, "VarList"))
        newSym->type->u.function->paraType = VarList(child);
	else
	{
		debug("wrong in FunDec : unknown para\n");
		//assert(0);
	}
    return newSym;
}

static FieldList VarList(Node *root)
{
    assert(root != NULL);
    Node *child = root->child;
	if(child == NULL)
	{
		debug("wrong in VarList : null child\n");
		//assert(0);
	}
    FieldList g1 = ParamDec(child);
    // VarList -> ParamDec
    if (child->brother == NULL)//仅有一项参数
        return g1;
    // VarList -> ParamDec COMMA VarList
    else//多参数
    {
        FieldList g2 = VarList(child->brother->brother);
        if (g1 == NULL)
            return g2;
        else
        {
            FieldList cur = g1;
            while (g1->tail != NULL)
                g1 = g1->tail;
            g1->tail = g2;
            return g1;
        }
    }   
}

static FieldList ParamDec(Node *root)
{
    assert(root != NULL);
    Node *child = root->child;
	if(child == NULL)
	{
		debug("wrong in ParamDec : null child\n");
		//assert(0);
	}
    // ParamDec -> Specifier VarDec
    Type paramType = Specifier(child);
    FieldList param = VarDec(paramType, child->brother, 0);
    return param;
}

static void CompSt(Type type, Node *root)
{
    assert(root != NULL);
    root = root->child->brother;//root = DefList
    // CompSt -> LC DefList StmtList RC
	int isStruct = 0;
	//if(type->kind == STRUCTURE)
		//isStruct = 1;
    DefList(root, isStruct);
    StmtList(type, root->brother);
}

static void StmtList(Type type, Node *root)
{
    assert(root != NULL);
    Node *child = root->child;
    // StmtList -> Stmt StmtList
    if(child != NULL)
    {
        Stmt(type, child);
        StmtList(type, child->brother);
    }
    // StmtList -> epsilon
    else
        return;
}

static void Stmt(Type type, Node *root)
{
    assert(root != NULL);
    Node *child = root->child;
    // Stmt -> Exp SEMI
    if (!strcmp(child->type, "Exp"))
        Exp(child);
    // Stmt -> CompSt
    else if (!strcmp(child->type, "CompSt"))
    {
        push();
		CompSt(type, child);
		pop();
    }
    // Stmt -> RETURN Exp SEMI
    else if (!strcmp(child->type, "RETURN"))
    {
        Type expType = Exp(child->brother);
        if (expType == NULL)
            return;
        if (!isTypeSame(expType, type))
        {
            printError(8, child->lineno, NULL);
            s_error++;
        }
    }
    // Stmt -> IF LP Exp RP Stmt (ELSE Stmt)
    else if (!strcmp(child->type, "IF"))
    {
        child = child->brother->brother;//Exp
        Type expType = Exp(child);
        child = child->brother->brother;//Stmt
        Stmt(type, child);
        // Stmt -> IF LP Exp RP Stmt ELSE Stmt
        if (child->brother != NULL)
            Stmt(type, child->brother->brother);
        // Stmt -> IF LP Exp RP Stmt
    }
    // Stmt -> WHILE LP Exp RP Stmt
    else
    {
        child = child->brother->brother;
        Type expType = Exp(child);
        child = child->brother->brother;
        Stmt(type, child);
    }
}

static FieldList DefList(Node *root, int isStruct)//一系列定义语句
{
    assert(root != NULL);
    Node *child = root->child;//Def
    // DefList -> epsilon
    if (child == NULL)
        return NULL;
    // DefList -> Def DefList
    else
    {
        FieldList g1 = Def(child, isStruct);
        FieldList g2 = DefList(child->brother, isStruct);
        FieldList cur = g1;
        while (cur != NULL && cur->tail != NULL)
            cur = cur->tail;
        if (g1 == NULL)
            g1 = g2;
        else
            cur->tail = g2;
        return g1;
    }
}

static FieldList Def(Node *root, int isStruct)
{
    assert(root != NULL);
    Type type = Specifier(root->child);
	//debug("in def\n");
    return DecList(type, root->child->brother, isStruct);
}

static FieldList DecList(Type type, Node *root, int isStruct)
{
    assert(root != NULL);
    Node *child = root->child;
    FieldList g1 = Dec(type, child, isStruct);
    // DecList -> Dec
    if (child->brother == NULL)
        return g1;
    // DecList -> Dec COMMA DecList
    else
    {
		//debug("in DecList\n");
        FieldList g2 = DecList(type, child->brother->brother, isStruct);
        if (g1 != NULL)
            g1->tail = g2;
        else
            g1 = g2;
        return g1;
    }
}

static FieldList Dec(Type type, Node *root, int isStruct)
{
    assert(root != NULL);
    Node *child = root->child;
    FieldList var = VarDec(type, child, isStruct);
    // Dec -> VarDec
    if (child->brother == NULL)
        return var;
    // Dec -> VarDec ASSIGNOP Exp
    else
    {
        if (isStruct)
        {
			//debug("Dec\n");
            printError(15, child->brother->lineno, NULL);
            s_error++;
            return NULL;
        }
        else
        {
            Type expType = Exp(child->brother->brother);
            if (expType == NULL)
                return NULL;
            if (!isTypeSame(expType, type))
            {
                printError(5, child->brother->brother->lineno, NULL);
                s_error++;
                return NULL;
            }
        } 
    }
    return var;
}

static Type Exp(Node *root)
{
    assert(root != NULL);
    Node *child = root->child;
    // Exp -> Exp .....
    if (!strcmp(child->type, "Exp"))
    {
        // Exp -> Exp ASSIGNOP Exp
        if (!strcmp(child->brother->type, "ASSIGNOP"))
        {
            Type left = Exp(child);
            Type right = Exp(child->brother->brother);
            if (left == NULL || right == NULL)
				return NULL;
            // 判断左表达式是否合法
            // 普通变量
            if (!strcmp(child->child->type, "ID") && child->child->brother == NULL)
            {
                // do nothing
            }
            // 数组访问
            else if(!strcmp(child->child->type, "Exp") && child->child->brother != NULL && !strcmp(child->child->brother->type, "LB"))
            {
                // do nothing
            }
            // 结构体
            else if(!strcmp(child->child->type, "Exp") && child->child->brother != NULL && !strcmp(child->child->brother->type, "DOT"))
            {
                // do nothing
            }
            // 非法左表达式
            else
            {
                printError(6, root->lineno, NULL);
                s_error++;
                return NULL;
            }

            // 判断左右类型是否匹配
            if (isTypeSame(left, right))
                return left;
            else
            {
                printError(5, root->lineno, NULL);
                s_error++;
                return NULL;
            }   
        }
        // Exp -> Exp AND/OR/RELOP/PLUS/MINUS/STAR/DIV Exp
        else if (  !strcmp(child->brother->type, "AND")
                || !strcmp(child->brother->type, "OR")
                || !strcmp(child->brother->type, "RELOP")
                || !strcmp(child->brother->type, "PLUS")
                || !strcmp(child->brother->type, "MINUS")
                || !strcmp(child->brother->type, "STAR")
                || !strcmp(child->brother->type, "DIV") 
        ) 
        {
            Type left = Exp(child);
            Type right = Exp(child->brother->brother);
            if (left == NULL || right == NULL)
				return NULL;
            if (isTypeSame(left, right) && left->kind == BASIC && right->kind == BASIC)
                return left;
            else
            {
                printError(7, root->lineno, NULL);
                s_error++;
                return NULL;
            }    
        }
        // Exp -> Exp LB Exp RB
        else if (!strcmp(child->brother->type, "LB"))
        {
            Type arrayType = Exp(child);
            if (arrayType == NULL)
                return NULL;
            if (arrayType->kind != ARRAY)
            {
                printError(10, child->lineno, NULL);
                s_error++;
                return NULL;
            }
            child = child->brother->brother;
            Type indexType = Exp(child);
            if (indexType == NULL)
				return NULL;
            if (indexType->kind != BASIC || indexType->u.basic != INT_TYPE)
            {
                printError(12, child->lineno, NULL);
                s_error++;
                return NULL;
            }
            return arrayType->u.array.elem;
        }
        // Exp -> Exp DOT ID
        else if (!strcmp(child->brother->type, "DOT"))
        {
            Type strType = Exp(child);
            if (strType == NULL)
                return NULL;
            if (strType->kind != STRUCTURE)
            {
                printError(13, child->lineno, NULL);
                s_error++;
                return NULL;
            }
            FieldList memberList = strType->u.structure->content;
            child = child->brother->brother;
            while (memberList != NULL)
            {
                if (!strcmp(memberList->name, child->info.name))
                    return memberList->type;
                memberList = memberList->tail;
            }
            printError(14, child->lineno, NULL);
            s_error++;
            return NULL;
        }
    }
    // Exp -> LP Exp RP
    else if (!strcmp(child->type, "LP"))
    {
        return Exp(child->brother);
    }
    // Exp -> MINUS Exp
    else if (!strcmp(child->type, "MINUS"))
    {
        Type type = Exp(child->brother);
		if (type == NULL)
			return NULL;
        if (type->kind != BASIC) 
        {
			printError(7, child->brother->lineno, NULL);
			s_error++;
			return NULL;
		}
		return type;
    }
    // Exp -> NOT Exp
    else if (!strcmp(child->type, "NOT"))
    {
        Type type = Exp(child->brother);
		if (type == NULL)
			return NULL;
        if (type->kind != BASIC || type->u.basic != INT_TYPE) 
        {
			printError(7, child->brother->lineno, NULL);
			s_error++;
			return NULL;
		}
		return type;
    }
    // Exp -> ID xxxxx
    else if (!strcmp(child->type, "ID"))
    {
        // Exp -> ID
        if (child->brother == NULL)
        {
            SymList sym = getSym(child->info.name);
            if (sym == NULL)
            {
                printError(1, child->lineno, child->info.name);
                s_error++;
                return NULL;
            }
            return sym->type;
        }
        // Exp -> ID LP....
        else
        {
            SymList checker = getSym(child->info.name);
            if (checker != NULL && checker->type->kind != FUNCTION)
            {
				//printf("kind:%d\n",checker->type->kind);
                printError(11, child->lineno, NULL);
                s_error++;
                return NULL;
            }
            else if (checker == NULL)
            {
                printError(2, child->lineno, child->info.name);
                s_error++;
                return NULL;
            }
            FieldList paramList = checker->type->u.function->paraType;
            child = child->brother->brother;
            // Exp -> ID LP RP
            if (!strcmp(child->type, "RP"))
            {
                if (paramList != NULL)
                {
                    printError(9, root->child->lineno, NULL);
                    s_error++;
                    return NULL;
                }
            }
            // Exp -> ID LP Args RP
            else//child = args
            {
				
                if(Args(child, paramList) == 0)
                {
                    printError(9, root->child->lineno, NULL);
                    s_error++;
                    return NULL;
                }
				else
				{
					//debug("---para match!---\n");
				}
            }
            return checker->type->u.function->retType;
        }
    }
    // Exp -> INT
    else if (!strcmp(child->type, "INT"))
    {
        Type type = (Type)malloc(sizeof(struct Type_));
		type->kind = BASIC;
		type->u.basic = INT_TYPE;
		return type;
    }
    // Exp -> FLOAT
    else if (!strcmp(child->type, "FLOAT"))
    {
        Type type = (Type)malloc(sizeof(struct Type_));
		type->kind = BASIC;
		type->u.basic = FLOAT_TYPE;
		return type;
    }
}   

static int Args(Node *root, FieldList paramList)
{
    assert(root != NULL);
    if (paramList == NULL)
        return 0;
    Node *child = root->child;
    Type typePara = Exp(child);
    if (isTypeSame(typePara, paramList->type))
    {
		//("%d-%d\n",typePara->kind, paramList->type->kind);
        // Args -> Exp
        if (child->brother == NULL)
        {
			//printf("return :%d\n", paramList->tail == NULL);
			return 1;
		}
        // Args -> Exp COMMA Args
        else
		{
            return Args(child->brother->brother, paramList->tail); 
		}
    }
    else
        return 0;
}