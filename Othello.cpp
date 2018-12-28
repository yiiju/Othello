#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <windows.h>

using namespace std;

#define BLACK 1
#define WHITE 2

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
void PrintBoard(int);
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
    ChessBoard[4][3] = BLACK; ChessBoard[3][4] = BLACK;
    ChessBoard[3][3] = WHITE; ChessBoard[4][4] = WHITE;
}

/*
 * Print the ChessBoard
 */
void PrintBoard(int color)
{
    SavecanGo(color);
    int another_color = (color == BLACK) ? WHITE : BLACK;
    if(possiblePosition.size() == 0) SavecanGo(another_color);
    // Windows print colored text
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    WORD saved_attributes;

    /* Save current attributes */
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    saved_attributes = consoleInfo.wAttributes;

    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
    printf("  0 1 2 3 4 5 6 7\n");

    /* Restore original attributes */
    SetConsoleTextAttribute(hConsole, saved_attributes);

    int i,j,in = 0;
    for(i = 0; i < 8; i++) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
        printf("%d ",i);
        SetConsoleTextAttribute(hConsole, saved_attributes);
        for(j = 0; j < 8; j++) {
            for (Position pos : possiblePosition) {
                if(i == pos.x && j == pos.y) {
                    SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
                    printf("%d ",ChessBoard[i][j]);
                    SetConsoleTextAttribute(hConsole, saved_attributes);
                    in = 1;
                    break;
                }
            }
            if(in == 0) printf("%d ",ChessBoard[i][j]);
            in = 0;
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
            if(ChessBoard[i][j] == BLACK) {
                if(allOne == WHITE) return 0;
                allOne = BLACK;
            }
            if(allOne == BLACK) return 0;
            allOne = WHITE;
        }
    }
    return allOne;
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
    if(isWhat(x, y) != 0 && onlyCheck) return false;
    
    static vector<Position> posToReverse;
    for(int i = 0; i < 8; i++) {
        int xt = x;
        int yt = y;
        int opinionColor = (color == BLACK) ? WHITE : BLACK; 
        posToReverse.clear();

        while(1) {
            xt += DIRECTION_X[i];
            yt += DIRECTION_Y[i];
            if(xt < 0 || xt > 7 || yt < 0 || yt > 7) {
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
                if(color == 1) ChessBoard[pos.x][pos.y] = 1;
                else ChessBoard[pos.x][pos.y] = 2;
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
int CheckWin(int go_color)
{
    int n_white = 0;
    int n_black = 0;
    // chessboard is full or no one can go
    if(BoardisFull() || go_color == 0) {
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
    if(winColor != 0) {
        printf("Chessboard is remain %d color\n",winColor);
        return winColor;
    }
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
    opinion_color = (my_color == BLACK) ? WHITE : BLACK;
    InitBoard();
    PrintBoard(BLACK);

    // first ai enter
    bool first = false;
    if(my_color == BLACK && first == false) {
        first = true;
        // check where can go
        SavecanGo(my_color);
        // print ai move
        printf("Please input your move: ");
        int tx,ty;
        scanf("%d %d", &tx, &ty);
        //Position ai_go = Go();
        // update chessboard
        UpdateBoard(tx, ty, my_color);
        //UpdateBoard(ai_go.x, ai_go.y, my_color);
        // print chessboard
        PrintBoard(opinion_color);
    }               

    bool cannotGo_opinion = false;
    bool cannotGo_me = false;
    while(1) {
        // human enter
        int ox, oy;
        cannotGo_opinion = false;
        while(1) {
            SavecanGo(opinion_color);
            // can not go -> pass
            if(possiblePosition.size() == 0) {
                printf("You can not go! Pass!\n");
                cannotGo_opinion = true;
                break;
            }
            // print can go where
            for (Position pos : possiblePosition) {
                printf("%d %d, ",pos.x,pos.y);
            }
            printf("choose one to move.\n");
            printf("Please input your move: ");
            scanf("%d %d", &ox, &oy);
            // check if this move is legal
            bool notGo = true;
            for(int i = 0; i < possiblePosition.size(); i++) {
                if(ox == possiblePosition[i].x && oy == possiblePosition[i].y) {
                    notGo = false;
                    break;
                }
            }
            if(notGo) printf("You can not go that move!!\n");
            else break;
        }
        if(!cannotGo_opinion) {
            // update chessboard
            UpdateBoard(ox, oy, opinion_color);
            // print chessboard
            PrintBoard(my_color); 
            // check win
            int win = CheckWin(opinion_color);
            if(win != 0) {
                if(win == 1) printf("Black win~\n");
                else printf("White win~\n");
                break;
            }
        }

        // ai enter
        cannotGo_me = false;
        // check where can go
        SavecanGo(my_color);
        // can not go -> pass
        if(possiblePosition.size() == 0) {
            printf("Pass!\n");
            cannotGo_me = true;
        }
        if(!cannotGo_me) {
            // print ai move
            printf("Please input your move: ");
            int tx,ty;
            scanf("%d %d", &tx, &ty);
            //Position ai_go = Go();
            // update chessboard
            UpdateBoard(tx, ty, my_color);
            // print chessboard
            PrintBoard(opinion_color);
            // check win
            int win = CheckWin(my_color);
            if(win != 0) {
                if(win == 1) printf("Black win~\n");
                else printf("White win~\n");
                break;
            }
        }
        // both can not move 
        if(cannotGo_opinion && cannotGo_me) {
            // check win
            int win = CheckWin(0);
            if(win != 0) {
                if(win == 1) printf("Black win~\n");
                else printf("White win~\n");
                break;
            }
        }
    }
    return 0;
}