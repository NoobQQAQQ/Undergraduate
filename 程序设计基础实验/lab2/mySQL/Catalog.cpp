#include<iostream>
#include<string>
#include<cctype>
#include"Table.h"
#include"Catalog.h"
using namespace std;

Catalog::Catalog() { TableNum = 0; }

Catalog::~Catalog() {}

Catalog::Catalog(const string &FileName)
{
	CatalogFileName = FileName;
	TableNum = f.GetCatalog(CatalogFileName, TableList);
}

bool Catalog::CheckTableName(const string &TableName) const
{
	for (auto Titr = TableList.begin(); Titr != TableList.end(); Titr++)
		if (Titr->__TableName == TableName)
			return 0;
	return 1;
}

string Catalog::GetFileName(const string &TableName) const
{
	for (auto Titr = TableList.begin(); Titr != TableList.end(); Titr++)
		if (Titr->__TableName == TableName)
			return Titr->__FileName;
	return "";
}

string Catalog::Check(const string &FileName) const
{
	for (auto Titr = TableList.begin(); Titr != TableList.end(); Titr++)
		if (Titr->__FileName == FileName)
			return Titr->__TableName;
	return "";
}

void Catalog::CreateTable(const string &FileName, const string &TableName)
{
	Pair p(FileName, TableName);
	TableList.push_back(p);
	f.SyncCatalog(CatalogFileName, TableList);
	TableNum++;
}

void Catalog::DropTable(const string &TableName)
{
	for (auto Titr = TableList.begin(); Titr != TableList.end(); ++Titr)
		if (Titr->__TableName == TableName)
		{
			f.DeleteFile(Titr->__FileName);
			Titr = TableList.erase(Titr);
			f.SyncCatalog(CatalogFileName, TableList);
			TableNum--;
			break;
		}
}

void Catalog::PrintList() const
{
	cout << "  total: " << TableNum << endl;
	for (auto Titr = TableList.begin(); Titr != TableList.end(); ++Titr)
	{
		Table temp;
		temp.GetTableName(Titr->__TableName);
		temp.OpenFile(Titr->__FileName);
		temp.ShowAttribute();
	}
}

Catalog SQLINFO("Catalog.txt");