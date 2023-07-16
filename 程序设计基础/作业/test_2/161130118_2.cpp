#include<iostream>
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

// 根据错位排列的概率计算得到结果， n较小时手动控制返回值
int result(int n)
{
	if (n == 1)
		return 0;
	if (n == 2)
		return 1;
	if (n == 3)
		return 2;
	double sum = 0;
	for (int k = 1; k <= n; k++)
	{
		if (k % 2 == 0)
			sum -= 1 / fac(k);
		else
			sum += 1 / fac(k);
	}
	return fac(n) * sum;
}

int w(int n)
{
	if (n == 1)
		return 0;
	if (n == 2)
		return 1;
	else
		return (n - 1) * w(n - 1);
}

int main()
{
	int n;
	while (cin >> n)
		cout << result(n) << endl;
}