#include"ChessBoard.h"
#include"CatAI.h"

extern int MAXDEPTH;
bool inRange(int i, int j)
{
	if (i < 8 && i >= 0 && j < 8 && j >= 0)
		return 1;
	return 0;
}

double AISearch(ChessBoard &tmpboard, int &r, int &c, bool color, int depth = MAXDEPTH, double alpha = ALPHA, double beta = BETA);

void ChessBoard::ReLeftUp(int row, int col, int color) // 左上
{
	if (color == 0) // 黑棋
	{ 
		int i, j;
		for (i = row - 1, j = col - 1; ((i >= 0) && (j >= 0)); i--, j--)
		{
			if (board[i][j] == 'O')
				return;
			if (board[i][j] == 'B')
				break;
		}
		if ((i >= 0) && (j >= 0)) // 未超出范围, 进行翻转
		{
			for (int p = row - 1, q = col - 1; ((p > i) && (q > j)); p--, q--)
			{
				board[p][q] = 'B';
				Wchessmen--; Bchessmen++;
			}

		}
	}
	else // 白棋
	{
		int i, j;
		for (i = row - 1, j = col - 1; ((i >= 0) && (j >= 0)); i--, j--)
		{
			if (board[i][j] == 'O')
				return;
			if (board[i][j] == 'W')
				break;
		}
		if ((i >= 0) && (j >= 0)) // 未超出范围, 进行翻转
		{
			for (int p = row - 1, q = col - 1; ((p > i) && (q > j)); p--, q--)
			{
				board[p][q] = 'W';
				Wchessmen++; Bchessmen--;
			}
		}
	}
}

void ChessBoard::ReLeftDown(int row, int col, int color) // 左下
{
	if (color == 0) // 黑棋
	{
		int i, j;
		for (i = row + 1, j = col - 1; ((i < 8) && (j >= 0)); i++, j--)
		{
			if (board[i][j] == 'O')
				return;
			if (board[i][j] == 'B')
				break;
		}
		if ((i < 8) && (j >= 0)) // 未超出范围, 进行翻转
		{
			for (int p = row + 1, q = col - 1; ((p < i) && (q > j)); p++, q--)
			{
				board[p][q] = 'B';
				Wchessmen--; Bchessmen++;
			}
		}
	}
	else // 白棋
	{
		int i, j;
		for (i = row + 1, j = col - 1; ((i < 8) && (j >= 0)); i++, j--)
		{
			if (board[i][j] == 'O')
				return;
			if (board[i][j] == 'W')
				break;
		}
		if ((i < 8) && (j >= 0)) // 未超出范围, 进行翻转
		{
			for (int p = row + 1, q = col - 1; ((p < i) && (q > j)); p++, q--)
			{
				board[p][q] = 'W';
				Wchessmen++; Bchessmen--;
			}
		}
	}
}

void ChessBoard::ReRightUp(int row, int col, int color) // 右上
{
	if (color == 0) // 黑棋
	{
		int i, j;
		for (i = row - 1, j = col + 1; ((i >= 0) && (j < 8)); i--, j++)
		{
			if (board[i][j] == 'O')
				return;
			if (board[i][j] == 'B')
				break;
		}
		if ((i >= 0) && (j < 8)) // 未超出范围, 进行翻转
		{
			for (int p = row - 1, q = col + 1; ((p > i) && (q < j)); p--, q++)
			{
				board[p][q] = 'B';
				Wchessmen--; Bchessmen++;
			}
		}
	}
	else // 白棋
	{
		int i, j;
		for (i = row - 1, j = col + 1; ((i >= 0) && (j < 8)); i--, j++)
		{
			if (board[i][j] == 'O')
				return;
			if (board[i][j] == 'W')
				break;
		}
		if ((i >= 0) && (j < 8)) // 未超出范围, 进行翻转
		{
			for (int p = row - 1, q = col + 1; ((p > i) && (q < j)); p--, q++)
			{
				board[p][q] = 'W';
				Wchessmen++; Bchessmen--;
			}
		}
	}
}

void ChessBoard::ReRightDown(int row, int col, int color) // 右下
{
	if (color == 0) // 黑棋
	{
		int i, j;
		for (i = row + 1, j = col + 1; ((i < 8) && (j < 8)); i++, j++)
		{
			if (board[i][j] == 'O')
				return;
			if (board[i][j] == 'B')
				break;
		}
		if ((i < 8) && (j < 8)) // 未超出范围, 进行翻转
		{
			for (int p = row + 1, q = col + 1; ((p < i) && (q < j)); p++, q++)
			{
				board[p][q] = 'B';
				Wchessmen--; Bchessmen++;
			}
		}
	}
	else // 白棋
	{
		int i, j;
		for (i = row + 1, j = col + 1; ((i < 8) && (j < 8)); i++, j++)
		{
			if (board[i][j] == 'O')
				return;
			if (board[i][j] == 'W')
				break;
		}
		if ((i < 8) && (j < 8)) // 未超出范围, 进行翻转
		{
			for (int p = row + 1, q = col + 1; ((p < i) && (q < j)); p++, q++)
			{
				board[p][q] = 'W';
				Wchessmen++; Bchessmen--;
			}
		}
	}
}

void ChessBoard::ReLeft(int row, int col, int color) // 左
{
	if (color == 0) // 黑棋
	{
		int i;
		for (i = col - 1; i >= 0; i--)
		{
			if (board[row][i] == 'O')
				return;
			if (board[row][i] == 'B')
				break;
		}
		if (i >= 0) // 未超出范围, 进行翻转
		{
			for (int j = col - 1; j > i; j--)
			{
				board[row][j] = 'B';
				Wchessmen--; Bchessmen++;
			}
		}
	}
	else // 白棋
	{
		int i;
		for (i = col - 1; i >= 0; i--)
		{
			if (board[row][i] == 'O')
				return;
			if (board[row][i] == 'W')
				break;
		}
		if (i >= 0) // 未超出范围, 进行翻转
		{
			for (int j = col - 1; j > i; j--)
			{
				board[row][j] = 'W';
				Wchessmen++; Bchessmen--;
			}
		}
	}
}

void ChessBoard::ReRight(int row, int col, int color) // 右
{
	if (color == 0) // 黑棋
	{
		int i;
		for (i = col + 1; i < 8; i++)
		{
			if (board[row][i] == 'O')
				return;
			if (board[row][i] == 'B')
				break;
		}
		if (i < 8) // 未超出范围, 进行翻转
		{
			for (int j = col + 1; j < i; j++)
			{
				board[row][j] = 'B';
				Wchessmen--; Bchessmen++;
			}
		}
	}
	else // 白棋
	{
		int i;
		for (i = col + 1; i < 8; i++)
		{
			if (board[row][i] == 'O')
				return;
			if (board[row][i] == 'W')
				break;
		}
		if (i < 8) // 未超出范围, 进行翻转
		{
			for (int j = col + 1; j < i; j++)
			{
				board[row][j] = 'W';
				Wchessmen++; Bchessmen--;
			}
		}
	}
}

void ChessBoard::ReUp(int row, int col, int color) // 上
{
	if (color == 0) // 黑棋
	{
		int i;
		for (i = row - 1; i >= 0; i--)
		{
			if (board[i][col] == 'O')
				return;
			if (board[i][col] == 'B')
				break;
		}
		if (i >= 0) // 未超出范围, 进行翻转
		{
			for (int j = row - 1; j > i; j--)
			{
				board[j][col] = 'B';
				Wchessmen--; Bchessmen++;
			}
		}
	}
	else // 白棋
	{
		int i;
		for (i = row - 1; i >= 0; i--)
		{
			if (board[i][col] == 'O')
				return;
			if (board[i][col] == 'W')
				break;
		}
		if (i >= 0) // 未超出范围, 进行翻转
		{
			for (int j = row - 1; j > i; j--)
			{
				board[j][col] = 'W';
				Wchessmen++; Bchessmen--;
			}
		}
	}
}

void ChessBoard::ReDown(int row, int col, int color) // 下
{
	if (color == 0) // 黑棋
	{
		int i;
		for (i = row + 1; i < 8; i++)
		{
			if (board[i][col] == 'O')
				return;
			if (board[i][col] == 'B')
				break;
		}
		if (i < 8) // 未超出范围, 进行翻转
		{
			for (int j = row + 1; j < i; j++)
			{
				board[j][col] = 'B';
				Wchessmen--; Bchessmen++;
			}
		}
	}
	else // 白棋
	{
		int i;
		for (i = row + 1; i < 8; i++)
		{
			if (board[i][col] == 'O')
				return;
			if (board[i][col] == 'W')
				break;
		}
		if (i < 8) // 未超出范围, 进行翻转
		{
			for (int j = row + 1; j < i; j++)
			{
				board[j][col] = 'W';
				Wchessmen++; Bchessmen--;
			}
		}
	}
}


bool ChessBoard::CheckUp(int row, int col, int color) const
{
	if (color == 0) // 黑棋
	{
		int i;
		for (i = row - 1; i >= 0; i--)
		{
			if (board[i][col] == 'O' || board[i][col] == 'Y' || board[i][col] == 'P')
				return 0;
			if (board[i][col] == 'B')
				break;
		}
		if (i >= 0 && i != row - 1) // 未超出范围, 进行翻转
			return 1;
	}
	else // 白棋
	{
		int i;
		for (i = row - 1; i >= 0; i--)
		{
			if (board[i][col] == 'O' || board[i][col] == 'Y' || board[i][col] == 'P')
				return 0;
			if (board[i][col] == 'W')
				break;
		}
		if (i >= 0 && i != row - 1) // 未超出范围, 进行翻转
			return 1;
	}
	return 0;
}

bool ChessBoard::CheckDown(int row, int col, int color) const
{
	if (color == 0) // 黑棋
	{
		int i;
		for (i = row + 1; i < 8; i++)
		{
			if (board[i][col] == 'O' || board[i][col] == 'Y' || board[i][col] == 'P')
				return 0;
			if (board[i][col] == 'B')
				break;
		}
		if (i < 8 && i != row + 1) // 未超出范围, 进行翻转
			return 1;
	}
	else // 白棋
	{
		int i;
		for (i = row + 1; i < 8; i++)
		{
			if (board[i][col] == 'O' || board[i][col] == 'Y' || board[i][col] == 'P')
				return 0;
			if (board[i][col] == 'W')
				break;
		}
		if (i < 8 && i != row + 1) // 未超出范围, 进行翻转
			return 1;
	}
	return 0;
}

bool ChessBoard::CheckLeft(int row, int col, int color) const
{
	if (color == 0) // 黑棋
	{
		int i;
		for (i = col - 1; i >= 0; i--)
		{
			if (board[row][i] == 'O' || board[row][i] == 'Y' || board[row][i] == 'P')
				return 0;
			if (board[row][i] == 'B')
				break;
		}
		if (i >= 0 && i != col - 1) // 未超出范围, 进行翻转
			return 1;
	}
	else // 白棋
	{
		int i;
		for (i = col - 1; i >= 0; i--)
		{
			if (board[row][i] == 'O' || board[row][i] == 'Y' || board[row][i] == 'P')
				return 0;
			if (board[row][i] == 'W')
				break;
		}
		if (i >= 0 && i != col - 1) // 未超出范围, 进行翻转
			return 1;
	}
	return 0;
}

bool ChessBoard::CheckRight(int row, int col, int color) const
{
	if (color == 0) // 黑棋
	{
		int i;
		for (i = col + 1; i < 8; i++)
		{
			if (board[row][i] == 'O' || board[row][i] == 'Y' || board[row][i] == 'P')
				return 0;
			if (board[row][i] == 'B')
				break;
		}
		if (i < 8 && i != col + 1) // 未超出范围, 进行翻转
			return 1;
	}
	else // 白棋
	{
		int i;
		for (i = col + 1; i < 8; i++)
		{
			if (board[row][i] == 'O' || board[row][i] == 'Y' || board[row][i] == 'P')
				return 0;
			if (board[row][i] == 'W')
				break;
		}
		if (i < 8 && i != col + 1) // 未超出范围, 进行翻转
			return 1;
	}
	return 0;
}

bool ChessBoard::CheckLeftUp(int row, int col, int color) const
{
	if (color == 0) // 黑棋
	{
		int i, j;
		for (i = row - 1, j = col - 1; ((i >= 0) && (j >= 0)); i--, j--)
		{
			if (board[i][j] == 'O' || board[i][j] == 'Y' || board[i][j] == 'P')
				return 0;
			if (board[i][j] == 'B')
				break;
		}
		if ((i >= 0) && (j >= 0) && (i != row -1))// 未超出范围, 进行翻转
			return 1;
	}
	else // 白棋
	{
		int i, j;
		for (i = row - 1, j = col - 1; ((i >= 0) && (j >= 0)); i--, j--)
		{
			if (board[i][j] == 'O' || board[i][j] == 'Y' || board[i][j] == 'P')
				return 0;
			if (board[i][j] == 'W')
				break;
		}
		if ((i >= 0) && (j >= 0) && (i != row - 1)) // 未超出范围, 进行翻转
			return 1;
	}
	return 0;
}

bool ChessBoard::CheckLeftDown(int row, int col, int color) const
{
	if (color == 0) // 黑棋
	{
		int i, j;
		for (i = row + 1, j = col - 1; ((i < 8) && (j >= 0)); i++, j--)
		{
			if (board[i][j] == 'O' || board[i][j] == 'Y' || board[i][j] == 'P')
				return 0;
			if (board[i][j] == 'B')
				break;
		}
		if ((i < 8) && (j >= 0) && (i != row + 1))// 未超出范围, 进行翻转
			return 1;
	}
	else // 白棋
	{
		int i, j;
		for (i = row + 1, j = col - 1; ((i < 8) && (j >= 0)); i++, j--)
		{
			if (board[i][j] == 'O' || board[i][j] == 'Y' || board[i][j] == 'P')
				return 0;
			if (board[i][j] == 'W')
				break;
		}
		if ((i < 8) && (j >= 0) && (i != row + 1)) // 未超出范围, 进行翻转
			return 1;
	}
	return 0;
}

bool ChessBoard::CheckRightUp(int row, int col, int color) const
{
	if (color == 0) // 黑棋
	{
		int i, j;
		for (i = row - 1, j = col + 1; ((i >= 0) && (j < 8)); i--, j++)
		{
			if (board[i][j] == 'O' || board[i][j] == 'Y' || board[i][j] == 'P')
				return 0;
			if (board[i][j] == 'B')
				break;
		}
		if ((i >= 0) && (j < 8) && (i != row - 1)) // 未超出范围, 进行翻转
			return 1;
	}
	else // 白棋
	{
		int i, j;
		for (i = row - 1, j = col + 1; ((i >= 0) && (j < 8)); i--, j++)
		{
			if (board[i][j] == 'O' || board[i][j] == 'Y' || board[i][j] == 'P')
				return 0;
			if (board[i][j] == 'W')
				break;
		}
		if ((i >= 0) && (j < 8) && (i != row - 1)) // 未超出范围, 进行翻转
			return 1;
	}
	return 0;
}

bool ChessBoard::CheckRightDown(int row, int col, int color) const
{
	if (color == 0) // 黑棋
	{
		int i, j;
		for (i = row + 1, j = col + 1; ((i < 8) && (j < 8)); i++, j++)
		{
			if (board[i][j] == 'O' || board[i][j] == 'Y' || board[i][j] == 'P')
				return 0;
			if (board[i][j] == 'B')
				break;
		}
		if ((i < 8) && (j < 8) && (i != row + 1)) // 未超出范围, 进行翻转
			return 1;
	}
	else // 白棋
	{
		int i, j;
		for (i = row + 1, j = col + 1; ((i < 8) && (j < 8)); i++, j++)
		{
			if (board[i][j] == 'O' || board[i][j] == 'Y' || board[i][j] == 'P')
				return 0;
			if (board[i][j] == 'W')
				break;
		}
		if ((i < 8) && (j < 8) && (i != row + 1)) // 未超出范围, 进行翻转
			return 1;
	}
	return 0;
}


bool ChessBoard::checkRevesi(int row, int col, int color) const
{
	if (CheckUp(row, col, color) || CheckDown(row, col, color) || 
		CheckLeft(row, col, color) || CheckRight(row, col, color)||
		CheckLeftUp(row, col, color) || CheckLeftDown(row, col, color) || 
		CheckRightUp(row, col, color) || CheckRightDown(row, col, color))
		return 1;
	return 0;
}

void ChessBoard::SetNewBan(int row, int col) // 设置新禁止域
{
	//上
	if (inRange(row - 1, col) && board[row - 1][col] == 'O')
	{
		board[row - 1][col] = 'P';
		banPos[banPosNum++] = make_pair(row - 1, col);
	}
	// 下
	if (inRange(row + 1, col) && board[row + 1][col] == 'O')
	{
		board[row + 1][col] = 'P';
		banPos[banPosNum++] = make_pair(row + 1, col);
	}
	// 左
	if (inRange(row, col - 1) && board[row][col - 1] == 'O')
	{
		board[row][col - 1] = 'P';
		banPos[banPosNum++] = make_pair(row, col - 1);
	}
	// 右
	if (inRange(row, col + 1) && board[row][col + 1] == 'O')
	{
		board[row][col + 1] = 'P';
		banPos[banPosNum++] = make_pair(row, col + 1);
	}
}

void ChessBoard::SetNewLegal(int color) // 设置新可行域
{
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			if (board[i][j] == 'O' && checkRevesi(i, j, color^1))
			{
				board[i][j] = 'Y';
				LegalPos[LegalPosNum++] = make_pair(i, j);
			}		
	
}






ChessBoard::ChessBoard()
{
	steps = 0;
	banPosNum = LegalPosNum = Bchessmen = Wchessmen = 0;
	memset(board, 'O', sizeof(board));
}

ChessBoard::~ChessBoard() {}

void ChessBoard::Init(int ownColor)
{
	MAXDEPTH = 6;
	MyColor = ownColor;
	FileName.clear();
	steps = 0;
	Bchessmen = Wchessmen = 2;
	banPosNum = 0;
	LegalPosNum = 4;
	LegalPos[0] = make_pair(2, 4);
	LegalPos[1] = make_pair(3, 5);
	LegalPos[2] = make_pair(4, 2);
	LegalPos[3] = make_pair(5, 3);
	memset(board, 'O', sizeof(board));
	board[3][3] = board[4][4] = 'B';
	board[3][4] = board[4][3] = 'W';
	board[2][4] = board[3][5] = board[4][2] = board[5][3] = 'Y';
}

void ChessBoard::Update(int row, int col, int color) // 更新棋盘
{
	steps++; // 步数+1
	// 遍历原可行域换为O
	for (int i = 0; i < LegalPosNum; i++)
		board[LegalPos[i].first][LegalPos[i].second] = 'O';
	LegalPosNum = 0;

	// 遍历原禁止域换为O
	for (int i = 0; i < banPosNum; i++)
		board[banPos[i].first][banPos[i].second] = 'O';
	banPosNum = 0;


	// 落子并判断翻转
	if (row != -1)
	{
		if (color == 0) // 黑棋
		{
			board[row][col] = 'B';
			Bchessmen++;
			ReLeftUp(row, col, color); // 左上
			ReLeftDown(row, col, color); // 左下
			ReRightUp(row, col, color); // 右上
			ReRightDown(row, col, color); // 右下
			ReLeft(row, col, color); // 左
			ReRight(row, col, color); // 右
			ReUp(row, col, color); // 上
			ReDown(row, col, color); // 下
		}
		else // 白棋
		{
			board[row][col] = 'W';
			Wchessmen++;
			ReLeftUp(row, col, color);
			ReLeftDown(row, col, color);
			ReRightUp(row, col, color);
			ReRightDown(row, col, color);
			ReLeft(row, col, color);
			ReRight(row, col, color);
			ReUp(row, col, color);
			ReDown(row, col, color);
		}
		// 设置新禁止域
		SetNewBan(row, col);
	}

	// 设置新可行域
	SetNewLegal(color);
}

void ChessBoard::step(int &r, int &c) const // 产生一步新棋
{
	if (LegalPosNum == 0)
		r = c = -1;
	else
	{
		CallAI(r, c);
	}
}

void ChessBoard::saveHead(int ownColor, int round) 
{
	FileName.append("Round");
	FileName.push_back('0' + round);
	FileName.append(".txt");
	ofstream fout;
	fout.open(folder + FileName, ios::trunc);
	if (ownColor == 0)
	{
		fout << "MY_COLOR: BLACK\n";
		fout << "ENEMY_COLOR: WHITE\n";
	}
	else
	{
		fout << "MY_COLOR: WHITE\n";
		fout << "ENEMY_COLOR: BLACK\n";
	}
	fout << "ROUND START!\n\n";
	fout << "INIT:\n";
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 7; j++)
			fout << board[i][j] << " ";
		fout << board[i][7] << endl;
	}
	fout << "BLACK CHESSMEN: " << Bchessmen << endl;
	fout << "WHITE CHESSMEN: " << Wchessmen << endl;
	fout << endl;
	fout.close();
}

void ChessBoard::save(int row, int col, int color) const
{
	ofstream fout;
	fout.open(folder + FileName, ios::app);
	if (color == 0)
		fout << "STEP " << steps << ": (" << row << ", " << col << ") BY BLACK\n";
	else
		fout << "STEP " << steps << ": (" << row << ", " << col << ") BY WHITE\n";
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 7; j++)
			fout << board[i][j] << " ";
		fout << board[i][7] << endl;
	}
	fout << "BLACK CHESSMEN: " << Bchessmen << endl;
	fout << "WHITE CHESSMEN: " << Wchessmen << endl;
	fout << endl;
	fout.close();
}

void ChessBoard::saveTail(int myInvalidStep, int oppInvalidStep) const
{
	ofstream fout;
	fout.open(folder + FileName, ios::app);
	fout << "ROUND OVER!\n";
	fout << "BLACK CHESSMEN: " << Bchessmen << "\tWHITE CHESSMEN: " << Wchessmen << endl;
	fout << "INVALID_STEPS: (MY : ENEMY = " << myInvalidStep << " : " << oppInvalidStep << ")";
	fout.close();
}



clock_t starttime;
const short ValueTable[8][8] = // 位置评估表
{
	{ 20, -3, 11, 8, 8, 11, -3, 20 },
	{ -3, -7, -4, 1, 1, -4, -7, -3 },
	{ 11, -4, 2, 2, 2, 2, -4, 11 },
	{ 8, 1, 2, -3, -3, 2, 1, 8 },
	{ 8, 1, 2, -3, -3, 2, 1, 8 },
	{ 11, -4, 2, 2, 2, 2, -4, 11 },
	{ -3, -7, -4, 1, 1, -4, -7, -3 },
	{ 20, -3, 11, 8, 8, 11, -3, 20 }
};
const short SideVal[8] = { 1, 1, 1, 2, 3, 4, 6, 7 };
double BW_FAC; // 黑白棋比例参数
double CN_FAC; // 角子参数
double DC_FAC; // 近角位参数
double SD_FAC; // 边界稳定子参数
double ST_FAC; // 内部稳定子的参数
double MV_FAC; // 行动力参数
double POS_FAC; // 位置参数
double FT_FAC; // 前沿点参数
//--------------------------------------------//





void ChessBoard::Copy(ChessBoard &chessboard) const
{
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			chessboard.board[i][j] = board[i][j];
	chessboard.MyColor = MyColor;
	chessboard.Bchessmen = Bchessmen;
	chessboard.Wchessmen = Wchessmen;
	for (int i = 0; i < LegalPosNum; i++)
		chessboard.LegalPos[i] = LegalPos[i];
	chessboard.LegalPosNum = LegalPosNum;
	for (int i = 0; i < banPosNum; i++)
		chessboard.banPos[i] = banPos[i];
	chessboard.banPosNum = banPosNum;
	chessboard.steps = steps;
}

pair<double,double> ChessBoard::EvalPos() const
{
	double black, white;
	black = white = 0;
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			if (board[i][j] == 'B')
				black += ValueTable[i][j];
			if (board[i][j] == 'W')
				white += ValueTable[i][j];
		}
	return make_pair(black, white);
}


void ChessBoard::CallAI(int &r, int &c) const
{
	starttime = clock();
	if (steps > 44)
		MAXDEPTH = 10;
	else if (steps > 38)
		MAXDEPTH = 8;
	if (steps > 46) //根据回合数调整参数
	{
		BW_FAC = 0.20; //终局时调高黑白子比例参数
		DC_FAC = 0.60; //调低危险角子参数
		CN_FAC = 8.00;
		SD_FAC = 6.00;
		ST_FAC = 4.00;
		MV_FAC = 0.24;
		FT_FAC = 0.24;
		POS_FAC = 0.01;
	}
	else if (steps > 42) // 42-46
	{
		BW_FAC = 0.15;
		DC_FAC = 1.20;
		CN_FAC = 8.00;
		SD_FAC = 6.00;
		ST_FAC = 4.00;
		MV_FAC = 0.24; //调低行动力参数
		FT_FAC = 0.24;
		POS_FAC = 0.01; //调低特征估值参数
	}
	else if (steps > 36) // 36-42
	{
		BW_FAC = 0.12;
		DC_FAC = 1.50;
		CN_FAC = 8.00;
		SD_FAC = 5.00;
		ST_FAC = 3.00;
		FT_FAC = 0.66;
		MV_FAC = 0.45;
		POS_FAC = 0.04;
	}
	else if (steps > 28) // 28-36
	{
		BW_FAC = 0.08;
		CN_FAC = 9.50;
		DC_FAC = 4.80;
		SD_FAC = 4.00;
		ST_FAC = 1.00;
		FT_FAC = 0.89;
		MV_FAC = 0.60;
		POS_FAC = 0.09;
	}
	else if (steps > 16)  // 16-28
	{
		BW_FAC = 0.06;
		CN_FAC = 9.50;
		DC_FAC = 4.00;
		SD_FAC = 4.00;
		ST_FAC = 0.00;
		FT_FAC = 1.02 - double(steps) / 100;
		MV_FAC = 0.71 - double(steps) / 120;
		POS_FAC = 0.12;
	}
	else // 0-16
	{
		BW_FAC = 0.04; //开局时不重视黑白比例
		CN_FAC = 7.50;
		DC_FAC = 3.90;
		SD_FAC = 0.00;
		ST_FAC = 0.00; //不计算基于稳定子的估值
		FT_FAC = 1.27 - double(steps) / 100;
		MV_FAC = 0.82 - double(steps) / 120;
		POS_FAC = 0.13; //重视位置特征估值
	}
	ChessBoard temp;
	Copy(temp);
	AISearch(temp, r, c, MyColor);
}




double ChessBoard::Eval() const
{
	//Evaluation Based on Position
	double PosEval;
	pair<double, double> BWpos = EvalPos();

	if (MyColor == 0) // black
		PosEval = BWpos.first - BWpos.second;
	else
		PosEval = BWpos.second - BWpos.first;




	//Evaluation Based on Stone Rate
	double BWrateEval;
	if (Bchessmen == Wchessmen)
		BWrateEval = 0;
	else
	{
		double Brate = Bchessmen / (Bchessmen + Wchessmen);
		double Wrate = Wchessmen / (Bchessmen + Wchessmen);
		if (MyColor == 0)
		{
			if (Bchessmen > Wchessmen)
				BWrateEval = 100.0 * Brate;
			else
				BWrateEval = -100.0 * Wrate;
		}
		else
		{
			if (Bchessmen > Wchessmen)
				BWrateEval = -100.0 * Brate;
			else
				BWrateEval = 100.0 * Wrate;
		}
	}
	



	//Evaluation Based on Corner Stone Count
	double CNEval;
	int bCN, wCN;
	bCN = wCN = 0;
	if (board[0][0] == 'B')
		bCN++;
	if (board[0][0] == 'W')
		wCN++;
	if (board[0][7] == 'B')
		bCN++;
	if (board[0][7] == 'W')
		wCN++;
	if (board[7][0] == 'B')
		bCN++;
	if (board[7][0] == 'W')
		wCN++;
	if (board[7][7] == 'B')
		bCN++;
	if (board[7][7] == 'W')
		wCN++;
	if (MyColor == 0)
		CNEval = 25 * (bCN - wCN);
	else
		CNEval = 25 * (wCN - bCN);




	//Evaluation Based on Dangerous Corner-Beside-Stone Count
	double DCEval;
	int bDC, wDC;
	bDC = wDC = 0;
	if (board[0][1] == 'B')
		bDC++;
	if (board[0][1] == 'W')
		wDC++;
	if (board[1][0] == 'B')
		bDC++;
	if (board[1][0] == 'W')
		wDC++;
	if (board[0][6] == 'B')
		bDC++;
	if (board[0][6] == 'W')
		wDC++;
	if (board[1][7] == 'B')
		bDC++;
	if (board[1][7] == 'W')
		wDC++;
	if (board[6][0] == 'B')
		bDC++;
	if (board[6][0] == 'W')
		wDC++;
	if (board[7][1] == 'B')
		bDC++;
	if (board[7][1] == 'W')
		wDC++;
	if (board[6][7] == 'B')
		bDC++;
	if (board[6][7] == 'W')
		wDC++;
	if (board[7][6] == 'B')
		bDC++;
	if (board[7][6] == 'W')
		wDC++;
	// *位
	if (board[1][1] == 'B')
		bDC+=2;
	if (board[1][1] == 'W')
		wDC+=2;
	if (board[1][6] == 'B')
		bDC+=2;
	if (board[1][6] == 'W')
		wDC+=2;
	if (board[6][1] == 'B')
		bDC += 2;
	if (board[6][1] == 'W')
		wDC += 2;
	if (board[6][6] == 'B')
		bDC += 2;
	if (board[6][6] == 'W')
		wDC += 2;
	if (MyColor == 0)
		DCEval = -12.5 * (bDC - wDC);
	else
		DCEval = -12.5 * (wDC - bDC);


	

	//Evaluation Based on Side Stable Stone
	double SDEval = 0;
	if (SD_FAC)
	{
		int bSD, wSD;
		bSD = wSD = 0;
		if (board[0][0] == 'B')
		{
			for (int i = 0; i < 8; i++)
			{
				if (board[0][i] == 'B')
					bSD += SideVal[i];
				else
					break;
			}
			for (int i = 0; i < 8; i++)
			{
				if (board[i][0] == 'B')
					bSD += SideVal[i];
				else
					break;
			}
		}
		else if (board[0][0] == 'W')
		{
			for (int i = 0; i < 8; i++)
			{
				if (board[0][i] == 'W')
					wSD += SideVal[i];
				else
					break;
			}
			for (int i = 0; i < 8; i++)
			{
				if (board[i][0] == 'W')
					wSD += SideVal[i];
				else
					break;
			}
		}


		if (board[0][7] == 'B')
		{
			for (int i = 0; i < 8; i++)
			{
				if (board[i][7] == 'B')
					bSD += SideVal[i];
				else
					break;
			}
			for (int i = 7; i >= 0; i--)
			{
				if (board[0][i] == 'B')
					bSD += SideVal[7 - i];
				else
					break;
			}
		}
		else if (board[0][7] == 'W')
		{
			for (int i = 0; i < 8; i++)
			{
				if (board[i][7] == 'W')
					wSD += SideVal[i];
				else
					break;
			}
			for (int i = 7; i >= 0; i--)
			{
				if (board[0][i] == 'W')
					wSD += SideVal[7 - i];
				else
					break;
			}
		}


		if (board[7][7] == 'B')
		{
			for (int i = 7; i >= 0; i--)
			{
				if (board[i][7] == 'B')
					bSD += SideVal[7 - i];
				else
					break;
			}
			for (int i = 7; i >= 0; i--)
			{
				if (board[7][i] == 'B')
					bSD += SideVal[7 - i];
				else
					break;
			}
		}
		else if (board[7][7] == 'W')
		{
			for (int i = 7; i >= 0; i--)
			{
				if (board[i][7] == 'W')
					wSD += SideVal[7 - i];
				else
					break;
			}
			for (int i = 7; i >= 0; i--)
			{
				if (board[7][i] == 'W')
					wSD += SideVal[7 - i];
				else
					break;
			}
		}


		if (board[7][0] == 'B')
		{
			for (int i = 0; i < 8; i++)
			{
				if (board[7][i] == 'B')
					bSD += SideVal[i];
				else
					break;
			}
			for (int i = 8; i >= 0; i--)
			{
				if (board[i][0] == 'B')
					bSD += SideVal[7 - i];
				else
					break;
			}
		}
		else if (board[7][0] == 'W')
		{
			for (int i = 0; i < 8; i++)
			{
				if (board[7][i] == 'W')
					wSD += SideVal[i];
				else
					break;
			}
			for (int i = 8; i >= 0; i--)
			{
				if (board[i][0] == 'W')
					wSD += SideVal[7 - i];
				else
					break;
			}
		}

		if (MyColor == 0)
			SDEval = 2.5 * (bSD - wSD);
		else
			SDEval = 2.5 * (wSD - bSD);
	}



	//Evaluation Based on Inside Stable Stone
	double STEval = 0;
	if (ST_FAC)
	{
		int bST, wST;
		bST = wST = 0;
		for (int i = 1; i < 7; i++)
			for (int j = 1; j < 7; j++)
				if (isStable(i, j))
				{
					if (board[i][j] == 'B')
						bST++;
					if (board[i][j] == 'W')
						wST++;
				}
		if (MyColor == 0)
			STEval = 12.5 * (bST - wST);
		else
			STEval = 12.5 * (wST - bST);
	}



	//Evaluation Based on Move ability
	double MVEval = 0;
	int bMV, wMV;
	bMV = wMV = 0;
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			if (board[i][j] == 'O' && checkRevesi(i, j, 0))
			{
				bMV++; continue;
			}
			if (board[i][j] == 'O' && checkRevesi(i, j, 1))
				wMV++;
		}
	if (MyColor == 0)
	{
		if (wMV == 0)
			MVEval = 150;
		else if (bMV == 0)
			MVEval = -450;
		else if (bMV == wMV)
			MVEval = 0;
		else if (bMV > wMV)
			MVEval = (100 * bMV) / (bMV + wMV);
		else if (bMV < wMV)
			MVEval = -(100 * wMV) / (bMV + wMV);
	}
	else
	{
		if (bMV == 0)
			MVEval = 150;
		else if (wMV == 0)
			MVEval = -450;
		else if (bMV == wMV)
			MVEval = 0;
		else if (bMV > wMV)
			MVEval = -(100 * bMV) / (bMV + wMV);
		else if (bMV < wMV)
			MVEval = (100 * wMV) / (bMV + wMV);
	}



	//Evaluation Based on Frontier Stone
	double FTEval;
	int bFT, wFT;
	bFT = wFT = 0;
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			if (board[i][j] == 'B')
			{
				if (inRange(i - 1, j) && board[i - 1][j] == 'O')
					bFT++;
				if (inRange(i - 1, j - 1) && board[i - 1][j - 1] == 'O')
					bFT++;
				if (inRange(i - 1, j + 1) && board[i - 1][j + 1] == 'O')
					bFT++;
				if (inRange(i, j - 1) && board[i][j - 1] == 'O')
					bFT++;
				if (inRange(i, j + 1) && board[i][j + 1] == 'O')
					bFT++;
				if (inRange(i + 1, j) && board[i + 1][j] == 'O')
					bFT++;
				if (inRange(i + 1, j - 1) && board[i + 1][j - 1] == 'O')
					bFT++;
				if (inRange(i + 1, j + 1) && board[i + 1][j + 1] == 'O')
					bFT++;
			}
			else if (board[i][j] == 'W')
			{
				if (inRange(i - 1, j) && board[i - 1][j] == 'O')
					wFT++;
				if (inRange(i - 1, j - 1) && board[i - 1][j - 1] == 'O')
					wFT++;
				if (inRange(i - 1, j + 1) && board[i - 1][j + 1] == 'O')
					wFT++;
				if (inRange(i, j - 1) && board[i][j - 1] == 'O')
					wFT++;
				if (inRange(i, j + 1) && board[i][j + 1] == 'O')
					wFT++;
				if (inRange(i + 1, j) && board[i + 1][j] == 'O')
					wFT++;
				if (inRange(i + 1, j - 1) && board[i + 1][j - 1] == 'O')
					wFT++;
				if (inRange(i + 1, j + 1) && board[i + 1][j + 1] == 'O')
					wFT++;
			}
		}

	if (bFT == wFT)
		FTEval = 0.0;
	else
	{
		if (MyColor == 0)
		{
			if (bFT > wFT)
				FTEval = -100.0 * bFT / (bFT + wFT);
			else
				FTEval = 100.0 * wFT / (bFT + wFT);
		}
		else
		{
			if (bFT > wFT)
				FTEval = 100.0 * bFT / (bFT + wFT);
			else
				FTEval = -100.0 * wFT / (bFT + wFT);
		}
	}









	//Weighed Evaluation
	double Eval =
		(BW_FAC * BWrateEval) +
		(CN_FAC * CNEval) +
		(DC_FAC * DCEval) +
		(SD_FAC * SDEval) +
		(ST_FAC * STEval) +
		(MV_FAC * MVEval) +
		(POS_FAC * PosEval) +
		(FT_FAC * FTEval);

	return Eval;

}


bool ChessBoard::isStable(int row, int col) const
{
	if (board[row - 1][col] != 'B' && board[row - 1][col] != 'W') // up
		return 0;
	if (board[row - 1][col - 1] != 'B' && board[row - 1][col - 1] != 'W') // leftup
		return 0;
	if (board[row][col - 1] != 'B' && board[row][col - 1] != 'W') // left
		return 0;
	if (board[row + 1][col] != 'B' && board[row + 1][col] != 'W') // down
		return 0;
	if (board[row][col + 1] != 'B' && board[row][col + 1] != 'W') // right
		return 0;
	if (board[row - 1][col + 1] != 'B' && board[row - 1][col + 1] != 'W') // rightup
		return 0;
	if (board[row + 1][col - 1] != 'B' && board[row + 1][col - 1] != 'W') // leftdown
		return 0;
	if (board[row + 1][col + 1] != 'B' && board[row + 1][col + 1] != 'W') // rightdown
		return 0;
	return 1;
}

