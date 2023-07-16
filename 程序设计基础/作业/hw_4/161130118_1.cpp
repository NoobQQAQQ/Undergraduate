#include<iostream>

using namespace std;

int gcd(int a, int b)
{
	return (b == 0) ? a : gcd(b, a % b);
}

int gcd(int a, int b, int c)
{
	return gcd(a, gcd(b, c));
}

int lcm(int a, int b)
{
	return (a * b) / gcd(a, b);
}

int lcm(int a, int b, int c)
{
	return lcm(a, lcm(b, c));
}

int main()
{
	int a, b, c;
	while (cin >> a >> b >> c)
	{
		cout << "gcd: " << gcd(a, b, c) << endl;
		cout << "lcm: " << lcm(a, b, c) << endl;
	}
}