#include"user.h"
#include"Stock.h"
#include"UserInfo.h"
#include"SoldList.h"
#include<iostream>
#include<fstream>
#include<opencv2\opencv.hpp>

using namespace std;
using namespace cv;

extern SoldList soldlist;
extern UserInfo usersinfo;
extern Stock stock;

void ShowPhoto()
{
	string ItemID;
	cout << "请输入商品的ID: ";
	cin >> ItemID;
	ifstream file;
	file.open(".\\Data\\" + ItemID + ".jpg");
	if (file) // 打开成功
	{
		Mat pic = imread(".\\Data\\" + ItemID + ".jpg");
		imshow(ItemID, pic);
		waitKey();
	}
	else
	{
		Mat pic = imread(".\\Data\\default.png");
		imshow(ItemID, pic);
		waitKey();
	}
	file.close();
}

void User::ShowUserMenu()
{
	cout << "尊敬的用户: " << __username << ", 您好!" << endl;
	cout << "===========================================================================";
	cout << "=================================================================================\n";
	cout << "1.注销登录  2.查看商品  3.商品搜索  4.添加商品至购物车  5.删除购物车商品  ";
	cout << "6.查看购物车  7.结账  8.修改用户名  9.修改密码  p.购买记录  s.高级搜索  c.图片展示\n";
	cout << "===========================================================================";
	cout << "=================================================================================\n";
	cout << "请输入操作: ";
}

void User::What2Do(char ch)
{
	switch (ch)
	{
	case ('2'):
		stock.UserPrintStock();
		break;
	case ('3'):
		stock.SearchItem();
		break;
	case ('4'):
		AddItem();
		break;
	case ('5'):
		RemoveItem();
		break;
	case ('6'):
		PrintCart();
		break;
	case ('7'):
		Pay();
		break;
	case ('8'):
		ChangeUserName();
		break;
	case ('9'):
		ChangeCipher();
		break;
	case ('p'):
		soldlist.PrintMyRecord(__username);
		break;
	case ('s'):
		stock.AdvanceSearch();
		break;
	case ('c'):
		ShowPhoto();
		break;
	default:
		printf("输入了无效字符, 请重新输入!");
	}
	ShowUserMenu();
}

void User::SetShoppingCart()
{
	__shoppingcart.GetUserName(__username);
	__shoppingcart.__SetShoppingCart(".\\Data\\" + __username + ".txt");
}

User::User()
{
	
}

User::~User()
{
	__shoppingcart.~ShoppingCart();
}

void User::Login()
{
	string username, UserCipher;
	while (1)
	{
		printf("输入用户名: ");
		cin >> username;
		printf("输入密码: ");
		cin >> UserCipher;
		if (!usersinfo.CheckAccount(username, UserCipher))
		{
			puts("用户名或密码错误, 请重新输入!");
			continue;
		}
		else
		{
			puts("*********");
			puts("登录成功!");
			puts("*********");
			__username = username;
			__cipher = UserCipher;
			break;
		}
	}
}

void User::Regis()
{
	string username, UserCipher;
	printf("输入用户名(不超过20个字符，只由小写字母和数字组成): ");
	while (cin >> username)
	{
		if (usersinfo.CheckName(username)) // 有重复
		{
			cout << "该用户名已被注册, 请重新输入: ";
			continue;
		}
		break;
	}
	printf("输入密码(不超过20个字符，只由小写字母和数字组成): ");
	cin >> UserCipher;
	usersinfo.AddAccount(username, UserCipher);
	__username = username;
	__cipher = UserCipher;
	puts("*********");
	puts("登录成功!");
	puts("*********");
}

void User::cmd()
{
	SetShoppingCart();
	ShowUserMenu();
	char ch;
	while (cin >> ch)
	{
		if (ch == '1')
		{
			puts("*********");
			puts("成功退出!");
			puts("*********");
			break;
		}
		else
			What2Do(ch);
	}
}

void User::SearchItem()
{
	stock.SearchItem();
}

void User::AddItem()
{
	__shoppingcart.AddItem();
}

void User::RemoveItem()
{
	__shoppingcart.RemoveItem();
}

void User::PrintCart()
{
	__shoppingcart.Show();
}

void User::Pay()
{
	__shoppingcart.Clear();
}

void User::ChangeUserName()
{
	string NewName;
	cout << "请输入新用户名: ";
	while (cin >> NewName)
	{
		if (usersinfo.CheckName(NewName))
		{
			cout << "该用户名已被注册或和原用户名相同, 请重新输入: ";
			continue;
		}
		break;
	}
	soldlist.Search2ChangeName(__username, NewName);
	usersinfo.Search2ChangeName(__username, NewName);
	__shoppingcart.ResetName(NewName);
	__username = NewName;
	puts("********");
	puts("修改成功!");
	puts("********");
}

void User::ChangeCipher()
{
	string OldCipher, NewCipher;
	cout << "请输入原密码: ";
	while (cin >> OldCipher)
	{
		if (OldCipher != __cipher)
		{
			cout << "原密码输入错误, 请重新输入: ";
			continue;
		}
		break;
	}
	cout << "请输入新密码: ";
	while (cin >> NewCipher)
	{
		if (NewCipher == OldCipher)
		{
			cout << "新密码不能和原密码相同, 请重新输入: ";
			continue;
		}
		break;
	}
	__cipher = NewCipher;
	usersinfo.Search2ChangeCipher(__username, NewCipher);
	puts("********");
	puts("修改成功!");
	puts("********");
}