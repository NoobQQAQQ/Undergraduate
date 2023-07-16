#define _CRT_SECURE_NO_WARNINGS

#include<iostream>
#include<cstring>
using namespace std;

void input(char A[101][101], int &count) // 输入字符串, 以XXXXXX结束
{
	while (cin >> A[count]) 
	{
		if (strcmp(A[count], "XXXXXX") != 0)
			count++;
		else
			break;
	}
}

void CalFre(int alphabet[26], char *word) // 统计字符串中各字母出现频率
{
	int len = strlen(word);
	for (int i = 0; i < len; i++)
		alphabet[word[i] - 'a']++;
}

bool Eq(int a[26], int b[26]) // 判断字母频率是否相等
{
	for (int i = 0; i < 26; i++)
		if (a[i] != b[i])
			return false;
	return true;
}

void DicOrderAndOutput(char A[101][101], int nums) // 字典序排序并输入结果
{
	if (nums == 0)
		cout << "NOT A VALID WORD" << endl;
	else if (nums == 1)
		cout << A[0] << endl;
	else
	{
		char temp[101]; // 临时变量存字符串
		memset(temp, '\0', sizeof(temp)); // 初始化
		for (int i = 0; i < nums; i++)
			for (int j = i + 1; j < nums; j++)
				if (strcmp(A[i], A[j]) > 0)
				{
					strcpy(temp, A[i]);
					strcpy(A[i], A[j]);
					strcpy(A[j], temp);
				}
		for (int i = 0; i < nums; i++) // 输出
			cout << A[i] << endl;
	}
}

int main()
{
	char dic[101][101]; // 字典	
	char rid[101][101]; // 谜语
	int Dcount = 0; // 字典计数
	int Rcount = 0; // 谜语计数
	input(dic, Dcount); // 输入字典
	input(rid, Rcount); // 输入字典
	for (int i = 0; i < Rcount; i++) // 遍历谜语
	{
		int alphabet1[26] = { 0 }; // 字母表1记录谜语
		CalFre(alphabet1, rid[i]); // 统计字母频率
		char Result[101][101]; // 记录谜底
		memset(Result, '\0', sizeof(Result)); // 初始化谜底集
		int ResultNum = 0; // 记录谜底个数
		for (int j = 0; j < Dcount; j++) // 遍历字典寻找谜底
		{
			int alphabet2[26] = { 0 }; // 字母表2记录字典
			CalFre(alphabet2, dic[j]);
			if (Eq(alphabet1, alphabet2))
				strcpy(Result[ResultNum++], dic[j]); // 记录谜底
		}
		DicOrderAndOutput(Result, ResultNum); // 字典序排序并输出
		cout << "******" << endl;
	}
}