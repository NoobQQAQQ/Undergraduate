#include<iostream>
#include<iomanip>
using namespace std;

double fac(int n)
{
	if (n == 0)
		return 1;
	else
	{
		double sum = 1;
		for (int i = 2; i <= n; i++)
			sum *= i;
		return sum;
	}
}

double Mypow(double x, int n)
{
	if (x == 0)
		return 0;
	if (n == 0)
		return 1;
	else
	{
		double sum = x;
		for (int i = 0; i < n - 1; i++)
			sum *= x;
		return sum;
	}
}

double exp(double x)
{
	double result = 0;
	for (int i = 0; i <= 1000000000; i++)
	{
		if (Mypow(x, i) / fac(i) < 1e-7)
			break;
		else
		result += Mypow(x, i) / fac(i);
	}
	return result;
}

int main()
{
	double x;
	while (cin >> x)
		cout << fixed << setprecision(6) << exp(x) << endl;
}