#include"SoldList.h"
#include<iostream>
#include<iomanip>
using namespace std;

bool cmp(SoldItemNode *pi, SoldItemNode *pj)
{
	if (pi->id > pj->id)
		return 1;
	return 0;
}

void Swap(SoldItemNode *pi, SoldItemNode *pj)
{
	SoldItemNode *temp = new SoldItemNode;
	temp->id = pi->id;
	temp->name = pi->name;
	temp->brand = pi->brand;
	temp->price = pi->price;
	temp->num = pi->num;
	temp->username = pi->username;
	pi->id = pj->id;
	pi->name = pj->name;
	pi->brand = pj->brand;
	pi->price = pj->price;
	pi->num = pj->num;
	pi->username = pj->username;
	pj->id = temp->id;
	pj->name = temp->name;
	pj->brand = temp->brand;
	pj->price = temp->price;
	pj->num = temp->num;
	pj->username = temp->username;
}

SoldList::SoldList(string FileName)
{
	Head = new SoldItemNode;
	Tail = Head;
	f.FileToSoldList(Head, Tail, FileName);
}

SoldList::~SoldList()
{
	Tail = NULL;
	while (Head != NULL)
	{
		SoldItemNode *cur = Head;
		Head = Head->next;
		delete cur;
	}
}

void SoldList::SyncData(string FileName)
{
	f.SoldListToFile(Head, FileName);
}

void SoldList::AddSoldItem(ItemNode *cur, string username)
{
	SoldItemNode *p = new SoldItemNode;
	p->id = cur->id;
	p->name = cur->name;
	p->brand = cur->brand;
	p->price = cur->price;
	p->num = cur->num;
	p->username = username;
	Tail->next = p;
	Tail = Tail->next;
	SyncData(".\\Data\\已售清单.txt");
}

void SoldList::PrintSoldList()
{
	SoldItemNode *pi, *pj;
	pi = Head->next;
	while (pi != NULL)
	{
		pj = pi->next;
		SoldItemNode *pjpre = pi;
		while (pj != NULL)
		{
			if ((pj->id == pi->id) && (pj->price == pi->price))
			{
				pi->num += pj->num;
				SoldItemNode *p = pj;
				pjpre->next = p->next;
				//delete p;
			}
			pjpre = pj;
			pj = pj->next;
		}
		pi = pi->next;
	}
	pi = Head->next;
	while (pi != NULL)
	{
		pj = pi->next;
		while (pj != NULL)
		{
			if (cmp(pi, pj))
				Swap(pi, pj);
			pj = pj->next;
		}
		pi = pi->next;
	}
	cout << "****************************************************\n";
	cout << "ID\t名称\t\t品牌\t\t价格\t数量\n";
	SoldItemNode *cur = Head->next;
	while (cur != NULL)
	{
		cout << cur->id << '\t';
		cout << left << setw(10) << cur->name << '\t';
		cout << left << setw(10) << cur->brand << '\t';
		cout << fixed << setprecision(1) << cur->price << '\t';
		cout << cur->num << '\n';
		cur = cur->next;
	}
	cout << "****************************************************\n";
}

void SoldList::Search2ChangeName(string __username, string NewName)
{
	SoldItemNode *cur = Head->next;
	while (cur != NULL)
	{
		if (cur->username == __username)
			cur->username = NewName;
		cur = cur->next;
	}
	SyncData(".\\Data\\已售清单.txt");
}

void SoldList::PrintRecords()
{
	string UserName;
	cout << "请输入要查询的用户名: ";
	cin >> UserName;
	SoldItemNode *cur = Head->next;
	cout << "****************************************************\n";
	cout << "该用户购买的商品如下: " << endl;
	cout << "ID\t名称\t\t品牌\t\t价格\t数量\n";
	while (cur != NULL)
	{
		if (cur->username == UserName)
		{
			cout << cur->id << '\t';
			cout << left << setw(10) << cur->name << '\t';
			cout << left << setw(10) << cur->brand << '\t';
			cout << fixed << setprecision(1) << cur->price << '\t';
			cout << cur->num << '\n';
		}
		cur = cur->next;
	}
	cout << "****************************************************\n";
}

void SoldList::PrintMyRecord(string __username)
{
	SoldItemNode *cur = Head->next;
	cout << "****************************************************\n";
	cout << "您已买的商品有: " << endl;
	cout << "ID\t名称\t\t品牌\t\t价格\t数量\n";
	while (cur != NULL)
	{
		if (cur->username == __username)
		{
			cout << cur->id << '\t';
			cout << left << setw(10) << cur->name << '\t';
			cout << left << setw(10) << cur->brand << '\t';
			cout << fixed << setprecision(1) << cur->price << '\t';
			cout << cur->num << '\n';
		}
		cur = cur->next;
	}
	cout << "****************************************************\n";
}