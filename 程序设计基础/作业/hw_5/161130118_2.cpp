#include<iostream>
using namespace std;
typedef unsigned int uint;

void PrimeFactorization(uint n)
{
	uint Fprime = 2;
	uint count = 0;
	while (n != 1)
	{
		if (n % Fprime == 0)
		{
			count++;
			n /= Fprime;
			if (n == 1)
				cout << Fprime << '(' << count << ')';
		}
		else
		{
			if (count > 0)
				cout << Fprime << '(' << count << ')';
			count = 0;
			Fprime++;
		}
	}
	cout << endl;
}

int main()
{
	uint n;
	while (cin >> n)
		PrimeFactorization(n);
}