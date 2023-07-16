#pragma once
#include<vector>
#include"File.h"
using namespace std;

class Table
{
public:
	Table();
	~Table();
	bool isEqual(const vector<string> &Record1, const vector<string> &Record2) const; // 整条记录
	bool isEqual(const vector<string> &AttNameVec, const vector<string> &Record1, const vector<string> &Record2) const; // 记录中的部分列
	void UniRecordCopy(vector< vector<string> > &RecordCopy) const ; // 整条记录去重
	void UniRecordCopy(const vector<string> &AttNameVec, vector< vector<string> > &RecordCopy) const; // 根据部分列去重
	bool isGreat(const vector<int> &AttIndexVec, vector<string> &Record1, vector<string> &Record2) const; // R1 > R2 return 1
	void OpenFile(const string &FileName); // 打开文件读取表信息
	void CreateFile(const string &FileName, const vector<string> &AttNameVec); // 创建新表
	void CloseFile(); // 关闭文件并同步信息
	void ShowAttribute() const; // 打印表信息
	void GetTableName(const string &TableName); // 得到表名
	void ShowTable() const; // 展示整张表, 自动添加ID列
	void RegetMaxLen(); // 得到列最宽值
	int FindColIndex(const string &ColName) const; // 返回名称为ColName的列下标
	void InsertRecord(const vector<string> &Record); // 向记录里插入一行
	void InsertRecord(const vector<string> &AttNameVec, const vector<string> &ValueNameVec); // 插入一条记录, 只插入指定列
	// 删除ColName = RecordValue的记录, 若不传参则删除全部
	void DeleteRecord(const string &ColName = "", const string &RecordValue = ""); 
	// 更新SelectedCol = SelectedValue的记录, 若不传参则更新所有行
	void UpdateRecord(const vector<string> &AttNameVec, const vector<string> &NewValueVec, const string &SelectedCol = "", const string &SelectedValue = "");
	// 去重展示所有列, 条件可选
	void DisSelectedShow(const string &SelectedCol = "", const string &SelectedValue = "") const;
	// 不去重展示所有列, 条件可选
	void NoDisSelectedShow(const string &SelectedCol = "", const string &SelectedValue = "") const;
	// 去重展示指定列, 条件可选
	void DisSelectedShow(const vector<string> &AttNameVec, const string &SelectedCol = "", const string &SelectedValue = "") const;
	// 不去重展示指定列, 条件可选
	void NoDisSelectedShow(const vector<string> &AttNameVec, const string &SelectedCol = "", const string &SelectedValue = "") const;
	// 以指定列排序, 可选升序降序
	void SelectedSort(const vector<string> &AttNameVec, const string &mode) const;
	// 去重保存所有列, 条件可选
	void DisSelectedToFile(const string &FileName, const string &SelectedCol = "", const string &SelectedValue = "") const;
	// 不去重保存所有列, 条件可选
	void NoDisSelectedToFile(const string &FileName, const string &SelectedCol = "", const string &SelectedValue = "") const;
	// 去重保存指定列, 条件可选
	void DisSelectedToFile(const string &FileName, const vector<string> &AttNameVec, const string &SelectedCol = "", const string &SelectedValue = "") const;
	// 不去重保存指定列, 条件可选
	void NoDisSelectedToFile(const string &FileName, const vector<string> &AttNameVec, const string &SelectedCol = "", const string &SelectedValue = "") const;
	void UpdateRecordLess(const vector<string> &AttNameVec, const vector<string> &ValueNameVec, const string &SelectedCol, const string &SelectedValue);
	void UpdateRecordGreat(const vector<string> &AttNameVec, const vector<string> &ValueNameVec, const string &SelectedCol, const string &SelectedValue);
	void UpdateRecordNoEqual(const vector<string> &AttNameVec, const vector<string> &ValueNameVec, const string &SelectedCol, const string &SelectedValue);
	void UpdateRecordNoLess(const vector<string> &AttNameVec, const vector<string> &ValueNameVec, const string &SelectedCol, const string &SelectedValue);
	void UpdateRecordNoGreat(const vector<string> &AttNameVec, const vector<string> &ValueNameVec, const string &SelectedCol, const string &SelectedValue);
	void DeleteRecordLess(const string &SelectedCol, const string &SelectedValue);
	void DeleteRecordGreat(const string &SelectedCol, const string &SelectedValue);
	void DeleteRecordNoEqual(const string &SelectedCol, const string &SelectedValue);
	void DeleteRecordNoLess(const string &SelectedCol, const string &SelectedValue);
	void DeleteRecordNoGreat(const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedShowLess(const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedShowGreat(const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedShowNoEqual(const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedShowNoLess(const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedShowNoGreat(const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedShowLess(const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedShowGreat(const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedNoEqual(const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedShowNoLess(const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedShowNoGreat(const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedToFileLess(const string &SFileName, const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedToFileGreat(const string &SFileName, const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedToFileNoEqual(const string &SFileName, const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedToFileNoLess(const string &SFileName, const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedToFileNoGreat(const string &SFileName, const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedToFileLess(const string &SFileName, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedToFileGreat(const string &SFileName, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedToFileNoEqual(const string &SFileName, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedToFileNoLess(const string &SFileName, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedToFileNoGreat(const string &SFileName, const string &SelectedCol, const string &SelectedValue);
	void ShowMax(const vector<string> &AttNameVec) const;
	void ShowMin(const vector<string> &AttNameVec) const;
	void ShowAvg(const vector<string> &AttNameVec) const;
private:
	int __ColNum; // 列数
	vector<string> ColNameVec; // 储存每列的名称
	vector<int> MaxLenVec; // 储存每一列的最宽值
	int __RecordNum; // 记录数量
	vector< vector<string> > RecordVec; // 储存记录
	string __TableName; // 表名
	string __FileName; // 文件名
	File f; // 用于文件IO
};