#include<iostream>

using namespace std;

bool Have7(int n)
{
	int te = n / 10;
	int on = n % 10;
	if (on == 7 || te == 7)
		return 1;
	return 0;
}

int main()
{
	int n, sum = 0;
	cin >> n;
	for (int i = 1; i <= n; i++)
		if (!Have7(i))
			sum += i;
	cout << sum << endl;
}