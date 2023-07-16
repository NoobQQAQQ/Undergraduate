#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>

void sort(int (&v)[100])
{
	for (int i = 0; i < 100; i++)
		for (int j = i + 1; j < 100; j++)
			if (v[i] > v[j])
			{
				v[i] ^= v[j];
				v[j] ^= v[i];
				v[i] ^= v[j];
			}
}

int main()
{
	FILE *sor = fopen("ex3_data.txt", "r"); // 读取源文件
	FILE *des = fopen("result.txt", "w"); // 新建目标文件
	int v[100] = { 0 }; // 存累加后的数字
	for (int i = 0; i < 100; i++)
	{
		int sum5 = 0; // 存每列5个数的和
		for (int j = 0; j < 5; j++)
		{
			int temp = 0;
			fscanf(sor, "%d", &temp);
			sum5 += temp;
		}
		v[i] = sum5;
	}
	sort(v);
	for (int i = 0; i < 100; i++)
		fprintf(des, "%d\n", v[i]);
	fclose(sor);
	fclose(des);
}