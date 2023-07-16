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
	cout << "�𾴵Ĺ���Ա: " << __AdminName << ", ����!" << endl;
	puts("===========================================================================================");
	puts("0.ע����½ 1.��ѯ��Ʒ 2.������Ʒ 3.ɾ����Ʒ 4.�޸���Ʒ 5.�ۻ��嵥 6.�û��嵥 7.��ѯ�����¼");
	puts("===========================================================================================");
	printf("���������: ");
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
		printf("��������Ч�ַ�, ����������!");
	}
	ShowAdminMenu();
}

void Admin::Login()
{
	string AdminName, AdminCipher;
	while (1)
	{
		printf("�������Ա: ");
		cin >> AdminName;
		printf("�������Ա����: ");
		cin >> AdminCipher;
		if (AdminName != __AdminName || AdminCipher != __AdminCipher)
		{
			puts("�û������������, ����������!");
			continue;
		}
		else
		{
			puts("*********");
			puts("��¼�ɹ�!");
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
			puts("�ɹ��˳�!");
			puts("*********");
			break;
		}
		else
			What2Do(ch);
	}
}