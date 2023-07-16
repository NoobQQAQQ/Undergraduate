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

bool UserInfo::CheckName(string username) // ����������1, ����������0
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
	fstream f; // �������ļ�
	f.open(".\\Data\\" + username + ".txt", ios::out);
	f << "ID\t����\tƷ��\t�۸�\t����\n";
	f.close();
	SyncData(".\\Data\\�û�.txt");
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
			SyncData(".\\Data\\�û�.txt");
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
			SyncData(".\\Data\\�û�.txt");
			return;
		}
		cur = cur->next;
	}
}

void UserInfo::PrintUsers()
{
	UserNode *cur = Head->next;
	cout << "***********************************************************\n";
	cout << "����" << NumOfUsers << "���û�\n";
	cout << "�û���\t����\n";
	while (cur != NULL)
	{
		cout << cur->AcountInfo.username << '\t' << cur->AcountInfo.cipher << endl;
		cur = cur->next;
	}
}