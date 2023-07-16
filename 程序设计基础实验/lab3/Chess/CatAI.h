#pragma once
#include<climits>
#include<ctime>
using namespace std;

//-------------------AI Part---------------------//

const double ALPHA = INT_MIN;
const double BETA = INT_MAX;

double AISearch(ChessBoard &tmpboard, int &r, int &c, bool color, int depth, double alpha, double beta);



