#include"Stock.h"
#include"UserInfo.h"
#include<iostream>
#include<sstream>
#include<iomanip>
using namespace std;

extern UserInfo usersinfo;

Stock::Stock(string FileName)
{
	Head = new ItemNode;
	Tail = Head;
	__ItemNum = f.FileToList(Head, Tail, FileName);
}

Stock::~Stock()
{
	Tail = NULL;
	while (Head != NULL)
	{
		ItemNode *cur = Head;
		Head = Head->next;
		delete cur;
	}
}

void Stock::GetID(string &next)
{
	int Sum = __ItemNum;
	char Id[6] = { 0 };
	Id[0] = 'F';
	int digits[4];
	digits[0] = Sum / 1000;
	Sum %= 1000;
	digits[1] = Sum / 100;
	Sum %= 100;
	digits[2] = Sum / 10;
	digits[3] = Sum % 10;
	Id[1] = (char) (digits[0] + 48);
	Id[2] = (char) (digits[1] + 48);
	Id[3] = (char) (digits[2] + 48);
	Id[4] = (char) (digits[3] + 48);
	Id[5] = '\0';
	next = Id;
}

void Stock::SyncData(string FileName)
{
	f.ListToFile(Head, FileName);
}

void Stock::PrintStock()
{
	cout << "****************************************************\n";
	cout << "ID\t名称\t\t品牌\t\t价格\t数量\n";
	ItemNode *cur = Head->next;
	while (cur != NULL)
	{
		cout << cur->id << '\t';
		cout << std::left << setw(10) << cur->name << '\t';
		cout << std::left << setw(10) << cur->brand << '\t';
		cout << fixed << setprecision(1) << cur->price << '\t';
		cout << cur->num << '\n';
		cur = cur->next;
	}
	cout << "****************************************************\n";
}

void Stock::UserPrintStock()
{
	cout << "****************************************************\n";
	cout << "ID\t名称\t\t品牌\t\t价格\t数量\n";
	ItemNode *cur = Head->next;
	while (cur != NULL)
	{
		if (cur->num <= 0)
		{
			cur = cur->next;
			continue;
		}
		else
		{
			cout << cur->id << '\t';
			cout << std::left << setw(10) << cur->name << '\t';
			cout << std::left << setw(10) << cur->brand << '\t';
			cout << fixed << setprecision(1) << cur->price << '\t';
			cout << cur->num << '\n';
			cur = cur->next;
		}
	}
	cout << "****************************************************\n";
}

void Stock::AddItem()
{
	string ItemName;
	string ItemBrand;
	double ItemPrice;
	int ItemNum;
	printf("请输入要添加的商品名: ");
	cin >> ItemName;
	printf("请输入要添加的商品品牌: ");
	cin >> ItemBrand;
	ItemNode *p = SearchItem(ItemName, ItemBrand);
	if (p != NULL) // 已有该商品
	{
		printf("请输入要添加的商品数量: ");
		cin >> ItemNum;
		p->num = (p->num == -1) ? ItemNum : (p->num + ItemNum);
	}
	else // 添加新商品
	{
		printf("请输入要添加的商品价格: ");
		cin >> ItemPrice;
		printf("请输入要添加的商品数量: ");
		cin >> ItemNum;
		ItemNode *q = new ItemNode;
		q->name = ItemName;
		q->brand = ItemBrand;
		q->price = ItemPrice;
		q->num = ItemNum;
		if (Head->next == NULL) // 库存为空
			q->id = "F0000";
		else
			GetID(q->id); // 得到商品ID
		Tail->next = q;
		Tail = Tail->next;
		__ItemNum++;
	}
	puts("修改后的库存如下: ");
	PrintStock();
	SyncData(".\\Data\\库存.txt"); // 同步数据
}

void Stock::RemoveItem()
{
	cout << "请输入需要下架的商品ID: ";
	string ItemID;
	while (cin >> ItemID)
	{
		ItemNode *cur = SearchItem(ItemID);
		if (cur == NULL) // 不存在该商品
		{
			cout << "不存在该商品, 请重新输入: ";
			continue;
		}
		else // 找到商品下架
		{
			cur->next->num = -1;
			break;
		}
	}
	usersinfo.Traverse(ItemID); // 同步购物车
	puts("修改后的库存如下: ");
	PrintStock();
	SyncData(".\\Data\\库存.txt"); // 同步数据
}

void Stock::ChangeItem()
{
	char ch;
	cout << "请选择操作(1: 修改商品数量; 2: 修改商品价格): ";
	while (cin >> ch)
	{
		string ItemID;
		if (ch != '1' && ch != '2')
		{
			cout << "输入无效字符, 请重新输入: ";
			continue;
		}
		else if (ch == '1')
		{
			int NewNum;
			ItemNode *cur = NULL;
			cout << "请输入商品ID: ";
			while (cin >> ItemID)
			{
				cur = SearchItem(ItemID);
				if (cur == NULL) // 不存在该商品
				{
					cout << "不存在该商品, 请重新输入: ";
					continue;
				}
				break;
			}
			cout << "请输入修改后的数量: ";
			while (cin >> NewNum)
			{
				if (NewNum < 0)
				{
					cout << "数量不能为负, 请重新输入: ";
					continue;
				}
				break;
			}
			cur->next->num = NewNum;
		}
		else // ch = 2
		{
			double NewPrice;
			ItemNode *cur = NULL;
			cout << "请输入商品ID: ";
			while (cin >> ItemID)
			{
				cur = SearchItem(ItemID);
				if (cur == NULL) // 不存在该商品
				{
					cout << "不存在该商品, 请重新输入: ";
					continue;
				}
				break;
			}
			cout << "请输入修改后的价格: ";
			while (cin >> NewPrice)
			{
				if (NewPrice < 0)
				{
					cout << "价格不能为负, 请重新输入: ";
					continue;
				}
				break;
			}
			cur->next->price = NewPrice;
			// 遍历购物车寻找有无ID为ItemID的商品, 若有则修改价格.
			usersinfo.Traverse(ItemID, NewPrice);
		}
		break;
	}
	puts("修改后的库存如下: ");
	PrintStock();
	SyncData(".\\Data\\库存.txt"); // 同步数据
}

ItemNode* Stock::SearchItem(string ItemName, string ItemBrand) const
{
	ItemNode *cur = Head->next;
	while (cur != NULL)
	{
		if (cur->name == ItemName && cur->brand == ItemBrand)
			return cur;
		cur = cur->next;
	}
	return NULL;
}

ItemNode* Stock::SearchItem(string ItemID) const // 返回指定ID的前一个指针
{
	ItemNode *cur = Head;
	while (cur->next != NULL)
	{
		if (cur->next->id == ItemID)
			return cur;
		cur = cur->next;
	}
	return NULL;
}

void Stock::SearchItem()
{
	string ItemName;
	cout << "输入商品名称: ";
	cin >> ItemName;
	ItemNode *cur = Head->next;
	cout << "****************************************************\n";
	cout << "ID\t名称\t\t品牌\t\t价格\t数量\n";
	while (cur != NULL)
	{
		if (cur->name == ItemName && cur->num > 0)
		{
			cout << cur->id << '\t';
			cout << std::left << setw(10) << cur->name << '\t';
			cout << std::left << setw(10) << cur->brand << '\t';
			cout << fixed << setprecision(1) << cur->price << '\t';
			cout << cur->num << '\n';
		}
		cur = cur->next;
	}
	cout << "****************************************************\n";
}

void Stock::SearchNameBrand()
{
	string ItemName, ItemBrand;
	cout << "输入商品名称: ";
	cin >> ItemName;
	cout << "输入商品品牌: ";
	cin >> ItemBrand;
	ItemNode *cur = Head->next;
	cout << "****************************************************\n";
	cout << "ID\t名称\t\t品牌\t\t价格\t数量\n";
	while (cur != NULL)
	{
		if (cur->name == ItemName && cur->brand == ItemBrand && cur->num > 0)
		{
			cout << cur->id << '\t';
			cout << std::left << setw(10) << cur->name << '\t';
			cout << std::left << setw(10) << cur->brand << '\t';
			cout << fixed << setprecision(1) << cur->price << '\t';
			cout << cur->num << '\n';
		}
		cur = cur->next;
	}
	cout << "****************************************************\n";
}

void Stock::SearchID()
{
	string ItemID;
	cout << "输入商品ID: ";
	cin >> ItemID;
	ItemNode *cur = Head->next;
	cout << "****************************************************\n";
	cout << "ID\t名称\t\t品牌\t\t价格\t数量\n";
	while (cur != NULL)
	{
		if (cur->id == ItemID && cur->num > 0)
		{
			cout << cur->id << '\t';
			cout << std::left << setw(10) << cur->name << '\t';
			cout << std::left << setw(10) << cur->brand << '\t';
			cout << fixed << setprecision(1) << cur->price << '\t';
			cout << cur->num << '\n';
		}
		cur = cur->next;
	}
	cout << "****************************************************\n";
}

void Stock::SearchBrand()
{
	string ItemBrand;
	cout << "输入商品品牌: ";
	cin >> ItemBrand;
	ItemNode *cur = Head->next;
	cout << "****************************************************\n";
	cout << "ID\t名称\t\t品牌\t\t价格\t数量\n";
	while (cur != NULL)
	{
		if (cur->brand == ItemBrand && cur->num > 0)
		{
			cout << cur->id << '\t';
			cout << std::left << setw(10) << cur->name << '\t';
			cout << std::left << setw(10) << cur->brand << '\t';
			cout << fixed << setprecision(1) << cur->price << '\t';
			cout << cur->num << '\n';
		}
		cur = cur->next;
	}
	cout << "****************************************************\n";
}

void Stock::AdvanceSearch()
{
	char ch;
	bool flag = 0;
	cout << "请选择搜索模式(1: 名称+品牌||2: ID||3: 品牌): ";
	while (cin >> ch)
	{
		switch (ch)
		{
		case('1'):
			flag = 1;
			SearchNameBrand();
			break;
		case('2'):
			flag = 1;
			SearchID();
			break;
		case('3'):
			flag = 1;
			SearchBrand();
			break;
		default:
			cout << "输入了无效字符, 请重新输入: ";
		}
		if (flag)
			break;
	}
}

bool Stock::IsEnough(ItemNode *Scp)
{
	ItemNode *cur = SearchItem(Scp->id);
	if (cur->next->num >= Scp->num)
		return 1;
	return 0;
}

void Stock::Traverse(ItemNode *Scp)
{
	ItemNode *cur = SearchItem(Scp->id);
	cur->next->num -= Scp->num;
}