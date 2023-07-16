#include<iostream>

using namespace std;

int Cow(int year)
{
	if (year < 4)
		return 1;
	if (year == 4)
		return 2;
	else
		return Cow(year - 1) + Cow(year - 3);
}

int main()
{
	int year;
	while (cin >> year)
		cout << Cow(year) << endl;
}