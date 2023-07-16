//
//  Reversi.h
//  ReversiClient
//
//  Created by ganjun on 2018/3/6.
//  Copyright © 2018年 ganjun. All rights reserved.
//

#ifndef Reversi_h
#define Reversi_h
#include <stdio.h>
#include "ClientSocket.h"
#include"ChessBoard.h"

using namespace std;

class Reversi
{
private:
	ChessBoard chess_board;
    ClientSocket client_socket;
    int ownColor;
    int oppositeColor;
	int myInvalidStep;
	int oppInvalidStep;

	//function 
	void handleMessage(int row, int col, int color);

	 // according to chessman position (row , col) , generate one step message in order to send to server
    void generateOneStepMessage(int row , int col);
    
    pair<int,int> step() const;
    
    void saveChessBoardHead(int round);

	void saveChessBoardTail() const;

	void initChessBoard();
public:
    Reversi();
    ~Reversi();
    
    void authorize(const char *id , const char *pass);
    
    void gameStart();
    
    void gameOver();
    
    void roundStart(int round);
    
    void oneRound();
    
    void roundOver(int round);
    
    int observe();
    
};

#endif /* Reversi_h */
