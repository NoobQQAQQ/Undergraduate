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
	for (int i = 0; i < vowel; i++) // 遍历每个元音字母
		des[pos[(i + m) % vowel]] = str[pos[i]];
}

int main()
{
	int n;
	cin >> n;
	for (int ini = 0; ini < n; ini++)
	{
		char str[101]; // 存字符串
		int pos[101] = { 0 }; // 存元音字母的位置
		int vowel = 0; // 记录元音个数
		str[0] = '\0'; // 初始化
		int m = 0; // 初始化右移位数
		cin >> str; // 输入待处理字符串
		cin >> m; // 输入右移位数
		int len = strlen(str); // 得到字符串长度
		for (int i = 0; i < len; i++) // 统计元音个数和位置
			if (isVow(str[i]))
				pos[vowel++] = i;
		char des[101]; // 存结果
		RightShift(str, m, pos, vowel, des); // 右移函数
		cout << des << endl; //输出
	}
}