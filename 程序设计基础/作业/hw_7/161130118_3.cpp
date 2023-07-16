#include<iostream>
using namespace std;

void Input(int **&A, int n)
{
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			cin >> A[i][j];
}

void FindSaddlePoint(int **&A, int n)
{
	bool have = 0;
	for (int i = 0; i < n; i++)
	{
		int max = A[i][0];
		for (int j = 1; j < n; j++)
			if (A[i][j] > max)
				max = A[i][j];
		for (int j = 0; j < n; j++)
		{
			if (A[i][j] == max)
			{
				bool find = 1;
				for (int k = 0; k < n; k++)
					if (A[k][j] < max)
					{
						find = 0;
						break;
					}
				if (find)
				{
					have = 1;
					cout << "find it-->" << max << endl;
					cout << "row:" << i << "  " << "col:" << j << endl;
				}
			}
		}
	}
	if (!have)
		cout << "not find it..." << endl;
}

int main()
{
	int n;
	while (cin >> n)
	{
		int **A = new int*[n];
		for (int i = 0; i < n; i++)
			A[i] = new int[n];
		Input(A, n);
		FindSaddlePoint(A, n);
		for (int i = 0; i < n; i++)
			delete[] A[i];
		delete[] A;
	}
}