#pragma once
#include<list>
#include"File.h"
#include"Pair.h"
using namespace std;

class Catalog
{
public:
	Catalog(); // 默认构造函数
	~Catalog(); // 析构函数
	Catalog(const string &FileName); // 从本地文件读取表项信息
	bool CheckTableName(const string &TableName) const; // 检查表名
	string GetFileName(const string &TableName) const; // 根据表名返回文件名
	string Check(const string &FileName) const; // 检查文件是否已经和表配对并返回表名
	void CreateTable(const string &FileName, const string &TableName); // 创建一个表
	void DropTable(const string &TableName); // 删除一个表
	void PrintList() const; // 打印表的信息
private:
	int TableNum; // 表的数量
	list<Pair> TableList; // 储存表与文件的对应信息
	File f; // 用于文件IO
	string CatalogFileName; // 储存本地文件名
};