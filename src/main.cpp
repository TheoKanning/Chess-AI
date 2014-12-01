#include <stdio.h>
#include <iostream>
#include "globals.h"

using namespace std;

#define START_FEN		"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

BOARD_STRUCT board;
MOVE_LIST_STRUCT movelist;

int main()
{
	cout << PROGRAM_NAME << " version " << VERSION_NO << endl << AUTHOR << endl;	
	Init_Hashkeys();
	Parse_Fen(START_FEN, &board);
	//Init_Board(&board);
	Print_Board(&board);
	Add_Move(&movelist, E2, E4, wP, 0, 0, 0);
	Print_Movelist(&movelist);
	system("PAUSE");
}