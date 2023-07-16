#include<iostream>
#include<iomanip>
using namespace std;

void SetValue(int **&A, int n)
{
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			A[i][j] = i * n + j + 1;
}

void Trans(int **&A, int n)
{
	for (int i = 0; i < n; i++)
		for (int j = i + 1; j < n; j++)
		{
			int temp = A[i][j];
			A[i][j] = A[j][i];
			A[j][i] = temp;
		}
	cout << right;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
			cout << setw(4) << A[i][j];
		cout << endl;
	}

}

int main()
{
	int n;
	while (cin >> n)
	{
		int **A = new int*[n];
		for (int i = 0; i < n; i++)
			A[i] = new int[n];
		SetValue(A, n);
		Trans(A, n);
		for (int i = 0; i < n; i++)
			delete[] A[i];
		delete[] A;
	}
}