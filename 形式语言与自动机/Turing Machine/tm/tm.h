#pragma once
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<set>
#include<map>
#include<algorithm>
#include<cstring>
#include<cstdlib>
#include<cmath>

using namespace std;

const string fileTM = "/test.tm";
const string fileInput = "/input.txt";
const string fileOutput = "/console.txt";
const string fileResult = "/result.txt";
const int TAPE_SIZE = 1024;
const int base = 128;

struct TransCur // parameters of transition function
{
	string curState;
	string curSymbol; // start from tape0
	TransCur() {};
	TransCur(string s1, string s2)
	{
		curState = s1;
		curSymbol = s2;
	}
	inline bool operator < (TransCur const& c) const
	{
		return (curState < c.curState) || (curState == c.curState && curSymbol < c.curSymbol);
	}
};

struct TransNew // results of transition function
{
	string newSymbol;
	string direction;
	string newState;
	TransNew() {};
	TransNew(string s1, string s2, string s3)
	{
		newSymbol = s1;
		direction = s2;
		newState = s3;
	}
};

class TM
{
public:
	TM() {};
	TM(string fileDir) { this->fileDir = fileDir; }
	void parse();
	void execute();
	~TM() {};
private:
	string fileDir;
	set<string> Q;
	set<char> inputSymbols;
	set<char> tapeSymbols;
	string startState;
	char blank;
	set<string> finalStates;
	int numTape;
	multimap<string, string> transPara;
	map<TransCur, TransNew> transFuncs;
	int execute_line(string s, ofstream &fout);
	void parse_line(string s);
	bool checkLegality(string s);
	void parse_Q(string s);
	void parse_input(string s);
	void parse_tape(string s);
	void parse_final(string s);
	void parse_trans(string s);
	void print_tape(const char* tape, int head, int no, ofstream &fout);
	TransNew* get_trans(const TransCur& tc);
};