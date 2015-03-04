#include <stdio.h>
#include <iostream>
#include "globals.h"

using namespace std;

#define LASKER_FEN		"8/k7/3p4/p2P1p2/P2P1P2/8/8/K7 w - - 0 1"
#define KIWIPETE_FEN	"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define MATE_IN_2_FEN	"8/6K1/1p1B1RB1/8/2Q5/2n1kP1N/3b4/4n3 w - - 0 1"
#define ENDGAME_FEN		"8/3r1pp1/4k3/4p3/4P3/4K3/3R1PP1/8 w - - 0 1"
#define MATE_FEN "r1r2b1k/2q2pp1/p3pN1p/4P3/1P3QR1/3R4/PP4PP/1K6 w - - 0 1"
#define MATE_FEN2 "r1r2b1k/2q2p2/p3pN1p/4P3/1P4R1/3R4/PP4PP/1K6 w - - 0 1"
#define TEST_FEN "rnbqkb1r/pp1p1ppp/2p5/4P3/2B5/8/PPP1NnPP/RNBQK2R w KQkq - 0 6"

#define SEE_TEST_FEN "1k1r3q/1ppn3p/p4b2/4p3/8/P2N2P1/1PP1R1BP/2K1Q3 w - - 0 0"
#define SEE_TEST_FEN2 "2k5/8/8/3PrRrR/8/8/8/2K5 b - - 16 1"
#define SEE_TEST_FEN3 "q2k2q1/2nqn2b/1n1P1n1b/2rnr2Q/1NQ1QN1Q/3Q3B/2RQR2B/Q2K2Q1 w - - 0 0"

#define HASH_TEST_FEN "r1b1r1k1/ppq2p1p/3b2pQ/3pn3/8/2P4P/PPBN1PP1/R1B1R1K1 b - - 0 0"

BOARD_STRUCT board;
MOVE_LIST_STRUCT move_list;
SEARCH_INFO_STRUCT info;

int move_num = 0;
int done = 0;
int index;

int main()
{
	cout << PROGRAM_NAME << " version " << VERSION_NO << endl << AUTHOR << endl;
	Init_Hashkeys();
	Clear_Hash_Table();
	Clear_Pawn_Hash_Table();
	Init_Pawn_Masks();
	Init_Board(&board);
	Generate_Magic_Moves();
	Generate_Between_Squares();
	Set_King_End_Values();
	Clear_History_Data(&board);


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
			info.age = 0;
			Uci_Loop(&board, &info);
			if (info.quit == 1) break;
			continue;
		}
		if (!strncmp(line, "perft", 5)){
			Perft_Test("rnbqkb1r/pp1p1ppp/2p5/4P3/2B5/8/PPP1NnPP/RNBQK2R w KQkq - 0 6", 5, &board);
			//Perft_Test("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 5, &board);
			printf("Perft Complete\n");
		}
		else if (!strncmp(line, "test", 4))	{
			Parse_Fen(HASH_TEST_FEN, &board);
			//Parse_Position(MATE_ERROR_POS, &board);
			Print_Board(&board);
			info.stop_time = 100000;
			info.depth = 25;
			Search_Position(&board, &info);
			system("PAUSE");
			//break;
		}
		else if (!strncmp(line, "see", 3))	{
			Parse_Fen(SEE_TEST_FEN3, &board);
			Print_Board(&board);
			Magic_Generate_Moves(&board, &move_list);
			for (int i = 0; i < move_list.num; i++)
			{
				if (IS_CAPTURE(move_list.list[i].move))
				{
					Print_Move(&move_list.list[i]);
					printf(" SEE: %d\n", Static_Exchange_Evaluation(move_list.list[i].move, &board));
				}
			}
			system("PAUSE");
			break;
		}
		else if (!strncmp(line, "search_test", 11)) {
			Search_Test();
			system("PAUSE");
		}
		else if (!strncmp(line, "setoption", 9)) {
			Set_Option(line);
		}
		else if (!strncmp(line, "quit", 4))	{
			break;
		}
	}

	Parse_Fen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", &board);
	Print_Board(&board);
	while (!done)
	{
		Magic_Generate_Moves(&board, &move_list);
		Sort_Moves(&move_list);
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
			Make_Move(move_list.list[move_num].move, &board); //Make first move
		}
		//Take_Move(&board);
		board.eval_score = Evaluate_Board(&board);
		Print_Board(&board);
	}
	
	
	
	
}