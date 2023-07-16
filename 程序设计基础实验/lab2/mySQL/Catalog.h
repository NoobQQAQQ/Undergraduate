#pragma once
#include<list>
#include"File.h"
#include"Pair.h"
using namespace std;

class Catalog
{
public:
	Catalog(); // Ĭ�Ϲ��캯��
	~Catalog(); // ��������
	Catalog(const string &FileName); // �ӱ����ļ���ȡ������Ϣ
	bool CheckTableName(const string &TableName) const; // ������
	string GetFileName(const string &TableName) const; // ���ݱ��������ļ���
	string Check(const string &FileName) const; // ����ļ��Ƿ��Ѿ��ͱ���Բ����ر���
	void CreateTable(const string &FileName, const string &TableName); // ����һ����
	void DropTable(const string &TableName); // ɾ��һ����
	void PrintList() const; // ��ӡ�����Ϣ
private:
	int TableNum; // �������
	list<Pair> TableList; // ��������ļ��Ķ�Ӧ��Ϣ
	File f; // �����ļ�IO
	string CatalogFileName; // ���汾���ļ���
};