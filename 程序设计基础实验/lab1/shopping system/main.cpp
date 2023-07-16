#include"admin.h"
#include"user.h"
#include"UserInfo.h"
#include<iostream>
using namespace std;

extern UserInfo usersinfo;

void ShowMainMenu()
{
	puts("你好, 请登陆~");
	puts("==========================================================");
	puts("1.用户登录\t2.用户注册\t3.管理员登陆\t#.退出系统");
	puts("==========================================================");
	printf("请输入操作: ");
}

void What2Do(char flag, User &user, Admin &admin)
{
	switch (flag)
	{
	case ('1'):
		user.Login();
		user.cmd();
		break;
	case ('2'):
		user.Regis();
		user.cmd();
		break;
	case ('3'):
		admin.Login();
		admin.cmd();
		break;
	}
	ShowMainMenu();
}

int main()
{
	User user;
	Admin admin("Noob", "666666");
	char flag;
	ShowMainMenu();
	while (cin >> flag)
	{
		if (flag == '#')
			return 0;
		if (flag != '1' && flag != '2' && flag != '3')
		{
			printf("输入了无效字符, 请重新输入: ");
			continue;
		}
		What2Do(flag, user, admin);
	}
}