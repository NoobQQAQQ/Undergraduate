#include<iostream>
#include<cmath>

using namespace std;

int Sum(int n)
{
	int sum = 0, x = 0;
	for (int i = 0; i < n; i++)
	{
		cin >> x;
		sum += x;
	}
	cout << sum << endl;
	return sum;
}

bool IsPrime(int x)
{
	for (int i = 2; i <= sqrt(x); i++)
		if (x % i == 0)
			return false;
	return true;
}

void Prime(int sum)
{
	for (int i = 2; i < sum; i++)
		if (IsPrime(i))
			cout << i << ' ';
	cout << endl;
}

int main()
{
	int n = 0;
	cin >> n;
	int sum = Sum(n);
	Prime(sum);
}