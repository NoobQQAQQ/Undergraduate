#include<iostream>

using namespace std;

int main()
{
	int day = 0;
	cin >> day;
	if (day < 0 || day > 6)
		cout << "INPUT INVALID" << endl;
	if (day == 0)
		cout << "SUNDAY" << endl;
	else if (day == 1)
		cout << "MONDAY" << endl;
	else if (day == 2)
		cout << "TUESDAY" << endl;
	else if (day == 3)
		cout << "WEDNESDAY" << endl;
	else if (day == 4)
		cout << "THURSDAY" << endl;
	else if (day == 5)
		cout << "FRIDAY" << endl;
	else if (day == 6)
		cout << "SATURDAY" << endl;
//******************************************************
	switch (day)
	{
	case 0: cout << "SUNDAY" << endl; break;
	case 1: cout << "MONDAY" << endl; break;
	case 2: cout << "TUESDAY" << endl; break;
	case 3: cout << "WEDNESDAY" << endl; break;
	case 4: cout << "THURSDAY" << endl; break;
	case 5: cout << "FRIDAY" << endl; break;
	case 6: cout << "SATURDAY" << endl; break;	
	}
}