#include<iostream>

using namespace std;


int Max(int a, int b, int c)
{
	return (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);
}

double Max(double a, double b, double c)
{
	return (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);
}

int main()
{
	int ia, ib, ic;
	double da, db, dc;
	cout << "������������: ";
	cin >> ia >> ib >> ic;
	cout << Max(ia, ib, ic) << endl;
	cout << "��������������: ";
	cin >> da >> db >> dc;
	cout << Max(da, db, dc) << endl;
}