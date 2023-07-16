#include<iostream>
#include<cstdlib> 
using namespace std;

int cmp(const void *a, const void *b)
{
	return *((char*)a) - *((char*)b);
}

int main()
{
	char s[500] = { 0 };
	while (cin >> s)
	{
		qsort(s, strlen(s), sizeof(char), cmp);
		cout << s << endl;
	}
}