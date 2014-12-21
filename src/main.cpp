#include <stdio.h>
#include <iostream>
#include "globals.h"



using namespace std;

#define EP_TEST_FEN		"rnbqkbnr/p1p1pppp/8/1p1pP3/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 3 2"
#define EP_TEST_FEN2	"r1bqkbnr/pppppppp/2n5/4P3/8/8/PPPP1PPP/RNBQKBNR b KQkq - 3 2"
#define PN_TEST_FEN		"rnb1kbnr/ppp3pp/3q1p2/1N1pp3/3PP3/5N2/PPP2PPP/R1BQKB1R w KQkq - 4 10"
#define B_TEST_FEN		"r1bqk1nr/pppp1ppp/n2b4/4p3/2BPP3/4B3/PPP2PPP/RN1QK1NR b KQkq - 4 10"
#define CASTLE1_FEN		"rn1qkbnr/pppbpppp/B7/3p4/4P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 0 1"
#define CASTLE2_FEN		"r3kbnr/ppp2ppp/2npbq2/4p3/4P3/2NPBQ2/PPP2PPP/R3KBNR b KQkq - 0 1"
#define PROMOTION_FEN	"r1bqkbnr/pPppp1pp/2n5/8/8/5N2/PP1PPPpP/RNBQKB1R w KQkq - 0 1"
#define MATE_IN_2_FEN	"8/6K1/1p1B1RB1/8/2Q5/2n1kP1N/3b4/4n3 w - - 0 1"
BOARD_STRUCT board;
MOVE_LIST_STRUCT move_list;
SEARCH_INFO_STRUCT info;

int move_num = 0;
int done = 0;

int main()
{
	cout << PROGRAM_NAME << " version " << VERSION_NO << endl << AUTHOR << endl;
	Init_Hashkeys();
	Init_Pawn_Masks();
	Init_Board(&board);

	char line[256];

	setvbuf(stdin, NULL, _IONBF, NULL);
	setvbuf(stdout, NULL, _IONBF, NULL);

	while (1) {
		memset(&line[0], 0, sizeof(line));

		fflush(stdout);
		if (!fgets(line, 256, stdin))
			continue;
		if (line[0] == '\n')
			continue;
		if (!strncmp(line, "uci", 3)) {
			Uci_Loop(&board, &info);
			if (info.quit == 1) break;
			continue;
		}
		
		else if (!strncmp(line, "quit", 4))	{
			break;
		}
	}

	//Parse_Fen(MATE_IN_2_FEN, &board);
	//Print_Board(&board);
	//Iterative_Deepening(6, &board, &info);
	
	
	
	while (!done)
	{
		Generate_Moves(&board, &move_list);
		Print_Move_List(&move_list);

		cin >> move_num;
		if (move_num == -1)
		{
			Take_Move(&board);
		}
		else if (move_num < 0 || move_num >= move_list.num)
		{
			done = 1;
		}
		else
		{
			Make_Move(&move_list.list[move_num], &board); //Make first move
		}
		//Take_Move(&board);
		Print_Board(&board);
	}
	
	
	
	system("PAUSE");
}