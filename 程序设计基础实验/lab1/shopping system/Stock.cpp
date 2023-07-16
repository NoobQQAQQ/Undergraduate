#include"Stock.h"
#include"UserInfo.h"
#include<iostream>
#include<sstream>
#include<iomanip>
using namespace std;

extern UserInfo usersinfo;

Stock::Stock(string FileName)
{
	Head = new ItemNode;
	Tail = Head;
	__ItemNum = f.FileToList(Head, Tail, FileName);
}

Stock::~Stock()
{
	Tail = NULL;
	while (Head != NULL)
	{
		ItemNode *cur = Head;
		Head = Head->next;
		delete cur;
	}
}

void Stock::GetID(string &next)
{
	int Sum = __ItemNum;
	char Id[6] = { 0 };
	Id[0] = 'F';
	int digits[4];
	digits[0] = Sum / 1000;
	Sum %= 1000;
	digits[1] = Sum / 100;
	Sum %= 100;
	digits[2] = Sum / 10;
	digits[3] = Sum % 10;
	Id[1] = (char) (digits[0] + 48);
	Id[2] = (char) (digits[1] + 48);
	Id[3] = (char) (digits[2] + 48);
	Id[4] = (char) (digits[3] + 48);
	Id[5] = '\0';
	next = Id;
}

void Stock::SyncData(string FileName)
{
	f.ListToFile(Head, FileName);
}

void Stock::PrintStock()
{
	cout << "****************************************************\n";
	cout << "ID\t����\t\tƷ��\t\t�۸�\t����\n";
	ItemNode *cur = Head->next;
	while (cur != NULL)
	{
		cout << cur->id << '\t';
		cout << std::left << setw(10) << cur->name << '\t';
		cout << std::left << setw(10) << cur->brand << '\t';
		cout << fixed << setprecision(1) << cur->price << '\t';
		cout << cur->num << '\n';
		cur = cur->next;
	}
	cout << "****************************************************\n";
}

void Stock::UserPrintStock()
{
	cout << "****************************************************\n";
	cout << "ID\t����\t\tƷ��\t\t�۸�\t����\n";
	ItemNode *cur = Head->next;
	while (cur != NULL)
	{
		if (cur->num <= 0)
		{
			cur = cur->next;
			continue;
		}
		else
		{
			cout << cur->id << '\t';
			cout << std::left << setw(10) << cur->name << '\t';
			cout << std::left << setw(10) << cur->brand << '\t';
			cout << fixed << setprecision(1) << cur->price << '\t';
			cout << cur->num << '\n';
			cur = cur->next;
		}
	}
	cout << "****************************************************\n";
}

void Stock::AddItem()
{
	string ItemName;
	string ItemBrand;
	double ItemPrice;
	int ItemNum;
	printf("������Ҫ��ӵ���Ʒ��: ");
	cin >> ItemName;
	printf("������Ҫ��ӵ���ƷƷ��: ");
	cin >> ItemBrand;
	ItemNode *p = SearchItem(ItemName, ItemBrand);
	if (p != NULL) // ���и���Ʒ
	{
		printf("������Ҫ��ӵ���Ʒ����: ");
		cin >> ItemNum;
		p->num = (p->num == -1) ? ItemNum : (p->num + ItemNum);
	}
	else // �������Ʒ
	{
		printf("������Ҫ��ӵ���Ʒ�۸�: ");
		cin >> ItemPrice;
		printf("������Ҫ��ӵ���Ʒ����: ");
		cin >> ItemNum;
		ItemNode *q = new ItemNode;
		q->name = ItemName;
		q->brand = ItemBrand;
		q->price = ItemPrice;
		q->num = ItemNum;
		if (Head->next == NULL) // ���Ϊ��
			q->id = "F0000";
		else
			GetID(q->id); // �õ���ƷID
		Tail->next = q;
		Tail = Tail->next;
		__ItemNum++;
	}
	puts("�޸ĺ�Ŀ������: ");
	PrintStock();
	SyncData(".\\Data\\���.txt"); // ͬ������
}

void Stock::RemoveItem()
{
	cout << "��������Ҫ�¼ܵ���ƷID: ";
	string ItemID;
	while (cin >> ItemID)
	{
		ItemNode *cur = SearchItem(ItemID);
		if (cur == NULL) // �����ڸ���Ʒ
		{
			cout << "�����ڸ���Ʒ, ����������: ";
			continue;
		}
		else // �ҵ���Ʒ�¼�
		{
			cur->next->num = -1;
			break;
		}
	}
	usersinfo.Traverse(ItemID); // ͬ�����ﳵ
	puts("�޸ĺ�Ŀ������: ");
	PrintStock();
	SyncData(".\\Data\\���.txt"); // ͬ������
}

void Stock::ChangeItem()
{
	char ch;
	cout << "��ѡ�����(1: �޸���Ʒ����; 2: �޸���Ʒ�۸�): ";
	while (cin >> ch)
	{
		string ItemID;
		if (ch != '1' && ch != '2')
		{
			cout << "������Ч�ַ�, ����������: ";
			continue;
		}
		else if (ch == '1')
		{
			int NewNum;
			ItemNode *cur = NULL;
			cout << "��������ƷID: ";
			while (cin >> ItemID)
			{
				cur = SearchItem(ItemID);
				if (cur == NULL) // �����ڸ���Ʒ
				{
					cout << "�����ڸ���Ʒ, ����������: ";
					continue;
				}
				break;
			}
			cout << "�������޸ĺ������: ";
			while (cin >> NewNum)
			{
				if (NewNum < 0)
				{
					cout << "��������Ϊ��, ����������: ";
					continue;
				}
				break;
			}
			cur->next->num = NewNum;
		}
		else // ch = 2
		{
			double NewPrice;
			ItemNode *cur = NULL;
			cout << "��������ƷID: ";
			while (cin >> ItemID)
			{
				cur = SearchItem(ItemID);
				if (cur == NULL) // �����ڸ���Ʒ
				{
					cout << "�����ڸ���Ʒ, ����������: ";
					continue;
				}
				break;
			}
			cout << "�������޸ĺ�ļ۸�: ";
			while (cin >> NewPrice)
			{
				if (NewPrice < 0)
				{
					cout << "�۸���Ϊ��, ����������: ";
					continue;
				}
				break;
			}
			cur->next->price = NewPrice;
			// �������ﳵѰ������IDΪItemID����Ʒ, �������޸ļ۸�.
			usersinfo.Traverse(ItemID, NewPrice);
		}
		break;
	}
	puts("�޸ĺ�Ŀ������: ");
	PrintStock();
	SyncData(".\\Data\\���.txt"); // ͬ������
}

ItemNode* Stock::SearchItem(string ItemName, string ItemBrand) const
{
	ItemNode *cur = Head->next;
	while (cur != NULL)
	{
		if (cur->name == ItemName && cur->brand == ItemBrand)
			return cur;
		cur = cur->next;
	}
	return NULL;
}

ItemNode* Stock::SearchItem(string ItemID) const // ����ָ��ID��ǰһ��ָ��
{
	ItemNode *cur = Head;
	while (cur->next != NULL)
	{
		if (cur->next->id == ItemID)
			return cur;
		cur = cur->next;
	}
	return NULL;
}

void Stock::SearchItem()
{
	string ItemName;
	cout << "������Ʒ����: ";
	cin >> ItemName;
	ItemNode *cur = Head->next;
	cout << "****************************************************\n";
	cout << "ID\t����\t\tƷ��\t\t�۸�\t����\n";
	while (cur != NULL)
	{
		if (cur->name == ItemName && cur->num > 0)
		{
			cout << cur->id << '\t';
			cout << std::left << setw(10) << cur->name << '\t';
			cout << std::left << setw(10) << cur->brand << '\t';
			cout << fixed << setprecision(1) << cur->price << '\t';
			cout << cur->num << '\n';
		}
		cur = cur->next;
	}
	cout << "****************************************************\n";
}

void Stock::SearchNameBrand()
{
	string ItemName, ItemBrand;
	cout << "������Ʒ����: ";
	cin >> ItemName;
	cout << "������ƷƷ��: ";
	cin >> ItemBrand;
	ItemNode *cur = Head->next;
	cout << "****************************************************\n";
	cout << "ID\t����\t\tƷ��\t\t�۸�\t����\n";
	while (cur != NULL)
	{
		if (cur->name == ItemName && cur->brand == ItemBrand && cur->num > 0)
		{
			cout << cur->id << '\t';
			cout << std::left << setw(10) << cur->name << '\t';
			cout << std::left << setw(10) << cur->brand << '\t';
			cout << fixed << setprecision(1) << cur->price << '\t';
			cout << cur->num << '\n';
		}
		cur = cur->next;
	}
	cout << "****************************************************\n";
}

void Stock::SearchID()
{
	string ItemID;
	cout << "������ƷID: ";
	cin >> ItemID;
	ItemNode *cur = Head->next;
	cout << "****************************************************\n";
	cout << "ID\t����\t\tƷ��\t\t�۸�\t����\n";
	while (cur != NULL)
	{
		if (cur->id == ItemID && cur->num > 0)
		{
			cout << cur->id << '\t';
			cout << std::left << setw(10) << cur->name << '\t';
			cout << std::left << setw(10) << cur->brand << '\t';
			cout << fixed << setprecision(1) << cur->price << '\t';
			cout << cur->num << '\n';
		}
		cur = cur->next;
	}
	cout << "****************************************************\n";
}

void Stock::SearchBrand()
{
	string ItemBrand;
	cout << "������ƷƷ��: ";
	cin >> ItemBrand;
	ItemNode *cur = Head->next;
	cout << "****************************************************\n";
	cout << "ID\t����\t\tƷ��\t\t�۸�\t����\n";
	while (cur != NULL)
	{
		if (cur->brand == ItemBrand && cur->num > 0)
		{
			cout << cur->id << '\t';
			cout << std::left << setw(10) << cur->name << '\t';
			cout << std::left << setw(10) << cur->brand << '\t';
			cout << fixed << setprecision(1) << cur->price << '\t';
			cout << cur->num << '\n';
		}
		cur = cur->next;
	}
	cout << "****************************************************\n";
}

void Stock::AdvanceSearch()
{
	char ch;
	bool flag = 0;
	cout << "��ѡ������ģʽ(1: ����+Ʒ��||2: ID||3: Ʒ��): ";
	while (cin >> ch)
	{
		switch (ch)
		{
		case('1'):
			flag = 1;
			SearchNameBrand();
			break;
		case('2'):
			flag = 1;
			SearchID();
			break;
		case('3'):
			flag = 1;
			SearchBrand();
			break;
		default:
			cout << "��������Ч�ַ�, ����������: ";
		}
		if (flag)
			break;
	}
}

bool Stock::IsEnough(ItemNode *Scp)
{
	ItemNode *cur = SearchItem(Scp->id);
	if (cur->next->num >= Scp->num)
		return 1;
	return 0;
}

void Stock::Traverse(ItemNode *Scp)
{
	ItemNode *cur = SearchItem(Scp->id);
	cur->next->num -= Scp->num;
}