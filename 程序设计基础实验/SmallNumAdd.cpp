#define _CRT_SECURE_NO_WARNINGS

#include<iostream>
#include<cstring>

using namespace std;

int FindDot(char a[102])  // 找到小数点位置, 若无小数点, 返回-1
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
	if (dot == -1) // 检测有无小数部分
	{
		strcpy(integ, a);
		strcpy(frac, "0");
	}
	else
	{
		substr(a, 0, dot, integ); // 提取整数部分
		substr(a, dot + 1, len - dot - 1, frac); // 提取小数部分
	}
}

bool FA(char l[102], char s[102], char frac[103])
{
	bool co = 0; // 进位标志
	int lenl = strlen(l);
	int lens = strlen(s);
	for (int i = lenl - 1; i > lens - 1; i--) // 长的部分直接复制
		frac[i] = l[i];
	for (int i = lens - 1; i >= 0; i--) // 剩余部分
	{
		int ans = (co == 1) ? (l[i] + s[i] + 1 - '0' - '0') : (l[i] + s[i] - '0' - '0');
		co = (ans / 10 == 0) ? 0 : 1; // 计算是否进位
		frac[i] = ans % 10 + '0'; // 本位和
	}
	return co;
}

void IA(char l[102], char s[102], char intc[103], bool co)
{
	int lenl = strlen(l);
	int lens = strlen(s);
	for (int i = lenl - 1; i > lenl - lens - 1; i--) // 处理重合部分
	{
		int ans = (co == 1) ? (l[i] + s[i - lenl + lens] + 1 - '0' - '0') : (l[i] + s[i - lenl + lens] - '0' - '0');
		co = (ans / 10 == 0) ? 0 : 1; // 计算是否进位
		intc[lenl - 1 - i] = ans % 10 + '0';
	}
	for (int i = lenl - lens - 1; i >= 0; i--) // 处理多余部分
	{
		if (co == 0)
			intc[lenl - 1 - i] = l[i];
		else
		{
			int ans = l[i] - '0' + 1;
			co = (ans / 10 == 0) ? 0 : 1; // 计算是否进位
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
	for (int i = fraclen - 1; i >= 0; i--) // 去0
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
	int dota = FindDot(a); // 找到a的小数点位置
	int dotb = FindDot(b); // 找到b的小数点位置
	char inta[102] = { 0 }; // 记录a的整数部分
	char intb[102] = { 0 }; // 同上
	char intc[103] = { 0 }; // 记录c的整数
	char fraca[102] = { 0 }; // 记录a的小数部分
	char fracb[102] = { 0 }; // 同上
	char frac[102] = { 0 }; // 记录c的小数
	split(dota, a, inta, fraca); // 分割
	split(dotb, b, intb, fracb); // 分割
	bool co = FracAdd(fraca, fracb, frac); // 计算小数部分, 判断是否进位
	IntAdd(inta, intb, intc, co); // 计算整数部分
	Glue(intc, frac); // 拼接
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