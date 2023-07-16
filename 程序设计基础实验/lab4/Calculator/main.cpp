#include<iostream>
#include<string>
#include"Calculator.h"
#include"Complex.h"
using namespace std;

void ShowHelpList()
{
	cout << "*****************this calculator is case sensitive*****************" << endl;
	cout << "The supported commands are listed as follows: " << endl;
	cout << "[toexp(z)]: change a complex into its exponential form" << endl;
	cout << "[UP]: show the previous expression and its result" << endl;
	cout << "[DOWN]: show the next expression and its result" << endl;
	cout << "[SHOW]: show the last five expressions and their results" << endl;
	cout << "[dis(z1,z2)]: get the distance of tow complexes" << endl;
	cout << "[SOLVE]: solve a One-quadratic equation in Complex Field" << endl;
	cout << "[Regular]: input an arithmetic expression and get its result" << endl;
}

int main()
{ 
	string s;
	Calculator MyCal;
	cout << "Please Input an Expression you want to calculate(input /help to get help): \n";
	while (cin >> s)
	{
		cout << "Result: \n";
		if (s == "quit")
		{
			cout << "quit the program!\n";
			cout << "____________________________________________________________________\n";
			break;
		}
		else if (s == "/help")
			ShowHelpList();
		else
			MyCal.run(s);
		cout << "____________________________________________________________________\n";
		cout << "Please Input an Expression you want to calculate(input /help to get help): \n";
	}
}