#include<iostream>
#include<string>
#include"Catalog.h"
#include"Table.h"
using namespace std;

string tolow(string);
void Parse(string&);

void cmd()
{
	string s;
	do
	{
		cout << "(mysql)==> ";
		getline(cin, s);
		Parse(s);
	} while (tolow(s) != "quit");
}

int main()
{
	cout << "~$ ";
	string s;
	while (getline(cin, s))
	{
		if (s != "mySQL")
			cout << "INVALID INPUT, PLEASE INPUT AGAIN: ";
		else
			break;
	}
	cmd();
	cout << "~$ ";
}