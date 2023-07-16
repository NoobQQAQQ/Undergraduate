#include"File.h"
#include<fstream>
#include<sstream>
#include<string>
using namespace std;

void File::GetValue(stringstream &ss, ItemNode *p)
{
	ss >> p->id;
	ss >> p->name;
	ss >> p->brand;
	ss >> p->price;
	ss >> p->num;
	ss.clear();
}

void File::GetValue(stringstream &ss, UserNode *p)
{
	ss >> p->AcountInfo.username;
	ss >> p->AcountInfo.cipher;
	ss.clear();
}

void File::GetValue(stringstream &ss, SoldItemNode *p)
{
	ss >> p->id;
	ss >> p->name;
	ss >> p->brand;
	ss >> p->price;
	ss >> p->num;
	ss >> p->username;
	ss.clear();
}

void File::SetValue(ofstream &fout, ItemNode *cur)
{
	fout << cur->id << '\t';
	fout << cur->name << '\t';
	fout << cur->brand << '\t';
	fout << cur->price << '\t';
	fout << cur->num << '\n';
}

void File::SetValue(ofstream &fout, UserNode *cur)
{
	fout << cur->AcountInfo.username << '\t';
	fout << cur->AcountInfo.cipher << '\n';
}

void File::SetValue(ofstream &fout, SoldItemNode *cur)
{
	fout << cur->id << '\t';
	fout << cur->name << '\t';
	fout << cur->brand << '\t';
	fout << cur->price << '\t';
	fout << cur->num << '\t';
	fout << cur->username << '\n';
}

int File::FileToList(ItemNode *&Head, ItemNode *&Tail, string FileName)
{
	int Amout = 0;
	ifstream fin(FileName);
	ItemNode *cur = Head;
	char buffer[100] = { 0 };
	fin.getline(buffer, 100);
	char ch1 = fin.peek();
	if (ch1 == '\n')
		fin.get();
	while (!fin.eof())
	{
		fin.getline(buffer, 100);
		char ch2 = fin.peek();
		if (ch2 == '\n')
			fin.get();
		stringstream ss;
		ss << buffer;
		ItemNode *p = new ItemNode;
		GetValue(ss, p);
		Tail = cur->next = p;
		cur = cur->next;
		Amout++;
	}
	fin.close();
	return Amout;
}

void File::ListToFile(ItemNode *Head, string FileName) 
{
	ofstream fout;
	fout.open(FileName, ios::trunc);
	ItemNode *cur = Head->next;
	fout << "ID\t名称\t品牌\t价格\t数量\n";
	while (cur != NULL)
	{
		SetValue(fout, cur);
		cur = cur->next;
	}
	fout.close();
}

int File::GetUserInfo(UserNode *&Head, UserNode *&Tail, string FileName)
{
	int Amout = 0;
	ifstream fin(FileName);
	UserNode *cur = Head;
	char buffer[100] = { 0 };
	fin.getline(buffer, 100);
	char ch1 = fin.peek();
	if (ch1 == '\n')
		fin.get();
	while (!fin.eof())
	{
		fin.getline(buffer, 100);
		char ch2 = fin.peek();
		if (ch2 == '\n')
			fin.get();
		stringstream ss;
		ss << buffer;
		UserNode *p = new UserNode;
		GetValue(ss, p);
		Tail = cur->next = p;
		cur = cur->next;
		Amout++;
	}
	fin.close();
	return Amout;
}

void File::SetUserInfo(UserNode *&Head, string FileName)
{
	ofstream fout;
	fout.open(FileName, ios::trunc);
	UserNode *cur = Head->next;
	fout << "用户名\t密码\n";
	while (cur != NULL)
	{
		SetValue(fout, cur);
		cur = cur->next;
	}
	fout.close();
}

void File::FileToSoldList(SoldItemNode *&Head, SoldItemNode *&Tail, string FileName)
{
	ifstream fin(FileName);
	SoldItemNode *cur = Head;
	char buffer[100] = { 0 };
	fin.getline(buffer, 100);
	char ch1 = fin.peek();
	if (ch1 == '\n')
		fin.get();
	while (!fin.eof())
	{
		fin.getline(buffer, 100);
		char ch2 = fin.peek();
		if (ch2 == '\n')
			fin.get();
		stringstream ss;
		ss << buffer;
		SoldItemNode *p = new SoldItemNode;
		GetValue(ss, p);
		Tail = cur->next = p;
		cur = cur->next;
	}
	fin.close();
}

void File::SoldListToFile(SoldItemNode *Head, string FileName)
{
	ofstream fout;
	fout.open(FileName, ios::trunc);
	SoldItemNode *cur = Head->next;
	fout << "ID\t名称\t品牌\t价格\t数量\t用户名\n";
	while (cur != NULL)
	{
		SetValue(fout, cur);
		cur = cur->next;
	}
	fout.close();
}