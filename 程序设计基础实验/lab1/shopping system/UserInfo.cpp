#include"UserInfo.h"
#include"ShoppingCart.h"
#include<fstream>
#include<iostream>
using namespace std;

UserInfo::UserInfo(string FileName)
{
	Head = new UserNode;
	Tail = Head;
	NumOfUsers = f.GetUserInfo(Head, Tail, FileName);
}

void UserInfo::SyncData(string FileName)
{
	f.SetUserInfo(Head, FileName);
}

bool UserInfo::CheckName(string username) // 有重名返回1, 无重名返回0
{
	UserNode *cur = Head->next;
	while (cur != NULL)
	{
		if (cur->AcountInfo.username == username)
			return 1;
		cur = cur->next;
	}
	return 0;
}

bool UserInfo::CheckAccount(string username, string cipher)
{
	UserNode *cur = Head->next;
	while (cur != NULL)
	{
		if (cur->AcountInfo.username == username && cur->AcountInfo.cipher == cipher)
			return 1;
		cur = cur->next;
	}
	return 0;
}

UserInfo::~UserInfo()
{
	Tail = NULL;
	while (Head != NULL)
	{
		UserNode *cur = Head;
		Head = Head->next;
		delete cur;
	}
}

void UserInfo::Traverse(string ItemID)
{
	UserNode *cur = Head->next;
	while (cur != NULL)
	{
		ShoppingCart Sc(".\\Data\\" + cur->AcountInfo.username + ".txt");
		Sc.SearchID2Remove(ItemID);
		Sc.SyncData(".\\Data\\" + cur->AcountInfo.username + ".txt");
		cur = cur->next;
	}
}

void UserInfo::AddAccount(string username, string UserCipher)
{
	NumOfUsers++;
	UserNode *p = new UserNode;
	p->AcountInfo.username = username;
	p->AcountInfo.cipher = UserCipher;
	Tail->next = p;
	Tail = Tail->next;
	fstream f; // 创建新文件
	f.open(".\\Data\\" + username + ".txt", ios::out);
	f << "ID\t名称\t品牌\t价格\t数量\n";
	f.close();
	SyncData(".\\Data\\用户.txt");
}

void UserInfo::Traverse(string ItemID, double NewPrice)
{
	UserNode *cur = Head->next;
	while (cur != NULL)
	{
		ShoppingCart Sc(".\\Data\\" + cur->AcountInfo.username + ".txt");
		Sc.SearchID2ChangePrice(ItemID, NewPrice);
		Sc.SyncData(".\\Data\\" + cur->AcountInfo.username + ".txt");
		cur = cur->next;
	}
}

void UserInfo::Search2ChangeCipher(string username, string NewCipher)
{
	UserNode *cur = Head->next;
	while (cur != NULL)
	{
		if (username == cur->AcountInfo.username)
		{
			cur->AcountInfo.cipher = NewCipher;
			SyncData(".\\Data\\用户.txt");
			return;
		}
		cur = cur->next;
	}
}

void UserInfo::Search2ChangeName(string __username, string NewName)
{
	UserNode *cur = Head->next;
	while (cur != NULL)
	{
		if (__username == cur->AcountInfo.username)
		{
			cur->AcountInfo.username = NewName;
			SyncData(".\\Data\\用户.txt");
			return;
		}
		cur = cur->next;
	}
}

void UserInfo::PrintUsers()
{
	UserNode *cur = Head->next;
	cout << "***********************************************************\n";
	cout << "共有" << NumOfUsers << "名用户\n";
	cout << "用户名\t密码\n";
	while (cur != NULL)
	{
		cout << cur->AcountInfo.username << '\t' << cur->AcountInfo.cipher << endl;
		cur = cur->next;
	}
}