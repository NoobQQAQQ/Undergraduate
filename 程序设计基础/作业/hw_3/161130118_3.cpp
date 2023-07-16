#include<iostream>
#include<cstdio>
#include<cmath>
using namespace std;

double angle(int h, int m, int s, int t)
{
	int deltah = (t / 3600);
	int deltam = (t - deltah * 3600) / 60;
	int deltas = (t - deltah * 3600 - deltam * 60);
	int newh, newm, news;
	news = (s + deltas) % 60;
	newm = (s + deltas >= 60) ? ((m + deltam + 1) % 60) : ((m + deltam) % 60);
	newh = ((m + deltam >= 60) || ((m + deltam == 59) && (s + deltas >= 60))) ? ((h + deltah + 1) % 24) : ((h + deltah) % 24);
	double mm = newm + news / 60.0;
	double hh = (newh % 12) + mm / 60.0;
	double angle = fabs(hh * 30 - mm * 6);
	if (angle > 180)
		angle = 360 - angle;
	return angle;
}

int main()
{
	int T, h, m, s, t;
	cin >> T;
	for (int i = 0; i < T; i++)
	{
		cin >> h >> m >> s >> t;
		printf("%.4lf\n", angle(h, m, s, t));
	}
}
