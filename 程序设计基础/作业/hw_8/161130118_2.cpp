#include<iostream>

using namespace std;

struct Date
{
	int year;
	int month;
	int day;
	friend istream& operator >> (istream &in, Date &date)
	{
		return (in >> date.year >> date.month >> date.day);
	}
	bool LeapYear()
	{
		if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0))
			return 1;
		return 0;
	}
	int Dayth()
	{
		int sum = 0;
		switch (month)
		{
		case 12:
			sum += 30;
		case 11:
			sum += 31;
		case 10:
			sum += 30;
		case 9:
			sum += 31;
		case 8:
			sum += 31;
		case 7:
			sum += 30;
		case 6:
			sum += 31;
		case 5:
			sum += 30;
		case 4:
			sum += 31;
		case 3:
			if (LeapYear())
				sum += 29;
			else
				sum += 28;
		case 2:
			sum += 31;
		case 1:
			sum += day;
		}
		return sum;
	}
};

int main()
{
	Date date;
	while (cin >> date)
		cout << date.Dayth() << endl;
}