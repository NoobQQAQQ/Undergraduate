#pragma once
#include<string>
using namespace std;


struct Pair
{
	string __TableName; // ����
	string __FileName; // �ļ���
	Pair() { __TableName = __FileName = ""; }
	Pair(const string &FileName, const string &TableName)
	{
		__FileName = FileName;
		__TableName = TableName;
	}
};