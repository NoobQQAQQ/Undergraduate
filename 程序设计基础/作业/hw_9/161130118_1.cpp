#include<iostream>

using namespace std;

void mswap(int &a, int &b)
{
	if (a == b)
		return;
	a = a^b;
	b = a^b;
	a = a^b;
}

void swap(int a[], int m, int n)
{
	for (int i = 0; i < m / 2; i++)
		mswap(a[i], a[m - 1 - i]);
	for (int i = m; i < m + n / 2; i++)
		mswap(a[i], a[2 * m + n - 1 - i]);
	for (int i = 0; i < (m + n) / 2; i++)
		mswap(a[i], a[m + n - 1 - i]);
}

int main()
{
	int m, n;
	while (cin >> m >> n)
	{
		int *a = new int[m + n];
		for (int i = 0; i < m + n; i++)
			cin >> a[i];
		cout << "Original Array: ";
		for (int i = 0; i < m + n; i++)
			cout << a[i] << ' ';
		cout << endl << "After Swap: ";
		swap(a, m, n);
		for (int i = 0; i < m + n; i++)
			cout << a[i] << ' ';
		cout << endl;
	}
}