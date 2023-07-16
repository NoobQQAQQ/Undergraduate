#include<iostream>

using namespace std;

int th(int n) // 返回一个四位数的千位
{
	return n / 1000;
}

int hu(int n) // 返回一个四位数的百位
{
	return (n / 100) % 10;
}

int te(int n) // 返回一个四位数的十位
{
	return (n / 10) % 10;
}

int on(int n) // 返回一个四位数的个位
{
	return n % 10;
}

int main()
{
	for (int i = 32; i < 100; i++)
	{
		int n = i * i;
		if (th(n) == hu(n) && te(n) == on(n) && te(n) != hu(n))
			cout << n << endl;
	}
}