#include<iostream>
#include<cstring>

using namespace std;

void Check(char *str)
{
	int len = strlen(str);
	int pointer = 0, i = 0;
	for (int i = 0; i < len; i++)
	{
		if (pointer < 0)
		{
			cout << "false" << endl; return;
		}
		if (str[i] == '(')
			pointer++;
		else if (str[i] == ')')
			pointer--;
	}
	cout << ((!pointer) ? "true" : "false") << endl;
}

int main()
{
	char str[200] = { 0 };
	while (cin >> str)
		Check(str);
}