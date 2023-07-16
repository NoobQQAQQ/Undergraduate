#include"Calculator.h"


void SolveEquation(double a, double b, double c)
{
	cout << "The solution is: ";
	if (a == 0)
	{
		if (b == 0)
			cout << "Meaningless Equation\n";
		else
			cout << "x = " << (-c) / b << endl;
	}

	else
	{
		double delta = b * b - 4 * a * c;
		if (delta > 0)
		{
			double root1, root2;
			root1 = (-b + sqrt(delta)) / (2 * a);
			root2 = (-b - sqrt(delta)) / (2 * a);
			cout << "x1 = " << root1 << ", x2 = " << root2 << endl;
		}
		else if (delta < 0)
		{
			Complex root1((-b) / (2 * a), sqrt(-delta) / (2 * a));
			Complex root2((-b) / (2 * a), -sqrt(-delta) / (2 * a));
			cout << "x1 = " << root1 << ", x2 = " << root2 << endl;
		}
		else // delta == 0
			cout << "x1 = x2 = " << (-b) / (2 * a) << endl;
	}
}

Calculator::Calculator()
{
	VecNum = VecPtr = 0;
}

Calculator::~Calculator() {}

void Calculator::run(const string &s)
{
	if (s == "UP")
	{
		VecPtr--;
		if (VecPtr >= 0)
		{
			cout << "Input: " << history[VecPtr].first << endl;
			cout << "Result: " << history[VecPtr].second << endl;
		}
		else
		{
			cout << "have reached the head of history list\n";
			VecPtr = 0;
		}
	}

	else if (s == "DOWN")
	{
		VecPtr++;
		if (VecPtr < VecNum)
		{
			cout << "Input: " << history[VecPtr].first << endl;
			cout << "Result: " << history[VecPtr].second << endl;
		}
		else
		{
			cout << "have reached the tail of history list\n";
			VecPtr = VecNum;
		}
	}

	else if (s == "SHOW")
	{
		for (int i = 1; i <= 5; i++)
			if (VecNum - i >= 0)
			{
				cout << "Input: " << history[VecNum - i].first << endl;
				cout << "Result: " << history[VecNum - i].second << endl;
				cout << "____________________________________________________________________\n";
			}
	}

	else if (s == "SOLVE")
	{
		cout << "Please input the coefficients(in the order of [a b c]) of equation(Shaped like: ax^2 + bx + c = 0)\n";
		cout << "Input: ";
		double a, b, c;
		cin >> a >> b >> c;
		SolveEquation(a, b, c);
	}

	else if (s.substr(0, 5) == "toexp")
	{
		int len = s.length();
		Complex a(s.substr(6, len - 7));
		a.toExp();
	}

	else if (s.substr(0, 3) == "dis")
	{
		int comma = 4, len = s.length();
		while (s[comma] != ',')
			comma++;
		Complex z1(s.substr(4, comma - 4));
		Complex z2(s.substr(comma + 1, len - 2 - comma));
		Complex z(Complex::dis(z1, z2), 0);
		history.push_back(make_pair(s, z));
		cout << history[VecNum++].second << endl;
		VecPtr = VecNum;
	}

	else
	{
		if (parser.parse(s, history)) // get new result
			cout << history[VecNum++].second << endl;
		VecPtr = VecNum;
	}
}