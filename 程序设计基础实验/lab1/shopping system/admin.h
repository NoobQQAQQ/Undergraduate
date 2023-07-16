#pragma once
#include<string>
using std::string;

class Admin
{
public:
	Admin(string AdminName, string AdminCipher);
	void Login();
	void cmd();
private:
	string __AdminName;
	string __AdminCipher;
	void ShowAdminMenu();
	void What2Do(char ch);
};