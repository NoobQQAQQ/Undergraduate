#pragma once
#include<iostream>
#include<cstdio>
#include<string>
#include<vector>
#include<stack>
#include<cctype>
#include"Complex.h"
using namespace std;

class Parser
{
public:
	Parser();
	~Parser();
	bool parse(const string &s, vector< pair<string, Complex> > &history);
private:
	bool CheckHead(const string &s);
	bool CheckEnd(const string &s);
	bool CheckOper(const string &s);
	bool CheckReal(const string &s);
	bool CheckImag(const string &s);
	bool CheckLeft(const string &s);
	bool CheckRight(const string &s);
	bool CheckParen(const string &s);
	bool CheckSpell(const string &s);
	bool PreCheck(const string &s);

	bool HandleError(const string &s);
	void PreTreat(const string &s);
	void ToPostFix();
	Complex Calculate();
	string after_treat;
	string post_expression;
};