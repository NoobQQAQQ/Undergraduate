#pragma once
#include<string>
#include<list>
#include<vector>
#include"Pair.h"
using namespace std;

class File
{
public:
	int GetCatalog(const string &CatalogFileName, list<Pair> &TableList); // 读取目录文件
	void SyncCatalog(const string &CatalogFileName, const list<Pair> &TableList); // 同步目录文件
	void DeleteFile(const string &FileName); // 删除一个文件
	void GetTable(const string &FileName, vector<string> &ColNameVec, vector< vector<string> > &RecordVec, vector<int> &MaxLenVec); // 读取表格文件
	void SetTable(const string &FileName, vector<string> &ColNameVec, vector< vector<string> > &RecordVec); // 同步表格文件
};