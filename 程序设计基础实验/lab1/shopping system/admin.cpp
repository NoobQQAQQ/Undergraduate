#include"admin.h"
#include"Stock.h"
#include"SoldList.h"
#include"UserInfo.h"
#include<iostream>
using namespace std;

extern Stock stock;
extern SoldList soldlist;
extern UserInfo usersinfo;

Admin::Admin(string AdminName, string AdminCipher)
{
	__AdminName = AdminName;
	__AdminCipher = AdminCipher;
}

void Admin::ShowAdminMenu()
{
	cout << "尊敬的管理员: " << __AdminName << ", 您好!" << endl;
	puts("===========================================================================================");
	puts("0.注销登陆 1.查询商品 2.增加商品 3.删除商品 4.修改商品 5.售货清单 6.用户清单 7.查询购买记录");
	puts("===========================================================================================");
	printf("请输入操作: ");
}

void Admin::What2Do(char ch)
{
	switch (ch)
	{
	case ('1'):
		stock.PrintStock();
		break;
	case ('2'):
		stock.AddItem();
		break;
	case ('3'):
		stock.RemoveItem();
		break;
	case ('4'):
		stock.ChangeItem();
		break;
	case ('5'):
		soldlist.PrintSoldList();
		break;
	case ('6'):
		usersinfo.PrintUsers();
		break;
	case ('7'):
		soldlist.PrintRecords();
		break;
	default:
		printf("输入了无效字符, 请重新输入!");
	}
	ShowAdminMenu();
}

void Admin::Login()
{
	string AdminName, AdminCipher;
	while (1)
	{
		printf("输入管理员: ");
		cin >> AdminName;
		printf("输入管理员密码: ");
		cin >> AdminCipher;
		if (AdminName != __AdminName || AdminCipher != __AdminCipher)
		{
			puts("用户名或密码错误, 请重新输入!");
			continue;
		}
		else
		{
			puts("*********");
			puts("登录成功!");
			puts("*********");
			break;
		}
	}
}

void Admin::cmd()
{
	ShowAdminMenu();
	char ch;
	while (cin >> ch)
	{
		if (ch == '0')
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