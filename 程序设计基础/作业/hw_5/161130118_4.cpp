#include<iostream>
using namespace std;

int put(int apple, int plate)
{
	if (apple <= 1 || plate == 1)
		return 1;
	if (plate > apple)
		return put(apple, apple);
	else
		return put(apple, plate - 1) + put(apple - plate, plate);
}

int main()
{
	int apple, plate;
	int N;
	while (cin >> apple >> plate)
		cout << put(apple, plate) << endl;
}