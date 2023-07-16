#include"Complex.h"


int findAdd(const string &s, int len)
{
	for (int i = 0; i < len; i++)
		if (s[i] == '+')
			return i;
	return -1;
}

int findMinus(const string &s, int len)
{
	for (int i = 0; i < len; i++)
		if (s[i] == '-')
			return i;
	return -1;
}

Complex::Complex(double r, double i)
{
	real = r, imag = i;
}

Complex::Complex(const string &s)
{
	int len = s.length();
	if (s[len - 1] != 'i') // pure real
	{
		real = stod(s);
		imag = 0;
	}
	else // pure imag or complex
	{
		int AddIndex = findAdd(s, len);
		int MinusIndex = findMinus(s, len);
		if (isdigit(s[0]) && (AddIndex != -1 || MinusIndex != -1)) // complex
		{
			if (AddIndex != -1)
			{
				real = stod(s.substr(0, AddIndex));
				if (s[AddIndex + 1] == 'i')
					imag = 1;
				else
					imag = stod(s.substr(AddIndex + 1, len - AddIndex - 2));
			}
			else if (MinusIndex != -1)
			{
				real = stod(s.substr(0, MinusIndex));
				if (s[MinusIndex + 1] == 'i')
					imag = -1;
				else
					imag = stod(s.substr(MinusIndex, len - MinusIndex - 1));
			}
		}

		else if (len == 1)
		{
			real = 0;
			imag = 1;
		}

		else if (len == 2 && s[0] == '-')
		{
			real = 0;
			imag = -1;
		}

		else if (s[0] == '-')
		{
			int NewMinus = findMinus(s.substr(1, len), len - 1);
			if (AddIndex != -1)
			{
				real = stod(s.substr(0, AddIndex));
				if (s[AddIndex + 1] == 'i')
					imag = 1;
				else
					imag = stod(s.substr(AddIndex + 1, len - AddIndex - 2));
			}
			else if (NewMinus != -1)
			{
				real = stod(s.substr(0, NewMinus + 1));
				if (s[NewMinus + 2] == 'i')
					imag = -1;
				else
					imag = stod(s.substr(NewMinus + 1, len - NewMinus - 2));
			}
		}

		else
		{
			real = 0;
			imag = stod(s.substr(0, len - 1));
		}
	}
}

Complex::~Complex() {}

Complex operator +(Complex &a, Complex &b) // add
{
	Complex c;
	c.real = a.real + b.real;
	c.imag = a.imag + b.imag;
	return c;
}

Complex operator -(Complex &a, Complex &b) // minus
{
	Complex c;
	c.real = a.real - b.real;
	c.imag = a.imag - b.imag;
	return c;
}

Complex operator *(Complex &a, Complex &b) // time
{
	Complex c;
	c.real = (a.real * b.real - a.imag * b.imag);
	c.imag = (a.real * b.imag + a.imag * b.real);
	return c;
}

Complex operator /(Complex &a, Complex &b) // divide
{
	Complex c;
	double temp = b.real * b.real + b.imag * b.imag;
	c.real = (a.real * b.real + a.imag * b.imag) / temp;
	c.imag = (a.imag * b.real - a.real * b.imag) / temp;
	return c;
}

Complex& Complex::operator -() // get opposite number
{
	real = -real;
	imag = -imag;
	return *this;
}

Complex& Complex::operator =(const Complex& a)
{
	real = a.real;
	imag = a.imag;
	return *this;
}

Complex Complex::modu()
{
	Complex c(sqrt(real * real + imag * imag) , 0.0);
	return c;
}

Complex Complex::cjg()
{
	Complex c(real, -imag);
	return c;
}

Complex Complex::arg()
{
	Complex c(atan2(imag, real), 0.0);
	return c;
}

void Complex::toExp()
{
	double theta = atan2(imag, real);
	double mmoo = sqrt(real * real + imag * imag);
	cout << mmoo << "e^" << theta << 'i' << endl;
}

double Complex::dis(Complex a, Complex b)
{
	return sqrt((a.real - b.real) * (a.real - b.real) + (a.imag - b.imag) * (a.imag - b.imag));
}

Complex Complex::qpow(int n)
{
	Complex result(1, 0), base(real, imag), unit(1, 0);
	bool flag = 0;
	if (n < 0)
	{
		n = -n;
		flag = 1;
	}
	while (n)
	{
		if (n % 2)
			result = result * base;
		base = base * base;
		n /= 2;
	}
	if (flag)
		result = unit / result;
	return result;
}

double Complex::GetReal() const
{
	return real;
}

bool Complex::isReal() const
{
	if (imag == 0)
		return 1;
	return 0;
}

bool Complex::isImag() const
{
	if (real == 0 && imag == 1)
		return 1;
	return 0;
}

bool Complex::isComp() const
{
	if (imag != 0)
		return 1;
	return 0;
}

bool Complex::isZero() const
{
	if (real == 0 && imag == 0)
		return 1;
	return 0;
}