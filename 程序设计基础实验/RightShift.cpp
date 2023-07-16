#define _CRT_SECURE_NO_WARNINGS

#include<iostream>
#include<cstring>

using namespace std;

bool isVow(char ch)
{
	if (ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u' )
		return true;
	return false;
}

void RightShift(char str[101], int m, int pos[101], int vowel, char des[101])
{
	strcpy(des, str);
	for (int i = 0; i < vowel; i++) // ����ÿ��Ԫ����ĸ
		des[pos[(i + m) % vowel]] = str[pos[i]];
}

int main()
{
	int n;
	cin >> n;
	for (int ini = 0; ini < n; ini++)
	{
		char str[101]; // ���ַ���
		int pos[101] = { 0 }; // ��Ԫ����ĸ��λ��
		int vowel = 0; // ��¼Ԫ������
		str[0] = '\0'; // ��ʼ��
		int m = 0; // ��ʼ������λ��
		cin >> str; // ����������ַ���
		cin >> m; // ��������λ��
		int len = strlen(str); // �õ��ַ�������
		for (int i = 0; i < len; i++) // ͳ��Ԫ��������λ��
			if (isVow(str[i]))
				pos[vowel++] = i;
		char des[101]; // ����
		RightShift(str, m, pos, vowel, des); // ���ƺ���
		cout << des << endl; //���
	}
}