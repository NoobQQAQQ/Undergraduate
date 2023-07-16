#pragma once
#include<string>
#include<list>
#include<vector>
#include"Pair.h"
using namespace std;

class File
{
public:
	int GetCatalog(const string &CatalogFileName, list<Pair> &TableList); // ��ȡĿ¼�ļ�
	void SyncCatalog(const string &CatalogFileName, const list<Pair> &TableList); // ͬ��Ŀ¼�ļ�
	void DeleteFile(const string &FileName); // ɾ��һ���ļ�
	void GetTable(const string &FileName, vector<string> &ColNameVec, vector< vector<string> > &RecordVec, vector<int> &MaxLenVec); // ��ȡ����ļ�
	void SetTable(const string &FileName, vector<string> &ColNameVec, vector< vector<string> > &RecordVec); // ͬ������ļ�
};