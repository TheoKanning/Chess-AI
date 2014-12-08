#include <stdio.h>
#include <iostream>
#include "globals.h"

using namespace std;

#define START_FEN		"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define EP_TEST_FEN		"rnbqkbnr/p1p1pppp/8/1p1pP3/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 3 2"
#define PN_TEST_FEN		"rnb1kbnr/ppp3pp/3q1p2/1N1pp3/3PP3/5N2/PPP2PPP/R1BQKB1R w KQkq - 4 10"
#define B_TEST_FEN		"r1bqk1nr/pppp1ppp/n2b4/4p3/2BPP3/4B3/PPP2PPP/RN1QK1NR b KQkq - 4 10"
#define CASTLE1_FEN		"rn1qkbnr/pppbpppp/B7/3p4/4P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 0 1"
#define CASTLE2_FEN		"r3kbnr/ppp2ppp/2npbq2/4p3/4P3/2NPBQ2/PPP2PPP/R3KBNR b KQkq - 0 1"
#define PROMOTION_FEN	"r1bqkbnr/pPppp1pp/2n5/8/8/5N2/PP1PPPpP/RNBQKB1R w KQkq - 0 1"
BOARD_STRUCT board;
MOVE_LIST_STRUCT movelist;

int move_num = 0;
int done = 0;

int main()
{
	cout << PROGRAM_NAME << " version " << VERSION_NO << endl << AUTHOR << endl;	
	Init_Hashkeys();
	
	//Init_Board(&board);
	Parse_Fen(PROMOTION_FEN, &board);
	Print_Board(&board);
	while (!done)
	{
		Generate_Moves(&board, &movelist);
		Print_Movelist(&movelist);

		cin >> move_num;
		if (move_num < 0 || move_num >= movelist.num) done = 1;

		Make_Move(&movelist.list[move_num], &board); //Make first move
		Print_Board(&board);
	}
	
	system("PAUSE");
}