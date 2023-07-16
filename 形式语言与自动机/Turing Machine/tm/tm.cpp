#include "tm.h"

using namespace std;

void TM::parse()
{
	ifstream fin;
	fin.open(fileDir + fileTM);
	string line;
	while (getline(fin, line))
		parse_line(line);
	fin.close();
}

void TM::parse_line(string s)
{
	if (s[0] == ';' || s.length() == 0)
		return;
	else if (s[0] == '#')
	{
		switch (s[1])
		{
			case 'Q': parse_Q(s); break;
			case 'S': parse_input(s); break;
			case 'G': parse_tape(s); break;
			case 'q':
			{
				int ans = s.find(';', 6);
				if (ans != string::npos) // 有行内注释
					startState = s.substr(6, ans - 6);
				else // 无行内注释
					startState = s.substr(6);
				break;
			}
			case 'B': blank = s[5]; break;
			case 'F': parse_final(s); break;
			case 'N': numTape = stoi(s.substr(5)); break;
		}
	}
	else
		parse_trans(s);
}

void TM::parse_Q(string s)
{
	int start = s.find('{') + 1;
	int end = s.find('}');
	string sub = s.substr(start, end - start);
	istringstream iss(sub);
	string token;
	while (getline(iss, token, ','))
		Q.insert(token);
}

void TM::parse_input(string s)
{
	int start = s.find('{') + 1;
	int end = s.find('}');
	string sub = s.substr(start, end - start);
	istringstream iss(sub);
	string token;
	while (getline(iss, token, ','))
		inputSymbols.insert(token[0]);
}

void TM::parse_tape(string s)
{
	int start = s.find('{') + 1;
	int end = s.find('}');
	string sub = s.substr(start, end - start);
	istringstream iss(sub);
	string token;
	while (getline(iss, token, ','))
		tapeSymbols.insert(token[0]);
}

void TM::parse_final(string s)
{
	int start = s.find('{') + 1;
	int end = s.find('}');
	string sub = s.substr(start, end - start);
	istringstream iss(sub);
	string token;
	while (getline(iss, token, ','))
		finalStates.insert(token);
}

void TM::parse_trans(string s)
{
	istringstream iss(s);
	string s1, s2, s3, s4, s5;
	iss >> s1; iss >> s2; iss >> s3; iss >> s4; iss >> s5;
	TransCur tc(s1, s2);
	TransNew tn(s3, s4, s5);
	transFuncs.insert(make_pair(tc, tn));
	transPara.insert(make_pair(s1, s2));
}

void TM::execute()
{
	ifstream fin;
	fin.open(fileDir + fileInput);
	string line;
	ofstream foutRes;
	foutRes.open(fileDir + fileResult);
	ofstream foutCon;
	foutCon.open(fileDir + fileOutput);
	while (getline(fin, line))
	{
		switch (execute_line(line, foutCon))
		{
			case -1:
				foutRes << "Error\n"; break;
			case 0:
				foutRes << "False\n"; break;
			case 1:
				foutRes << "True\n"; break;
		}
	}
	fin.close();
	foutRes.close();
	foutCon.close();
}

int TM::execute_line(string s, ofstream &fout)
{
	fout << "input: " << s << endl;
	if (checkLegality(s) == 0)
	{
		fout << "==================== ERR ====================\n";
		fout << "The input \"" << s << "\" is illegal\n";
		fout << "==================== END ====================\n";
		return -1;
	}
	else
	{
		fout << "==================== RUN ====================\n";
		vector<char[TAPE_SIZE]> tapes(numTape);
		for (int i = 0; i < tapes.size(); i++)
			memset(tapes[i], blank, TAPE_SIZE);
		memcpy(tapes[0] + base, s.data(), s.size());
		vector<int> heads(numTape, base);
		int step = 0;
		string curState = startState;
		fout << "Step    :\t" << step << endl;
		for (int i = 0; i < tapes.size(); i++)
			print_tape(tapes[i], heads[i], i, fout);
		fout << "State   :\t" << curState << endl;
		fout << "------------------------------------------------------\n";
		while (1)
		{
			step++;
			string curSymbol;
			for (int i = 0; i < tapes.size(); i++)
				curSymbol.push_back(tapes[i][heads[i]]);
			TransCur tc(curState, curSymbol);			
			if (transFuncs.find(tc) != transFuncs.end()) // 精确匹配
			{
				TransNew tn = transFuncs[tc];
				for (int i = 0; i < numTape; i++)
					if (tn.newSymbol[i] != '*')
						tapes[i][heads[i]] = tn.newSymbol[i];
				for (int i = 0; i < numTape; i++)
				{
					if (tn.direction[i] == 'l') heads[i]--;
					else if (tn.direction[i] == 'r') heads[i]++;
				}
				curState = tn.newState;
				fout << "Step    :\t" << step << endl;
				for (int i = 0; i < tapes.size(); i++)
					print_tape(tapes[i], heads[i], i, fout);
				fout << "State   :\t" << curState << endl;
				fout << "------------------------------------------------------\n";
			}
			else
			{
				TransNew *ptn = get_trans(tc);
				if (ptn != NULL) // 模糊匹配
				{
					for (int i = 0; i < numTape; i++)
						if (ptn->newSymbol[i] != '*')
							tapes[i][heads[i]] = ptn->newSymbol[i];
					for (int i = 0; i < numTape; i++)
					{
						if (ptn->direction[i] == 'l') heads[i]--;
						else if (ptn->direction[i] == 'r') heads[i]++;
					}
					curState = ptn->newState;
					fout << "Step    :\t" << step << endl;
					for (int i = 0; i < tapes.size(); i++)
						print_tape(tapes[i], heads[i], i, fout);
					fout << "State   :\t" << curState << endl;
					fout << "------------------------------------------------------\n";
				}
				else break; // 匹配失败(可认为该情况只在停机状态下出现)
			}
		}
		fout << "Result: ";
		int leftmost, rightmost;
		leftmost = rightmost = heads[0];
		for (int i = 0; i < TAPE_SIZE; i++)
			if (tapes[0][i] != blank)
			{
				leftmost = i; break;
			}
		for (int i = TAPE_SIZE - 1; i >= 0; i--)
			if (tapes[0][i] != blank)
			{
				rightmost = i; break;
			}
		for (int i = leftmost; i <= rightmost; i++)
			fout << tapes[0][i];
		fout << endl;
		fout << "==================== END ====================\n";
		if (tapes[0][leftmost] == 'T')
			return 1;
		else return 0;
	}
}

bool TM::checkLegality(string s)
{
	int len = s.length();
	for (int i = 0; i < len; i++)
		if (inputSymbols.find(s[i]) == inputSymbols.end())
			return 0;
	return 1;
}

void TM::print_tape(const char* tape, int head, int no, ofstream &fout)
{
	int leftmost, rightmost;
	leftmost = rightmost = head;
	for (int i = 0; i < TAPE_SIZE; i++)
		if (tape[i] != blank)
		{
			leftmost = i; break;
		}
	for (int i = TAPE_SIZE - 1; i >= 0; i--)
		if (tape[i] != blank)
		{
			rightmost = i; break;
		}
	if (head < leftmost)
		leftmost = head;
	if (head > rightmost)
		rightmost = head;
	int no_len = (no)? (1 + log10(no)) : 1;
	fout << "Index" << no;
	for (int i = 0; i < 3 - no_len; i++)
		fout << ' ';
	fout << ":\t";
	for (int i = leftmost; i < rightmost; i++)
		fout << left << abs(i - base) << '\t';
	fout << left << abs(rightmost - base) << endl;
	fout << "Tape" << no;
	for (int i = 0; i < 4 - no_len; i++)
		fout << ' ';
	fout << ":\t";
	for (int i = leftmost; i < rightmost; i++)
		fout << left << tape[i] << '\t';
	fout << left << tape[rightmost] << endl;
	fout << "Head" << no;
	for (int i = 0; i < 4 - no_len; i++)
		fout << ' ';
	fout << ":\t";
	for (int i = leftmost; i < head; i++)
		fout << left << '\t';
	fout << left << '^' << endl;
}

TransNew* TM::get_trans(const TransCur& tc)
{
	set<string> tempSet;
	auto ret = transPara.equal_range(tc.curState);
	auto it = ret.first;
	while (it != ret.second)
	{
		tempSet.insert(it->second);
		++it;
	}
	if (tempSet.size() == 0)
		return NULL;
	for (int i = 0; i < tc.curSymbol.size(); i++)
	{
		auto iter = tempSet.begin();
		while (iter != tempSet.end())
		{
			if (iter->at(i) != tc.curSymbol[i] && iter->at(i) != '*')
			{
				iter = tempSet.erase(iter);
				if (tempSet.size() == 0)
					return NULL;
			}
			else ++iter;
		}
	}
	auto iter = tempSet.begin();
	string res = *iter;
	int numStar = 0;
	for (int i = 0; i < res.size(); i++)
		if (res[i] == '*')
			numStar++;
	++iter;
	while (iter != tempSet.end())
	{
		string tmp = *iter;
		int count = 0;
		for (int i = 0; i < tmp.size(); i++)
			if (tmp[i] == '*')
				count++;
		if (count < numStar)
		{
			res = tmp;
			numStar = count;
		}
		++iter;
	}
	TransCur tc1(tc.curState, res);
	return &transFuncs[tc1];
}
