#include<iostream>
#include<cmath>

using namespace std;

int main()
{
	int n = 0;
	while (cin >> n)
	{
		bool suc = 0;
		if (n == 0) break;
		else
		{
			int x1, x2, x3, x4;
			x1 = (int)sqrt(n);
			while (x1 >= 0)
			{
				x2 = (int)sqrt(n);
				while (x2 >= 0)
				{
					x3 = (int)sqrt(n);
					while (x3 >= 0)
					{
						x4 = (int)sqrt(n);
						while (x4 >= 0)
						{
							if (n == x4 * x4 + x3 * x3 + x2 * x2 + x1 * x1)
							{
								cout << x4 << '*' << x4 << " + ";
								cout << x3 << '*' << x3 << " + ";
								cout << x2 << '*' << x2 << " + ";
								cout << x1 << '*' << x1 << endl;
								suc = 1;
							}
							if (suc)
								break;
							x4--;
						}
						if (suc)
							break;
						x3--;
					}
					if (suc)
						break;
					x2--;
				}
				if (suc)
					break;
				x1--;
			}
		}
	}
}