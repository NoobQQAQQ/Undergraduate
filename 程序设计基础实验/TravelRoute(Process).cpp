#define _CRT_SECURE_NO_WARNINGS
#define NameLen 21 // ���������������10������
typedef unsigned int uint;
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

struct City // ������нṹ
{
	uint id;
	char cityname[NameLen];
	uint price;
};

int CITY_NUM;

struct CityNode // ·���ϵĳ��н��
{
	City *city; // ָ��ĳ������
	CityNode *next; // ָ��·���ϵ���һ�����н��
};

CityNode *Route = (CityNode*) malloc(sizeof(CityNode)); // �ô�����ͷ��������洢·��

int GetData(City *&citylist) // ��ó�����Ϣ
{
	FILE *fp = fopen("pricelist.txt", "r");
	int count = -1;
	while (!feof(fp))  // ��ó�������
	{
		uint id, price;
		char cityname[NameLen];
		count++;
		fscanf(fp, "%u\t%s\t%u", &id, &cityname, &price);
	}
	citylist = (City*) malloc(sizeof(City) * count);
	rewind(fp);
	for (int i = 0; i < count; i++) // ��ó�����Ϣ
		fscanf(fp, "%u\t%s\t%u", &citylist[i].id, &citylist[i].cityname, &citylist[i].price);
	fclose(fp);
	return count;
}

void ShowMenu() // ��ӡ�˵�
{
	puts("********************************************************************");
	puts("                                �˵�");
	puts("\t1, ��ӡ���γ��м۸��");
	puts("\t2, ����������·");
	puts("\t3, ������·���е�ĳ�������滻Ϊ�����ĳ���");
	puts("\t4, ������·���е���������֮������µĳ���");
	puts("\t5, ɾ������·���е�ĳ������");
	puts("\t6, �������γ��м۸����ĳ�����γ��м۸�");
	puts("\t*, �˳�����");
	puts("********************************************************************\n");
	printf("���������Ĳ���: ");
}

void PrintPriceTable(City *citylist) // ��ӡ�۸��
{
	for (int i = 0; i < CITY_NUM; i++)
		printf("%u\t%s\t%u\n", citylist[i].id, citylist[i].cityname, citylist[i].price);
}

uint CityIndex(char cityname[NameLen], City *citylist) // ���ݳ�������������ַ
{
	for (int i = 0; i < CITY_NUM; i++)
		if (!strcmp(cityname, citylist[i].cityname))
			return i;
	return -1; // �ó��в��ڳ��б��� ����-1
}

CityNode* SearchList(char OriName[NameLen])
{
	CityNode *cur = Route->next;
	while (cur != NULL)
	{
		if (!strcmp(cur->city->cityname, OriName)) // �ҵ�Ŀ�����
			return cur;
		cur = cur->next;
	}
	return NULL; // δ�ҵ�Ŀ�����
}

void PrintRoute() // ��ӡ��·������۸�
{
	CityNode *cur = Route->next;
	uint total = 0;
	while (cur->next != NULL) // ѭ�����������ڶ������
	{
		printf("%s-", cur->city->cityname);
		total += cur->city->price;
		cur = cur->next;
	}
	printf("%s\n", cur->city->cityname);
	total += cur->city->price;
	printf("�ܼ۸�: %u\n", total);
}

void CreateRoute(City *citylist)
{
	if (Route->next != NULL)
	{
		printf("�Ѵ���һ��������·, �����ظ�����\n");
		return;
	}
	printf("���������(�Կո�ָ�, ���н���): ");
	char cityname[NameLen] = { 0 }; // �������
	CityNode *cur = Route; 
	while (scanf("%s", cityname)) // ��������
	{
		char ch = getchar(); // �жϿո��ǻس�
		uint index = CityIndex(cityname, citylist); // ���ݳ�������������ַ
		if (index == -1) // ���벻���ڵĳ���
		{
			printf("δ��ͨ%s�����η���!\n", cityname);
			if (ch == '\n')
				break;
			else
				continue;
		}
		CityNode *Check = SearchList(cityname); // ����Ϸ����ظ�����
		if (Check != NULL) // ����Ϸ����ظ�����
		{
			printf("�ظ�������%s, ��Ϊ���Զ�ȥ��\n", cityname);
			if (ch == '\n')
				break;
			else
				continue;
		}
		CityNode *p = (CityNode*)malloc(sizeof(CityNode)); // �½����
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
		puts("�����ɹ�!");
		printf("������·Ϊ: ");
		PrintRoute(); // ��ӡ��·������۸�
	}
	else
		puts("����ʧ��!");
}

void ChangeCity(City *citylist)
{
	char OriName[NameLen] = { 0 }; // ԭ������
	char NewName[NameLen] = { 0 }; // �³�����
	CityNode *OriCity = NULL; // ָ���滻����
	uint NewIndex = -1; // �³����ڳ��б�������±�
	while (1)
	{
		printf("�����뱻�滻�ĳ���: ");
		scanf("%s", OriName);
		getchar(); // �������з�
		OriCity = SearchList(OriName);
		if (OriCity == NULL) // δ�ҵ�Ŀ�����
		{
			printf("�ó��в�����·��, ����������\n");
			continue;
		}
		else
			break;
	}
	while (1)
	{
		printf("�������滻����: ");
		scanf("%s", NewName);
		getchar(); // �������з�
		if (!strcmp(OriName, NewName)) // �滻���кͱ��滻������ͬ
		{
			printf("�ó����뱻�滻������ͬ, ����������\n");
			continue;
		}
		NewIndex = CityIndex(NewName, citylist);
		if (NewIndex == -1) // �³��в�����
		{
			printf("δ��ͨ%s�����η���, ����������\n", NewName);
			continue;
		}
		CityNode *NewCity = SearchList(NewName); // ����·�в����³���
		if (NewCity != NULL) // ԭ��·�������滻����
		{
			printf("�ó���������·��, ����������\n");
			continue;
		}
		break;
	}
	OriCity->city = &citylist[NewIndex]; // ָ���³���
	printf("������·����Ϊ: ");
	PrintRoute();
}

void InsertHead(City *citylist)
{
	char NewName[NameLen] = { 0 };
	uint Index = -1;
	CityNode *NewCity = NULL;
	while (1)
	{
		printf("������Ҫ����ĳ���: ");
		scanf("%s", NewName);
		getchar(); // �������з�
		Index = CityIndex(NewName, citylist);
		if (Index == -1) // ���벻���ڵĳ���
		{
			printf("δ��ͨ�ó��е����η���, ����������\n");
			continue;
		}
		NewCity = SearchList(NewName);
		if (NewCity != NULL) // �������г���
		{
			printf("�ó�������������·��, ����������\n");
			continue;
		}
		break;
	}
	CityNode *ToInsert = (CityNode*)malloc(sizeof(CityNode));
	ToInsert->city = &citylist[Index];
	ToInsert->next = Route->next;
	Route->next = ToInsert;
	printf("������·����Ϊ: ");
	PrintRoute();
}

void InsertMid(City *citylist)
{
	char OriNamePre[NameLen] = { 0 }; // ǰһ��
	char OriNameLat[NameLen] = { 0 }; // ��һ��
	char NewName[NameLen] = { 0 }; // �²���ĳ���
	uint NewIndex = -1;
	CityNode *OriCityPre, *OriCityLat;
	OriCityPre = OriCityLat = NULL;
	while (1)
	{
		printf("������������·���������ĳ���: ");
		scanf("%s", OriNamePre);
		getchar(); // �����ո�
		scanf("%s", OriNameLat);
		getchar(); // �������з�
		OriCityPre = SearchList(OriNamePre);
		OriCityLat = SearchList(OriNameLat);
		if (OriCityPre == NULL)
		{
			printf("ǰһ�����в���·����, ����������\n");
			continue;
		}
		if (OriCityLat == NULL)
		{
			printf("��һ�����в���·����, ����������\n");
			continue;
		}
		if (OriCityPre->next->city->id != OriCityLat->city->id)
		{
			printf("�����˲������ĳ���, ����������\n");
			continue;
		}
		break;
	}
	while (1)
	{
		printf("��������Ҫ����ĳ���: ");
		scanf("%s", NewName);
		getchar(); // �������з�
		NewIndex = CityIndex(NewName, citylist);
		if (NewIndex == -1) // ���벻���ڵĳ���
		{
			printf("δ��ͨ�ó��е����η���, ����������\n");
			continue;
		}
		CityNode *NewCity = SearchList(NewName);
		if (NewCity != NULL) // �������г���
		{
			printf("�ó�������������·��, ����������\n");
			continue;
		}
		break;
	}
	CityNode *ToInsert = (CityNode*)malloc(sizeof(CityNode));
	ToInsert->city = &citylist[NewIndex];
	ToInsert->next = OriCityPre->next;
	OriCityPre->next = ToInsert;
	printf("������·����Ϊ: ");
	PrintRoute();
}

void InsertTail(City *citylist)
{
	char NewName[NameLen] = { 0 };
	uint Index = -1;
	CityNode *NewCity = NULL;
	while (1)
	{
		printf("������Ҫ����ĳ���: ");
		scanf("%s", NewName);
		getchar(); // �������з�
		Index = CityIndex(NewName, citylist);
		if (Index == -1) // ���벻���ڵĳ���
		{
			printf("δ��ͨ�ó��е����η���, ����������\n");
			continue;
		}
		NewCity = SearchList(NewName);
		if (NewCity != NULL) // �������г���
		{
			printf("�ó�������������·��, ����������\n");
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
	printf("������·����Ϊ: ");
	PrintRoute();
}

void InsertCity(City *citylist)
{
	char ch; // �����ַ�
	printf("�������³��в���λ��(0:ͷ�� 1:��������֮�� 2:β��): ");
	scanf("%c", &ch);
	getchar(); // �������з�
	switch (ch)
	{
	case('0'):
		InsertHead(citylist); break;
	case('1'):
		InsertMid(citylist); break;
	case('2'):
		InsertTail(citylist); break;
	default:
		puts("�Ƿ�����");
	}
}

void DeleteCity()
{
	char NameToRemove[NameLen] = { 0 };
	while (1)
	{
		printf("��������Ҫɾ���ĳ���: ");
		scanf("%s", NameToRemove);
		getchar(); // �������з�
		CityNode *CityToDelete = SearchList(NameToRemove);
		if (CityToDelete == NULL) // Ҫɾ���ĳ��в�����·��
		{
			printf("�ó��в�����·��, ����������\n");
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
		printf("��ǰ������·�����! �����½���������·\n");
		return;
	}
	else
	{
		printf("������·����Ϊ: ");
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
		printf("��������Ҫ�����۸�ĳ���: ");
		scanf("%s", Name);
		getchar(); // �������з�
		Index = CityIndex(Name, citylist);
		if (Index == -1) // �ó��в��ڳ��б���
		{
			printf("δ��ͨ�ó��е����η���, �޷��ļ�, ����������\n");
			continue;
		}
		else
			break;
	}
	printf("%sԭ���ļ۸�Ϊ%u, �����������ļ۸�: ", Name, citylist[Index].price);
	scanf("%u", &NewPrice);
	getchar(); // �������з�
	citylist[Index].price = NewPrice;
	PrintPriceTable(citylist);
	printf("������·: ");
	PrintRoute();
}

void What2Do(char ch, City *citylist) // ����chѡ�����
{
	switch (ch)
	{
	case ('1'): // ��ӡ���м۸��
		PrintPriceTable(citylist); break;
	case ('2'): // ����·��
		CreateRoute(citylist); break;
	case ('3'): // �ı�·���е�ĳ������
		ChangeCity(citylist); break;
	case ('4'): // ��·���в����µĳ���
		InsertCity(citylist); break;
	case ('5'): // ɾ��·���е�ĳ������
		DeleteCity(); break;
	case ('6'): // �������м۸�
		ChangePrice(citylist); break;
	case ('*'): // �˳�����
		exit(0);
	default: // ����Ƿ�����
		puts("INVALID INPUT"); 
	}
	ShowMenu(); // �ٴ�չʾ�˵�
}

int main()
{
	Route->city = NULL; // ��ʼ��·��
	Route->next = NULL; // ��ʼ��·��
	char control; // ���ƽ��к��ֲ������ַ�
	City *citylist;// ���б�
    CITY_NUM = GetData(citylist); // ���ļ���ȡ���б���ó�������
	ShowMenu(); // չʾ�˵�
	while (scanf("%c", &control))
	{
		getchar();
		What2Do(control, citylist); // ѡ�����ʲô����
	}
}