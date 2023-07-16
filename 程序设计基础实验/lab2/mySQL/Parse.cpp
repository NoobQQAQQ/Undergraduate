#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<cstring>
#include<vector>
#include<sstream>
#include<fstream>
#include"Table.h"
#include"Catalog.h"
using namespace std;

const string ToDoPrefix = "./Todo/";

extern Catalog SQLINFO;
void RunFile(const string&);

string tolow(const string s)
{
	string result(s);
	for (auto Ritr = result.begin(); Ritr != result.end(); Ritr++)
		*Ritr = tolower(*Ritr);
	return result;
}

void GetVec(const string &s, vector<string> &Vec)
{
	char *cs = NULL;
	string t(s, 1, s.length() - 2);
	if (s[0] == '(')
		cs = (char*) t.c_str();
	else
		cs = (char*) s.c_str();
	const char * split = ",";
	// 以逗号为分隔符拆分字符串
	char *p = strtok(cs, split);
	while (p != NULL)
	{
		Vec.push_back(p);
		p = strtok(NULL, split);
	}
}

int GetCondition(const string &s, string &SelectedCol, string &SelectedValue)
{
	int flag = -1;
	int i = 0;
	while (i < s.length())
	{
		if (s[i] == '=')
		{
			flag = 1;
			SelectedCol = s.substr(0, i);
			SelectedValue = s.substr(i + 1, 2000000);
			return flag;
		}
		else if (s[i] == '<')
		{
			if (s[i + 1] == '=')
			{
				flag = 6;
				SelectedCol = s.substr(0, i);
				SelectedValue = s.substr(i + 2, 2000000);
				return flag;
			}
			else
			{
				flag = 2;
				SelectedCol = s.substr(0, i);
				SelectedValue = s.substr(i + 1, 2000000);
				return flag;
			}
		}
		else if (s[i] == '>')
		{
			if (s[i + 1] == '=')
			{
				flag = 5;
				SelectedCol = s.substr(0, i);
				SelectedValue = s.substr(i + 2, 2000000);
				return flag;
			}
			else
			{
				flag = 3;
				SelectedCol = s.substr(0, i);
				SelectedValue = s.substr(i + 1, 2000000);
				return flag;
			}
		}
		else if (s[i] == '!')
		{
			flag = 4;
			SelectedCol = s.substr(0, i);
			SelectedValue = s.substr(i + 2, 2000000);
			return flag;
		}
		i++;
	}
	return 7;
}

void Get2Vecs(const string &s, vector<string> &AttNameVec, vector<string> &ValueNameVec)
{
	vector<string> Vec;
	char *cs = (char *)s.c_str();
	const char * split = ",";
	// 以逗号为分隔符拆分字符串
	char *p = strtok(cs, split);
	while (p != NULL)
	{
		Vec.push_back(p);
		p = strtok(NULL, split);
	}
	for (int i = 0; i < Vec.size(); i++)
	{
		string Att, Value;
		GetCondition(Vec[i], Att, Value);
		AttNameVec.push_back(Att);
		ValueNameVec.push_back(Value);
	}
}

void UpdateOptions(int flag, string &FileName, vector<string> &AttNameVec, vector<string> &ValueNameVec, string &SelectedCol, string &SelectedValue)
{
	Table a;
	switch (flag)
	{
	case 1: // =
		a.OpenFile(FileName);
		a.UpdateRecord(AttNameVec, ValueNameVec, SelectedCol, SelectedValue);
		a.ShowTable();
		a.CloseFile();
		break;
	case 2: // <
		a.OpenFile(FileName);
		a.UpdateRecordLess(AttNameVec, ValueNameVec, SelectedCol, SelectedValue);
		a.ShowTable();
		a.CloseFile();
		break;
	case 3: // >
		a.OpenFile(FileName);
		a.UpdateRecordGreat(AttNameVec, ValueNameVec, SelectedCol, SelectedValue);
		a.ShowTable();
		a.CloseFile();
		break;
	case 4: // !=
		a.OpenFile(FileName);
		a.UpdateRecordNoEqual(AttNameVec, ValueNameVec, SelectedCol, SelectedValue);
		a.ShowTable();
		a.CloseFile();
		break;
	case 5: // >=
		a.OpenFile(FileName);
		a.UpdateRecordNoLess(AttNameVec, ValueNameVec, SelectedCol, SelectedValue);
		a.ShowTable();
		a.CloseFile();
		break;
	case 6: // <=
		a.OpenFile(FileName);
		a.UpdateRecordNoGreat(AttNameVec, ValueNameVec, SelectedCol, SelectedValue);
		a.ShowTable();
		a.CloseFile();
		break;
	case 7:
		cout << "请检查条件项正确性" << endl;
		break;
	}
}

void DeleteOptions(int flag, string &FileName, string &SelectedCol, string &SelectedValue)
{
	Table a;
	switch (flag)
	{
	case 1:
		a.OpenFile(FileName);
		a.DeleteRecord(SelectedCol, SelectedValue);
		a.ShowTable();
		a.CloseFile();
		break;
	case 2:
		a.OpenFile(FileName);
		a.DeleteRecordLess(SelectedCol, SelectedValue);
		a.ShowTable();
		a.CloseFile();
		break;
	case 3:
		a.OpenFile(FileName);
		a.DeleteRecordGreat(SelectedCol, SelectedValue);
		a.ShowTable();
		a.CloseFile();
		break;
	case 4:
		a.OpenFile(FileName);
		a.DeleteRecordNoEqual(SelectedCol, SelectedValue);
		a.ShowTable();
		a.CloseFile();
		break;
	case 5:
		a.OpenFile(FileName);
		a.DeleteRecordNoLess(SelectedCol, SelectedValue);
		a.ShowTable();
		a.CloseFile();
		break;
	case 6:
		a.OpenFile(FileName);
		a.DeleteRecordNoGreat(SelectedCol, SelectedValue);
		a.ShowTable();
		a.CloseFile();
		break;
	case 7:
		cout << "请检查条件项正确性" << endl;
		break;
	}
}

void SelectOptions(int flag, string &FileName, string &SelectedCol, string &SelectedValue)
{
	Table a;
	switch (flag)
	{
	case 1:
		a.OpenFile(FileName);
		a.NoDisSelectedShow(SelectedCol, SelectedValue);
		a.CloseFile();
		break;
	case 2:
		a.OpenFile(FileName);
		a.NoDisSelectedShowLess(SelectedCol, SelectedValue);
		a.CloseFile();
		break;
	case 3:
		a.OpenFile(FileName);
		a.NoDisSelectedShowGreat(SelectedCol, SelectedValue);
		a.CloseFile();
		break;
	case 4:
		a.OpenFile(FileName);
		a.NoDisSelectedShowNoEqual(SelectedCol, SelectedValue);
		a.CloseFile();
		break;
	case 5:
		a.OpenFile(FileName);
		a.NoDisSelectedShowNoLess(SelectedCol, SelectedValue);
		a.CloseFile();
		break;
	case 6:
		a.OpenFile(FileName);
		a.NoDisSelectedShowNoGreat(SelectedCol, SelectedValue);
		a.CloseFile();
		break;
	case 7:
		cout << "请检查条件项正确性" << endl;
		break;
	}
}

void SelectOptions(int flag, string &FileName, vector<string> &AttNameVec, string &SelectedCol, string &SelectedValue)
{
	Table a;
	switch (flag)
	{
	case 1:
		a.OpenFile(FileName);
		a.NoDisSelectedShow(AttNameVec, SelectedCol, SelectedValue);
		a.CloseFile();
		break;
	case 2:
		a.OpenFile(FileName);
		a.NoDisSelectedShowLess(AttNameVec, SelectedCol, SelectedValue);
		a.CloseFile();
		break;
	case 3:
		a.OpenFile(FileName);
		a.NoDisSelectedShowGreat(AttNameVec, SelectedCol, SelectedValue);
		a.CloseFile();
		break;
	case 4:
		a.OpenFile(FileName);
		a.NoDisSelectedNoEqual(AttNameVec, SelectedCol, SelectedValue);
		a.CloseFile();
		break;
	case 5:
		a.OpenFile(FileName);
		a.NoDisSelectedShowNoLess(AttNameVec, SelectedCol, SelectedValue);
		a.CloseFile();
		break;
	case 6:
		a.OpenFile(FileName);
		a.NoDisSelectedShowNoGreat(AttNameVec, SelectedCol, SelectedValue);
		a.CloseFile();
		break;
	case 7:
		cout << "请检查条件项正确性" << endl;
		break;
	}
}

void SelectOptionsToFile(int flag, string &FileName, string &SFileName, vector<string> &AttNameVec, string &SelectedCol, string &SelectedValue)
{
	Table a;
	switch (flag)
	{
	case 1:
		a.OpenFile(FileName);
		a.NoDisSelectedToFile(SFileName, AttNameVec, SelectedCol, SelectedValue);
		a.CloseFile();
		break;
	case 2:
		a.OpenFile(FileName);
		a.NoDisSelectedToFileLess(SFileName, AttNameVec, SelectedCol, SelectedValue);
		a.CloseFile();
		break;
	case 3:
		a.OpenFile(FileName);
		a.NoDisSelectedToFileGreat(SFileName, AttNameVec, SelectedCol, SelectedValue);
		a.CloseFile();
		break;
	case 4:
		a.OpenFile(FileName);
		a.NoDisSelectedToFileNoEqual(SFileName, AttNameVec, SelectedCol, SelectedValue);
		a.CloseFile();
		break;
	case 5:
		a.OpenFile(FileName);
		a.NoDisSelectedToFileNoLess(SFileName, AttNameVec, SelectedCol, SelectedValue);
		a.CloseFile();
		break;
	case 6:
		a.OpenFile(FileName);
		a.NoDisSelectedToFileNoGreat(SFileName, AttNameVec, SelectedCol, SelectedValue);
		a.CloseFile();
		break;
	case 7:
		cout << "请检查条件项正确性" << endl;
		break;
	}
}

void SelectOptionsToFile(int flag, string &FileName, string &SFileName, string &SelectedCol, string &SelectedValue)
{
	Table a;
	switch (flag)
	{
	case 1:
		a.OpenFile(FileName);
		a.NoDisSelectedToFile(SFileName, SelectedCol, SelectedValue);
		a.CloseFile();
		break;
	case 2:
		a.OpenFile(FileName);
		a.NoDisSelectedToFileLess(SFileName, SelectedCol, SelectedValue);
		a.CloseFile();
		break;
	case 3:
		a.OpenFile(FileName);
		a.NoDisSelectedToFileGreat(SFileName, SelectedCol, SelectedValue);
		a.CloseFile();
		break;
	case 4:
		a.OpenFile(FileName);
		a.NoDisSelectedToFileNoEqual(SFileName, SelectedCol, SelectedValue);
		a.CloseFile();
		break;
	case 5:
		a.OpenFile(FileName);
		a.NoDisSelectedToFileNoLess(SFileName, SelectedCol, SelectedValue);
		a.CloseFile();
		break;
	case 6:
		a.OpenFile(FileName);
		a.NoDisSelectedToFileNoGreat(SFileName, SelectedCol, SelectedValue);
		a.CloseFile();
		break;
	case 7:
		cout << "请检查条件项正确性" << endl;
		break;
	}
}

void Parse(string &s)
{
	string TableName, FileName, AttName, ValueName, SelectedCol, SelectedValue;
	vector<string> AttNameVec, ValueNameVec;
	if (tolow(s) == "quit")
		return;
	stringstream ss;
	ss << s;
	string s1;
	ss >> s1;
	// create语句
	if (tolow(s1) == "create")
	{
		string s2;
		ss >> s2;
		if (tolow(s2) == "table")
		{
			string s3;
			ss >> s3;
			if (SQLINFO.CheckTableName(s3))
			{
				TableName = s3;
				string s4;
				ss >> s4;
				if (tolow(s4) != "from") // To语句
				{
					if (s4[0] != '(' || (*s4.rbegin()) != ')')
					{
						cout << "Please Check Attributes" << endl;
						return;
					}
					else
					{
						GetVec(s4, AttNameVec);
						string s5;
						ss >> s5;
						if (tolow(s5) != "to")
						{
							cout << "Please Check To" << endl;
							return;
						}
						else
						{
							string s6;
							ss >> s6;
							string tn = SQLINFO.Check(s6);
							if (tn == "")
							{
								FileName = s6;
							}
							else
							{
								cout << "have already bound to" << tn << "!" << endl;
								return;
							}
						}
					}
					SQLINFO.CreateTable(FileName, TableName);
					Table a;
					a.CreateFile(FileName, AttNameVec);
					a.ShowTable();
					a.CloseFile();
				}
				if (tolow(s4) == "from") // from语句
				{
					string s5;
					ss >> s5;
					string tn = SQLINFO.Check(s5);
					if (tn == "")
					{
						FileName = s5;
					}
					else
					{
						cout << "have already bound to" << tn << "!" << endl;
						return;
					}
					SQLINFO.CreateTable(FileName, TableName);
					Table a;
					a.OpenFile(FileName);
					a.ShowTable();
					a.CloseFile();
				}
			}
			else
			{
				cout << "Please Check TableName" << endl;
				return;
			}
		}
		else
		{
			cout << "Please Check from the 2nd Word!" << endl;
			return;
		}
	}
	// drop语句
	else if (tolow(s1) == "drop")
	{
		string s2;
		ss >> s2;
		if (tolow(s2) == "table")
		{
			string s3;
			ss >> s3;
			SQLINFO.DropTable(s3);
		}
		else
		{
			cout << "Please Check from the 2nd Word!" << endl;
			return;
		}
	}
	// table list语句
	else if (tolow(s1) == "table")
	{
		string s2;
		ss >> s2;
		if (tolow(s2) == "list")
		{
			SQLINFO.PrintList();
		}
		else
		{
			cout << "Please Check the 2nd Word!" << endl;
			return;
		}
	}
	// insert语句
	else if (tolow(s1) == "insert")
	{
		string s2;
		ss >> s2;
		if (tolow(s2) == "into")
		{
			string s3;
			ss >> s3;
			TableName = s3;
			FileName = SQLINFO.GetFileName(TableName);
			string s4;
			ss >> s4;
			if (tolow(s4) == "values") // 一整行
			{
				string s5;
				ss >> s5;
				GetVec(s5, ValueNameVec);
				Table a;
				a.OpenFile(FileName);
				a.InsertRecord(ValueNameVec);
				a.ShowTable();
				a.CloseFile();
			}
			else // 部分
			{
				GetVec(s4, AttNameVec);
				string s5;
				ss >> s5;
				if (tolow(s5) == "values")
				{
					string s6;
					ss >> s6;
					GetVec(s6, ValueNameVec);
				}
				else
				{
					cout << "Please Check Values" << endl;
					return;
				}
				Table a;
				a.OpenFile(FileName);
				a.InsertRecord(AttNameVec, ValueNameVec);
				a.ShowTable();
				a.CloseFile();
			}
		}
		else
		{
			cout << "Please Check from the 2nd Word!" << endl;
			return;
		}
	}
	// delete语句
	else if (tolow(s1) == "delete")
	{
		string s2;
		ss >> s2;
		if (tolow(s2) == "from")
		{
			string s3;
			ss >> s3;
			TableName = s3;
			FileName = SQLINFO.GetFileName(TableName);
			string s4;
			ss >> s4;
			if (tolow(s4) == "where")
			{
				string s5;
				ss >> s5;
				int flag = GetCondition(s5, SelectedCol, SelectedValue);
				DeleteOptions(flag, FileName, SelectedCol, SelectedValue);
			}
			else
			{
				cout << "Please Check Where" << endl;
				return;
			}
		}
		else if (s2 == "*")
		{
			string s3;
			ss >> s3;
			if (tolow(s3) == "from")
			{
				string s4;
				ss >> s4;
				TableName = s4;
				FileName = SQLINFO.GetFileName(TableName);
			}
			else
			{
				cout << "Please Check From" << endl;
				return;
			}
			Table a;
			a.OpenFile(FileName);
			a.DeleteRecord();
			a.ShowTable();
			a.CloseFile();
		}
		else
		{
			cout << "Please Check from the 2nd Word!" << endl;
			return;
		}
	}
	// update语句
	else if (tolow(s1) == "update")
	{
		string s2;
		ss >> s2;
		TableName = s2;
		FileName = SQLINFO.GetFileName(TableName);
		string s3;
		ss >> s3;
		if (tolow(s3) == "set")
		{
			string s4;
			ss >> s4;
			Get2Vecs(s4, AttNameVec, ValueNameVec);
			if (ss.eof()) // 无条件
			{
				Table a;
				a.OpenFile(FileName);
				a.UpdateRecord(AttNameVec, ValueNameVec);
				a.ShowTable();
				a.CloseFile();
			}
			else // 有条件
			{
				string s5;
				ss >> s5;
				if (tolow(s5) == "where")
				{
					string s6;
					ss >> s6;
					int flag = GetCondition(s6, SelectedCol, SelectedValue);
					UpdateOptions(flag, FileName, AttNameVec, ValueNameVec, SelectedCol, SelectedValue);
				}
				else
				{
					cout << "Please Check Where" << endl;
					return;
				}
			}
		}
		else
		{
			cout << "Please Check Set" << endl;
			return;
		}
	}
	// select语句
	else if (tolow(s1) == "select")
	{
		string s2;
		ss >> s2;
		if (tolow(s2) == "distinct")
		{
			string s3;
			ss >> s3;
			GetVec(s3, AttNameVec);
			string s4;
			ss >> s4;
			if (tolow(s4) == "from")
			{
				string s5;
				ss >> s5;
				TableName = s5;
				FileName = SQLINFO.GetFileName(TableName);
			}
			else
			{
				cout << "Please Check from" << endl;
				return;
			}
			Table a;
			a.OpenFile(FileName);
			a.DisSelectedShow(AttNameVec);
			a.CloseFile();
		}
		else if (tolow(s2) == "max")
		{
			string s3;
			ss >> s3;
			GetVec(s3, AttNameVec);
			string s4;
			ss >> s4;
			if (tolow(s4) == "from")
			{
				string s5;
				ss >> s5;
				TableName = s5;
				FileName = SQLINFO.GetFileName(TableName);
			}
			else
			{
				cout << "Please Check from" << endl;
				return;
			}
			Table a;
			a.OpenFile(FileName);
			a.ShowMax(AttNameVec);
			a.CloseFile();
		}
		else if (tolow(s2) == "min")
		{
			string s3;
			ss >> s3;
			GetVec(s3, AttNameVec);
			string s4;
			ss >> s4;
			if (tolow(s4) == "from")
			{
				string s5;
				ss >> s5;
				TableName = s5;
				FileName = SQLINFO.GetFileName(TableName);
			}
			else
			{
				cout << "Please Check From" << endl;
				return;
			}
			Table a;
			a.OpenFile(FileName);
			a.ShowMin(AttNameVec);
			a.CloseFile();
		}
		else if (tolow(s2) == "avg")
		{
			string s3;
			ss >> s3;
			GetVec(s3, AttNameVec);
			string s4;
			ss >> s4;
			if (tolow(s4) == "from")
			{
				string s5;
				ss >> s5;
				TableName = s5;
				FileName = SQLINFO.GetFileName(TableName);
			}
			else
			{
				cout << "Please Check From" << endl;
				return;
			}
			Table a;
			a.OpenFile(FileName);
			a.ShowAvg(AttNameVec);
			a.CloseFile();
		}
		else if (s2 == "*")
		{
			string s3;
			ss >> s3;
			if (tolow(s3) == "from")
			{
				string s4;
				ss >> s4;
				TableName = s4;
				FileName = SQLINFO.GetFileName(TableName);
				if (ss.eof()) //第二种
				{
					Table a;
					a.OpenFile(FileName);
					a.NoDisSelectedShow();
					a.CloseFile();
				}
				else // 后三种
				{
					string s5;
					ss >> s5;
					if (tolow(s5) == "order")
					{
						string s6;
						ss >> s6;
						if (tolow(s6) == "by")
						{
							string s7;
							ss >> s7;
							GetVec(s7, AttNameVec);
							string s8;
							ss >> s8;
							if (tolow(s8) == "asc")
							{
								Table a;
								a.OpenFile(FileName);
								a.SelectedSort(AttNameVec, "asc");
								a.CloseFile();
							}
							else if (tolow(s8) == "desc")
							{
								Table a;
								a.OpenFile(FileName);
								a.SelectedSort(AttNameVec, "desc");
								a.CloseFile();
							}
							else
							{
								cout << "Please Check Sort Command" << endl;
								return;
							}
						}
						else
						{
							cout << "Please Check By" << endl;
							return;
						}
					}
					else if (tolow(s5) == "where")
					{
						string s6;
						ss >> s6;
						int flag = GetCondition(s6, SelectedCol, SelectedValue);
						if (ss.eof()) // 结束
							SelectOptions(flag, FileName, SelectedCol, SelectedValue);
						else
						{
							string s7;
							ss >> s7;
							if (tolow(s7) == "to")
							{
								string s8;
								ss >> s8;
								SelectOptionsToFile(flag, FileName, s8, SelectedCol, SelectedValue);
							}
							else
							{
								cout << "Please Check To" << endl;
								return;
							}
						}
					}
					else if (tolow(s5) == "to")
					{
						string s6;
						ss >> s6;
						Table a;
						a.OpenFile(FileName);
						a.NoDisSelectedToFile(s6, SelectedCol, SelectedValue);
						a.CloseFile();
					}
					else
					{
						cout << "Please Check order/where/to!" << endl;
						return;
					}
				}
			}
			else
			{
				cout << "Please Check From!" << endl;
				return;
			}
		}
		else
		{
			GetVec(s2, AttNameVec);
			string s3;
			ss >> s3;
			if (tolow(s3) == "from")
			{
				string s4;
				ss >> s4;
				TableName = s4;
				FileName = SQLINFO.GetFileName(TableName);
				if (ss.eof()) // 第一种
				{
					Table a;
					a.OpenFile(FileName);
					a.NoDisSelectedShow(AttNameVec);
					a.CloseFile();
				}
				else // 后三种
				{
					string s5;
					ss >> s5;
					if (tolow(s5) == "where")
					{
						string s6;
						ss >> s6;
						int flag = GetCondition(s6, SelectedCol, SelectedValue);
						if (ss.eof()) // 结束
							SelectOptions(flag, FileName, AttNameVec, SelectedCol, SelectedValue);
						else
						{
							string s7;
							ss >> s7;
							if (tolow(s7) == "to")
							{
								string s8;
								ss >> s8;
								SelectOptionsToFile(flag, FileName, s8, AttNameVec, SelectedCol, SelectedValue);
							}
							else
							{
								cout << "Please Check To!" << endl;
								return;
							}
						}
					}
					else if (tolow(s5) == "to")
					{
						string s6;
						ss >> s6;
						Table a;
						a.OpenFile(FileName);
						a.NoDisSelectedToFile(s6, AttNameVec);
						a.CloseFile();
					}
					else
					{
						cout << "Please Check where/to!" << endl;
						return;
					}
				}
			}
			else
			{
				cout << "Please Check From!" << endl;
				return;
			}
		}
	}
	// read语句
	else if (tolow(s1) == "read")
	{
		string s2;
		ss >> s2;
		if (tolow(s2) == "file")
		{
			string s3;
			ss >> s3;
			FileName = s3;
			RunFile(FileName);
		}
		else
		{
			cout << "Please Check file!" << endl;
			return;
		}
	}
	// 错误语句
	else
	{
		cout << "Please Check from the 1st Word!" << endl;
		return;
	}
}

void RunFile(const string &FileName)
{
	ifstream fin;
	fin.open(ToDoPrefix + FileName);
	while (!fin.eof())
	{
		char buffer[200] = { 0 };
		fin.getline(buffer, 200);
		char ch = fin.peek();
		if (ch == '\n')
			fin.get();
		cout << "RUNNING: " << buffer << endl;
		string s = buffer;
		Parse(s);
	}
	fin.close();
}

