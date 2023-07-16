#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//�ַ�������
#define MAX_LEN 101 
// �����С 
#define SIZE 201

bool mystrcmp(char *a, char *b)
{
	int lena = strlen(a);
	int lenb = strlen(b);
	if (lena > lenb)
		return 1;
	else if (lena == lenb && strcmp(a, b) > 0)
		return 1;
	return 0;
}

void sort(char (*strings)[MAX_LEN], int n)
{
	// NOTE: ����Ҫ������ʵ�ִ��� 
	char temp[MAX_LEN] = { 0 }; // ������ʱ����
	for (int i = 0; i < n; i++)
		for (int j = i + 1; j < n; j++)
			if (mystrcmp(strings[i], strings[j]))
			{
				strcpy(temp, strings[i]);
				strcpy(strings[i], strings[j]);
				strcpy(strings[j], temp);
			}
}


int main()
{
	//NOTE:���Լ�����ʹ��
	char str[SIZE][MAX_LEN];
	int n;
	scanf("%d", &n);
	for (int i = 0; i < n; i++)
	{
		int m;
		scanf("%d", &m);
		for (int j = 0; j < m; j++)
		{
			scanf("%s", str[j]);
		}
		sort(str, m);
		puts("*************");
		for (int j = 0; j < m; j++)
		{
			printf("%s\n", str[j]);
		}
		puts("*************");
	}
	return 0;	
}


// NOTE: ����ע��Ƭ���ǲ��Դ��룬�����޸ģ����޸ĺ����̲��Գ��ִ��� ����Ը� 
/*

int main()
{
	char str[SIZE][MAX_LEN];
	int n;
	scanf("%d",&n);
	for(int i = 0 ; i < n ; i++)
	{
		int m;
		scanf("%d",&m);
		for(int j = 0 ; j < m ; j++)
		{
			scanf("%s", str[j]);
		}
		sort(str, m);
		puts("*************");
		for(int j = 0 ; j < m ; j++)
		{
			printf("%s\n",str[j]);
		}
		puts("*************");
	}
	return 0;	
}

*/

