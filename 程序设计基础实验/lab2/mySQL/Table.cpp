#include"Table.h"
#include"Catalog.h"
#include<string>
#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<iomanip>
#include<algorithm>
using namespace std;
typedef unsigned int uint;
const string SelectPrefix = "./Select/";

extern Catalog SQLINFO;

Table::Table()
{
	__TableName = __FileName = "";
	__ColNum = __RecordNum = 0;
}

Table::~Table() {}

bool isDIGIT(const string &s)
{
	for (int i = 0; i < s.size(); i++)
	{
		if ((s.at(i) > '9') || (s.at(i) < '0'))
		{
			return 0;
		}
	}
	return 1;
}

bool isReal(const string &s)
{
	int Len = s.length();
	int count = 0;
	if (s[0] == '.' || s[Len - 1] == '.')
		return false;
	for (int i = 1; i < Len - 1; i++)
	{
		if (s[i] == '.')
			count++;
		if ((s.at(i) > '9') || (s.at(i) < '0'))
			return 0;
	}
	if (count == 1)
		return 1;
	else
		return 0;
}

bool isLess(const string &s1, const string &s2)
{
	if (isDIGIT(s1))
	{
		if (s1.length() < s2.length())
			return 1;
		else if (s1.length() == s2.length())
		{
			for (int i = 0; i < s1.length(); i++)
			{
				if (s1[i] < s2[i])
					return 1;
				if (s1[i] > s2[i])
					return 0;
			}
			return 0;
		}
		return 0;
	}
	else if (isReal(s1))
	{
		stringstream ss1, ss2;
		ss1 << s1;
		ss2 << s2;
		double r1, r2;
		ss1 >> r1;
		ss2 >> r2;
		return r1 < r2;
	}
	else
		return s1 < s2;
}

bool Table::isGreat(const vector<int> &AttIndexVec, vector<string> &Record1, vector<string> &Record2) const
{
	for (uint i = 0; i < AttIndexVec.size(); i++)
		if (isLess(Record1[AttIndexVec[i]], Record2[AttIndexVec[i]]) || (Record1[AttIndexVec[i]] == Record2[AttIndexVec[i]]) )
			return false;
	return true;
}

void Table::RegetMaxLen()
{
	MaxLenVec.clear();
	for (int i = 0; i < __ColNum; i++)
		MaxLenVec.push_back(ColNameVec[i].length());
	for (int i = 0; i < __RecordNum; i++)
		for (int j = 0; j < __ColNum; j++)
			if (RecordVec[i][j].length() > MaxLenVec[j])
				MaxLenVec[j] = RecordVec[i][j].length();
}

int Table::FindColIndex(const string &ColName) const
{
	for (int i = 0; i < __ColNum; i++)
		if (ColNameVec[i] == ColName)
			return i;
	return -1;
}

void Table::GetTableName(const string &TableName)
{
	__TableName = TableName;
}

void Table::OpenFile(const string &FileName)
{
	__FileName = FileName;
	f.GetTable(FileName, ColNameVec, RecordVec, MaxLenVec);
	__ColNum = ColNameVec.size();
	__RecordNum = RecordVec.size();
}

void Table::CreateFile(const string &FileName, const vector<string> &AttNameVec)
{
	__FileName = FileName;
	ColNameVec = AttNameVec;
	__ColNum = ColNameVec.size();
	RegetMaxLen();
}

void Table::CloseFile()
{
	f.SetTable(__FileName, ColNameVec, RecordVec);
}

void Table::ShowAttribute() const
{
	cout << __TableName << ": (" << __ColNum << ", " << __RecordNum << ") ";
	cout << '[';
	for (uint i = 0; i < ColNameVec.size() - 1; i++)
		cout << ColNameVec[i] << ',';
	cout << (*ColNameVec.rbegin()) << "]\n";
}

void Table::ShowTable() const
{
	// 设置左对齐
	cout << left;
	// 计算表头长度
	uint sum = 5;
	for (uint i = 0; i < MaxLenVec.size(); i++)
		sum += MaxLenVec[i] + 5;
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
	// 打印表头
	cout << setw(5) << "ID";
	for (uint i = 0; i < ColNameVec.size(); i++)
		cout << setw(MaxLenVec[i] + 5) << ColNameVec[i];
	cout << endl;
	// 打印表
	for (uint i = 0; i < RecordVec.size(); i++)
	{
		cout << setw(5) << i + 1;
		for (uint j = 0; j < RecordVec[0].size(); j++)
			cout << setw(MaxLenVec[j] + 5) << RecordVec[i][j];
		cout << endl;
	}
	// 打印表尾
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
}

bool Table::isEqual(const vector<string> &Record1, const vector<string> &Record2) const
{
	for (uint i = 0; i < Record1.size(); i++)
		if (Record1[i] != Record2[i])
			return false;
	return true;
}

bool Table::isEqual(const vector<string> &AttNameVec, const vector<string> &Record1, const vector<string> &Record2) const
{
	vector<int> AttIndexVec;
	for (int i = 0; i < AttNameVec.size(); i++)
		AttIndexVec.push_back(FindColIndex(AttNameVec[i]));
	for (uint i = 0; i < AttIndexVec.size(); i++)
		if (Record1[AttIndexVec[i]] != Record2[AttIndexVec[i]])
			return false;
	return true;
}

void Table::UniRecordCopy(vector< vector<string> > &RecordCopy) const
{
	for (uint i = 0; i < RecordCopy.size(); i++)
	{
		for (uint j = i + 1; j < RecordCopy.size(); )
		{
			if (isEqual(RecordCopy[i], RecordCopy[j]))
				RecordCopy.erase(RecordCopy.begin() + j);
			else
				j++;
		}
	}
}

void Table::UniRecordCopy(const vector<string> &AttNameVec, vector< vector<string> > &RecordCopy) const
{
	for (uint i = 0; i < RecordCopy.size(); i++)
	{
		for (uint j = i + 1; j < RecordCopy.size(); )
		{
			if (isEqual(AttNameVec, RecordCopy[i], RecordCopy[j]))
				RecordCopy.erase(RecordCopy.begin() + j);
			else
				j++;
		}
	}
}

void Table::InsertRecord(const vector<string> &Record)
{
	__RecordNum++;
	RecordVec.push_back(Record);
	RegetMaxLen();
}

void Table::InsertRecord(const vector<string> &AttNameVec, const vector<string> &ValueNameVec)
{
	__RecordNum++;
	vector<int> AttIndexVec;
	for (int i = 0; i < AttNameVec.size(); i++)
		AttIndexVec.push_back(FindColIndex(AttNameVec[i]));
	vector<string> record;
	record.resize(__ColNum);
	for (int i = 0; i < AttIndexVec.size(); i++)
		record[AttIndexVec[i]] = ValueNameVec[i];
	RecordVec.push_back(record);
	RegetMaxLen();
}

// 删除ColName = RecordValue的记录, 若不传参则删除全部
void Table::DeleteRecord(const string &ColName, const string &RecordValue)
{
	if (ColName == "")
	{
		RecordVec.clear();
		__RecordNum = 0;
	}
	else
	{
		int ColIndex = FindColIndex(ColName);
		for (int i = 0; i < __RecordNum; )
		{
			if (RecordVec[i][ColIndex] == RecordValue)
			{
				RecordVec.erase(RecordVec.begin() + i);
				__RecordNum--;
			}
			else
				i++;
		}
	}
	RegetMaxLen();
}

// 更新SelectedCol = SelectedValue的记录, 若不传参则更新所有行
void Table::UpdateRecord(const vector<string> &AttNameVec, const vector<string> &NewValueVec, const string &SelectedCol, const string &SelectedValue)
{
	if (SelectedCol == "")
	{
		for (int i = 0; i < AttNameVec.size(); i++)
		{
			int ColIndex = FindColIndex(AttNameVec[i]);
			for (int j = 0; j < __RecordNum; j++)
				RecordVec[j][ColIndex] = NewValueVec[i];
		}
	}
	else
	{
		int SelectedColIndex = FindColIndex(SelectedCol);
		for (int i = 0; i < AttNameVec.size(); i++)
		{
			int ColIndex = FindColIndex(AttNameVec[i]);
			for (int j = 0; j < __RecordNum; j++)
				if (RecordVec[j][SelectedColIndex] == SelectedValue)
					RecordVec[j][ColIndex] = NewValueVec[i];
		}
	}
	RegetMaxLen();
}

// 去重展示所有列, 条件可选
void Table::DisSelectedShow(const string &SelectedCol, const string &SelectedValue) const
{
	vector< vector<string> > RecordCopy(RecordVec);
	// 去重
	UniRecordCopy(RecordCopy);
	if (SelectedCol == "")
	{
		// 设置左对齐
		cout << left;
		// 计算表头长度
		uint sum = 5;
		for (uint i = 0; i < MaxLenVec.size(); i++)
			sum += MaxLenVec[i] + 5;
		for (uint i = 0; i < sum; i++)
			cout << "=";
		cout << endl;
		// 打印表头
		cout << setw(5) << "ID";
		for (uint i = 0; i < ColNameVec.size(); i++)
			cout << setw(MaxLenVec[i] + 5) << ColNameVec[i];
		cout << endl;
		// 打印表
		for (uint i = 0; i < RecordCopy.size(); i++)
		{
			cout << setw(5) << i + 1;
			for (uint j = 0; j < RecordCopy[0].size(); j++)
				cout << setw(MaxLenVec[j] + 5) << RecordCopy[i][j];
			cout << endl;
		}
		// 打印表尾
		for (uint i = 0; i < sum; i++)
			cout << "=";
		cout << endl;
	}
	else
	{
		// 找到特定列下标
		int SelectedColIndex = FindColIndex(SelectedCol);
		// 设置左对齐
		cout << left;
		// 计算表头长度
		uint sum = 5;
		for (uint i = 0; i < MaxLenVec.size(); i++)
			sum += MaxLenVec[i] + 5;
		for (uint i = 0; i < sum; i++)
			cout << "=";
		cout << endl;
		// 打印表头
		cout << setw(5) << "ID";
		for (uint i = 0; i < ColNameVec.size(); i++)
			cout << setw(MaxLenVec[i] + 5) << ColNameVec[i];
		cout << endl;
		// 打印表
		int id = 1;
		for (uint i = 0; i < RecordCopy.size(); i++)
			if (RecordCopy[i][SelectedColIndex] == SelectedValue)
			{
				cout << setw(5) << id++;
				for (uint j = 0; j < RecordCopy[0].size(); j++)
					cout << setw(MaxLenVec[j] + 5) << RecordCopy[i][j];
				cout << endl;
			}
		// 打印表尾
		for (uint i = 0; i < sum; i++)
			cout << "=";
		cout << endl;
	}
}

// 不去重展示所有列, 条件可选
void Table::NoDisSelectedShow(const string &SelectedCol, const string &SelectedValue) const
{
	if (SelectedCol == "")
		ShowTable();
	else
	{
		// 找到特定列下标
		int SelectedColIndex = FindColIndex(SelectedCol);
		// 设置左对齐
		cout << left;
		// 计算表头长度
		uint sum = 5;
		for (uint i = 0; i < MaxLenVec.size(); i++)
			sum += MaxLenVec[i] + 5;
		for (uint i = 0; i < sum; i++)
			cout << "=";
		cout << endl;
		// 打印表头
		cout << setw(5) << "ID";
		for (uint i = 0; i < ColNameVec.size(); i++)
			cout << setw(MaxLenVec[i] + 5) << ColNameVec[i];
		cout << endl;
		// 打印表
		int id = 1;
		for (uint i = 0; i < RecordVec.size(); i++)
			if (RecordVec[i][SelectedColIndex] == SelectedValue)
			{
				cout << setw(5) << id++;
				for (uint j = 0; j < RecordVec[0].size(); j++)
					cout << setw(MaxLenVec[j] + 5) << RecordVec[i][j];
				cout << endl;
			}
		// 打印表尾
		for (uint i = 0; i < sum; i++)
			cout << "=";
		cout << endl;
	}
}

// 去重展示指定列, 条件可选
void Table::DisSelectedShow(const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue) const
{
	vector< vector<string> > RecordCopy(RecordVec);
	// 去重
	UniRecordCopy(AttNameVec, RecordCopy);
	vector<int> AttIndexVec;
	for (int i = 0; i < AttNameVec.size(); i++)
		AttIndexVec.push_back(FindColIndex(AttNameVec[i]));
	cout << left;
	// 计算表头长度
	uint sum = 5;
	for (uint i = 0; i < AttNameVec.size(); i++)
		sum += MaxLenVec[AttIndexVec[i]] + 5;
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
	// 打印表头
	cout << setw(5) << "ID";
	for (uint i = 0; i < AttNameVec.size(); i++)
		cout << setw(MaxLenVec[AttIndexVec[i]] + 5) << AttNameVec[i];
	cout << endl;
	if (SelectedCol == "")
	{
		// 无条件打印表
		for (uint i = 0; i < RecordCopy.size(); i++)
		{
			cout << setw(5) << i + 1;
			for (uint j = 0; j < AttNameVec.size(); j++)
				cout << setw(MaxLenVec[AttIndexVec[j]] + 5) << RecordCopy[i][AttIndexVec[j]];
			cout << endl;
		}
	}
	else
	{
		int SelectedColIndex = FindColIndex(SelectedCol);
		// 有条件打印表
		int id = 1;
		for (uint i = 0; i < RecordCopy.size(); i++)
			if (RecordCopy[i][SelectedColIndex] == SelectedValue)
			{
				cout << setw(5) << id++;
				for (uint j = 0; j < AttNameVec.size(); j++)
					cout << setw(MaxLenVec[AttIndexVec[j]] + 5) << RecordCopy[i][AttIndexVec[j]];
				cout << endl;
			}
	}
	// 打印表尾
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
}

// 不去重展示指定列, 条件可选
void Table::NoDisSelectedShow(const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue) const
{
	vector<int> AttIndexVec;
	for (int i = 0; i < AttNameVec.size(); i++)
		AttIndexVec.push_back(FindColIndex(AttNameVec[i]));
	cout << left;
	// 计算表头长度
	uint sum = 5;
	for (uint i = 0; i < AttNameVec.size(); i++)
		sum += MaxLenVec[AttIndexVec[i]] + 5;
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
	// 打印表头
	cout << setw(5) << "ID";
	for (uint i = 0; i < AttNameVec.size(); i++)
		cout << setw(MaxLenVec[AttIndexVec[i]] + 5) << AttNameVec[i];
	cout << endl;
	if (SelectedCol == "")
	{
		// 无条件打印表
		for (uint i = 0; i < RecordVec.size(); i++)
		{
			cout << setw(5) << i + 1;
			for (uint j = 0; j < AttNameVec.size(); j++)
				cout << setw(MaxLenVec[AttIndexVec[j]] + 5) << RecordVec[i][AttIndexVec[j]];
			cout << endl;
		}
	}
	else
	{
		int SelectedColIndex = FindColIndex(SelectedCol);
		// 有条件打印表
		int id = 1;
		for (uint i = 0; i < RecordVec.size(); i++)
			if (RecordVec[i][SelectedColIndex] == SelectedValue)
			{
				cout << setw(5) << id++;
				for (uint j = 0; j < AttNameVec.size(); j++)
					cout << setw(MaxLenVec[AttIndexVec[j]] + 5) << RecordVec[i][AttIndexVec[j]];
				cout << endl;
			}
	}
	// 打印表尾
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
}

// 以指定列排序, 可选升序降序
void Table::SelectedSort(const vector<string> &AttNameVec, const string &mode) const
{
	vector< vector<string> > RecordCopy(RecordVec);
	vector<int> AttIndexVec;
	for (int i = 0; i < AttNameVec.size(); i++)
		AttIndexVec.push_back(FindColIndex(AttNameVec[i]));
	if (mode == "asc") // 升序
	{
		for (uint i = 0; i < RecordCopy.size(); i++)
		{
			for (uint j = i + 1; j < RecordCopy.size(); j++)
			{
				if (isGreat(AttIndexVec, RecordCopy[i], RecordCopy[j]))
				{
					vector<string> temp = RecordCopy[i];
					RecordCopy[i] = RecordCopy[j];
					RecordCopy[j] = temp;
				}
			}
		}
	}
	else // 降序
	{
		for (uint i = 0; i < RecordCopy.size(); i++)
		{
			for (uint j = i + 1; j < RecordCopy.size(); j++)
			{
				if (!isGreat(AttIndexVec, RecordCopy[i], RecordCopy[j]))
				{
					vector<string> temp = RecordCopy[i];
					RecordCopy[i] = RecordCopy[j];
					RecordCopy[j] = temp;
				}
			}
		}
	}
	// 设置左对齐
	cout << left;
	// 计算表头长度
	uint sum = 5;
	for (uint i = 0; i < MaxLenVec.size(); i++)
		sum += MaxLenVec[i] + 5;
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
	// 打印表头
	cout << setw(5) << "ID";
	for (uint i = 0; i < ColNameVec.size(); i++)
		cout << setw(MaxLenVec[i] + 5) << ColNameVec[i];
	cout << endl;
	for (uint i = 0; i < RecordCopy.size(); i++)
	{
		cout << setw(5) << i + 1;
		for (uint j = 0; j < MaxLenVec.size(); j++)
			cout << setw(MaxLenVec[j] + 5) << RecordCopy[i][j];
		cout << endl;
	}
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
}

// 去重保存所有列, 条件可选
void Table::DisSelectedToFile(const string &FileName, const string &SelectedCol, const string &SelectedValue) const
{
	vector< vector<string> > RecordCopy(RecordVec);
	// 去重
	UniRecordCopy(RecordCopy);
	ofstream fout;
	fout.open(SelectPrefix + FileName, ios::trunc);
	for (uint i = 0; i < ColNameVec.size() - 1; i++)
		fout << ColNameVec[i] << ' ';
	fout << ColNameVec[ColNameVec.size() - 1] << endl;
	if (SelectedCol == "")
	{
		for (uint i = 0; i < RecordCopy.size(); i++)
		{
			for (uint j = 0; j < RecordCopy[0].size() - 1; j++)
				fout << RecordCopy[i][j] << ' ';
			fout << RecordCopy[i][RecordCopy[0].size() - 1] << endl;
		}
	}
	else
	{
		int SelectedColIndex = FindColIndex(SelectedCol);
		for (uint i = 0; i < RecordCopy.size(); i++)
			if (RecordCopy[i][SelectedColIndex] == SelectedValue)
			{
				for (uint j = 0; j < RecordCopy[0].size() - 1; j++)
					fout << RecordCopy[i][j] << ' ';
				fout << RecordCopy[i][RecordCopy[0].size() - 1] << endl;
			}
	}
	fout.close();
}

// 不去重保存所有列, 条件可选
void Table::NoDisSelectedToFile(const string &FileName, const string &SelectedCol, const string &SelectedValue) const
{
	ofstream fout;
	fout.open(SelectPrefix + FileName, ios::trunc);
	for (uint i = 0; i < ColNameVec.size() - 1; i++)
		fout << ColNameVec[i] << ' ';
	fout << ColNameVec[ColNameVec.size() - 1] << endl;
	if (SelectedCol == "")
	{
		for (uint i = 0; i < RecordVec.size(); i++)
		{
			for (uint j = 0; j < RecordVec[0].size() - 1; j++)
				fout << RecordVec[i][j] << ' ';
			fout << RecordVec[i][RecordVec[0].size() - 1] << endl;
		}
	}
	else
	{
		int SelectedColIndex = FindColIndex(SelectedCol);
		for (uint i = 0; i < RecordVec.size(); i++)
			if (RecordVec[i][SelectedColIndex] == SelectedValue)
			{
				for (uint j = 0; j < RecordVec[0].size() - 1; j++)
					fout << RecordVec[i][j] << ' ';
				fout << RecordVec[i][RecordVec[0].size() - 1] << endl;
			}
	}
	fout.close();
}

// 去重保存指定列, 条件可选
void Table::DisSelectedToFile(const string &FileName, const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue) const
{
	vector< vector<string> > RecordCopy(RecordVec);
	// 去重
	UniRecordCopy(AttNameVec, RecordCopy);
	ofstream fout;
	fout.open(SelectPrefix + FileName, ios::trunc);
	vector<int> AttIndexVec;
	for (int i = 0; i < AttNameVec.size(); i++)
		AttIndexVec.push_back(FindColIndex(AttNameVec[i]));
	for (uint i = 0; i < AttNameVec.size() - 1; i++)
		fout << AttNameVec[i] << ' ';
	fout << AttNameVec[AttNameVec.size() - 1] << endl;
	if (SelectedCol == "")
	{
		for (uint i = 0; i < RecordCopy.size(); i++)
		{
			for (uint j = 0; j < AttNameVec.size() - 1; j++)
				fout << RecordCopy[i][AttIndexVec[j]] << ' ';
			fout << RecordCopy[i][AttIndexVec.size() - 1] << endl;
		}
	}
	else
	{
		int SelectedColIndex = FindColIndex(SelectedCol);
		for (uint i = 0; i < RecordCopy.size(); i++)
			if (RecordCopy[i][SelectedColIndex] == SelectedValue)
			{
				for (uint j = 0; j < AttNameVec.size() - 1; j++)
					fout << RecordCopy[i][AttIndexVec[j]] << ' ';
				fout << RecordCopy[i][AttIndexVec.size() - 1] << endl;
			}
	}
	fout.close();
}

// 不去重保存指定列, 条件可选
void Table::NoDisSelectedToFile(const string &FileName, const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue) const
{
	ofstream fout;
	fout.open(SelectPrefix + FileName, ios::trunc);
	vector<int> AttIndexVec;
	for (int i = 0; i < AttNameVec.size(); i++)
		AttIndexVec.push_back(FindColIndex(AttNameVec[i]));
	for (uint i = 0; i < AttNameVec.size() - 1; i++)
		fout << AttNameVec[i] << ' ';
	fout << AttNameVec[AttNameVec.size() - 1] << endl;
	if (SelectedCol == "")
	{
		for (uint i = 0; i < RecordVec.size(); i++)
		{
			for (uint j = 0; j < AttNameVec.size() - 1; j++)
				fout << RecordVec[i][AttIndexVec[j]] << ' ';
			fout << RecordVec[i][AttIndexVec.size() - 1] << endl;
		}
	}
	else
	{
		int SelectedColIndex = FindColIndex(SelectedCol);
		for (uint i = 0; i < RecordVec.size(); i++)
			if (RecordVec[i][SelectedColIndex] == SelectedValue)
			{
				for (uint j = 0; j < AttNameVec.size() - 1; j++)
					fout << RecordVec[i][AttIndexVec[j]] << ' ';
				fout << RecordVec[i][AttIndexVec.size() - 1] << endl;
			}
	}
	fout.close();
}

// here comes a shit mountain
void Table::UpdateRecordLess(const vector<string> &AttNameVec, const vector<string> &ValueNameVec, const string &SelectedCol, const string &SelectedValue)
{
	int SelectedColIndex = FindColIndex(SelectedCol);
	for (int i = 0; i < AttNameVec.size(); i++)
	{
		int ColIndex = FindColIndex(AttNameVec[i]);
		for (int j = 0; j < __RecordNum; j++)
			if (isLess(RecordVec[j][SelectedColIndex], SelectedValue))
				RecordVec[j][ColIndex] = ValueNameVec[i];
	}
	RegetMaxLen();
}

void Table::UpdateRecordGreat(const vector<string> &AttNameVec, const vector<string> &ValueNameVec, const string &SelectedCol, const string &SelectedValue)
{
	int SelectedColIndex = FindColIndex(SelectedCol);
	for (int i = 0; i < AttNameVec.size(); i++)
	{
		int ColIndex = FindColIndex(AttNameVec[i]);
		for (int j = 0; j < __RecordNum; j++)
			if (!isLess(RecordVec[j][SelectedColIndex], SelectedValue) && (RecordVec[j][SelectedColIndex] != SelectedValue))
				RecordVec[j][ColIndex] = ValueNameVec[i];
	}
	RegetMaxLen();
}

void Table::UpdateRecordNoEqual(const vector<string> &AttNameVec, const vector<string> &ValueNameVec, const string &SelectedCol, const string &SelectedValue)
{
	int SelectedColIndex = FindColIndex(SelectedCol);
	for (int i = 0; i < AttNameVec.size(); i++)
	{
		int ColIndex = FindColIndex(AttNameVec[i]);
		for (int j = 0; j < __RecordNum; j++)
			if (RecordVec[j][SelectedColIndex] != SelectedValue)
				RecordVec[j][ColIndex] = ValueNameVec[i];
	}
	RegetMaxLen();
}

void Table::UpdateRecordNoLess(const vector<string> &AttNameVec, const vector<string> &ValueNameVec, const string &SelectedCol, const string &SelectedValue)
{
	int SelectedColIndex = FindColIndex(SelectedCol);
	for (int i = 0; i < AttNameVec.size(); i++)
	{
		int ColIndex = FindColIndex(AttNameVec[i]);
		for (int j = 0; j < __RecordNum; j++)
			if (!isLess(RecordVec[j][SelectedColIndex], SelectedValue))
				RecordVec[j][ColIndex] = ValueNameVec[i];
	}
	RegetMaxLen();
}

void Table::UpdateRecordNoGreat(const vector<string> &AttNameVec, const vector<string> &ValueNameVec, const string &SelectedCol, const string &SelectedValue)
{
	int SelectedColIndex = FindColIndex(SelectedCol);
	for (int i = 0; i < AttNameVec.size(); i++)
	{
		int ColIndex = FindColIndex(AttNameVec[i]);
		for (int j = 0; j < __RecordNum; j++)
			if (isLess(RecordVec[j][SelectedColIndex], SelectedValue) || (RecordVec[j][SelectedColIndex] == SelectedValue))
				RecordVec[j][ColIndex] = ValueNameVec[i];
	}
	RegetMaxLen();
}

void Table::DeleteRecordLess(const string &SelectedCol, const string &SelectedValue)
{
	int SelectedColIndex = FindColIndex(SelectedCol);
	for (int i = 0; i < __RecordNum; )
	{
		if (isLess(RecordVec[i][SelectedColIndex], SelectedValue))
		{
			RecordVec.erase(RecordVec.begin() + i);
			__RecordNum--;
		}
		else
			i++;
	}
	RegetMaxLen();
}

void Table::DeleteRecordGreat(const string &SelectedCol, const string &SelectedValue)
{
	int SelectedColIndex = FindColIndex(SelectedCol);
	for (int i = 0; i < __RecordNum; )
	{
		if (!isLess(RecordVec[i][SelectedColIndex], SelectedValue) && (RecordVec[i][SelectedColIndex] != SelectedValue))
		{
			RecordVec.erase(RecordVec.begin() + i);
			__RecordNum--;
		}
		else
			i++;
	}
	RegetMaxLen();
}

void Table::DeleteRecordNoEqual(const string &SelectedCol, const string &SelectedValue)
{
	int SelectedColIndex = FindColIndex(SelectedCol);
	for (int i = 0; i < __RecordNum; )
	{
		if (RecordVec[i][SelectedColIndex] != SelectedValue)
		{
			RecordVec.erase(RecordVec.begin() + i);
			__RecordNum--;
		}
		else
			i++;
	}
	RegetMaxLen();
}

void Table::DeleteRecordNoLess(const string &SelectedCol, const string &SelectedValue)
{
	int SelectedColIndex = FindColIndex(SelectedCol);
	for (int i = 0; i < __RecordNum; )
	{
		if (!isLess(RecordVec[i][SelectedColIndex], SelectedValue))
		{
			RecordVec.erase(RecordVec.begin() + i);
			__RecordNum--;
		}
		else
			i++;
	}
	RegetMaxLen();
}

void Table::DeleteRecordNoGreat(const string &SelectedCol, const string &SelectedValue)
{
	int SelectedColIndex = FindColIndex(SelectedCol);
	for (int i = 0; i < __RecordNum; )
	{
		if (isLess(RecordVec[i][SelectedColIndex], SelectedValue) || (RecordVec[i][SelectedColIndex] == SelectedValue))
		{
			RecordVec.erase(RecordVec.begin() + i);
			__RecordNum--;
		}
		else
			i++;
	}
	RegetMaxLen();
}

void Table::NoDisSelectedShowLess(const string &SelectedCol, const string &SelectedValue)
{
	// 找到特定列下标
	int SelectedColIndex = FindColIndex(SelectedCol);
	// 设置左对齐
	cout << left;
	// 计算表头长度
	uint sum = 5;
	for (uint i = 0; i < MaxLenVec.size(); i++)
		sum += MaxLenVec[i] + 5;
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
	// 打印表头
	cout << setw(5) << "ID";
	for (uint i = 0; i < ColNameVec.size(); i++)
		cout << setw(MaxLenVec[i] + 5) << ColNameVec[i];
	cout << endl;
	// 打印表
	int id = 1;
	for (uint i = 0; i < RecordVec.size(); i++)
		if (isLess(RecordVec[i][SelectedColIndex], SelectedValue))
		{
			cout << setw(5) << id++;
			for (uint j = 0; j < RecordVec[0].size(); j++)
				cout << setw(MaxLenVec[j] + 5) << RecordVec[i][j];
			cout << endl;
		}
	// 打印表尾
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
}

void Table::NoDisSelectedShowGreat(const string &SelectedCol, const string &SelectedValue)
{
	// 找到特定列下标
	int SelectedColIndex = FindColIndex(SelectedCol);
	// 设置左对齐
	cout << left;
	// 计算表头长度
	uint sum = 5;
	for (uint i = 0; i < MaxLenVec.size(); i++)
		sum += MaxLenVec[i] + 5;
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
	// 打印表头
	cout << setw(5) << "ID";
	for (uint i = 0; i < ColNameVec.size(); i++)
		cout << setw(MaxLenVec[i] + 5) << ColNameVec[i];
	cout << endl;
	// 打印表
	int id = 1;
	for (uint i = 0; i < RecordVec.size(); i++)
		if (!isLess(RecordVec[i][SelectedColIndex], SelectedValue) && (RecordVec[i][SelectedColIndex] != SelectedValue))
		{
			cout << setw(5) << id++;
			for (uint j = 0; j < RecordVec[0].size(); j++)
				cout << setw(MaxLenVec[j] + 5) << RecordVec[i][j];
			cout << endl;
		}
	// 打印表尾
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
}

void Table::NoDisSelectedShowNoEqual(const string &SelectedCol, const string &SelectedValue)
{
	// 找到特定列下标
	int SelectedColIndex = FindColIndex(SelectedCol);
	// 设置左对齐
	cout << left;
	// 计算表头长度
	uint sum = 5;
	for (uint i = 0; i < MaxLenVec.size(); i++)
		sum += MaxLenVec[i] + 5;
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
	// 打印表头
	cout << setw(5) << "ID";
	for (uint i = 0; i < ColNameVec.size(); i++)
		cout << setw(MaxLenVec[i] + 5) << ColNameVec[i];
	cout << endl;
	// 打印表
	int id = 1;
	for (uint i = 0; i < RecordVec.size(); i++)
		if (RecordVec[i][SelectedColIndex] != SelectedValue)
		{
			cout << setw(5) << id++;
			for (uint j = 0; j < RecordVec[0].size(); j++)
				cout << setw(MaxLenVec[j] + 5) << RecordVec[i][j];
			cout << endl;
		}
	// 打印表尾
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
}

void Table::NoDisSelectedShowNoLess(const string &SelectedCol, const string &SelectedValue)
{
	// 找到特定列下标
	int SelectedColIndex = FindColIndex(SelectedCol);
	// 设置左对齐
	cout << left;
	// 计算表头长度
	uint sum = 5;
	for (uint i = 0; i < MaxLenVec.size(); i++)
		sum += MaxLenVec[i] + 5;
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
	// 打印表头
	cout << setw(5) << "ID";
	for (uint i = 0; i < ColNameVec.size(); i++)
		cout << setw(MaxLenVec[i] + 5) << ColNameVec[i];
	cout << endl;
	// 打印表
	int id = 1;
	for (uint i = 0; i < RecordVec.size(); i++)
		if (!isLess(RecordVec[i][SelectedColIndex], SelectedValue))
		{
			cout << setw(5) << id++;
			for (uint j = 0; j < RecordVec[0].size(); j++)
				cout << setw(MaxLenVec[j] + 5) << RecordVec[i][j];
			cout << endl;
		}
	// 打印表尾
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
}

void Table::NoDisSelectedShowNoGreat(const string &SelectedCol, const string &SelectedValue)
{
	// 找到特定列下标
	int SelectedColIndex = FindColIndex(SelectedCol);
	// 设置左对齐
	cout << left;
	// 计算表头长度
	uint sum = 5;
	for (uint i = 0; i < MaxLenVec.size(); i++)
		sum += MaxLenVec[i] + 5;
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
	// 打印表头
	cout << setw(5) << "ID";
	for (uint i = 0; i < ColNameVec.size(); i++)
		cout << setw(MaxLenVec[i] + 5) << ColNameVec[i];
	cout << endl;
	// 打印表
	int id = 1;
	for (uint i = 0; i < RecordVec.size(); i++)
		if (isLess(RecordVec[i][SelectedColIndex], SelectedValue) || (RecordVec[i][SelectedColIndex] == SelectedValue))
		{
			cout << setw(5) << id++;
			for (uint j = 0; j < RecordVec[0].size(); j++)
				cout << setw(MaxLenVec[j] + 5) << RecordVec[i][j];
			cout << endl;
		}
	// 打印表尾
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
}

void Table::NoDisSelectedShowLess(const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue)
{
	vector<int> AttIndexVec;
	for (int i = 0; i < AttNameVec.size(); i++)
		AttIndexVec.push_back(FindColIndex(AttNameVec[i]));
	cout << left;
	// 计算表头长度
	uint sum = 5;
	for (uint i = 0; i < AttNameVec.size(); i++)
		sum += MaxLenVec[AttIndexVec[i]] + 5;
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
	// 打印表头
	cout << setw(5) << "ID";
	for (uint i = 0; i < AttNameVec.size(); i++)
		cout << setw(MaxLenVec[AttIndexVec[i]] + 5) << AttNameVec[i];
	cout << endl;
	int SelectedColIndex = FindColIndex(SelectedCol);
	// 有条件打印表
	int id = 1;
	for (uint i = 0; i < RecordVec.size(); i++)
		if (isLess(RecordVec[i][SelectedColIndex], SelectedValue))
		{
			cout << setw(5) << id++;
			for (uint j = 0; j < AttNameVec.size(); j++)
				cout << setw(MaxLenVec[AttIndexVec[j]] + 5) << RecordVec[i][AttIndexVec[j]];
			cout << endl;
		}
	// 打印表尾
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
}

void Table::NoDisSelectedShowGreat(const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue)
{
	vector<int> AttIndexVec;
	for (int i = 0; i < AttNameVec.size(); i++)
		AttIndexVec.push_back(FindColIndex(AttNameVec[i]));
	cout << left;
	// 计算表头长度
	uint sum = 5;
	for (uint i = 0; i < AttNameVec.size(); i++)
		sum += MaxLenVec[AttIndexVec[i]] + 5;
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
	// 打印表头
	cout << setw(5) << "ID";
	for (uint i = 0; i < AttNameVec.size(); i++)
		cout << setw(MaxLenVec[AttIndexVec[i]] + 5) << AttNameVec[i];
	cout << endl;
	int SelectedColIndex = FindColIndex(SelectedCol);
	// 有条件打印表
	int id = 1;
	for (uint i = 0; i < RecordVec.size(); i++)
		if (!isLess(RecordVec[i][SelectedColIndex], SelectedValue) && (RecordVec[i][SelectedColIndex] != SelectedValue))
		{
			cout << setw(5) << id++;
			for (uint j = 0; j < AttNameVec.size(); j++)
				cout << setw(MaxLenVec[AttIndexVec[j]] + 5) << RecordVec[i][AttIndexVec[j]];
			cout << endl;
		}
	// 打印表尾
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
}

void Table::NoDisSelectedNoEqual(const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue)
{
	vector<int> AttIndexVec;
	for (int i = 0; i < AttNameVec.size(); i++)
		AttIndexVec.push_back(FindColIndex(AttNameVec[i]));
	cout << left;
	// 计算表头长度
	uint sum = 5;
	for (uint i = 0; i < AttNameVec.size(); i++)
		sum += MaxLenVec[AttIndexVec[i]] + 5;
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
	// 打印表头
	cout << setw(5) << "ID";
	for (uint i = 0; i < AttNameVec.size(); i++)
		cout << setw(MaxLenVec[AttIndexVec[i]] + 5) << AttNameVec[i];
	cout << endl;
	int SelectedColIndex = FindColIndex(SelectedCol);
	// 有条件打印表
	int id = 1;
	for (uint i = 0; i < RecordVec.size(); i++)
		if (RecordVec[i][SelectedColIndex] != SelectedValue)
		{
			cout << setw(5) << id++;
			for (uint j = 0; j < AttNameVec.size(); j++)
				cout << setw(MaxLenVec[AttIndexVec[j]] + 5) << RecordVec[i][AttIndexVec[j]];
			cout << endl;
		}
	// 打印表尾
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
}

void Table::NoDisSelectedShowNoLess(const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue)
{
	vector<int> AttIndexVec;
	for (int i = 0; i < AttNameVec.size(); i++)
		AttIndexVec.push_back(FindColIndex(AttNameVec[i]));
	cout << left;
	// 计算表头长度
	uint sum = 5;
	for (uint i = 0; i < AttNameVec.size(); i++)
		sum += MaxLenVec[AttIndexVec[i]] + 5;
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
	// 打印表头
	cout << setw(5) << "ID";
	for (uint i = 0; i < AttNameVec.size(); i++)
		cout << setw(MaxLenVec[AttIndexVec[i]] + 5) << AttNameVec[i];
	cout << endl;
	int SelectedColIndex = FindColIndex(SelectedCol);
	// 有条件打印表
	int id = 1;
	for (uint i = 0; i < RecordVec.size(); i++)
		if (!isLess(RecordVec[i][SelectedColIndex], SelectedValue))
		{
			cout << setw(5) << id++;
			for (uint j = 0; j < AttNameVec.size(); j++)
				cout << setw(MaxLenVec[AttIndexVec[j]] + 5) << RecordVec[i][AttIndexVec[j]];
			cout << endl;
		}
	// 打印表尾
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
}

void Table::NoDisSelectedShowNoGreat(const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue)
{
	vector<int> AttIndexVec;
	for (int i = 0; i < AttNameVec.size(); i++)
		AttIndexVec.push_back(FindColIndex(AttNameVec[i]));
	cout << left;
	// 计算表头长度
	uint sum = 5;
	for (uint i = 0; i < AttNameVec.size(); i++)
		sum += MaxLenVec[AttIndexVec[i]] + 5;
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
	// 打印表头
	cout << setw(5) << "ID";
	for (uint i = 0; i < AttNameVec.size(); i++)
		cout << setw(MaxLenVec[AttIndexVec[i]] + 5) << AttNameVec[i];
	cout << endl;
	int SelectedColIndex = FindColIndex(SelectedCol);
	// 有条件打印表
	int id = 1;
	for (uint i = 0; i < RecordVec.size(); i++)
		if (isLess(RecordVec[i][SelectedColIndex], SelectedValue) || (RecordVec[i][SelectedColIndex] == SelectedValue))
		{
			cout << setw(5) << id++;
			for (uint j = 0; j < AttNameVec.size(); j++)
				cout << setw(MaxLenVec[AttIndexVec[j]] + 5) << RecordVec[i][AttIndexVec[j]];
			cout << endl;
		}
	// 打印表尾
	for (uint i = 0; i < sum; i++)
		cout << "=";
	cout << endl;
}

void Table::NoDisSelectedToFileLess(const string &SFileName, const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue)
{
	ofstream fout;
	fout.open(SelectPrefix + SFileName, ios::trunc);
	vector<int> AttIndexVec;
	for (int i = 0; i < AttNameVec.size(); i++)
		AttIndexVec.push_back(FindColIndex(AttNameVec[i]));
	for (uint i = 0; i < AttNameVec.size() - 1; i++)
		fout << AttNameVec[i] << ' ';
	fout << AttNameVec[AttNameVec.size() - 1] << endl;
	int SelectedColIndex = FindColIndex(SelectedCol);
	for (uint i = 0; i < RecordVec.size(); i++)
		if (isLess(RecordVec[i][SelectedColIndex], SelectedValue))
		{
			for (uint j = 0; j < AttNameVec.size() - 1; j++)
				fout << RecordVec[i][AttIndexVec[j]] << ' ';
			fout << RecordVec[i][AttIndexVec.size() - 1] << endl;
		}
	fout.close();
}

void Table::NoDisSelectedToFileGreat(const string &SFileName, const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue)
{
	ofstream fout;
	fout.open(SelectPrefix + SFileName, ios::trunc);
	vector<int> AttIndexVec;
	for (int i = 0; i < AttNameVec.size(); i++)
		AttIndexVec.push_back(FindColIndex(AttNameVec[i]));
	for (uint i = 0; i < AttNameVec.size() - 1; i++)
		fout << AttNameVec[i] << ' ';
	fout << AttNameVec[AttNameVec.size() - 1] << endl;
	int SelectedColIndex = FindColIndex(SelectedCol);
	for (uint i = 0; i < RecordVec.size(); i++)
		if (!isLess(RecordVec[i][SelectedColIndex], SelectedValue) && (RecordVec[i][SelectedColIndex] != SelectedValue))
		{
			for (uint j = 0; j < AttNameVec.size() - 1; j++)
				fout << RecordVec[i][AttIndexVec[j]] << ' ';
			fout << RecordVec[i][AttIndexVec.size() - 1] << endl;
		}
	fout.close();
}

void Table::NoDisSelectedToFileNoEqual(const string &SFileName, const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue)
{
	ofstream fout;
	fout.open(SelectPrefix + SFileName, ios::trunc);
	vector<int> AttIndexVec;
	for (int i = 0; i < AttNameVec.size(); i++)
		AttIndexVec.push_back(FindColIndex(AttNameVec[i]));
	for (uint i = 0; i < AttNameVec.size() - 1; i++)
		fout << AttNameVec[i] << ' ';
	fout << AttNameVec[AttNameVec.size() - 1] << endl;
	int SelectedColIndex = FindColIndex(SelectedCol);
	for (uint i = 0; i < RecordVec.size(); i++)
		if (RecordVec[i][SelectedColIndex] != SelectedValue)
		{
			for (uint j = 0; j < AttNameVec.size() - 1; j++)
				fout << RecordVec[i][AttIndexVec[j]] << ' ';
			fout << RecordVec[i][AttIndexVec.size() - 1] << endl;
		}
	fout.close();
}

void Table::NoDisSelectedToFileNoLess(const string &SFileName, const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue)
{
	ofstream fout;
	fout.open(SelectPrefix + SFileName, ios::trunc);
	vector<int> AttIndexVec;
	for (int i = 0; i < AttNameVec.size(); i++)
		AttIndexVec.push_back(FindColIndex(AttNameVec[i]));
	for (uint i = 0; i < AttNameVec.size() - 1; i++)
		fout << AttNameVec[i] << ' ';
	fout << AttNameVec[AttNameVec.size() - 1] << endl;
	int SelectedColIndex = FindColIndex(SelectedCol);
	for (uint i = 0; i < RecordVec.size(); i++)
		if (!isLess(RecordVec[i][SelectedColIndex], SelectedValue))
		{
			for (uint j = 0; j < AttNameVec.size() - 1; j++)
				fout << RecordVec[i][AttIndexVec[j]] << ' ';
			fout << RecordVec[i][AttIndexVec.size() - 1] << endl;
		}
	fout.close();
}

void Table::NoDisSelectedToFileNoGreat(const string &SFileName, const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue)
{
	ofstream fout;
	fout.open(SelectPrefix + SFileName, ios::trunc);
	vector<int> AttIndexVec;
	for (int i = 0; i < AttNameVec.size(); i++)
		AttIndexVec.push_back(FindColIndex(AttNameVec[i]));
	for (uint i = 0; i < AttNameVec.size() - 1; i++)
		fout << AttNameVec[i] << ' ';
	fout << AttNameVec[AttNameVec.size() - 1] << endl;
	int SelectedColIndex = FindColIndex(SelectedCol);
	for (uint i = 0; i < RecordVec.size(); i++)
		if (isLess(RecordVec[i][SelectedColIndex], SelectedValue) || (RecordVec[i][SelectedColIndex] == SelectedValue))
		{
			for (uint j = 0; j < AttNameVec.size() - 1; j++)
				fout << RecordVec[i][AttIndexVec[j]] << ' ';
			fout << RecordVec[i][AttIndexVec.size() - 1] << endl;
		}
	fout.close();
}

void Table::NoDisSelectedToFileLess(const string &SFileName, const string &SelectedCol, const string &SelectedValue)
{
	ofstream fout;
	fout.open(SelectPrefix + SFileName, ios::trunc);
	for (uint i = 0; i < ColNameVec.size() - 1; i++)
		fout << ColNameVec[i] << ' ';
	fout << ColNameVec[ColNameVec.size() - 1] << endl;
	int SelectedColIndex = FindColIndex(SelectedCol);
	for (uint i = 0; i < RecordVec.size(); i++)
		if (isLess(RecordVec[i][SelectedColIndex], SelectedValue))
		{
			for (uint j = 0; j < RecordVec[0].size() - 1; j++)
				fout << RecordVec[i][j] << ' ';
			fout << RecordVec[i][RecordVec[0].size() - 1] << endl;
		}
	fout.close();
}

void Table::NoDisSelectedToFileGreat(const string &SFileName, const string &SelectedCol, const string &SelectedValue)
{
	ofstream fout;
	fout.open(SelectPrefix + SFileName, ios::trunc);
	for (uint i = 0; i < ColNameVec.size() - 1; i++)
		fout << ColNameVec[i] << ' ';
	fout << ColNameVec[ColNameVec.size() - 1] << endl;
	int SelectedColIndex = FindColIndex(SelectedCol);
	for (uint i = 0; i < RecordVec.size(); i++)
		if (!isLess(RecordVec[i][SelectedColIndex], SelectedValue) && (RecordVec[i][SelectedColIndex] != SelectedValue))
		{
			for (uint j = 0; j < RecordVec[0].size() - 1; j++)
				fout << RecordVec[i][j] << ' ';
			fout << RecordVec[i][RecordVec[0].size() - 1] << endl;
		}
	fout.close();
}

void Table::NoDisSelectedToFileNoEqual(const string &SFileName, const string &SelectedCol, const string &SelectedValue)
{
	ofstream fout;
	fout.open(SelectPrefix + SFileName, ios::trunc);
	for (uint i = 0; i < ColNameVec.size() - 1; i++)
		fout << ColNameVec[i] << ' ';
	fout << ColNameVec[ColNameVec.size() - 1] << endl;
	int SelectedColIndex = FindColIndex(SelectedCol);
	for (uint i = 0; i < RecordVec.size(); i++)
		if (RecordVec[i][SelectedColIndex] != SelectedValue)
		{
			for (uint j = 0; j < RecordVec[0].size() - 1; j++)
				fout << RecordVec[i][j] << ' ';
			fout << RecordVec[i][RecordVec[0].size() - 1] << endl;
		}
	fout.close();
}

void Table::NoDisSelectedToFileNoLess(const string &SFileName, const string &SelectedCol, const string &SelectedValue)
{
	ofstream fout;
	fout.open(SelectPrefix + SFileName, ios::trunc);
	for (uint i = 0; i < ColNameVec.size() - 1; i++)
		fout << ColNameVec[i] << ' ';
	fout << ColNameVec[ColNameVec.size() - 1] << endl;
	int SelectedColIndex = FindColIndex(SelectedCol);
	for (uint i = 0; i < RecordVec.size(); i++)
		if (!isLess(RecordVec[i][SelectedColIndex], SelectedValue))
		{
			for (uint j = 0; j < RecordVec[0].size() - 1; j++)
				fout << RecordVec[i][j] << ' ';
			fout << RecordVec[i][RecordVec[0].size() - 1] << endl;
		}
	fout.close();
}

void Table::NoDisSelectedToFileNoGreat(const string &SFileName, const string &SelectedCol, const string &SelectedValue)
{
	ofstream fout;
	fout.open(SelectPrefix + SFileName, ios::trunc);
	for (uint i = 0; i < ColNameVec.size() - 1; i++)
		fout << ColNameVec[i] << ' ';
	fout << ColNameVec[ColNameVec.size() - 1] << endl;
	int SelectedColIndex = FindColIndex(SelectedCol);
	for (uint i = 0; i < RecordVec.size(); i++)
		if (isLess(RecordVec[i][SelectedColIndex], SelectedValue) || (RecordVec[i][SelectedColIndex] == SelectedValue))
		{
			for (uint j = 0; j < RecordVec[0].size() - 1; j++)
				fout << RecordVec[i][j] << ' ';
			fout << RecordVec[i][RecordVec[0].size() - 1] << endl;
		}
	fout.close();
}

void Table::ShowMax(const vector<string> &AttNameVec) const
{
	vector<int> AttIndexVec;
	vector<string> MaxValues;
	for (int i = 0; i < AttNameVec.size(); i++)
		AttIndexVec.push_back(FindColIndex(AttNameVec[i]));
	for (uint i = 0; i < AttIndexVec.size(); i++)
		MaxValues.push_back(RecordVec[0][AttIndexVec[i]]);
	for (uint i = 1; i < RecordVec.size(); i++)
		for (uint j = 0; j < AttIndexVec.size(); j++)
			if (!isLess(RecordVec[i][AttIndexVec[j]], MaxValues[j]) && (RecordVec[i][AttIndexVec[j]] != MaxValues[j]))
				MaxValues[j] = RecordVec[i][AttIndexVec[j]];
	for (int i = 0; i < AttNameVec.size(); i++)
		cout << AttNameVec[i] << "(MAX): " << MaxValues[i] << endl;
}

void Table::ShowMin(const vector<string> &AttNameVec) const
{
	vector<int> AttIndexVec;
	vector<string> MinValues;
	for (int i = 0; i < AttNameVec.size(); i++)
		AttIndexVec.push_back(FindColIndex(AttNameVec[i]));
	for (uint i = 0; i < AttIndexVec.size(); i++)
		MinValues.push_back(RecordVec[0][AttIndexVec[i]]);
	for (uint i = 1; i < RecordVec.size(); i++)
		for (uint j = 0; j < AttIndexVec.size(); j++)
			if (isLess(RecordVec[i][AttIndexVec[j]], MinValues[j]))
				MinValues[j] = RecordVec[i][AttIndexVec[j]];
	for (int i = 0; i < AttNameVec.size(); i++)
		cout << AttNameVec[i] << "(MIN): " << MinValues[i] << endl;
}

void Table::ShowAvg(const vector<string> &AttNameVec) const
{
	vector<int> AttIndexVec;
	vector<double> SumValues(AttNameVec.size(), 0.0);
	for (int i = 0; i < AttNameVec.size(); i++)
		AttIndexVec.push_back(FindColIndex(AttNameVec[i]));
	for (uint i = 0; i < RecordVec.size(); i++)
		for (uint j = 0; j < AttIndexVec.size(); j++)
		{
			stringstream ss;
			ss << RecordVec[i][AttIndexVec[j]];
			double val;
			ss >> val;
			SumValues[j] += val;
		}
	for (int i = 0; i < AttNameVec.size(); i++)
		cout << AttNameVec[i] << "(AVG): " << SumValues[i] / RecordVec.size() << endl;
}