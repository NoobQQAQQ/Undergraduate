#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<fstream>
#include<cstdio>
#include<cstring>
typedef unsigned int uint;

using namespace std;

struct City // 定义城市结构
{
	uint id;
	string cityname;
	uint price;
	City() { id = -1; cityname = ""; price = 0; }
};

struct CityNode // 定义城市节点
{
	City *city;
	CityNode *next;
	CityNode() { city = NULL; next = NULL; }
};

class File // 负责存取文件
{
public:
	void GetInName(const string &inName) { inFileName = inName; } // 获取文件名
	void GetOutName(const string &outName) { outFileName = outName; } // 获取文件名
	void inPriceList(City *&citydata, uint &num) const // 从文件读取价格表
	{
		ifstream fin(inFileName); // 打开文件
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
	void outRoute(CityNode *&Head, bool vip) const // 把路线写入文件
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

class CityList // 存放城市表和城市表相关操作
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
			cout << "请输入需要调整价格的城市: ";
			cin >> cityname;
			Index = CityIndex(cityname);
			if (Index == -1) // 该城市不在城市表中
			{
				cout << "未开通该城市的旅游服务, 无法改价, 请重新输入\n";
				continue;
			}
			else
				break;
		}
		cout << cityname << "原来的价格为" << citydata[Index].price << ", 请输入调整后的价格: ";
		cin >> NewPrice;
		citydata[Index].price = NewPrice;
		PrintPriceList();
		cout << "旅游线路为: ";
	}
	uint CityIndex(const string &cityname) const // 返回城市名对应的下标, 若无该城市返回-1
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
	Route() { Head = new CityNode; Total = 0; } // 带附加头结点
	~Route() // 析构
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
			puts("已存在一条旅游线路, 请勿重复创建");
			return;
		}
		cout << "请输入城市(以空格分割, 换行结束): ";
		string cityname;
		char ch; // 判断空格回车
		CityNode *cur = Head;
		while ((cin >> cityname).get(ch)) // 建立链表
		{
			uint index = citylist.CityIndex(cityname); // 根据城市名获得数组地址
			if (index == -1) // 输入不存在的城市
			{
				cout << "未开通" << cityname << "的旅游服务!\n";
				if (ch == '\n')
					break;
				else
					continue;
			}
			CityNode *Check = SearchNode(cityname); // 输入合法的重复城市
			if (Check != NULL) // 输入合法的重复城市
			{
				cout << "重复输入了" << cityname << ", 已为您自动去重\n";
				if (ch == '\n')
					break;
				else
					continue;
			}
			CityNode *p = new CityNode; // 新建结点
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
			puts("创建成功!");
			cout << "旅游线路为: ";
			PrintRoute(); // 打印线路并输出价格
		}
		else
			puts("创建失败!");
	}
	void ChangeCity(const CityList &citylist)
	{
		string OriName; // 原城市名
		string NewName; // 新城市名
		CityNode *OriCity = NULL; // 指向被替换城市
		uint NewIndex = -1; // 新城市在城市表数组的下标
		while (1)
		{
			printf("请输入被替换的城市: ");
			cin >> OriName;
			OriCity = SearchNode(OriName);
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
			cin >> NewName;
			if (OriName == NewName) // 替换城市和被替换城市相同
			{
				printf("该城市与被替换城市相同, 请重新输入\n");
				continue;
			}
			NewIndex = citylist.CityIndex(NewName);
			if (NewIndex == -1) // 新城市不存在
			{
				printf("未开通该城市的旅游服务, 请重新输入\n");
				continue;
			}
			CityNode *NewCity = SearchNode(NewName); // 在线路中查找新城市
			if (NewCity != NULL) // 原线路中已有替换城市
			{
				printf("该城市已在线路中, 请重新输入\n");
				continue;
			}
			break;
		}
		OriCity->city = citylist.GetAddress(NewIndex); // 指向新城市
		printf("旅游线路更改为: ");
		PrintRoute();
	}
	void InsertCity(const CityList &citylist)
	{
		char ch; // 控制字符
		printf("请输入新城市插入位置(0:头部 1:两个城市之间 2:尾部): ");
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
			puts("非法输入");
		}
	}
	void DeleteCity()
	{
		string NameToRemove;
		while (1)
		{
			printf("请输入需要删除的城市: ");
			cin >> NameToRemove;
			CityNode *CityToDelete = SearchNode(NameToRemove);
			if (CityToDelete == NULL) // 要删除的城市不在线路中
			{
				printf("该城市不在线路中, 请重新输入\n");
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
			printf("当前旅游线路已清空! 请重新建立旅游线路\n");
			return;
		}
		else
		{
			printf("旅游线路更改为: ");
			PrintRoute();
		}
	}
	void PrintRoute() 
	{
		Total = 0;
		CityNode *cur = Head->next;;
		while (cur->next != NULL) // 循环到链表倒数第二个结点
		{
			cout << cur->city->cityname << '-';
			Total += cur->city->price;
			cur = cur->next;
		}
		cout << cur->city->cityname << endl;
		Total += cur->city->price;
		PrintPrice();
	}
	virtual void PrintPrice() const { cout << "总价格为: " << Total << endl; } // 打印价格
	virtual void SaveToFile(const string &outFileName) 
	{
		f.GetOutName(outFileName);
		f.outRoute(Head, 0);
		cout << "尊敬的用户, 已为您保存路线\n";
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
		cout << "尊敬的用户, 正在为您读取路线...\n";
		inRoute(inFileName, citylist);
		if (Head->next != NULL)
			cout << "读取成功!\n";
		else
			cout << "读取失败!\n";
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
			if (cur->city->cityname == cityname) // 找到目标城市
				return cur;
			cur = cur->next;
		}
		return NULL; // 未找到目标城市
	}
	void InsertHead(const CityList &citylist)
	{
		string NewName;
		uint Index = -1;
		CityNode *NewCity = NULL;
		while (1)
		{
			printf("请输入要加入的城市: ");
			cin >> NewName;
			Index = citylist.CityIndex(NewName);
			if (Index == -1) // 输入不存在的城市
			{
				printf("未开通该城市的旅游服务, 请重新输入\n");
				continue;
			}
			NewCity = SearchNode(NewName);
			if (NewCity != NULL) // 输入已有城市
			{
				printf("该城市已在旅游线路中, 请重新输入\n");
				continue;
			}
			break;
		}
		CityNode *ToInsert = new CityNode;
		ToInsert->city = citylist.GetAddress(Index);
		ToInsert->next = Head->next;
		Head->next = ToInsert;
		printf("旅游线路更改为: ");
		PrintRoute();
	}
	void InsertMid(const CityList &citylist)
	{
		string OriNamePre; // 前一个
		string OriNameLat; // 后一个
		string NewName; // 新插入的城市
		uint NewIndex = -1;
		CityNode *OriCityPre, *OriCityLat;
		OriCityPre = OriCityLat = NULL;
		while (1)
		{
			printf("请输入两个在路线中连续的城市: ");
			cin >> OriNamePre;			
			cin >> OriNameLat;
			OriCityPre = SearchNode(OriNamePre);
			OriCityLat = SearchNode(OriNameLat);
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
			cin >> NewName;
			NewIndex = citylist.CityIndex(NewName);
			if (NewIndex == -1) // 输入不存在的城市
			{
				printf("未开通该城市的旅游服务, 请重新输入\n");
				continue;
			}
			CityNode *NewCity = SearchNode(NewName);
			if (NewCity != NULL) // 输入已有城市
			{
				printf("该城市已在旅游线路中, 请重新输入\n");
				continue;
			}
			break;
		}
		CityNode *ToInsert = new CityNode;
		ToInsert->city = citylist.GetAddress(NewIndex);
		ToInsert->next = OriCityPre->next;
		OriCityPre->next = ToInsert;
		printf("旅游线路更改为: ");
		PrintRoute();
	}
	void InsertTail(const CityList &citylist)
	{
		string NewName;
		uint Index = -1;
		CityNode *NewCity = NULL;
		while (1)
		{
			printf("请输入要加入的城市: ");
			cin >> NewName;
			Index = citylist.CityIndex(NewName);
			if (Index == -1) // 输入不存在的城市
			{
				printf("未开通该城市的旅游服务, 请重新输入\n");
				continue;
			}
			NewCity = SearchNode(NewName);
			if (NewCity != NULL) // 输入已有城市
			{
				printf("该城市已在旅游线路中, 请重新输入\n");
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
		printf("旅游线路更改为: ");
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
		cout << "尊敬的vip用户, 已为您保存路线\n";
	}
	void PrintPrice() const { cout << "总价格为: " << (double) Total * 0.8 << endl; }
};

void ShowMenu()
{
	puts("********************************************************************");
	puts("                                菜单");
	puts("\t1, 打印旅游城市价格表");
	puts("\t2, 创建旅游线路");
	puts("\t3, 将旅游路线中的某个城市替换为其他的城市");
	puts("\t4, 在旅游路线中的两个城市之间插入新的城市");
	puts("\t5, 删除旅游路线中的某个城市");
	puts("\t6, 调整旅游城市价格表中某个旅游城市价格");
	puts("\t7, 将旅游路线数据输出到文件中");
	puts("\t8, 从文件中读取旅游路线数据");
	puts("\t*, 退出程序");
	puts("********************************************************************\n");
	printf("请输入您的操作: ");
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
	case ('1'): // 打印城市价格表
		citylist.PrintPriceList(); break;
	case ('2'): // 创建路线
		char vip;
		cout << "您是否为本旅行社的VIP? (Y/N) 请输入: ";
		cin >> vip;
		if (vip == 'Y')
		{
			cout << "您好, 尊敬的VIP客户!" << endl;
			isVip = 1;
			viproute.CreateRoute(citylist);
		}
		else if (vip == 'N')
		{
			cout << "您好, 尊敬的客户!" << endl;
			isVip = 0;
			route.CreateRoute(citylist);
		}
		else
			cout << "INVALID INPUT" << endl;
		break;
	case ('3'): // 改变路线中的某个城市
		if (isVip)
			viproute.ChangeCity(citylist);
		else
			route.ChangeCity(citylist);
		break;
	case ('4'): // 在路线中插入新的城市
		if (isVip)
			viproute.InsertCity(citylist);
		else
			route.InsertCity(citylist);
		break;
	case ('5'): // 删除路线中的某个城市
		if (isVip)
			viproute.DeleteCity();
		else
			route.DeleteCity();
		break;
	case ('6'): // 调整城市价格
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
	case ('*'): // 退出程序
		exit(0);
	default: // 处理非法输入
		puts("INVALID INPUT");
	}
	ShowMenu(); // 再次展示菜单
}

int main()
{
	CityList citylist("pricelist.txt") ; // 建立城市表
	Route route;
	VipRoute viproute;
	bool isVip = 0;
	char control; // 控制字符
	ShowMenu(); // 展示菜单
	while (cin >> control)
		What2Do(control, citylist, route, viproute, isVip);
}
