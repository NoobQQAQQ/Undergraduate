#define _CRT_SECURE_NO_WARNINGS
#define NameLen 21 // 假设城市名不超过10个汉字
typedef unsigned int uint;
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

struct City // 定义城市结构
{
	uint id;
	char cityname[NameLen];
	uint price;
};

int CITY_NUM;

struct CityNode // 路线上的城市结点
{
	City *city; // 指向某个城市
	CityNode *next; // 指向路线上的下一个城市结点
};

CityNode *Route = (CityNode*) malloc(sizeof(CityNode)); // 用带附加头结点的链表存储路线

int GetData(City *&citylist) // 获得城市信息
{
	FILE *fp = fopen("pricelist.txt", "r");
	int count = -1;
	while (!feof(fp))  // 获得城市数量
	{
		uint id, price;
		char cityname[NameLen];
		count++;
		fscanf(fp, "%u\t%s\t%u", &id, &cityname, &price);
	}
	citylist = (City*) malloc(sizeof(City) * count);
	rewind(fp);
	for (int i = 0; i < count; i++) // 获得城市信息
		fscanf(fp, "%u\t%s\t%u", &citylist[i].id, &citylist[i].cityname, &citylist[i].price);
	fclose(fp);
	return count;
}

void ShowMenu() // 打印菜单
{
	puts("********************************************************************");
	puts("                                菜单");
	puts("\t1, 打印旅游城市价格表");
	puts("\t2, 创建旅游线路");
	puts("\t3, 将旅游路线中的某个城市替换为其他的城市");
	puts("\t4, 在旅游路线中的两个城市之间插入新的城市");
	puts("\t5, 删除旅游路线中的某个城市");
	puts("\t6, 调整旅游城市价格表中某个旅游城市价格");
	puts("\t*, 退出程序");
	puts("********************************************************************\n");
	printf("请输入您的操作: ");
}

void PrintPriceTable(City *citylist) // 打印价格表
{
	for (int i = 0; i < CITY_NUM; i++)
		printf("%u\t%s\t%u\n", citylist[i].id, citylist[i].cityname, citylist[i].price);
}

uint CityIndex(char cityname[NameLen], City *citylist) // 根据城市名获得数组地址
{
	for (int i = 0; i < CITY_NUM; i++)
		if (!strcmp(cityname, citylist[i].cityname))
			return i;
	return -1; // 该城市不在城市表中 返回-1
}

CityNode* SearchList(char OriName[NameLen])
{
	CityNode *cur = Route->next;
	while (cur != NULL)
	{
		if (!strcmp(cur->city->cityname, OriName)) // 找到目标城市
			return cur;
		cur = cur->next;
	}
	return NULL; // 未找到目标城市
}

void PrintRoute() // 打印线路并输出价格
{
	CityNode *cur = Route->next;
	uint total = 0;
	while (cur->next != NULL) // 循环到链表倒数第二个结点
	{
		printf("%s-", cur->city->cityname);
		total += cur->city->price;
		cur = cur->next;
	}
	printf("%s\n", cur->city->cityname);
	total += cur->city->price;
	printf("总价格: %u\n", total);
}

void CreateRoute(City *citylist)
{
	if (Route->next != NULL)
	{
		printf("已存在一条旅游线路, 请勿重复创建\n");
		return;
	}
	printf("请输入城市(以空格分割, 换行结束): ");
	char cityname[NameLen] = { 0 }; // 存城市名
	CityNode *cur = Route; 
	while (scanf("%s", cityname)) // 建立链表
	{
		char ch = getchar(); // 判断空格还是回车
		uint index = CityIndex(cityname, citylist); // 根据城市名获得数组地址
		if (index == -1) // 输入不存在的城市
		{
			printf("未开通%s的旅游服务!\n", cityname);
			if (ch == '\n')
				break;
			else
				continue;
		}
		CityNode *Check = SearchList(cityname); // 输入合法的重复城市
		if (Check != NULL) // 输入合法的重复城市
		{
			printf("重复输入了%s, 已为您自动去重\n", cityname);
			if (ch == '\n')
				break;
			else
				continue;
		}
		CityNode *p = (CityNode*)malloc(sizeof(CityNode)); // 新建结点
		p->city = &citylist[index];
		cur->next = p;
		cur = cur->next;
		cur->next = NULL;
		p = NULL;
		if (ch == '\n')
			break;
	}
	if (Route->next != NULL)
	{
		puts("创建成功!");
		printf("旅游线路为: ");
		PrintRoute(); // 打印线路并输出价格
	}
	else
		puts("创建失败!");
}

void ChangeCity(City *citylist)
{
	char OriName[NameLen] = { 0 }; // 原城市名
	char NewName[NameLen] = { 0 }; // 新城市名
	CityNode *OriCity = NULL; // 指向被替换城市
	uint NewIndex = -1; // 新城市在城市表数组的下标
	while (1)
	{
		printf("请输入被替换的城市: ");
		scanf("%s", OriName);
		getchar(); // 抛弃换行符
		OriCity = SearchList(OriName);
		if (OriCity == NULL) // 未找到目标城市
		{
			printf("该城市不在线路中, 请重新输入\n");
			continue;
		}
		else
			break;
	}
	while (1)
	{
		printf("请输入替换城市: ");
		scanf("%s", NewName);
		getchar(); // 抛弃换行符
		if (!strcmp(OriName, NewName)) // 替换城市和被替换城市相同
		{
			printf("该城市与被替换城市相同, 请重新输入\n");
			continue;
		}
		NewIndex = CityIndex(NewName, citylist);
		if (NewIndex == -1) // 新城市不存在
		{
			printf("未开通%s的旅游服务, 请重新输入\n", NewName);
			continue;
		}
		CityNode *NewCity = SearchList(NewName); // 在线路中查找新城市
		if (NewCity != NULL) // 原线路中已有替换城市
		{
			printf("该城市已在线路中, 请重新输入\n");
			continue;
		}
		break;
	}
	OriCity->city = &citylist[NewIndex]; // 指向新城市
	printf("旅游线路更改为: ");
	PrintRoute();
}

void InsertHead(City *citylist)
{
	char NewName[NameLen] = { 0 };
	uint Index = -1;
	CityNode *NewCity = NULL;
	while (1)
	{
		printf("请输入要加入的城市: ");
		scanf("%s", NewName);
		getchar(); // 抛弃换行符
		Index = CityIndex(NewName, citylist);
		if (Index == -1) // 输入不存在的城市
		{
			printf("未开通该城市的旅游服务, 请重新输入\n");
			continue;
		}
		NewCity = SearchList(NewName);
		if (NewCity != NULL) // 输入已有城市
		{
			printf("该城市已在旅游线路中, 请重新输入\n");
			continue;
		}
		break;
	}
	CityNode *ToInsert = (CityNode*)malloc(sizeof(CityNode));
	ToInsert->city = &citylist[Index];
	ToInsert->next = Route->next;
	Route->next = ToInsert;
	printf("旅游线路更改为: ");
	PrintRoute();
}

void InsertMid(City *citylist)
{
	char OriNamePre[NameLen] = { 0 }; // 前一个
	char OriNameLat[NameLen] = { 0 }; // 后一个
	char NewName[NameLen] = { 0 }; // 新插入的城市
	uint NewIndex = -1;
	CityNode *OriCityPre, *OriCityLat;
	OriCityPre = OriCityLat = NULL;
	while (1)
	{
		printf("请输入两个在路线中连续的城市: ");
		scanf("%s", OriNamePre);
		getchar(); // 抛弃空格
		scanf("%s", OriNameLat);
		getchar(); // 抛弃换行符
		OriCityPre = SearchList(OriNamePre);
		OriCityLat = SearchList(OriNameLat);
		if (OriCityPre == NULL)
		{
			printf("前一个城市不在路线中, 请重新输入\n");
			continue;
		}
		if (OriCityLat == NULL)
		{
			printf("后一个城市不在路线中, 请重新输入\n");
			continue;
		}
		if (OriCityPre->next->city->id != OriCityLat->city->id)
		{
			printf("输入了不连续的城市, 请重新输入\n");
			continue;
		}
		break;
	}
	while (1)
	{
		printf("请输入需要插入的城市: ");
		scanf("%s", NewName);
		getchar(); // 抛弃换行符
		NewIndex = CityIndex(NewName, citylist);
		if (NewIndex == -1) // 输入不存在的城市
		{
			printf("未开通该城市的旅游服务, 请重新输入\n");
			continue;
		}
		CityNode *NewCity = SearchList(NewName);
		if (NewCity != NULL) // 输入已有城市
		{
			printf("该城市已在旅游线路中, 请重新输入\n");
			continue;
		}
		break;
	}
	CityNode *ToInsert = (CityNode*)malloc(sizeof(CityNode));
	ToInsert->city = &citylist[NewIndex];
	ToInsert->next = OriCityPre->next;
	OriCityPre->next = ToInsert;
	printf("旅游线路更改为: ");
	PrintRoute();
}

void InsertTail(City *citylist)
{
	char NewName[NameLen] = { 0 };
	uint Index = -1;
	CityNode *NewCity = NULL;
	while (1)
	{
		printf("请输入要加入的城市: ");
		scanf("%s", NewName);
		getchar(); // 抛弃换行符
		Index = CityIndex(NewName, citylist);
		if (Index == -1) // 输入不存在的城市
		{
			printf("未开通该城市的旅游服务, 请重新输入\n");
			continue;
		}
		NewCity = SearchList(NewName);
		if (NewCity != NULL) // 输入已有城市
		{
			printf("该城市已在旅游线路中, 请重新输入\n");
			continue;
		}
		break;
	}
	CityNode *ToInsert = (CityNode*)malloc(sizeof(CityNode));
	ToInsert->city = &citylist[Index];
	ToInsert->next = NULL;
	CityNode *cur = Route->next;
	while (cur->next != NULL)
		cur = cur->next;
	cur->next = ToInsert;
	printf("旅游线路更改为: ");
	PrintRoute();
}

void InsertCity(City *citylist)
{
	char ch; // 控制字符
	printf("请输入新城市插入位置(0:头部 1:两个城市之间 2:尾部): ");
	scanf("%c", &ch);
	getchar(); // 抛弃换行符
	switch (ch)
	{
	case('0'):
		InsertHead(citylist); break;
	case('1'):
		InsertMid(citylist); break;
	case('2'):
		InsertTail(citylist); break;
	default:
		puts("非法输入");
	}
}

void DeleteCity()
{
	char NameToRemove[NameLen] = { 0 };
	while (1)
	{
		printf("请输入需要删除的城市: ");
		scanf("%s", NameToRemove);
		getchar(); // 抛弃换行符
		CityNode *CityToDelete = SearchList(NameToRemove);
		if (CityToDelete == NULL) // 要删除的城市不在线路中
		{
			printf("该城市不在线路中, 请重新输入\n");
			continue;
		}
		else
			break;
	}
	CityNode *cur = Route;
	while (cur->next != NULL)
	{
		if (!strcmp(cur->next->city->cityname, NameToRemove))
		{
			CityNode *CityToRemove = cur->next;
			cur->next = CityToRemove->next;
			free(CityToRemove);
		}
		else
			cur = cur->next;
	}
	if (Route->next == NULL)
	{
		printf("当前旅游线路已清空! 请重新建立旅游线路\n");
		return;
	}
	else
	{
		printf("旅游线路更改为: ");
		PrintRoute();
	}
}

void ChangePrice(City *citylist)
{
	char Name[NameLen] = { 0 };
	uint NewPrice = 0;
	uint Index = -1;
	while (1)
	{
		printf("请输入需要调整价格的城市: ");
		scanf("%s", Name);
		getchar(); // 抛弃换行符
		Index = CityIndex(Name, citylist);
		if (Index == -1) // 该城市不在城市表中
		{
			printf("未开通该城市的旅游服务, 无法改价, 请重新输入\n");
			continue;
		}
		else
			break;
	}
	printf("%s原来的价格为%u, 请输入调整后的价格: ", Name, citylist[Index].price);
	scanf("%u", &NewPrice);
	getchar(); // 抛弃换行符
	citylist[Index].price = NewPrice;
	PrintPriceTable(citylist);
	printf("旅游线路: ");
	PrintRoute();
}

void What2Do(char ch, City *citylist) // 根据ch选择操作
{
	switch (ch)
	{
	case ('1'): // 打印城市价格表
		PrintPriceTable(citylist); break;
	case ('2'): // 创建路线
		CreateRoute(citylist); break;
	case ('3'): // 改变路线中的某个城市
		ChangeCity(citylist); break;
	case ('4'): // 在路线中插入新的城市
		InsertCity(citylist); break;
	case ('5'): // 删除路线中的某个城市
		DeleteCity(); break;
	case ('6'): // 调整城市价格
		ChangePrice(citylist); break;
	case ('*'): // 退出程序
		exit(0);
	default: // 处理非法输入
		puts("INVALID INPUT"); 
	}
	ShowMenu(); // 再次展示菜单
}

int main()
{
	Route->city = NULL; // 初始化路线
	Route->next = NULL; // 初始化路线
	char control; // 控制进行何种操作的字符
	City *citylist;// 城市表
    CITY_NUM = GetData(citylist); // 从文件读取城市表并获得城市数量
	ShowMenu(); // 展示菜单
	while (scanf("%c", &control))
	{
		getchar();
		What2Do(control, citylist); // 选择进行什么操作
	}
}