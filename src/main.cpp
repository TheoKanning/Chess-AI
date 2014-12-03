#include <stdio.h>
#include <iostream>
#include "globals.h"

using namespace std;

#define START_FEN		"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define EP_TEST_FEN		"rnbqkbnr/p1p1pppp/8/1p1pP3/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 3 2"
BOARD_STRUCT board;
MOVE_LIST_STRUCT movelist;

int main()
{
	cout << PROGRAM_NAME << " version " << VERSION_NO << endl << AUTHOR << endl;	
	Init_Hashkeys();
	Parse_Fen(EP_TEST_FEN, &board);
	//Init_Board(&board);
	Print_Board(&board);
	Generate_Moves(&board, &movelist);
	Print_Movelist(&movelist);
	system("PAUSE");
}