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
	printf("������Ҫ��ӵ���ƷID������: ");
	while (cin >> ItemID >> ItemNum)
	{
		if (ItemNum < 0)
		{
			cout << "��������Ϊ����, ������������ƷID������: ";
			continue;
		}
		break;
	}
	ItemNode *p = SearchItem(ItemID);
	ItemNode *stockinfo = stock.SearchItem(ItemID);
	if (p != NULL) // ���и���Ʒ
		p->num += ItemNum;
	else // �������Ʒ
	{
		if (stockinfo == NULL)
		{
			cout << "�����û�и���Ʒ, ���ʧ��!" << endl;
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
	puts("�Ѿ����빺�ﳵ");
	puts("*************");
	puts("���ﳵ����: ");
	Show();
	SyncData(fName);
}

void ShoppingCart::RemoveItem()
{
	printf("������Ҫɾ������ƷID������: ");
	string ItemID;
	int ItemNum;
	while (cin >> ItemID >> ItemNum)
	{
		ItemNode *cur = SearchItem(ItemID);
		ItemNode *pre = SearchItemPre(ItemID);
		if(ItemNum < 0)
		{
			cout << "��������Ϊ����, ����������Ҫɾ������ƷID������: ";
			continue;
		}
		if (cur == NULL) // �����ڸ���Ʒ
		{
			cout << "�����ڸ���Ʒ, ����������Ҫɾ������ƷID������: ";
			continue;
		}
		else // �ҵ���Ʒ�¼�
		{
			if (cur->num < ItemNum)
			{
				cout << "ɾ����������, ����������Ҫɾ������ƷID������: ";
				continue;
			}
			else
			{
				cur->num -= ItemNum;
				if (cur->num == 0) // ȫ��ɾ��
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
	puts("ɾ���ɹ�!");
	puts("********");
	puts("���ﳵ����: ");
	Show();
	SyncData(fName);
}

void ShoppingCart::Show()
{
	cout << "****************************************************\n";
	cout << "ID\t����\t\tƷ��\t\t�۸�\t����\n";
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
	cout << "һ����Ҫ����" << TotalPrice << "Ԫ! ȷ�ϸ����밴1, ȡ���밴0: ";
	char ch;
	while (cin >> ch)
	{
		if (ch == '0')
		{
			cout << "��ȡ������!\n";
			return;
		}
		if (ch == '1')
		{
			cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n";
			break;
		}
		else
		{
			cout << "��Ч�ַ�, ����������: ";
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
			cout << "��������, ����ʧ��!\n";
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
	stock.SyncData(".\\Data\\���.txt");
	soldlist.SyncData(".\\Data\\�����嵥.txt");
	SyncData(fName);
	puts("*********");
	puts("����ɹ�!");
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

ItemNode* ShoppingCart::SearchItemPre(string ItemID) // ����ָ��ID����һ������ָ��
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