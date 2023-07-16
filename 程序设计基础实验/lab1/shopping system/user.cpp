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
	cout << "��������Ʒ��ID: ";
	cin >> ItemID;
	ifstream file;
	file.open(".\\Data\\" + ItemID + ".jpg");
	if (file) // �򿪳ɹ�
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
	cout << "�𾴵��û�: " << __username << ", ����!" << endl;
	cout << "===========================================================================";
	cout << "=================================================================================\n";
	cout << "1.ע����¼  2.�鿴��Ʒ  3.��Ʒ����  4.�����Ʒ�����ﳵ  5.ɾ�����ﳵ��Ʒ  ";
	cout << "6.�鿴���ﳵ  7.����  8.�޸��û���  9.�޸�����  p.�����¼  s.�߼�����  c.ͼƬչʾ\n";
	cout << "===========================================================================";
	cout << "=================================================================================\n";
	cout << "���������: ";
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
		printf("��������Ч�ַ�, ����������!");
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
		printf("�����û���: ");
		cin >> username;
		printf("��������: ");
		cin >> UserCipher;
		if (!usersinfo.CheckAccount(username, UserCipher))
		{
			puts("�û������������, ����������!");
			continue;
		}
		else
		{
			puts("*********");
			puts("��¼�ɹ�!");
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
	printf("�����û���(������20���ַ���ֻ��Сд��ĸ���������): ");
	while (cin >> username)
	{
		if (usersinfo.CheckName(username)) // ���ظ�
		{
			cout << "���û����ѱ�ע��, ����������: ";
			continue;
		}
		break;
	}
	printf("��������(������20���ַ���ֻ��Сд��ĸ���������): ");
	cin >> UserCipher;
	usersinfo.AddAccount(username, UserCipher);
	__username = username;
	__cipher = UserCipher;
	puts("*********");
	puts("��¼�ɹ�!");
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
			puts("�ɹ��˳�!");
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
	cout << "���������û���: ";
	while (cin >> NewName)
	{
		if (usersinfo.CheckName(NewName))
		{
			cout << "���û����ѱ�ע����ԭ�û�����ͬ, ����������: ";
			continue;
		}
		break;
	}
	soldlist.Search2ChangeName(__username, NewName);
	usersinfo.Search2ChangeName(__username, NewName);
	__shoppingcart.ResetName(NewName);
	__username = NewName;
	puts("********");
	puts("�޸ĳɹ�!");
	puts("********");
}

void User::ChangeCipher()
{
	string OldCipher, NewCipher;
	cout << "������ԭ����: ";
	while (cin >> OldCipher)
	{
		if (OldCipher != __cipher)
		{
			cout << "ԭ�����������, ����������: ";
			continue;
		}
		break;
	}
	cout << "������������: ";
	while (cin >> NewCipher)
	{
		if (NewCipher == OldCipher)
		{
			cout << "�����벻�ܺ�ԭ������ͬ, ����������: ";
			continue;
		}
		break;
	}
	__cipher = NewCipher;
	usersinfo.Search2ChangeCipher(__username, NewCipher);
	puts("********");
	puts("�޸ĳɹ�!");
	puts("********");
}