#include<iostream>

using namespace std;

int Joseph(int n, int m, int k) // n����, ������m, �ӵ�k����ʼ����1��ʼ��
{
	if (n == 1)
		return k;
	else
		return (Joseph(n - 1, m, k) + m - 1) % n + k;
}

int main()
{
	int n;
	while(cin >> n)
		cout << Joseph(n, 3, 1) << endl;
}