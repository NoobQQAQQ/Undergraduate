#pragma once
#include"ItemAndUser.h"
using std::stringstream;
using std::ofstream;

class File
{
public:
	int FileToList(ItemNode *&Head, ItemNode *&Tail, string FileName); // 从文件读取商品信息到链表
	void ListToFile(ItemNode *Head, string FileName); // 把存商品信息的链表写进文件
	int GetUserInfo(UserNode *&Head, UserNode *&Tail, string FileName); // 得到用户信息
	void SetUserInfo(UserNode *&Head, string FileName); // 储存用户信息
	void FileToSoldList(SoldItemNode *&Head, SoldItemNode *&Tail, string FileName); // 从文件读取售出清单
	void SoldListToFile(SoldItemNode *Head, string FileName); // 将售出清单写进文件
private:
	void GetValue(stringstream &ss, ItemNode *p); // 从字符串流获得商品数据
	void GetValue(stringstream &ss, UserNode *p); // 从字符串流获得用户数据
	void GetValue(stringstream &ss, SoldItemNode *p); // 从字符串流获得售出商品数据
	void SetValue(ofstream &fout, ItemNode *cur); // 向文件里写商品数据
	void SetValue(ofstream &fout, UserNode *cur); // 向文件里写用户数据
	void SetValue(ofstream &fout, SoldItemNode *cur); // 向文件里写售出商品数据
};