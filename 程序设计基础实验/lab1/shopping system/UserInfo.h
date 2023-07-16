#pragma once
#include"File.h"
using std::string;

class UserInfo
{
public:
	UserInfo(string FileName);
	void SyncData(string FileName);
	bool CheckName(string username);
	bool CheckAccount(string username, string cipher);
	void Traverse(string ItemID);
	void AddAccount(string username, string UserCipher);
	void Traverse(string ItemID, double NewPrice);
	void Search2ChangeCipher(string username, string NewCipher);
	void Search2ChangeName(string __username, string NewName);
	void PrintUsers();
	~UserInfo();
private:
	UserNode *Head;
	UserNode *Tail;
	File f;
	int NumOfUsers;
};