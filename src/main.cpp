#include <stdio.h>
#include <iostream>
#include "globals.h"

using namespace std;

#define START_FEN		"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

BOARD_STRUCT board;


int main()
{
	cout << PROGRAM_NAME << " version " << VERSION_NO << endl << AUTHOR << endl;	
	Init_Hashkeys();
	Parse_Fen(START_FEN, &board);
	//Init_Board(&board);
	Print_Board(&board);
	Print_Bitboards(&board);
	system("PAUSE");
}