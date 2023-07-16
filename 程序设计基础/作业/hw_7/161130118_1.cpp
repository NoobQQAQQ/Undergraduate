#include<iostream>

using namespace std;

int squeeze(char s1[], const char s2[])
{
	const char *cur2 = s2;
	int count = 0;
	while (*cur2 != '\0')
	{
		char *cur1 = s1;
		while (*cur1 != '\0')
		{
			if (*cur1 == *cur2)
			{
				count++;
				char *cur = cur1;
				while (*cur != '\0')
				{
					*cur = *(cur + 1);
					cur++;
				}
			}
			else
				cur1++;
		}
		cur2++;
	}
	return count;
}

int main()
{
	char s1[200] = { 0 };
	char s2[200] = { 0 };
	while (cin >> s1 >> s2)
	{
		cout << "(Original)s1: " << s1 << endl;
		int n = squeeze(s1, s2);
		cout << "(Now)s1: " << s1 << endl;
		cout << "Number of Removed Characters: " << n << endl;
	}
}