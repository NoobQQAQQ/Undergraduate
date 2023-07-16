#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//字符串长度
#define MAX_LEN 101 
// 数组大小 
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
	// NOTE: 你需要在下面实现代码 
	char temp[MAX_LEN] = { 0 }; // 建立临时变量
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
	//NOTE:你自己测试使用
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


// NOTE: 以下注释片段是测试代码，请勿修改，若修改后，助教测试出现错误， 后果自负 
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

