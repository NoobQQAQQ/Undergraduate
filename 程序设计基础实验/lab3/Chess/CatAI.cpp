#include"ChessBoard.h"
#include"CatAI.h"
int MAXDEPTH;
extern clock_t starttime;

double AISearch(ChessBoard &tmpboard, int &r, int &c, bool color, int depth = MAXDEPTH, double alpha = ALPHA, double beta = BETA)
{
	if (!depth || !tmpboard.LegalPos || clock() - starttime > 8 * CLOCKS_PER_SEC)
		return tmpboard.Eval();

	double Eval;

	ChessBoard t;
	tmpboard.Copy(t);
	t.Update(t.LegalPos[0].first, t.LegalPos[0].second, color);
	

	Eval = -AISearch(t, r, c, !color, depth - 1, -beta, -alpha);
	if (Eval >= beta)
	{
		if (depth == MAXDEPTH && color == tmpboard.MyColor)
		{
			r = tmpboard.LegalPos[0].first;
			c = tmpboard.LegalPos[0].second;
		}
		return beta;
	}
	if (Eval > alpha)
	{
		alpha = Eval;
		if (depth == MAXDEPTH && color == tmpboard.MyColor)
		{
			r = tmpboard.LegalPos[0].first;
			c = tmpboard.LegalPos[0].second;
		}
	}

	for (size_t i = 1; i < tmpboard.LegalPosNum; i++)
	{
		ChessBoard t;
		tmpboard.Copy(t);
		t.Update(t.LegalPos[i].first, t.LegalPos[i].second, color);

		Eval = -AISearch(t, r, c, !color, depth - 1, -alpha - 1, -alpha);

		if (Eval > alpha&&Eval < beta)
			Eval = -AISearch(t, r, c, !color, depth - 1, -beta, -alpha - 1);

		if (Eval >= beta)
		{
			if (depth == MAXDEPTH && color == tmpboard.MyColor)
			{
				r = tmpboard.LegalPos[i].first;
				c = tmpboard.LegalPos[i].second;
			}
			return beta;
		}
		if (Eval > alpha)
		{
			alpha = Eval;
			if (depth == MAXDEPTH && color == tmpboard.MyColor)
			{
				r = tmpboard.LegalPos[i].first;
				c = tmpboard.LegalPos[i].second;
			}
		}
	}
	return alpha;
}


