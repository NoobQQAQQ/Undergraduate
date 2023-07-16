#define _CRT_SECURE_NO_WARNINGS
#include "Reversi.h"

#define random(x) (rand()%x)
#define ROWS 8
#define COLS 8
#define ROUNDS 3

Reversi::Reversi()
{
    client_socket = ClientSocket();
    oppositeColor = ownColor = -1;
	myInvalidStep = oppInvalidStep = 0;
}

Reversi::~Reversi(){};

/*
 send id and password to server by socket
 rtn != 0 represents socket transfer error
 */
void Reversi::authorize(const char *id , const char *pass)
{
    client_socket.connectServer();
    std::cout << "Authorize " << id << std::endl;
    char msgBuf[BUFSIZE];
    memset(msgBuf , 0 , BUFSIZE);
    msgBuf[0] = 'A';
    memcpy(&msgBuf[1] , id , 9);
    memcpy(&msgBuf[10] , pass , 6);
    int rtn = client_socket.sendMsg(msgBuf);
    if (rtn != 0) printf("Authorized Failed!\n");
}

/* user input id and password that should match id and password in file in ReversiServer "players-X.txt"
 Where X should be the maximum number of the numbers.
*/
void Reversi::gameStart()
{
    char id[12] = {0}, passwd[10] = {0};
    //char id[12] = "111111110", passwd[10] = "123456";
    printf("ID: %s\n" , id);
    scanf("%s" , id);
    printf("PASSWD: %s\n", passwd);
    scanf("%s", passwd);
    
    authorize(id, passwd);
    
    printf("Game Start!\n");
    
    for (int round = 0 ; round < ROUNDS ; round++)
	{
        roundStart(round);
        oneRound();
        roundOver(round);
    }
    gameOver();
    client_socket.close();
}

void Reversi::gameOver()
{
    printf("Game Over!\n");
}

/* send a message, lazi in position (row, col)
 receive 2 message:
 first message is the answer to your step.
 second message is the step of opponent player.
*/
void Reversi::roundStart(int round)
{
    printf("Round %d Ready Start!\n" , round);
    
    // first time receive msg from server
    int rtn = client_socket.recvMsg();
    if (rtn != 0) return;
    if(strlen(client_socket.getRecvMsg()) < 2)
        printf("Authorize Failed!\n");
    else
        printf("Round start received msg %s\n", client_socket.getRecvMsg());
    switch (client_socket.getRecvMsg()[1]) 
	{
    // this client : black chessman
    case 'B':
		ownColor = 0;
		oppositeColor = 1;
		rtn = client_socket.sendMsg("BB");
		printf("Send BB -> rtn: %d\n", rtn);
		if (rtn != 0) return ;
		break;
	case 'W':
		ownColor = 1;
		oppositeColor = 0;
		rtn = client_socket.sendMsg("BW");
		printf("Send BW -> rtn: %d\n", rtn);
		if (rtn != 0) return ;
		break;
	default:
		printf("Authorized Failed!\n");
		break;
    }
	
	//chess board init
	initChessBoard();
	saveChessBoardHead(round);
}

void Reversi::oneRound()
{
    int STEP = 1;
    switch (ownColor)
	{
	case 0:
		while (STEP < 10000) 
		{
			pair<int, int> chess = step();		// take action, send message

			// lazi only excute after server's message confirm  in observe function
			generateOneStepMessage(chess.first, chess.second);


			if (observe() >= 1) break;     // receive RET Code

			if (observe() >= 1) break;    // see white move
			STEP++;                   
		}
		printf("One Round End\n");
		break;
	case 1:
		while (STEP < 10000) 
		{
			if (observe() >= 1) break;    // see black move

			pair<int, int> chess = step();		// take action, send message
			// lazi only excute after server's message confirm  in observe function
			generateOneStepMessage(chess.first, chess.second);


			if (observe() >= 1) break;     // receive RET Code
			STEP++;              
		}
		printf("One Round End\n");
		break;            
	default:
		break;
    }
}

void Reversi::roundOver(int round)
{
    printf("Round %d Over!\n", round);
    // reset initializer
	saveChessBoardTail();
    ownColor = oppositeColor = -1;
	myInvalidStep = oppInvalidStep = 0;
}

int Reversi::observe()
{
    int rtn = 0;
    int recvrtn = client_socket.recvMsg();
    if (recvrtn != 0) return 1;
    printf("receive msg %s\n" , client_socket.getRecvMsg());
    switch (client_socket.getRecvMsg()[0]) 
	{
	case 'R':
	{
		switch (client_socket.getRecvMsg()[1])
		{
		case 'Y':   // valid step
		{
			switch (client_socket.getRecvMsg()[2])
			{
			case 'P':   // update chessboard
			{
				int desRow = (client_socket.getRecvMsg()[3] - '0') * 10 + client_socket.getRecvMsg()[4] - '0';
				int desCol = (client_socket.getRecvMsg()[5] - '0') * 10 + client_socket.getRecvMsg()[6] - '0';
				int color = (client_socket.getRecvMsg()[7] - '0');
				//use handleMessage to handle the position (desRow , desCol)
				handleMessage(desRow, desCol, color);

				printf("a valid step of : (%d %d)\n", desRow, desCol);
				break;
			}
			case 'N':   // R0N: enemy wrong step
			{
				int desRow = -1, desCol = -1;
				int color = (client_socket.getRecvMsg()[3] - '0');
				handleMessage(desRow, desCol, color);

				//
				printf("a true judgement of no step\n");
				break;
			}
			}
			break;
		}
		case 'W':
		{
			// invalid step

			switch (client_socket.getRecvMsg()[2])
			{
			case 'P':
			{
				int desRow = (client_socket.getRecvMsg()[3] - '0') * 10 + client_socket.getRecvMsg()[4] - '0';
				int desCol = (client_socket.getRecvMsg()[5] - '0') * 10 + client_socket.getRecvMsg()[6] - '0';

				int color = (client_socket.getRecvMsg()[7] - '0');
				printf("Invalid step , server random a true step of : (%d %d)\n", desRow, desCol);
				//use handleMessage to handle the position (desRow , desCol)
				(color == ownColor) ? myInvalidStep++ : oppInvalidStep++;
				handleMessage(desRow, desCol, color);
				break;
			}
			case 'N':
			{
				int desRow = -1, desCol = -1;
				int color = (client_socket.getRecvMsg()[3] - '0');
				(color == ownColor) ? myInvalidStep++ : oppInvalidStep++;
				handleMessage(desRow, desCol, color);
				printf("a wrong judgement of no step\n");
				break;
			}
			default:
				break;
			}
			break;
		}
		default:
			printf("Error : Other error!\n");
			rtn = -5;
			break;
		}
		break;
	}
	case 'E':
	{
		switch (client_socket.getRecvMsg()[1])
		{
		case '0':
			// game over
			rtn = 2;
			break;
		case '1':
			// round over
			rtn = 1;
		default:
			break;
		}
		break;
	}
	default:
		break;
    }
    return rtn;
}

void Reversi::generateOneStepMessage(int row, int col)
{
    char msg[BUFSIZE];
    memset(msg , 0 , sizeof(msg));
    
	
    //put row and col in the message
    msg[0] = 'S';
    msg[1] = 'P';
    msg[2] = '0' + row / 10;
    msg[3] = '0' + row % 10;
    msg[4] = '0' + col / 10;
    msg[5] = '0' + col % 10;
    msg[6] = '\0';
	if(row < 0 || col < 0)
	{
		row = -1;
		col = -1;
	}
    if(row == -1 && col == -1)
	{
		msg[2] = '-';
		msg[3] = '1';
		msg[4] = '-';
		msg[5] = '1';
	}
    //print
    printf("generate one step at possition (%2d,%2d) : %s\n", row , col , msg);
    
    
    client_socket.sendMsg(msg);
}

/*-------------------------last three function--------------------------------
 * step : find a good position to lazi your chess.
 * saveChessBoard : save the chess board now.
 * handleMessage: handle the message from server.
 */

pair<int,int> Reversi::step() const
{
    //algorithm
    int r = -1;
    int c = -1;
	chess_board.step(r, c);
    return make_pair(r,c);
}

void Reversi::saveChessBoardHead(int round)
{
	chess_board.saveHead(ownColor, round);
}

void Reversi::saveChessBoardTail() const
{
	chess_board.saveTail(myInvalidStep, oppInvalidStep);
}

void Reversi::handleMessage(int row, int col, int color)
{
	chess_board.Update(row, col, color);
	chess_board.save(row, col, color);
}

//init your chess board here
void Reversi::initChessBoard()
{
	chess_board.Init(ownColor);
}