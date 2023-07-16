#include<iostream>
#include<cstring>

using namespace std;

void case1(int acount[9], char b[7])
{
	for (int i = 8; i > b[0] - '1'; i--)
		if (acount[i] > 0)
		{
			cout << "YES" << endl;
			return;
		}
	cout << "NO" << endl;
}

void case2(int acount[9], char b[7])
{
	for (int i = 8; i > b[0] - '1'; i--)
		if (acount[i] >= 2)
		{
			cout << "YES" << endl;
			return;
		}
	cout << "NO" << endl;
}

void case3(int acount[9], char b[7])
{
	for (int i = 8; i > b[0] - '1'; i--)
		if (acount[i] >= 3)
		{
			cout << "YES" << endl;
			return;
		}
	cout << "NO" << endl;
}

void case4(int acount[9], char b[7])
{
	if (b[0] == b[1] && b[0] == b[2]) // 四连
	{
		for (int i = 8; i > b[0] - '1'; i--)
			if (acount[i] >= 4)
			{
				cout << "YES" << endl;
				return;
			}
		cout << "NO" << endl;
	}
	else // 连对
	{
		char s = b[0];
		for (int i = 1; i < 4; i++)
			if (b[i] > s)
				s = b[i];
		for (int i = 8; i > s - '1'; i--)
			if (acount[i] >= 2 && acount[i - 1] >= 2)
			{
				cout << "YES" << endl;
				return;
			}
		cout << "NO" << endl;
	}
}

void case5(int acount[9], char b[7])
{ 
	char s = b[0];
	for (int i = 1; i < 5; i++)
		if (b[i] > s)
			s = b[i];
	for (int i = 8; i > s - '1'; i--)
		if (acount[i] && acount[i - 1] && acount[i - 2] && acount[i - 3] && acount[i - 4])
		{
			cout << "YES" << endl;
			return;
		}
	cout << "NO" << endl;
}

void case6(int acount[9], char b[7])
{
	char s = b[0];
	for (int i = 1; i < 6; i++)
		if (b[i] > s)
			s = b[i];
	for (int i = 8; i > s - '1'; i--)
		if (acount[i] >= 3 && acount[i - 1] >= 3)
		{
			cout << "YES" << endl;
			return;
		}
	cout << "NO" << endl;
}

void Judge(char a[41], char b[7])
{
	int lena = strlen(a); 
	int lenb = strlen(b);
	int acount[9] = { 0 }; // 统计手牌数量
	for (int i = 0; i < lena; i++)
		acount[a[i] - 1 - '0']++;
	switch (lenb)
	{
	case 1:
		case1(acount, b); break;
	case 2:
		case2(acount, b); break;
	case 3:
		case3(acount, b); break;
	case 4:
		case4(acount, b); break;
	case 5:
		case5(acount, b); break;
	case 6:
		case6(acount, b); break;
	}
}

int main()
{
	int n;
	cin >> n;
	for (int ini = 0; ini < n; ini++)
	{
		char a[41] = { 0 };
		char b[7] = { 0 };
		cin >> a >> b;
		Judge(a, b);
	}
}