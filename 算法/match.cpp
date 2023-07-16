#include<iostream>
#include<fstream>
#include<algorithm>
#include<ctime>
using namespace std;

bool find_rec(int i, bool *visit, int *m, int sup, int k, int **BiAdj)
{
	for (int j = 0; j < k; j++)
		if (BiAdj[i][j] <= sup && visit[j] == 0)
		{
			visit[j] = 1;
			if (m[j] == -1 || find_rec(m[j], visit, m, sup, k, BiAdj))
			{
				m[j] = i;
				return 1;
			}
		}
	return 0;
}

int find_max_match(int **BiAdj, int k, int sup)
{
	// the number of edges of the current max match
	int match_num = 0;

	// initial match array
	int *m = new int[k];
	for (int i = 0; i < k; i++)
		m[i] = -1;

	// go through every node of left part
	for (int i = 0; i < k; i++)
	{
		// initial visit array
		bool *visit = new bool[k];
		for (int i = 0; i < k; i++)
			visit[i] = 0;

		// find what we need~
		if (find_rec(i, visit, m, sup, k, BiAdj))
			match_num++;
	}

	// return result
	return match_num;
}

int match(int **BiAdj, int k)
{
	// store and sort weights for bisearch
	int *w = new int[k * k];
	for (int i = 0; i < k; i++)
		for (int j = 0; j < k; j++)
			w[i * k + j] = BiAdj[i][j];
	sort(w, w + k * k);

	// initial bisearch
	int s = 0;
	int e = k * k - 1;
	int mid = (s + e) / 2;

	while (s != e)
	{
		// find a max match under the restriction of w[mid]
		// return the edges number of found match
		int match_num = find_max_match(BiAdj, k, w[mid]);

		// update s, e, mid
		if (match_num == k) // get a whole match, try to find a smaller w[mid]
			e = mid;
		else // not a whole match, use a bigger w[mid]
			s = mid + 1;
		mid = (s + e) / 2;
	}

	return w[e];
}

int main()
{
	// start time counter
	clock_t start = clock();

	// open needed files
	ifstream fin;
	ofstream fout;
	fin.open("part.in", ios::in);
	fout.open("ans.out", ios::out | ios::trunc);

	// get needed data from file
	int T;
	fin >> T;
	while (T--)
	{
		int k;
		fin >> k;

		int **BiAdj = new int*[k]; // biadjacency matrix
		for (int i = 0; i < k; i++)
			BiAdj[i] = new int[k];

		for (int i = 0; i < k; i++)
			for (int j = 0; j < k; j++)
				fin >> BiAdj[i][j];

		// find target W
		fout << match(BiAdj, k) << endl;
	}

	// close files
	fin.close();
	fout.close();

	// end time counter, print time cost
	clock_t end = clock();
	cout << end - start << "ms" << endl;
}