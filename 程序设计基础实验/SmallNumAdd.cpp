#define _CRT_SECURE_NO_WARNINGS

#include<iostream>
#include<cstring>

using namespace std;

int FindDot(char a[102])  // �ҵ�С����λ��, ����С����, ����-1
{
	for (int i = 0; a[i] != '\0'; i++)
		if (a[i] == '.')
			return i;
	return -1;
}

void substr(char a[102], int pos, int n, char t[102])
{
	for (int i = 0; i < n; i++)
		t[i] = a[pos + i];
}

void split(int dot, char a[102], char integ[102], char frac[103])
{
	int len = strlen(a);
	if (dot == -1) // �������С������
	{
		strcpy(integ, a);
		strcpy(frac, "0");
	}
	else
	{
		substr(a, 0, dot, integ); // ��ȡ��������
		substr(a, dot + 1, len - dot - 1, frac); // ��ȡС������
	}
}

bool FA(char l[102], char s[102], char frac[103])
{
	bool co = 0; // ��λ��־
	int lenl = strlen(l);
	int lens = strlen(s);
	for (int i = lenl - 1; i > lens - 1; i--) // ���Ĳ���ֱ�Ӹ���
		frac[i] = l[i];
	for (int i = lens - 1; i >= 0; i--) // ʣ�ಿ��
	{
		int ans = (co == 1) ? (l[i] + s[i] + 1 - '0' - '0') : (l[i] + s[i] - '0' - '0');
		co = (ans / 10 == 0) ? 0 : 1; // �����Ƿ��λ
		frac[i] = ans % 10 + '0'; // ��λ��
	}
	return co;
}

void IA(char l[102], char s[102], char intc[103], bool co)
{
	int lenl = strlen(l);
	int lens = strlen(s);
	for (int i = lenl - 1; i > lenl - lens - 1; i--) // �����غϲ���
	{
		int ans = (co == 1) ? (l[i] + s[i - lenl + lens] + 1 - '0' - '0') : (l[i] + s[i - lenl + lens] - '0' - '0');
		co = (ans / 10 == 0) ? 0 : 1; // �����Ƿ��λ
		intc[lenl - 1 - i] = ans % 10 + '0';
	}
	for (int i = lenl - lens - 1; i >= 0; i--) // ������ಿ��
	{
		if (co == 0)
			intc[lenl - 1 - i] = l[i];
		else
		{
			int ans = l[i] - '0' + 1;
			co = (ans / 10 == 0) ? 0 : 1; // �����Ƿ��λ
			intc[lenl - 1 - i] = ans % 10 + '0';
		}
	}
	intc[lenl] = (co == 1) ? '1' : '\0';
}

bool FracAdd(char fraca[102], char fracb[102], char frac[103])
{
	bool co = (strlen(fraca) > strlen(fracb)) ? FA(fraca, fracb, frac) : FA(fracb, fraca, frac);
	return co;
}

void IntAdd(char inta[102], char intb[102], char intc[103], bool co)
{
	(strlen(inta) > strlen(intb)) ? IA(inta, intb, intc, co) : IA(intb, inta, intc, co);
}

void Glue(char intc[103], char frac[103])
{
	int intlen = strlen(intc);
	int fraclen = strlen(frac);
	for (int i = 0; i < intlen; i++)
		cout << intc[intlen - 1 - i];
	for (int i = fraclen - 1; i >= 0; i--) // ȥ0
	{
		if (frac[i] == '0')
			frac[i] = '\0';
		else
			break;
	}
	fraclen = strlen(frac);
	if (fraclen != 0)
	{
		cout << '.';
		for (int i = 0; i < fraclen; i++)
			cout << frac[i];
	}
	cout << endl;
}

void add(char a[102], char b[102])
{
	int dota = FindDot(a); // �ҵ�a��С����λ��
	int dotb = FindDot(b); // �ҵ�b��С����λ��
	char inta[102] = { 0 }; // ��¼a����������
	char intb[102] = { 0 }; // ͬ��
	char intc[103] = { 0 }; // ��¼c������
	char fraca[102] = { 0 }; // ��¼a��С������
	char fracb[102] = { 0 }; // ͬ��
	char frac[102] = { 0 }; // ��¼c��С��
	split(dota, a, inta, fraca); // �ָ�
	split(dotb, b, intb, fracb); // �ָ�
	bool co = FracAdd(fraca, fracb, frac); // ����С������, �ж��Ƿ��λ
	IntAdd(inta, intb, intc, co); // ������������
	Glue(intc, frac); // ƴ��
}

int main()
{
	int n;
	cin >> n;
	for (int ini = 0; ini < n; ini++)
	{
		char a[102] = { 0 };
		char b[102] = { 0 };
		cin >> a >> b;
		add(a, b);
	}
}