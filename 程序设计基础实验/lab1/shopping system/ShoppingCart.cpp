#include"ShoppingCart.h"
#include"Stock.h"
#include"SoldList.h"
#include<iostream>
#include<iomanip>
#include<fstream>
using namespace std;

extern Stock stock;
extern SoldList soldlist;

void ShoppingCart::GetUserName(string username)
{
	__username = username;
}

ShoppingCart::ShoppingCart(string FileName)
{
	fName = FileName;
	Head = new ItemNode;
	Tail = Head;
	f.FileToList(Head, Tail, FileName);
}

ShoppingCart::~ShoppingCart()
{
	Tail = NULL;
	while (Head != NULL)
	{
		ItemNode *cur = Head;
		Head = Head->next;
		delete cur;
	}
}

void ShoppingCart::SyncData(string FileName)
{
	f.ListToFile(Head, FileName);
}

void ShoppingCart::SearchID2Remove(string ItemID)
{
	ItemNode *cur = Head;
	while (cur->next != NULL)
	{
		if (cur->next->id == ItemID)
		{
			ItemNode *p = cur->next;
			cur->next = p->next;
			delete p;
			break;
		}
		cur = cur->next;
	}
}

void ShoppingCart::__SetShoppingCart(string FileName)
{
	fName = FileName;
	Head = new ItemNode;
	Tail = Head;
	f.FileToList(Head, Tail, FileName);
}

void ShoppingCart::AddItem()
{
	string ItemID;
	int ItemNum;
	printf("请输入要添加的商品ID和数量: ");
	while (cin >> ItemID >> ItemNum)
	{
		if (ItemNum < 0)
		{
			cout << "数量不能为负数, 请重新输入商品ID和数量: ";
			continue;
		}
		break;
	}
	ItemNode *p = SearchItem(ItemID);
	ItemNode *stockinfo = stock.SearchItem(ItemID);
	if (p != NULL) // 已有该商品
		p->num += ItemNum;
	else // 添加新商品
	{
		if (stockinfo == NULL)
		{
			cout << "库存中没有该商品, 添加失败!" << endl;
			return;
		}
		else
		{
			ItemNode *q = new ItemNode;
			q->id = ItemID;
			q->name = stockinfo->next->name;
			q->brand = stockinfo->next->brand;
			q->price = stockinfo->next->price;
			q->num = ItemNum;
			Tail->next = q;
			Tail = Tail->next;
		}
	}
	__ItemsNum++;
	puts("*************");
	puts("已经加入购物车");
	puts("*************");
	puts("购物车中有: ");
	Show();
	SyncData(fName);
}

void ShoppingCart::RemoveItem()
{
	printf("请输入要删除的商品ID和数量: ");
	string ItemID;
	int ItemNum;
	while (cin >> ItemID >> ItemNum)
	{
		ItemNode *cur = SearchItem(ItemID);
		ItemNode *pre = SearchItemPre(ItemID);
		if(ItemNum < 0)
		{
			cout << "数量不能为负数, 请重新输入要删除的商品ID和数量: ";
			continue;
		}
		if (cur == NULL) // 不存在该商品
		{
			cout << "不存在该商品, 请重新输入要删除的商品ID和数量: ";
			continue;
		}
		else // 找到商品下架
		{
			if (cur->num < ItemNum)
			{
				cout << "删除数量超限, 请重新输入要删除的商品ID和数量: ";
				continue;
			}
			else
			{
				cur->num -= ItemNum;
				if (cur->num == 0) // 全部删除
				{
					pre->next = cur->next;
					delete cur;
					__ItemsNum--;
				}
				break;
			}
		}
	}
	puts("********");
	puts("删除成功!");
	puts("********");
	puts("购物车中有: ");
	Show();
	SyncData(fName);
}

void ShoppingCart::Show()
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

void ShoppingCart::Clear()
{
	double TotalPrice = 0;
	ItemNode *Scp = Head->next;
	while (Scp != NULL)
	{
		TotalPrice += Scp->num * Scp->price;
		Scp = Scp->next;
	}
	cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n";
	cout << "一共需要付款" << TotalPrice << "元! 确认付款请按1, 取消请按0: ";
	char ch;
	while (cin >> ch)
	{
		if (ch == '0')
		{
			cout << "已取消付款!\n";
			return;
		}
		if (ch == '1')
		{
			cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n";
			break;
		}
		else
		{
			cout << "无效字符, 请重新输入: ";
			continue;
		}
	}
	bool flag = 1;
	Scp = Head->next;
	while (Scp != NULL)
	{
		flag = stock.IsEnough(Scp);
		if (flag == 0)
		{
			cout << "余量不足, 结账失败!\n";
			return;
		}
		Scp = Scp->next;
	}
	Scp = Head->next;
	while (Scp != NULL)
	{
		stock.Traverse(Scp);
		Scp = Scp->next;
	}
	Scp = Head->next;
	while (Scp != NULL)
	{
		soldlist.AddSoldItem(Scp, __username);
		Scp = Scp->next;
	}
	Tail = NULL;
	while (Head->next != NULL)
	{
		ItemNode *cur = Head;
		Head = Head->next;
		delete cur;
	}
	Tail = Head;
	stock.SyncData(".\\Data\\库存.txt");
	soldlist.SyncData(".\\Data\\已售清单.txt");
	SyncData(fName);
	puts("*********");
	puts("付款成功!");
	puts("*********");
}

ItemNode* ShoppingCart::SearchItem(string ItemID)
{
	ItemNode *cur = Head->next;
	while (cur != NULL)
	{
		if (cur->id == ItemID)
			return cur;
		cur = cur->next;
	}
	return NULL;
}

void ShoppingCart::SearchID2ChangePrice(string ItemID, double NewPrice)
{
	ItemNode *cur = Head->next;
	while (cur != NULL)
	{
		if (cur->id == ItemID)
			cur->price = NewPrice;
		cur = cur->next;
	}
}

ItemNode* ShoppingCart::SearchItemPre(string ItemID) // 返回指定ID的上一个结点的指针
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

void ShoppingCart::ResetName(string NewName)
{
	string NewPath = ".\\Data\\" + NewName + ".txt";
	rename(fName.c_str(), NewPath.c_str());
	fName = ".\\Data\\" + NewName + ".txt";
	__username = NewName;
}