#define _CRT_SECURE_NO_WARNINGS

#include<iostream>
#include<cstring>
using namespace std;

void input(char A[101][101], int &count) // �����ַ���, ��XXXXXX����
{
	while (cin >> A[count]) 
	{
		if (strcmp(A[count], "XXXXXX") != 0)
			count++;
		else
			break;
	}
}

void CalFre(int alphabet[26], char *word) // ͳ���ַ����и���ĸ����Ƶ��
{
	int len = strlen(word);
	for (int i = 0; i < len; i++)
		alphabet[word[i] - 'a']++;
}

bool Eq(int a[26], int b[26]) // �ж���ĸƵ���Ƿ����
{
	for (int i = 0; i < 26; i++)
		if (a[i] != b[i])
			return false;
	return true;
}

void DicOrderAndOutput(char A[101][101], int nums) // �ֵ�������������
{
	if (nums == 0)
		cout << "NOT A VALID WORD" << endl;
	else if (nums == 1)
		cout << A[0] << endl;
	else
	{
		char temp[101]; // ��ʱ�������ַ���
		memset(temp, '\0', sizeof(temp)); // ��ʼ��
		for (int i = 0; i < nums; i++)
			for (int j = i + 1; j < nums; j++)
				if (strcmp(A[i], A[j]) > 0)
				{
					strcpy(temp, A[i]);
					strcpy(A[i], A[j]);
					strcpy(A[j], temp);
				}
		for (int i = 0; i < nums; i++) // ���
			cout << A[i] << endl;
	}
}

int main()
{
	char dic[101][101]; // �ֵ�	
	char rid[101][101]; // ����
	int Dcount = 0; // �ֵ����
	int Rcount = 0; // �������
	input(dic, Dcount); // �����ֵ�
	input(rid, Rcount); // �����ֵ�
	for (int i = 0; i < Rcount; i++) // ��������
	{
		int alphabet1[26] = { 0 }; // ��ĸ��1��¼����
		CalFre(alphabet1, rid[i]); // ͳ����ĸƵ��
		char Result[101][101]; // ��¼�յ�
		memset(Result, '\0', sizeof(Result)); // ��ʼ���յ׼�
		int ResultNum = 0; // ��¼�յ׸���
		for (int j = 0; j < Dcount; j++) // �����ֵ�Ѱ���յ�
		{
			int alphabet2[26] = { 0 }; // ��ĸ��2��¼�ֵ�
			CalFre(alphabet2, dic[j]);
			if (Eq(alphabet1, alphabet2))
				strcpy(Result[ResultNum++], dic[j]); // ��¼�յ�
		}
		DicOrderAndOutput(Result, ResultNum); // �ֵ����������
		cout << "******" << endl;
	}
}