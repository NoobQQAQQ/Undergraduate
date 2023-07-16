#include<iostream>
using namespace std;

typedef unsigned int uint;

int Dec2Bi(uint N, int a[40])
{
	int i = 0;
	for ( ; N != 0; i++, N /= 2)
		a[i] = N % 2;
	return i;
}

int Purify(uint N)
{
	int a[40] = { 0 };
	int len = Dec2Bi(N, a);
	int Num0 = 0, Num1 = 0;
	for (int i = 0; i < len; i++)
		(a[i] == 0) ? Num0++ : Num1++;
	return (Num0 < Num1) ? (len - 2 * Num0) : (len - 2 * Num1);
}

int main()
{
	uint N;
	while (cin >> N)
		cout << Purify(N) << endl;
}