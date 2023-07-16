#include<iostream>
#include<cstring>

using namespace std;

int main()
{
	char str[200];
	while (cin.getline(str, 200))
	{
		int len = strlen(str);
		int i;
		for (i = len - 1; (str[i] != ' ')&&(i >= 0); i--);
		cout << len - 1 - i << endl;
	}
}