#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <windows.h>

using namespace std;

#define BLACK 1
#define WHITE 2
#define REAL 1
#define PREDICT 2
#define LEVEL 9

struct Position {
    Position():x(-1),y(-1) {}
    Position(int, int);
    int x;
    int y;
};

Position::Position(int cx, int cy) {
    x = cx;
    y = cy;
}

struct MinOpen {
    MinOpen():min(100000),pos(-1,-1) {}
    MinOpen(int, Position);
    int min;
    Position pos;
};

MinOpen::MinOpen(int cmin, Position cpos) {
    min = cmin;
    pos = cpos;
}

void InitBoard();
void PrintBoard(int);
void UpdateBoard(int, int, int, int);
bool BoardisFull();
int BoardisOneColor();
int isWhat(int, int, int);
void SavecanGo(int);
bool canGo(int);
bool put(int, int, int, bool, int);
int Openness(int, int, int, int, int);
vector<Position> PredictcanGo(int);
int WeightedStrategy(Position);
struct MinOpen Go(int, int);
struct Position AI_GO(int, int);
int CheckWin();

int ChessBoard[8][8];
int PredictChessBoard[8][8];
vector<Position> possiblePosition;
int MyColor;

// L,LD,D,RD,R,RU,U,LU
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
            PredictChessBoard[i][j] = 0;
        }
    }
    ChessBoard[4][3] = BLACK; ChessBoard[3][4] = BLACK;
    ChessBoard[3][3] = WHITE; ChessBoard[4][4] = WHITE;
    PredictChessBoard[4][3] = BLACK; PredictChessBoard[3][4] = BLACK;
    PredictChessBoard[3][3] = WHITE; PredictChessBoard[4][4] = WHITE;
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
            if(in == 0) {
                if(ChessBoard[i][j] == BLACK) {
                    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                    printf("%d ",ChessBoard[i][j]);
                    SetConsoleTextAttribute(hConsole, saved_attributes);
                }
                else if(ChessBoard[i][j] == WHITE){
                    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                    printf("%d ",ChessBoard[i][j]);
                    SetConsoleTextAttribute(hConsole, saved_attributes);
                }
                else printf("%d ",ChessBoard[i][j]);
            }
            in = 0;
        }
        printf("\n");
    }
}

/*
 * Update the ChessBoard
 */
void UpdateBoard(int px, int py, int color, int board)
{
    if(board == REAL) ChessBoard[px][py] = color;
    else PredictChessBoard[px][py] = color;
    put(px, py, color, false, board);
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
int isWhat(int x, int y, int board)
{
    return (board == REAL) ? ChessBoard[x][y] : PredictChessBoard[x][y];
}

/*
 * Put color on chessboard
 */
void putColor(int x, int y, int color, int board)
{
    (board == REAL) ? (ChessBoard[x][y] = color) : (PredictChessBoard[x][y] = color);
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
			if(put(x, y, color, true, REAL)) {
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
bool put(int x, int y, int color, bool onlyCheck, int board)
{
    if(isWhat(x, y, board) != 0 && onlyCheck) return false;
    
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
            int chess = isWhat(xt, yt, board);
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
                putColor(pos.x, pos.y, color, board);
            }
		}
    }
    
}

/*
 * Calculate the openness
 */
int Openness(int x, int y, int color, int level, int board)
{
    static vector<Position> posToReverse;
    int openness = 0;
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
            int chess = isWhat(xt, yt, board);
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
            for (Position pos : posToReverse) {
                for(int j = 0; j < 8; j++) {
                    int openx = pos.x + DIRECTION_X[j];
                    int openy = pos.y + DIRECTION_Y[j];
                    if(openx < 0 || openx > 7 || openy < 0 || openy > 7) continue;
                    if(ChessBoard[openx][openy] == 0) ++openness;
                }
            }
		}
    }
    return openness;
}

/*
 * Predict where can go
 */
vector<Position> PredictcanGo(int color)
{
    vector<Position> predictPosition;
    int x,y;
    for(x = 0; x < 8; ++x) {
		for(y = 0; y < 8; ++y) {
			if(put(x, y, color, true, PREDICT)) {
			    predictPosition.push_back(Position(x, y));
            }
		}
    }
    return predictPosition;
}

/*
 * AI choice the best way to go
 */
Position AI_GO(int color, int level)
{
    struct MinOpen minopen = Go(color, level);
    printf("pos:%d %d %d\n",minopen.min,minopen.pos.x,minopen.pos.y);
    while(minopen.pos.x == -1) minopen = Go(color, --level);
    return minopen.pos;
}

/*
 * Use the weighted square strategy and return 0 if it is bad move 
 */
int WeightedStrategy(Position pos)
{
    if((pos.x == 1 || pos.x == 6) && (pos.y == 1 || pos.y == 6)) {
        return 10;
    }
    else if(((pos.x == 0 || pos.x == 7) && (pos.y == 1 || pos.y == 6)) ||
        ((pos.x == 1 || pos.x == 6) && (pos.y == 0 || pos.y == 7))) {
        return 5;
    }
    else if((pos.x == 0 || pos.x == 7) && (pos.y == 0 || pos.y == 7)) {
        return -5;
    }
    else return 0;
}

/*
 * AI test the best way
 */
struct MinOpen Go(int color, int level)
{
    int opinion_color = (color == BLACK) ? WHITE : BLACK;
    vector<Position> position = PredictcanGo(color);
    int OpennessVector[64]; 
    for(int i = 0; i < 64; i++) OpennessVector[i] = 0;
    int cut = (position.size() > 5) ? 5 : position.size();
    if(level == LEVEL) cut = position.size();
    for (int i = 0; i < cut; i++) {
        OpennessVector[i] += WeightedStrategy(position[i]);
        OpennessVector[i] += Openness(position[i].x, position[i].y, color, level, PREDICT);
        if(level != 1) {
            --level;
            // update predict chessboard
            UpdateBoard(position[i].x, position[i].y, color, PREDICT);
            struct MinOpen minGo = Go(opinion_color, level);
            int mingo = minGo.min;
            if(opinion_color != MyColor) mingo = -mingo;
            OpennessVector[i] += mingo;
            for(int j = 0; j < 8; j++) {
                for(int k = 0; k < 8; k++) {
                    PredictChessBoard[j][k] = ChessBoard[j][k];
                }
            } 
            ++level;
        }
    }
    int minindex = 0;
    int min = 100000;
    for(int i = 0; i < cut; i++) {
        //printf("%d ",OpennessVector[i]);
        if(min > OpennessVector[i]) {
            minindex = i;
            min = OpennessVector[i];
        }
    }
    //printf("\n");
    struct MinOpen minopen;
    if(min != 100000) {
        minopen.min = min;
        minopen.pos = position[minindex];
    }
    return minopen;
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
    MyColor = my_color;
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
        Position ai_go = AI_GO(my_color, LEVEL);
        printf("Go %d %d\n",ai_go.x, ai_go.y);
        // update chessboard
        UpdateBoard(ai_go.x, ai_go.y, my_color, REAL);
        UpdateBoard(ai_go.x, ai_go.y, my_color, PREDICT);
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
            UpdateBoard(ox, oy, opinion_color, REAL);
            UpdateBoard(ox, oy, opinion_color, PREDICT);
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
            Position ai_go = AI_GO(my_color, LEVEL);
            // update chessboard
            UpdateBoard(ai_go.x, ai_go.y, my_color, REAL);
            UpdateBoard(ai_go.x, ai_go.y, my_color, PREDICT);
            printf("Go %d %d\n",ai_go.x, ai_go.y);
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