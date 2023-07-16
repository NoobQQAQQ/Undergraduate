#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<fstream>
#include<cstdio>
#include<cstring>
typedef unsigned int uint;

using namespace std;

struct City // ������нṹ
{
	uint id;
	string cityname;
	uint price;
	City() { id = -1; cityname = ""; price = 0; }
};

struct CityNode // ������нڵ�
{
	City *city;
	CityNode *next;
	CityNode() { city = NULL; next = NULL; }
};

class File // �����ȡ�ļ�
{
public:
	void GetInName(const string &inName) { inFileName = inName; } // ��ȡ�ļ���
	void GetOutName(const string &outName) { outFileName = outName; } // ��ȡ�ļ���
	void inPriceList(City *&citydata, uint &num) const // ���ļ���ȡ�۸��
	{
		ifstream fin(inFileName); // ���ļ�
		int count = -1;
		while (!fin.eof())
		{
			uint id, price;
			string cityname;
			fin >> id >> cityname >> price;
			count++;
		}
		citydata = new City[count];
		num = count;
		fin.clear();
		fin.seekg(0, ios::beg);
		for (uint i = 0; i < num; i++)
			fin >> citydata[i].id >> citydata[i].cityname >> citydata[i].price;
		fin.close();
	}
	void outRoute(CityNode *&Head, bool vip) const // ��·��д���ļ�
	{
		ofstream fout;
		fout.open(outFileName, ios::binary | ios::trunc);
		if (vip == 1)
			fout.put('v');
		else
			fout.put('n');
		fout.put(':');
		CityNode *cur = Head;
		if (cur->next != NULL)
		{
			cur = cur->next;
			fout.write(cur->city->cityname.c_str(), sizeof(char) * cur->city->cityname.size());
			fout.put('-');
			cur = cur->next;
		}
		while (cur->next != NULL)
		{
			fout.write(cur->city->cityname.c_str(), sizeof(char) * cur->city->cityname.size());
			fout.put('-');
			cur = cur->next;
		}
		fout.write(cur->city->cityname.c_str(), sizeof(char) * cur->city->cityname.size());
		fout.close();
	}
private:
	string inFileName;
	string outFileName;
};

class CityList // ��ų��б�ͳ��б���ز���
{
public:
	CityList(const string &inFileName)
	{
		infile.GetInName(inFileName);
		infile.inPriceList(citydata, CITY_NUM);
	}
	void PrintPriceList() const
	{
		for (uint i = 0; i < CITY_NUM; i++)
			cout << citydata[i].id << '\t' << citydata[i].cityname << '\t' << citydata[i].price << endl;
	}
	void ChangeCityPrice()
	{
		string cityname;
		uint NewPrice = 0;
		uint Index = -1;
		while (1)
		{
			cout << "��������Ҫ�����۸�ĳ���: ";
			cin >> cityname;
			Index = CityIndex(cityname);
			if (Index == -1) // �ó��в��ڳ��б���
			{
				cout << "δ��ͨ�ó��е����η���, �޷��ļ�, ����������\n";
				continue;
			}
			else
				break;
		}
		cout << cityname << "ԭ���ļ۸�Ϊ" << citydata[Index].price << ", �����������ļ۸�: ";
		cin >> NewPrice;
		citydata[Index].price = NewPrice;
		PrintPriceList();
		cout << "������·Ϊ: ";
	}
	uint CityIndex(const string &cityname) const // ���س�������Ӧ���±�, ���޸ó��з���-1
	{
		for (uint i = 0; i < CITY_NUM; i++)
			if (cityname == citydata[i].cityname)
				return i;
		return -1;
	}
	City* GetAddress(uint index) const { return &citydata[index]; }
	~CityList() { delete[] citydata; }
private:
	City *citydata;
	uint CITY_NUM;
	File infile;
};

class Route
{
public:
	Route() { Head = new CityNode; Total = 0; } // ������ͷ���
	~Route() // ����
	{
		while (Head != NULL)
		{
			CityNode *cur = Head;
			Head = Head->next;
			delete cur;
		}
	}
	void CreateRoute(const CityList &citylist)
	{
		if (Head->next != NULL)
		{
			puts("�Ѵ���һ��������·, �����ظ�����");
			return;
		}
		cout << "���������(�Կո�ָ�, ���н���): ";
		string cityname;
		char ch; // �жϿո�س�
		CityNode *cur = Head;
		while ((cin >> cityname).get(ch)) // ��������
		{
			uint index = citylist.CityIndex(cityname); // ���ݳ�������������ַ
			if (index == -1) // ���벻���ڵĳ���
			{
				cout << "δ��ͨ" << cityname << "�����η���!\n";
				if (ch == '\n')
					break;
				else
					continue;
			}
			CityNode *Check = SearchNode(cityname); // ����Ϸ����ظ�����
			if (Check != NULL) // ����Ϸ����ظ�����
			{
				cout << "�ظ�������" << cityname << ", ��Ϊ���Զ�ȥ��\n";
				if (ch == '\n')
					break;
				else
					continue;
			}
			CityNode *p = new CityNode; // �½����
			p->city = citylist.GetAddress(index);
			cur->next = p;
			cur = cur->next;
			cur->next = NULL;
			p = NULL;
			if (ch == '\n')
				break;
		}
		if (Head->next != NULL)
		{
			puts("�����ɹ�!");
			cout << "������·Ϊ: ";
			PrintRoute(); // ��ӡ��·������۸�
		}
		else
			puts("����ʧ��!");
	}
	void ChangeCity(const CityList &citylist)
	{
		string OriName; // ԭ������
		string NewName; // �³�����
		CityNode *OriCity = NULL; // ָ���滻����
		uint NewIndex = -1; // �³����ڳ��б�������±�
		while (1)
		{
			printf("�����뱻�滻�ĳ���: ");
			cin >> OriName;
			OriCity = SearchNode(OriName);
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
			cin >> NewName;
			if (OriName == NewName) // �滻���кͱ��滻������ͬ
			{
				printf("�ó����뱻�滻������ͬ, ����������\n");
				continue;
			}
			NewIndex = citylist.CityIndex(NewName);
			if (NewIndex == -1) // �³��в�����
			{
				printf("δ��ͨ�ó��е����η���, ����������\n");
				continue;
			}
			CityNode *NewCity = SearchNode(NewName); // ����·�в����³���
			if (NewCity != NULL) // ԭ��·�������滻����
			{
				printf("�ó���������·��, ����������\n");
				continue;
			}
			break;
		}
		OriCity->city = citylist.GetAddress(NewIndex); // ָ���³���
		printf("������·����Ϊ: ");
		PrintRoute();
	}
	void InsertCity(const CityList &citylist)
	{
		char ch; // �����ַ�
		printf("�������³��в���λ��(0:ͷ�� 1:��������֮�� 2:β��): ");
		cin >> ch;
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
		string NameToRemove;
		while (1)
		{
			printf("��������Ҫɾ���ĳ���: ");
			cin >> NameToRemove;
			CityNode *CityToDelete = SearchNode(NameToRemove);
			if (CityToDelete == NULL) // Ҫɾ���ĳ��в�����·��
			{
				printf("�ó��в�����·��, ����������\n");
				continue;
			}
			else
				break;
		}
		CityNode *cur = Head;
		while (cur->next != NULL)
		{
			if (cur->next->city->cityname == NameToRemove)
			{
				CityNode *CityToRemove = cur->next;
				cur->next = CityToRemove->next;
				delete CityToRemove;
			}
			else
				cur = cur->next;
		}
		if (Head->next == NULL)
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
	void PrintRoute() 
	{
		Total = 0;
		CityNode *cur = Head->next;;
		while (cur->next != NULL) // ѭ�����������ڶ������
		{
			cout << cur->city->cityname << '-';
			Total += cur->city->price;
			cur = cur->next;
		}
		cout << cur->city->cityname << endl;
		Total += cur->city->price;
		PrintPrice();
	}
	virtual void PrintPrice() const { cout << "�ܼ۸�Ϊ: " << Total << endl; } // ��ӡ�۸�
	virtual void SaveToFile(const string &outFileName) 
	{
		f.GetOutName(outFileName);
		f.outRoute(Head, 0);
		cout << "�𾴵��û�, ��Ϊ������·��\n";
	}
	void inRoute(const string &inFileName, const CityList &citylist)
	{
		FILE *fp = fopen(inFileName.c_str(), "r");
		char whole[50] = { 0 };
		CityNode *cur = Head;
		fread(whole, sizeof(char), 50, fp);
		int len = strlen(whole);
		for (int i = 2; i < len; i += 5)
		{
			char cityname[5] = { 0 };
			for (int j = 0; j < 4; j++)
				cityname[j] = whole[i + j];
			uint index = citylist.CityIndex(cityname);
			City *p = citylist.GetAddress(index);
			CityNode *q = new CityNode;
			q->city = p;
			cur->next = q;
			cur = cur->next;
		}
		fclose(fp);
	}
	void ReadFromFile(const string &inFileName, const CityList &citylist)
	{
		cout << "�𾴵��û�, ����Ϊ����ȡ·��...\n";
		inRoute(inFileName, citylist);
		if (Head->next != NULL)
			cout << "��ȡ�ɹ�!\n";
		else
			cout << "��ȡʧ��!\n";
		PrintRoute();
	}
protected:
	CityNode *Head;
	File f;
	uint Total;
	CityNode* SearchNode(const string &cityname)
	{
		CityNode *cur = Head->next;
		while (cur != NULL)
		{
			if (cur->city->cityname == cityname) // �ҵ�Ŀ�����
				return cur;
			cur = cur->next;
		}
		return NULL; // δ�ҵ�Ŀ�����
	}
	void InsertHead(const CityList &citylist)
	{
		string NewName;
		uint Index = -1;
		CityNode *NewCity = NULL;
		while (1)
		{
			printf("������Ҫ����ĳ���: ");
			cin >> NewName;
			Index = citylist.CityIndex(NewName);
			if (Index == -1) // ���벻���ڵĳ���
			{
				printf("δ��ͨ�ó��е����η���, ����������\n");
				continue;
			}
			NewCity = SearchNode(NewName);
			if (NewCity != NULL) // �������г���
			{
				printf("�ó�������������·��, ����������\n");
				continue;
			}
			break;
		}
		CityNode *ToInsert = new CityNode;
		ToInsert->city = citylist.GetAddress(Index);
		ToInsert->next = Head->next;
		Head->next = ToInsert;
		printf("������·����Ϊ: ");
		PrintRoute();
	}
	void InsertMid(const CityList &citylist)
	{
		string OriNamePre; // ǰһ��
		string OriNameLat; // ��һ��
		string NewName; // �²���ĳ���
		uint NewIndex = -1;
		CityNode *OriCityPre, *OriCityLat;
		OriCityPre = OriCityLat = NULL;
		while (1)
		{
			printf("������������·���������ĳ���: ");
			cin >> OriNamePre;			
			cin >> OriNameLat;
			OriCityPre = SearchNode(OriNamePre);
			OriCityLat = SearchNode(OriNameLat);
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
			cin >> NewName;
			NewIndex = citylist.CityIndex(NewName);
			if (NewIndex == -1) // ���벻���ڵĳ���
			{
				printf("δ��ͨ�ó��е����η���, ����������\n");
				continue;
			}
			CityNode *NewCity = SearchNode(NewName);
			if (NewCity != NULL) // �������г���
			{
				printf("�ó�������������·��, ����������\n");
				continue;
			}
			break;
		}
		CityNode *ToInsert = new CityNode;
		ToInsert->city = citylist.GetAddress(NewIndex);
		ToInsert->next = OriCityPre->next;
		OriCityPre->next = ToInsert;
		printf("������·����Ϊ: ");
		PrintRoute();
	}
	void InsertTail(const CityList &citylist)
	{
		string NewName;
		uint Index = -1;
		CityNode *NewCity = NULL;
		while (1)
		{
			printf("������Ҫ����ĳ���: ");
			cin >> NewName;
			Index = citylist.CityIndex(NewName);
			if (Index == -1) // ���벻���ڵĳ���
			{
				printf("δ��ͨ�ó��е����η���, ����������\n");
				continue;
			}
			NewCity = SearchNode(NewName);
			if (NewCity != NULL) // �������г���
			{
				printf("�ó�������������·��, ����������\n");
				continue;
			}
			break;
		}
		CityNode *ToInsert = new CityNode;
		ToInsert->city = citylist.GetAddress(Index);
		ToInsert->next = NULL;
		CityNode *cur = Head->next;
		while (cur->next != NULL)
			cur = cur->next;
		cur->next = ToInsert;
		printf("������·����Ϊ: ");
		PrintRoute();
	}
};

class VipRoute: public Route
{
public:
	void SaveToFile(const string &outFileName)
	{
		f.GetOutName(outFileName);
		f.outRoute(Head, 1);
		cout << "�𾴵�vip�û�, ��Ϊ������·��\n";
	}
	void PrintPrice() const { cout << "�ܼ۸�Ϊ: " << (double) Total * 0.8 << endl; }
};

void ShowMenu()
{
	puts("********************************************************************");
	puts("                                �˵�");
	puts("\t1, ��ӡ���γ��м۸��");
	puts("\t2, ����������·");
	puts("\t3, ������·���е�ĳ�������滻Ϊ�����ĳ���");
	puts("\t4, ������·���е���������֮������µĳ���");
	puts("\t5, ɾ������·���е�ĳ������");
	puts("\t6, �������γ��м۸����ĳ�����γ��м۸�");
	puts("\t7, ������·������������ļ���");
	puts("\t8, ���ļ��ж�ȡ����·������");
	puts("\t*, �˳�����");
	puts("********************************************************************\n");
	printf("���������Ĳ���: ");
}

bool IsVip(const string &inFileName)
{
	ifstream fin;
	fin.open(inFileName, ios::binary);
	char ch;
	fin.get(ch);
	fin.close();
	if (ch == 'v')
		return 1;
	else
		return 0;
}

void What2Do(char ch, CityList &citylist, Route &route, VipRoute &viproute, bool &isVip)
{
	switch (ch)
	{
	case ('1'): // ��ӡ���м۸��
		citylist.PrintPriceList(); break;
	case ('2'): // ����·��
		char vip;
		cout << "���Ƿ�Ϊ���������VIP? (Y/N) ������: ";
		cin >> vip;
		if (vip == 'Y')
		{
			cout << "����, �𾴵�VIP�ͻ�!" << endl;
			isVip = 1;
			viproute.CreateRoute(citylist);
		}
		else if (vip == 'N')
		{
			cout << "����, �𾴵Ŀͻ�!" << endl;
			isVip = 0;
			route.CreateRoute(citylist);
		}
		else
			cout << "INVALID INPUT" << endl;
		break;
	case ('3'): // �ı�·���е�ĳ������
		if (isVip)
			viproute.ChangeCity(citylist);
		else
			route.ChangeCity(citylist);
		break;
	case ('4'): // ��·���в����µĳ���
		if (isVip)
			viproute.InsertCity(citylist);
		else
			route.InsertCity(citylist);
		break;
	case ('5'): // ɾ��·���е�ĳ������
		if (isVip)
			viproute.DeleteCity();
		else
			route.DeleteCity();
		break;
	case ('6'): // �������м۸�
		citylist.ChangeCityPrice();
		if (isVip)
			viproute.PrintRoute();
		else
			route.PrintRoute();
		break;
	case ('7'):
		if (isVip)
			viproute.SaveToFile("route.dat");
		else
			route.SaveToFile("route.dat");
		break;
	case ('8'):
		isVip = IsVip("route.dat");
		if (isVip)
			viproute.ReadFromFile("route.dat", citylist);
		else
			route.ReadFromFile("route.dat", citylist);
		break;
	case ('*'): // �˳�����
		exit(0);
	default: // ����Ƿ�����
		puts("INVALID INPUT");
	}
	ShowMenu(); // �ٴ�չʾ�˵�
}

int main()
{
	CityList citylist("pricelist.txt") ; // �������б�
	Route route;
	VipRoute viproute;
	bool isVip = 0;
	char control; // �����ַ�
	ShowMenu(); // չʾ�˵�
	while (cin >> control)
		What2Do(control, citylist, route, viproute, isVip);
}
