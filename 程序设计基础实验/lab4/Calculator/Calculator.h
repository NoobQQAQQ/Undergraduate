#pragma once
#include<iostream>
#include<cmath>
#include<string>
#include<vector>
#include"Parser.h"
#include"Complex.h"
using namespace std;

class Calculator
{
public:
	Calculator();
	~Calculator();
	void run(const string &s);
private:
	Parser parser;
	vector< pair<string, Complex> > history;
	int VecNum;
	int VecPtr;
};