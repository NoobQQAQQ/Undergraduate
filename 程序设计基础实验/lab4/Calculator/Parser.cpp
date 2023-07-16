#include"Parser.h"

void Parser::PreTreat(const string &s)
{
	after_treat = post_expression = "";
	after_treat = s;
	int len = s.length();
	if (after_treat[0] == '-')
		after_treat[0] = 'o';
	for (int i = 0; i < len; i++)
	{
		if (after_treat[i] == '-')
		{
			if (after_treat[i - 1] == '(')
				after_treat[i] = 'o';
		}
		else if (after_treat[i] == '|')
		{
			int j = i + 1;
			while (after_treat[j] != '|')
				j++;
			after_treat[i] = '(';
			after_treat[j] = ')';
			after_treat.push_back('#');
			for (int k = len; k > i; k--)
				after_treat[k] = after_treat[k - 1];
			after_treat[i] = 'm';
			len++;
		}
		else if (after_treat[i] == 'i')
		{
			if (i == 0)
				continue;
			if ((isdigit(after_treat[i - 1]) || after_treat[i - 1] == ')'))
			{
				after_treat.push_back('#');
				for (int k = len; k > i; k--)
					after_treat[k] = after_treat[k - 1];
				after_treat[i] = '*';
				len++;
				i++;
			}
		}
	}
}

bool IsOperator(char ch)
{
	if (ch == '(')
		return 1;
	if (ch == '^')
		return 1;
	if (ch == 'o')
		return 1;
	if (ch == 'm')
		return 1;
	if (ch == 'c')
		return 1;
	if (ch == 'a')
		return 1;
	if (ch == '*')
		return 1;
	if (ch == '/')
		return 1;
	if (ch == '+')
		return 1;
	if (ch == '-')
		return 1;
	if (ch == ')')
		return 1;
	return 0;
}

int Isp(char ch) // in stack priority
{
	switch (ch)
	{
	case ('('):
		return 1;
	case ('^'):
		return 9;
	case ('o'):
	case ('m'):
	case ('c'):
	case ('a'):
		return 7;
	case ('*'):
	case ('/'):
		return 5;
	case ('+'):
	case ('-'):
		return 3;
	case (')'):
		return 10;
	case ('#'):
		return 0;
	}
}

int Icp(char ch) // in coming priority
{
	switch (ch)
	{
	case ('('):
		return 10;
	case ('^'):
		return 8;
	case ('o'):
	case ('m'):
	case ('c'):
	case ('a'):
		return 6;
	case ('*'):
	case ('/'):
		return 4;
	case ('+'):
	case ('-'):
		return 2;
	case (')'):
		return 1;
	}
}

Parser::Parser() {}

Parser::~Parser() {}

bool Parser::PreCheck(const string &s)
{
	int cnt = 0;
	int len = s.length();
	for (int i = 0; i < len; i++)
	{
		if (s[i] == '|')
			cnt++;
	}
	if (cnt % 2)
	{
		cout << s << endl;
		cout << "error: ";
		cout << "\"|\" does not match!\n";
		return 1;
	}
	return 0;
}

bool Parser::parse(const string &s, vector< pair<string, Complex> > &history)
{
	if (PreCheck(s))
		return 0;
	PreTreat(s);
	if (HandleError(s))
		return 0;
	else
	{
		ToPostFix();
		Complex result = Calculate();
		if (result.GetReal() == 0.0000001)
			return 0;
		else
		{
			history.push_back(make_pair(s, result));
			return 1;
		}
	}
}

bool Parser::HandleError(const string &s)
{
	bool b1 = CheckHead(s);
	bool b2 = CheckEnd(s);
	bool b3 = CheckOper(s);
	bool b4 = CheckReal(s);
	bool b5 = CheckImag(s);
	bool b6 = CheckLeft(s);
	bool b7 = CheckRight(s);
	bool b8 = CheckParen(s);
	bool b9 = CheckSpell(s);
	if (b1 && b2 && b3 && b4 && b5 && b6 && b7 && b8 && b9)
		return 0;
	return 1;
}

void Parser::ToPostFix()
{
	stack<char> st;
	st.push('#');
	int len = after_treat.length();
	for (int i = 0; i < len; )
	{
		if (IsOperator(after_treat[i]))
		{
			if (Icp(after_treat[i]) > Isp(st.top()))
			{
				st.push(after_treat[i]);
				if (after_treat[i] == 'c' || after_treat[i] == 'a')
					i += 3;
				else
					i++;
			}

			else if (Icp(after_treat[i]) < Isp(st.top()))
			{
				do
				{
					post_expression.push_back(st.top());
					st.pop();
				} while (Icp(after_treat[i]) < Isp(st.top()));
			}

			else
			{
				if (st.top() == '(')
					i++;
				st.pop();
			}
		}


		else
		{
			post_expression.push_back('(');
			do 
			{
				post_expression.push_back(after_treat[i++]);
			} while (i < len && !IsOperator(after_treat[i]));
			post_expression.push_back(')');
		}
	}
	while (st.top() != '#')
	{
		post_expression.push_back(st.top());
		st.pop();
	}
}

Complex Parser::Calculate()
{
	stack<Complex> st;
	int len = post_expression.length();
	for (int i = 0; i < len; )
	{
		if (post_expression[i] == '(')
		{
			int j = i + 1;
			while (post_expression[j] != ')')
				j++;
			Complex a(post_expression.substr(i + 1, j - i - 1));
			st.push(a);
			i = j + 1;
		}

		else if (post_expression[i] == '+')
		{
			Complex a = st.top();
			st.pop();
			Complex b = st.top();
			st.pop();
			st.push(b + a);
			i++;
		}

		else if (post_expression[i] == '-')
		{
			Complex a = st.top();
			st.pop();
			Complex b = st.top();
			st.pop();
			st.push(b - a);
			i++;
		}

		else if (post_expression[i] == '*')
		{
			Complex a = st.top();
			st.pop();
			Complex b = st.top();
			st.pop();
			if (a.isImag() && b.isComp())
			{
				cout << after_treat << endl;
				cout << "error: ";
				cout << "i can't be connected directly with imaginary number!\n";
				return Complex(0.0000001, 0);
			}
			else
			{
				st.push(b * a);
				i++;
			}
		}

		else if (post_expression[i] == '/')
		{
			Complex a = st.top();
			if (a.isZero())
			{
				cout << after_treat << endl;
				cout << "error: ";
				cout << "divided by zero!\n";
				return Complex(0.0000001, 0);
			}
			else
			{
				st.pop();
				Complex b = st.top();
				st.pop();
				st.push(b / a);
				i++;
			}
		}

		else if (post_expression[i] == 'o')
		{
			Complex a = st.top();
			st.pop();
			st.push(-a);
			i++;
		}

		else if (post_expression[i] == 'm')
		{
			Complex a = st.top();
			st.pop();
			st.push(a.modu());
			i++;
		}

		else if (post_expression[i] == 'c')
		{
			Complex a = st.top();
			st.pop();
			st.push(a.cjg());
			i++;
		}

		else if (post_expression[i] == 'a')
		{
			Complex a = st.top();
			st.pop();
			st.push(a.arg());
			i++;
		}

		else if (post_expression[i] == '^')
		{
			Complex a = st.top();
			st.pop();
			if (!a.isReal())
			{
				cout << after_treat << endl;
				cout << "error: ";
				cout << "imaginary number can't be served as index part\n";
				return Complex(0.0000001, 0);
			}
			else
			{
				Complex b = st.top();
				st.pop();
				st.push(b.qpow(a.GetReal()));
				i++;
			}
		}
	}
	after_treat = post_expression = "";
	return st.top();
}



bool IsNormalOpr(char ch)
{
	if (ch == '+')
		return 1;
	if (ch == '-')
		return 1;
	if (ch == '*')
		return 1;
	if (ch == '/')
		return 1;
	return 0;
}




bool Parser::CheckHead(const string &s) // no error return 1
{
	if (isdigit(s[0]))
		return 1;
	if (s[0] == '|')
		return 1;
	if (s[0] == '-')
		return 1;
	if (s[0] == 'i')
		return 1;
	if (s[0] == 'a')
		return 1;
	if (s[0] == 'c')
		return 1;
	if (s[0] == '(')
		return 1;
	int len = s.length();
	for (int i = 0; i < len; i++)
	{
		if (i == 0)
			printf("\033[40;31m%c\033[0m", s[i]);
		else
			printf("%c", s[i]);
	}
	cout << ", error: " << s[0] << " can't be at the beginning!\n";
	return 0;
}

bool Parser::CheckEnd(const string &s)
{
	int len = s.length();
	if (isdigit(s[len - 1]))
		return 1;
	if (s[len - 1] == '|')
		return 1;
	if (s[len - 1] == 'i')
		return 1;
	if (s[len - 1] == ')')
		return 1;
	for (int i = 0; i < len; i++)
	{
		if (i == len - 1)
			printf("\033[40;31m%c\033[0m", s[i]);
		else
			printf("%c", s[i]);
	}
	cout << ", error: " << s[len - 1] << " can't be at the ending!\n";
	return 0;
}

bool Parser::CheckOper(const string &s)
{
	int len = s.length();
	for (int i = 0; i < len; i++)
	{
		if (IsNormalOpr(s[i]) && (i+1 < len))
		{
			if (IsNormalOpr(s[i + 1]))
			{
				for (int j = 0; j < len; j++)
				{
					if (j == i || j == i + 1)
						printf("\033[40;31m%c\033[0m", s[j]);
					else
						printf("%c", s[j]);
				}
				cout << ", error: ";
				cout << s[i] << " and " << s[i + 1] << " do not match!\n";
				return 0;
			}

			else if (s[i + 1] == ')')
			{
				for (int j = 0; j < len; j++)
				{
					if (j == i || j == i + 1)
						printf("\033[40;31m%c\033[0m", s[j]);
					else
						printf("%c", s[j]);
				}
				cout << ", error: ";
				cout << s[i] << " and " << s[i + 1] << " do not match!\n";
				return 0;
			}
		}
	}
	return 1;
}

bool Parser::CheckReal(const string &s)
{
	int len = after_treat.length();
	for (int i = 0; i < len; )
	{
		if (isdigit(after_treat[i]))
		{
			int j = i + 1, dot = 0;
			if (j >= len)
				i++;
			for ( ; j < len; j++)
			{
				if (after_treat[j] == '.')
				{
					dot++;
					if (dot > 1)
					{
						for (int k = 0; k < len; k++)
						{
							if (k == j)
								printf("\033[40;31m%c\033[0m", after_treat[k]);
							else
								printf("%c", after_treat[k]);
						}
						cout << ", error: ";
						cout << after_treat[j] << " is not valid!\n";
						return 0;
					}
				}

				else if (!isdigit(after_treat[j]))
				{
					if (after_treat[j] == '(' || after_treat[j] == 'm' || after_treat[j] == 'a' || after_treat[j] == 'c')
					{
						for (int k = 0; k < len; k++)
						{
							if (k == j)
								printf("\033[40;31m%c\033[0m", after_treat[k]);
							else
								printf("%c", after_treat[k]);
						}
						cout << ", error: ";
						cout << after_treat[j] << " is not valid!\n";
						return 0;
					}

					else if (after_treat[j] == ')' || after_treat[j] == 'i' || after_treat[j] == '^' || IsNormalOpr(after_treat[j]))
					{
						i = j + 1;
						break;
					}

					else
					{
						for (int k = 0; k < len; k++)
						{
							if (k == j)
								printf("\033[40;31m%c\033[0m", after_treat[k]);
							else
								printf("%c", after_treat[k]);
						}
						cout << ", error: ";
						cout << after_treat[j] << " is not valid!\n";
						return 0;
					}
				}
			}
		}
		else
			i++;
	}
	return 1;
}

bool Parser::CheckImag(const string &s)
{
	int len = s.length();
	for (int i = 0; i < len; i++)
	{
		if (s[i] == 'i' && i + 1 < len)
		{
			if (s[i + 1] == '(' || s[i + 1] == 'i' || isdigit(s[i + 1]))
			{
				for(int j = 0; j < len; j++)
				{
					if (j == i || j == i + 1)
						printf("\033[40;31m%c\033[0m", s[j]);
					else
						printf("%c", s[j]);
				}
				cout << ", error: ";
				cout << s[i] << " and " << s[i + 1] << " do not match!\n";
				return 0;
			}
		}
	}

	int lenA = after_treat.length();
	for (int i = 0; i < lenA; i++)
	{
		if (after_treat[i] == 'i' && i + 1 < lenA)
		{
			if (after_treat[i + 1] == 'm')
			{
				for (int j = 0; j < lenA; j++)
				{
					if (j == i || j == i + 1)
						printf("\033[40;31m%c\033[0m", after_treat[j]);
					else
						printf("%c", after_treat[j]);
				}
				cout << ", error: ";
				cout << after_treat[i] << " and " << after_treat[i + 1] << " do not match!\n";
				return 0;
			}
		}
	}
	return 1;
}

bool Parser::CheckLeft(const string &s)
{
	int len = s.length();
	for (int i = 0; i < len; i++)
	{
		if (s[i] == '(' && (i + 1 < len))
		{
			if (s[i + 1] == ')' || s[i + 1] == '+' || s[i + 1] == '*' || s[i + 1] == '/')
			{
				for (int j = 0; j < len; j++)
				{
					if (j == i || j == i + 1)
						printf("\033[40;31m%c\033[0m", s[j]);
					else
						printf("%c", s[j]);
				}
				cout << ", error: ";
				cout << s[i] << " and " << s[i + 1] << " do not match!\n";
				return 0;
			}
		}
	}
	return 1;
}

bool Parser::CheckRight(const string &s)
{
	int len = after_treat.length();
	for (int i = 0; i < len; i++)
	{
		if (after_treat[i] == ')' && (i + 1 < len))
		{
			if (after_treat[i + 1] == '(' || after_treat[i + 1] == 'm' || after_treat[i + 1] == 'a' || after_treat[i + 1] == 'c' || isdigit(after_treat[i + 1]))
			{
				for (int j = 0; j < len; j++)
				{
					if (j == i || j == i + 1)
						printf("\033[40;31m%c\033[0m", s[j]);
					else
						printf("%c", s[j]);
				}
				cout << ", error: ";
				cout << after_treat[i] << " and " << after_treat[i + 1] << " do not match!\n";
				return 0;
			}
		}
	}
}

bool Parser::CheckParen(const string &s)
{
	int len = s.length();
	int cnt1 = 0, cnt2 = 0;
	for (int i = 0; i < len; i++)
	{
		if (s[i] == '(')
			cnt1++;
		else if (s[i] == ')')
			cnt2++;

		if (cnt2 > cnt1)
		{
			for (int j = 0; j < len; j++)
			{
				if (j == i)
					printf("\033[40;31m%c\033[0m", s[j]);
				else
					printf("%c", s[j]);
			}
			cout << ", error: right parentheses are more than left parentheses\n";
			return 0;
		}
	}
	if (cnt1 == cnt2)
		return 1;
	else
	{
		for (int j = 0; j < len; j++)
			printf("%c", s[j]);
		cout << ", error: the parentheses do not match\n";
		return 0;
	}
}

bool Parser::CheckSpell(const string &s)
{
	int len = s.length();
	for (int i = 0; i < len; )
	{
		if (isalpha(s[i]) && s[i] != 'i') 
		{
			if (s[i] == 'a')
			{
				if (i + 1 < len)
				{
					if (s[i + 1] == 'r')
					{
						if (i + 2 < len)
						{
							if (s[i + 2] == 'g')
								i += 3;

							else
							{
								for (int j = 0; j < len; j++)
								{
									if (j == i + 2)
										printf("\033[40;31m%c\033[0m", s[j]);
									else
										printf("%c", s[j]);
								}
								cout << ", error: ";
								cout << s[i + 2] << " is not valid in the expression!\n";
								return 0;
							}
						}

						else
						{
							for (int j = 0; j < len; j++)
							{
								if (j == i + 1)
									printf("\033[40;31m%c\033[0m", s[j]);
								else
									printf("%c", s[j]);
							}
							cout << ", error: ";
							cout << s[i + 1] << " is not valid in the expression!\n";
							return 0;
						}
					}

					else
					{
						for (int j = 0; j < len; j++)
						{
							if (j == i + 1)
								printf("\033[40;31m%c\033[0m", s[j]);
							else
								printf("%c", s[j]);
						}
						cout << ", error: ";
						cout << s[i + 1] << " is not valid in the expression!\n";
						return 0;
					}
				}

				else
				{
					for (int j = 0; j < len; j++)
					{
						if (j == i)
							printf("\033[40;31m%c\033[0m", s[j]);
						else
							printf("%c", s[j]);
					}
					cout << ", error: ";
					cout << s[i] << " is not valid in the expression!\n";
					return 0;
				}
			}

			else if (s[i] == 'c')
			{
				if (i + 1 < len)
				{
					if (s[i + 1] == 'j')
					{
						if (i + 2 < len)
						{
							if (s[i + 2] == 'g')
								i += 3;

							else
							{
								for (int j = 0; j < len; j++)
								{
									if (j == i + 2)
										printf("\033[40;31m%c\033[0m", s[j]);
									else
										printf("%c", s[j]);
								}
								cout << ", error: ";
								cout << s[i + 2] << " is not valid in the expression!\n";
								return 0;
							}
						}

						else
						{
							for (int j = 0; j < len; j++)
							{
								if (j == i + 1)
									printf("\033[40;31m%c\033[0m", s[j]);
								else
									printf("%c", s[j]);
							}
							cout << ", error: ";
							cout << s[i + 1] << " is not valid in the expression!\n";
							return 0;
						}
					}

					else
					{
						for (int j = 0; j < len; j++)
						{
							if (j == i + 1)
								printf("\033[40;31m%c\033[0m", s[j]);
							else
								printf("%c", s[j]);
						}
						cout << ", error: ";
						cout << s[i + 1] << " is not valid in the expression!\n";
						return 0;
					}
				}

				else
				{
					for (int j = 0; j < len; j++)
					{
						if (j == i)
							printf("\033[40;31m%c\033[0m", s[j]);
						else
							printf("%c", s[j]);
					}
					cout << ", error: ";
					cout << s[i] << " is not valid in the expression!\n";
					return 0;
				}
			}

			else
			{
				for (int j = 0; j < len; j++)
				{
					if (j == i)
						printf("\033[40;31m%c\033[0m", s[j]);
					else
						printf("%c", s[j]);
				}
				cout << ", error: ";
				cout << s[i] << " is not valid in the expression!\n";
				return 0;
			}
		}

		else
			i++;
	}
	return 1;
}