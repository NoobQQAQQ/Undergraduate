#include<iostream>

using namespace std;

int th(int n) // ����һ����λ����ǧλ
{
	return n / 1000;
}

int hu(int n) // ����һ����λ���İ�λ
{
	return (n / 100) % 10;
}

int te(int n) // ����һ����λ����ʮλ
{
	return (n / 10) % 10;
}

int on(int n) // ����һ����λ���ĸ�λ
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