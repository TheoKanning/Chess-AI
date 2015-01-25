#include <stdio.h>
#include <iostream>
#include "globals.h"

#define DRAW_ERROR_POS "position startpos moves d2d4 g8f6 c2c4 c7c6 e2e3 d7d6 g1f3 c8g4 f1e2 b8d7 b1c3 e7e5 e1g1 f8e7 d1b3 g4f3 e2f3 d8b6 b3b6 d7b6 d4d5 c6d5 c3d5 b6d5 f3d5 f6d5 c4d5 e5e4 f2f3 e4f3 f1f3 e7f6 e3e4 e8g8 f3b3 f8e8 b3b4 a8b8 c1f4 a7a5 b4b5 e8e4 f4d6 b8e8 d6c5 e4c4 c5a3 f6d4 g1h1 e8c8 b2b3 c4c2 a1d1 c2a2 b5a5 d4c3 d1c1 c8e8 a5c5 a2e2 h2h4 c3f6 h4h5 e2e4 g2g3 f6e7 c5a5 b7b6 a5a7 e7a3 a7a3 e4d4 h5h6 g7h6 a3a6 e8e3 a6b6 e3g3 b6h6 d4d5 b3b4 g3g4 c1g1 g4g6 h6h3 d5d4 b4b5 d4b4 h3h5 b4b2 g1d1 g6b6 d1d5 g8g7 h1g1 b6g6 g1f1 g6f6 f1e1 f6f2 d5g5 g7f6 g5g8 b2e2 e1d1 e2d2 d1e1"
#define DRAW_ERROR_POS2 "position startpos moves d2d4 g8f6 c2c4 c7c6 e2e3 d7d6 g1f3 c8g4 f1e2 b8d7 b1c3 e7e5 e1g1 f8e7 d1b3 g4f3 e2f3 d8b6 b3b6 d7b6 d4d5 c6d5 c3d5 b6d5 f3d5 f6d5 c4d5 e5e4 f2f3 e4f3 f1f3 e7f6 e3e4 e8g8 f3b3 f8e8 b3b4 a8b8 c1f4 a7a5 b4b5 e8e4 f4d6 b8e8 d6c5 e4c4 c5a3 f6d4 g1h1 e8c8 b2b3 c4c2 a1d1 c2a2 b5a5 d4c3 d1c1 c8e8 a5c5 a2e2 h2h4 c3f6 h4h5 e2e4 g2g3 f6e7 c5a5 b7b6 a5a7 e7a3 a7a3 e4d4 h5h6 g7h6 a3a6 e8e3 a6b6 e3g3 b6h6 d4d5 b3b4 g3g4 c1g1 g4g6 h6h3 d5d4 b4b5 d4b4 h3h5 b4b2 g1d1 g6b6 d1d5 g8g7 h1g1 b6g6 g1f1 g6f6 f1e1 f6f2 d5g5 g7f6 g5g8 b2e2 e1d1 e2d2 d1e1 d2e2 e1d1 e2d2 d1e1"

#define INCORRECT_MATE_POS "position startpos moves d2d4 g8f6 c2c4 g7g6 g1f3 f8g7 g2g3 e8g8 f1g2 d7d6 e1g1 b8d7 b1c3 e7e5 e2e4 e5d4 f3d4 d7b6 b2b3 c8g4 d1d3 b6d7 c1f4 d7e5 d3e3 c7c5 d4e2 g4e2 e3e2 f8e8 a1d1 e8e6 g2h3 e6e7 c3b5 f6e8 f4g5 f7f6 g5e3 b7b6 h3g2 a7a6 b5c3 e8c7 f2f4 e5f7 f4f5 a8b8 c3d5 c7d5 c4d5 b8a8 f5g6 h7g6 f1e1 f7e5 g2h3 g8f8 e3f4 g6g5 f4e3 e5f7 h3e6 f8e8 h2h4 g5h4 g3h4 b6b5 h4h5 g7f8 h5h6 d8a5 e1f1 a5c3 d1d3 c3b4 f1f6 c5c4 e6f7 e7f7 e2h5 b4e1 g1h2 e1e2 h5e2 f7f6 h6h7 f8g7 h7h8q g7h8 e2h5 e8e7 h5h7 e7e8 h7h8 e8f7 h8h7 f7e8 h7g8 e8d7 g8g7 d7c8 e3b6 a8a7 g7g8 c8b7 b6a7 b7a7 g8g7 a7b6 g7f6 c4d3 f6d6 b6a7 d6e7 a7a8 e7e8 a8a7 e8e7 a7a8 e7f6 a8b7"
#define INCORRECT_MATE_POS2 "position startpos moves d2d4 g8f6 c2c4 g7g6 g1f3 f8g7 g2g3 e8g8 f1g2 d7d6 e1g1 b8d7 b1c3 e7e5 e2e4 e5d4 f3d4 d7b6 b2b3 c8g4 d1d3 b6d7 c1f4 d7e5 d3e3 c7c5 d4e2 g4e2 e3e2 f8e8 a1d1 e8e6 g2h3 e6e7 c3b5 f6e8 f4g5 f7f6 g5e3 b7b6 h3g2 a7a6 b5c3 e8c7 f2f4 e5f7 f4f5 a8b8 c3d5 c7d5 c4d5 b8a8 f5g6 h7g6 f1e1 f7e5 g2h3 g8f8 e3f4 g6g5 f4e3 e5f7 h3e6 f8e8 h2h4 g5h4 g3h4 b6b5 h4h5 g7f8 h5h6 d8a5 e1f1 a5c3 d1d3 c3b4 f1f6 c5c4 e6f7 e7f7 e2h5 b4e1 g1h2 e1e2 h5e2 f7f6 h6h7 f8g7 h7h8q g7h8 e2h5 e8e7 h5h7 e7e8 h7h8 e8f7 h8h7 f7e8 h7g8 e8d7 g8g7 d7c8 e3b6 a8a7 g7g8 c8b7 b6a7 b7a7 g8g7 a7b6 g7f6 c4d3 f6d6 b6a7 d6e7 a7a8 e7e8 a8a7 e8e7 a7a8"

using namespace std;

#define LASKER_FEN		"8/k7/3p4/p2P1p2/P2P1P2/8/8/K7 w - - 0 1"
#define MATE_IN_2_FEN	"8/6K1/1p1B1RB1/8/2Q5/2n1kP1N/3b4/4n3 w - - 0 1"
#define ENDGAME_FEN		"8/3r1pp1/4k3/4p3/4P3/4K3/3R1PP1/8 w - - 0 1"
#define MATE_FEN "r1r2b1k/2q2pp1/p3pN1p/4P3/1P3QR1/3R4/PP4PP/1K6 w - - 0 1"
#define MATE_FEN2 "r1r2b1k/2q2p2/p3pN1p/4P3/1P4R1/3R4/PP4PP/1K6 w - - 0 1"

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
	Init_Hash_Table();
	Init_Pawn_Masks();
	Set_King_End_Values();
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
			info.age = 0;
			Uci_Loop(&board, &info);
			if (info.quit == 1) break;
			continue;
		}
		
		else if (!strncmp(line, "quit", 4))	{
			break;
		}
	}
	
	Parse_Position(DRAW_ERROR_POS, &board);
	//Parse_Fen(MATE_FEN, &board);
	Print_Board(&board);
	info.stop_time = 100000;
	info.depth = 12;
	Search_Position(&board, &info);

	/*
	while (!done)
	{
		Generate_Moves(&board, &move_list);
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
	*/
	
	
	system("PAUSE");
	
}