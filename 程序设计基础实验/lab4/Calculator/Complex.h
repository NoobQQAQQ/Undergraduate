#pragma once
#include<iostream>
#include<string>
#include<cmath>
#include<cstdio>
#include<cctype>
using namespace std;

class Complex
{
public:
	Complex(double r = 0.0, double i = 0.0);
	Complex(const string &s);
	~Complex();
	friend Complex operator +(Complex &a, Complex &b); // add
	friend Complex operator -(Complex &a, Complex &b); // minus
	friend Complex operator *(Complex &a, Complex &b); // time
	friend Complex operator /(Complex &a, Complex &b); // divide
	Complex& operator -(); // get opposite number
	Complex& operator =(const Complex& a);
	Complex modu();
	Complex cjg();
	Complex arg();
	Complex qpow(int n);
	double GetReal() const;
	bool isReal() const;
	bool isImag() const;
	bool isComp() const;
	bool isZero() const;
	void toExp();
	static double dis(Complex a, Complex b);
	friend inline ostream &operator << (ostream& out, Complex& a)
	{
		if (!a.real && !a.imag) // both zero
			cout << 0;
		else if (!a.real && a.imag)
			printf("%.6lfi", a.imag);
		else if (a.real && !a.imag)
			printf("%.6lf", a.real);
		else if (a.real && a.imag < 0)
			printf("%.6lf%.6lfi", a.real, a.imag);
		else if (a.real && a.imag > 0)
			printf("%.6lf+%.6lfi", a.real, a.imag);
		return out;
	}
private:
	double real;
	double imag;
};