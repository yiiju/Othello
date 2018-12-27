#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

struct Position {
    int x;
    int y;
};

int ChessBoard[8][8];
vector<Position> possiblePosition;

/*
 * Check where can go and save result at vector possiblePosition
 */
void canGo(int color)
{
    possiblePosition.clear();
    int x,y;
    for(x = 0; x < 8; ++x) {
		for(y = 0; y < 8; ++y) {
			
			possiblePosition.push_back(Position.x, Position.y));
		}
    }
}

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

int main()
{
    int my_color = 0;
    puts("please input my color (1 is black, 2 is while)");
    while(my_color > 2 || my_color < 1) {
        // my_clor = 1 -> black, my_color = 2 -> while
        if(scanf("%d", &my_color) <= 0) 
		    getchar();
    }
    printf("Color is %d.\n",my_color);
    InitBoard();
    PrintBoard();
    return 0;
}