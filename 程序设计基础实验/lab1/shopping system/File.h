#pragma once
#include"ItemAndUser.h"
using std::stringstream;
using std::ofstream;

class File
{
public:
	int FileToList(ItemNode *&Head, ItemNode *&Tail, string FileName); // ���ļ���ȡ��Ʒ��Ϣ������
	void ListToFile(ItemNode *Head, string FileName); // �Ѵ���Ʒ��Ϣ������д���ļ�
	int GetUserInfo(UserNode *&Head, UserNode *&Tail, string FileName); // �õ��û���Ϣ
	void SetUserInfo(UserNode *&Head, string FileName); // �����û���Ϣ
	void FileToSoldList(SoldItemNode *&Head, SoldItemNode *&Tail, string FileName); // ���ļ���ȡ�۳��嵥
	void SoldListToFile(SoldItemNode *Head, string FileName); // ���۳��嵥д���ļ�
private:
	void GetValue(stringstream &ss, ItemNode *p); // ���ַ����������Ʒ����
	void GetValue(stringstream &ss, UserNode *p); // ���ַ���������û�����
	void GetValue(stringstream &ss, SoldItemNode *p); // ���ַ���������۳���Ʒ����
	void SetValue(ofstream &fout, ItemNode *cur); // ���ļ���д��Ʒ����
	void SetValue(ofstream &fout, UserNode *cur); // ���ļ���д�û�����
	void SetValue(ofstream &fout, SoldItemNode *cur); // ���ļ���д�۳���Ʒ����
};