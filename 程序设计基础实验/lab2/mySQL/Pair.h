#pragma once
#include<string>
using namespace std;


struct Pair
{
	string __TableName; // 表名
	string __FileName; // 文件名
	Pair() { __TableName = __FileName = ""; }
	Pair(const string &FileName, const string &TableName)
	{
		__FileName = FileName;
		__TableName = TableName;
	}
};