#include"File.h"
#include<string>
#include<list>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cstdio>
using namespace std;
typedef unsigned int uint;

const string DataPrefix = "./Data/";

int File::GetCatalog(const string &CatalogFileName, list<Pair> &TableList)
{
	int Amout = 0;
	ifstream fin(CatalogFileName);
	while (!fin.eof())
	{
		Pair temp;
		char buffer[100] = { 0 };
		fin.getline(buffer, 100);
		char ch = fin.peek();
		if (ch == '\n')
			fin.get();
		stringstream ss;
		ss << buffer;
		ss >> temp.__TableName;
		ss >> temp.__FileName;
		TableList.push_back(temp);
		ss.clear();
		Amout++;
	}
	fin.close();
	return Amout;
}

void File::SyncCatalog(const string &CatalogFileName, const list<Pair> &TableList)
{
	ofstream fout;
	fout.open(CatalogFileName, ios::trunc);
	for (auto Titr = TableList.begin(); Titr != TableList.end(); Titr++)
		fout << Titr->__TableName << ' ' << Titr->__FileName << endl;
	fout.close();
}

void File::DeleteFile(const string &FileName)
{
	if (remove((DataPrefix + FileName).c_str()))
		cout << "Fail!" << endl;
	else
		cout << "Success!" << endl;
}

void File::GetTable(const string &FileName, vector<string> &ColNameVec, vector< vector<string> > &RecordVec, vector<int> &MaxLenVec)
{
	ifstream fin(DataPrefix + FileName);
	// 读取ColNameVec
	char buffer[300] = { 0 };
	fin.getline(buffer, 300);
	stringstream ss;
	ss << buffer;
	while (!ss.eof())
	{
		string ColName;
		ss >> ColName;
		ColNameVec.push_back(ColName);
		MaxLenVec.push_back(ColName.length());
	}
	ss.clear();
	char ch1 = fin.peek();
	if (ch1 == '\n')
		fin.get();
	// 读取RecordVec
	while (!fin.eof())
	{
		vector<string> record;
		fin.getline(buffer, 300);
		char ch2 = fin.peek();
		if (ch2 == '\n')
			fin.get();
		ss << buffer;
		int Mindex = 0; // 宽度下标
		while (!ss.eof())
		{
			string temp;
			ss >> temp;
			if (temp == "***") // 空串
				record.push_back("");
			else
				record.push_back(temp);
			if (temp.length() > MaxLenVec[Mindex])
				MaxLenVec[Mindex] = temp.length();
			Mindex++;
		}
		ss.clear();
		RecordVec.push_back(record);
	}
	fin.close();
}

void File::SetTable(const string &FileName, vector<string> &ColNameVec, vector< vector<string> > &RecordVec)
{
	ofstream fout;
	fout.open(DataPrefix + FileName, ios::trunc);
	for (uint i = 0; i < ColNameVec.size() - 1; i++)
		fout << ColNameVec[i] << ' ';
	fout << ColNameVec[ColNameVec.size() - 1] << endl;
	for (uint i = 0; i < RecordVec.size(); i++)
	{
		for (uint j = 0; j < RecordVec[0].size() - 1; j++)
		{
			if (RecordVec[i][j] == "")
				fout << "***" << ' ';
			else
				fout << RecordVec[i][j] << ' ';
		}
		if (RecordVec[i][RecordVec[0].size() - 1] == "")
			fout << "***" << endl;
		else
			fout << RecordVec[i][RecordVec[0].size() - 1] << endl;
	}
	fout.close();
}