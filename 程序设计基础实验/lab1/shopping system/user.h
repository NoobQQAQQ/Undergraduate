#pragma once
#include<string>
#include"ShoppingCart.h"
using std::string;

class User
{
public:
	User();
	~User(); 
	void SetShoppingCart();
	void Login();
	void Regis();
	void cmd();
	void SearchItem();
	void AddItem();
	void RemoveItem();
	void PrintCart();
	void Pay();
	void ChangeUserName();
	void ChangeCipher();
private:
	string __username;
	string __cipher;
	ShoppingCart __shoppingcart;
	void ShowUserMenu();
	void What2Do(char ch);
};