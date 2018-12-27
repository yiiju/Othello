#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

using namespace std;

struct Position {
    Position(int, int);
    int x;
    int y;
};

Position::Position(int cx, int cy) {
    x = cx;
    y = cy;
}

void InitBoard();
void PrintBoard();
void UpdateBoard(int, int, int);
bool BoardisFull();
int BoardisOneColor();
int isWhat(int, int);
void SavecanGo(int);
bool canGo(int);
bool put(int, int, int, bool);
Position Go();
int CheckWin();

int ChessBoard[8][8];
vector<Position> possiblePosition;

// U,UR,R,DR,D,DL,L,UL
int DIRECTION_X[8] = {0,1,1,1,0,-1,-1,-1};
int DIRECTION_Y[8] = {-1,-1,0,1,1,1,0,-1};

/*
 * Initial the ChessBoard
 */
void InitBoard()
{
    int i,j;
    for(i = 0; i < 8; ++i) {
        for(j = 0; j < 8; ++j) {
            ChessBoard[i][j] = 0;
        }
    }
    ChessBoard[4][3] = 1; ChessBoard[3][4] = 1;
    ChessBoard[3][3] = 2; ChessBoard[4][4] = 2;
}

/*
 * Print the ChessBoard
 */
void PrintBoard()
{
    int i,j;
    for(i = 0; i < 8; i++) {
        for(j = 0; j < 8; j++) {
            printf("%d ",ChessBoard[i][j]);
        }
        printf("\n");
    }
}

/*
 * Update the ChessBoard
 */
void UpdateBoard(int px, int py, int color)
{
    ChessBoard[px][py] = color;
    put(px, py, color, false);
}

/*
 * Check chessboard is full
 */
bool BoardisFull()
{
    bool noEmpty;
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            if(ChessBoard[i][j] == 0) {
                return false;
            }
            noEmpty = true;
        }
    }
    return true;
}

/*
 * Check chessboard is all one color
 */
int BoardisOneColor()
{
    bool allOne = 0;
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            if(ChessBoard[i][j] == 1) {
                if(allOne == 2) return 0;
                allOne = 1;
            }
            if(allOne == 1) return 0;
            allOne = 2;
        }
    }
    if(allOne == 1) return 1;
    else return 2;
}

/*
 * Return chessboard color
 */
int isWhat(int x, int y)
{
    return ChessBoard[x][y];
}

/*
 * Check where can go and save result at vector possiblePosition
 */
void SavecanGo(int color)
{
    possiblePosition.clear();
    int x,y;
    for(x = 0; x < 8; ++x) {
		for(y = 0; y < 8; ++y) {
			if(put(x, y, color, true)) {
			    possiblePosition.push_back(Position(x, y));
            }
		}
    }
}

/*
 * Check whether can go 
 */
bool canGo(int color)
{
    SavecanGo(color);
    if(possiblePosition.empty()) return false;
	return true;
}

/*
 * Try to go on chessboard and return true for can go
 */
bool put(int x, int y, int color, bool onlyCheck)
{
    if(isWhat(x, y) != 0) return false;
    
    static vector<Position> posToReverse;
    for(int i = 0; i < 8; i++) {
        int xt = x;
        int yt = y;
        int opinionColor = (color == 1) ? 2 : 1; 
        posToReverse.clear();

        while(1) {
            xt += DIRECTION_X[i];
            yt += DIRECTION_Y[i];
            if(xt < 0 || xt > 8 || yt < 0 || yt > 8) {
                posToReverse.clear();
                break;
            }
            int chess = isWhat(xt, yt);
            if(chess == opinionColor) {
                posToReverse.push_back(Position(xt, yt));
            }
            else if(chess == 0) {
                posToReverse.clear();
                break;
            }
            else {
                break;
            }
        }
        if (posToReverse.size()) {
			if(onlyCheck) return true;
            for (Position pos : posToReverse) {
                if(color == 1) ChessBoard[pos.x][pos.y] = 2;
                else ChessBoard[pos.x][pos.y] = 1;
            }
		}
    }
    
}

/*
 * AI choice best way to go
 */
Position Go()
{

}

/*
 * Check who win
 */
int CheckWin()
{
    int n_white = 0;
    int n_black = 0;
    // chessboard is full
    if(BoardisFull()) {
        for(int i = 0; i < 8; i++) {
            for(int j = 0; j < 8; j++) {
                if(ChessBoard[i][j] == 1) ++n_black;
                else ++n_white;
            }
        }
        if(n_black > n_white) return 1;
        else return 2;
    }

    // no black or white in chessboard
    int winColor = BoardisOneColor();
    if(winColor != 0) return winColor;

    // black can not move
    if(!canGo(1)) return 2;

    // white can not move
    if(!canGo(2)) return 1;

    return 0;
}

int main()
{
    int my_color = 0;
    int opinion_color = 0;
    puts("please input my color (1 is black, 2 is while)");
    while(my_color > 2 || my_color < 1) {
        // my_clor = 1 -> black, my_color = 2 -> while
        if(scanf("%d", &my_color) <= 0) 
		    getchar();
    }
    printf("Color is %d.\n",my_color);
    opinion_color = (my_color == 1) ? 2 : 1;
    InitBoard();
    PrintBoard();
    bool first = false;

    while(1) {
        // check win
        int win = CheckWin();
        if(win != 0) {
            if(win == 1) printf("Black win~\n");
            else printf("White win~\n");
            break;
        }

        if(my_color == 1 && first == false) {
            first = true;
            // first ai enter
            // check where can go
            SavecanGo(my_color);
            // print ai move
            Position ai_go = Go();
            // update chessboard
            UpdateBoard(ai_go.x, ai_go.y, my_color);
            // print chessboard
            PrintBoard();
        }
        
        // human enter
        int ox, oy;
        while(1) {
            printf("Please input your move (ex. 0 0): ");
            scanf("%d %d", &ox, &oy);
            // check if this move is legal
            SavecanGo(opinion_color);
            bool notGo;
            for(int i = 0; i < possiblePosition.size(); i++) {
                if(ox == possiblePosition[i].x && oy == possiblePosition[i].y) {
                    notGo = false;
                    break;
                }
            }
            if(notGo) printf("You can not go that move!!\n");
            else break;
        }
        // update chessboard
        UpdateBoard(ox, oy, opinion_color);
        // print chessboard
        PrintBoard(); 

        // ai enter
        // check where can go
        SavecanGo(my_color);
        // print ai move
        Position ai_go = Go();
        // update chessboard
        UpdateBoard(ai_go.x, ai_go.y, my_color);
        // print chessboard
        PrintBoard();
    }
    return 0;
}