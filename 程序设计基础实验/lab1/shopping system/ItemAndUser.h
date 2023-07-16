#pragma once
#include<string>
using std::string;

struct Item // 商品信息
{
	string id;
	string name;
	string brand;
	double price;
	int num;
};

struct ItemNode : public Item // 商品结点
{
	ItemNode *next;
	ItemNode() { next = NULL; }
};

struct SoldItemNode : public Item // 售出商品结点
{
	string username;
	SoldItemNode *next;
	SoldItemNode() { next = NULL; }
};

struct UserNode // 用户信息结点
{
	struct AccountInfo
	{
		string username;
		string cipher;
	} AcountInfo;
	UserNode *next;
	UserNode() { next = NULL; }
};