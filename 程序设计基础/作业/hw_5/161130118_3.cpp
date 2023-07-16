#include<iostream>

using namespace std;
typedef unsigned int uint;

bool IsPrime(uint n)
{
	for (uint i = 2; i * i <= n; i++)
		if (n % i == 0)
			return 0;
	return 1;
}

void GoldBach(uint n)
{
	for (uint i = 2; i <= n / 2; i++)
		if (IsPrime(i) && IsPrime(n - i))
		{
			cout << n << '=' << i << '+' << n - i << endl;
			return;
		}
}

int main()
{
	uint n;
	while (cin >> n)
		GoldBach(n);
}