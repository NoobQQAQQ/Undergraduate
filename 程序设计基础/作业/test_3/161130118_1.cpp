#include<iostream>
#include<cstring>
using namespace std;

int Bf(char *s1, char *s2)
{
	int len1 = strlen(s1), len2 = strlen(s2);
	for (int i = 0; i < len1; i++)
	{
		int j = i, k = 0;
		while (k < len2)
		{
			if (s1[j] == s2[k])
				j++, k++;
			else
				break;
		}
		if (k == len2)
			return i;
	}
	return -1;
}

int main()
{
	char s1[100] = { 0 };
	char s2[50] = { 0 };
	while (cin >> s1 >> s2)
		cout << Bf(s1, s2) << endl;
}