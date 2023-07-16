#pragma once
#include<string>
using std::string;

struct Item // ��Ʒ��Ϣ
{
	string id;
	string name;
	string brand;
	double price;
	int num;
};

struct ItemNode : public Item // ��Ʒ���
{
	ItemNode *next;
	ItemNode() { next = NULL; }
};

struct SoldItemNode : public Item // �۳���Ʒ���
{
	string username;
	SoldItemNode *next;
	SoldItemNode() { next = NULL; }
};

struct UserNode // �û���Ϣ���
{
	struct AccountInfo
	{
		string username;
		string cipher;
	} AcountInfo;
	UserNode *next;
	UserNode() { next = NULL; }
};