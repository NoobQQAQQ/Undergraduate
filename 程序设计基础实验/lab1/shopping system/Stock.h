#pragma once
#include"File.h"
using std::string;

class Stock
{
public:
	Stock(string FileName); // ���ļ���ȡ�������
	void SyncData(string FileName); // ͬ���ڴ��еĿ�浽�ļ���
	void PrintStock(); // ����Ա��ӡ���
	void UserPrintStock(); // �û���ӡ���
	void AddItem(); // ������Ʒ
	void RemoveItem(); // �Ƴ���Ʒ
	void ChangeItem(); // �޸���Ʒ����
	ItemNode* SearchItem(string ItemName, string ItemBrand) const; // ���ҿ�����ض���Ʒ
	ItemNode* SearchItem(string ItemID) const; // ���ҿ�����ض���Ʒ
	void SearchItem(); // ������Ʒ��չʾ
	void AdvanceSearch(); // �߼�����
	void SearchNameBrand(); // ��������Ʒ������
	void SearchID(); // ����ID����
	void SearchBrand(); // ����Ʒ������
	bool IsEnough(ItemNode *Scp);  // �жϿ���Ƿ��㹻
	void Traverse(ItemNode *Scp); // ��������
	~Stock();
private:
	ItemNode *Head; // ������ͷ���
	ItemNode *Tail;
	File f;
	int __ItemNum;
	void GetID(string &next); // ��������ƷID
};