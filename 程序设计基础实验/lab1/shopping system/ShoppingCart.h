#pragma once
#include"File.h"
using std::string; 

class ShoppingCart
{
public:
	ShoppingCart() { Head = Tail = NULL; };
	ShoppingCart(string FileName);
	void __SetShoppingCart(string FileName);
	~ShoppingCart();
	void SyncData(string FileName);
	void AddItem();
	void RemoveItem();
	void Show();
	void Clear();
	void SearchID2Remove(string ItemID);
	void SearchID2ChangePrice(string ItemID, double NewPrice);
	ItemNode* SearchItem(string ItemID);
	ItemNode* SearchItemPre(string ItemID);
	void GetUserName(string username);
	void ResetName(string NewName);
private:
	ItemNode *Head;
	ItemNode *Tail;
	string fName;
	string __username;
	int __ItemsNum;
	File f;
};
