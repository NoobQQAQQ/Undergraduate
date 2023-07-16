#include<iostream>
#include<cmath>
#include<iomanip>
using namespace std;

double cube(double x)
{
	return x * x * x;
}

double square(double x)
{
	return x * x;
}

double f(double x, double a, double b, double c, double d)
{
	return a * cube(x) + b * square(x) + c * x + d;
}

double g(double x, double a, double b, double c, double d)
{
	return 3 * a * square(x) + 2 * b * x + c;
}

double NewTon(double a, double b, double c, double d)
{
	double x1 = 1.0, x2 = 0.0;
	while (fabs(x2 - x1) >= 10e-6)
	{
		x1 = x2;
		x2 = x1 - f(x1, a, b, c, d) / g(x1, a, b, c, d);
	}
	return x2;
}

int main()
{
	double a, b, c, d;
	while (cin >> a >> b >> c >> d)
		cout << fixed << setprecision(4) << NewTon(a, b, c, d) << endl;
}