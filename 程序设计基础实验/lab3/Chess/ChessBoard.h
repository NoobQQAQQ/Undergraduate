#pragma once
#include<iostream>
#include<fstream>
#include<cstring>
#include<string>
#include<climits>
#include<ctime>
using namespace std;
const string folder = "./Records/";


class ChessBoard
{
public:
	ChessBoard();
	~ChessBoard();
	void Init(int ownColor);
	void Update(int row, int col, int color);
	void save(int row, int col, int color) const;
	void saveHead(int ownColor, int round);
	void saveTail(int myInvalidStep, int oppInvalidStep) const;
	void step(int &r, int &c) const;
	double Eval() const;
	void Copy(ChessBoard &chessboard) const;



	pair<int, int> LegalPos[30];
	int LegalPosNum;
	bool MyColor;
private:
	char board[8][8];
	int Bchessmen;
	int Wchessmen;
	string FileName;
	pair<int, int> banPos[4];
	int banPosNum;
	int steps;
	pair<double, double> EvalPos() const;
	void CallAI(int &r, int &c) const;

	void ReLeftUp(int row, int col, int color); // ����
	void ReLeftDown(int row, int col, int color); // ����
	void ReRightUp(int row, int col, int color); // ����
	void ReRightDown(int row, int col, int color); // ����
	void ReLeft(int row, int col, int color); // ��
	void ReRight(int row, int col, int color); // ��
	void ReUp(int row, int col, int color); // ��
	void ReDown(int row, int col, int color); // ��
	bool CheckUp(int row, int col, int color) const; 
	bool CheckDown(int row, int col, int color) const;
	bool CheckLeft(int row, int col, int color) const;
	bool CheckRight(int row, int col, int color) const;
	bool CheckLeftUp(int row, int col, int color) const;
	bool CheckLeftDown(int row, int col, int color) const;
	bool CheckRightUp(int row, int col, int color) const;
	bool CheckRightDown(int row, int col, int color) const;
	bool isStable(int row, int col) const;
	void SetNewBan(int row, int col); // �����½�ֹ��
	void SetNewLegal(int color); // �����¿�����
	bool checkRevesi(int row, int col, int color) const; // �����(row, col)��һ����ɫΪcolor�������ܲ��ܷ�ת
};