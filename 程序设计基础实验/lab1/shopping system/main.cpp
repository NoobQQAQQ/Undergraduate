#include"admin.h"
#include"user.h"
#include"UserInfo.h"
#include<iostream>
using namespace std;

extern UserInfo usersinfo;

void ShowMainMenu()
{
	puts("���, ���½~");
	puts("==========================================================");
	puts("1.�û���¼\t2.�û�ע��\t3.����Ա��½\t#.�˳�ϵͳ");
	puts("==========================================================");
	printf("���������: ");
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
			printf("��������Ч�ַ�, ����������: ");
			continue;
		}
		What2Do(flag, user, admin);
	}
}