#include<iostream>

using namespace std;

int walk(int n)
{
	if (n == 0)
		return 1;
	if (n == 1)
		return 3;
	else
		return 2 * walk(n - 1) + walk(n - 2);
}

int main()
{
	int n;
	while (cin >> n)
		cout << walk(n) << endl;
}