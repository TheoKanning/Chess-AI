#include <stdio.h>
#include <iostream>
#include "globals.h"


#define DRAW_ERROR_FEN "position startpos moves c2c4 e7e5 b1c3 b8c6 g1f3 g8f6 e2e4 f8c5 f1e2 d7d6 e1g1 c5b4 d2d3 b4c3 b2c3 c8e6 a1b1 a8b8 f3g5 e6d7 e2h5 e8g8 h5f7 f8f7 g5f7 g8f7 c1g5 d7e6 f2f4 f7g8 f4e5 d6e5 d1f3 g8f7 b1b2 f7g6 g5h4 d8e7 f1b1 b7b6 b2f2 b8f8 f3e3 h7h6 b1f1 e7c5 e3c5 b6c5 a2a4 f8f7 f2b2 a7a6 b2b7 f7d7 f1f3 f6e8 b7b2 e8d6 h4e1 g6h7 f3f8 d6f7 f8e8 f7d8 b2d2 e6f7 e8f8 g7g5 e1f2 f7c4 f8f3 d8e6 f3f6 h7g7 f6f3 c4b3 f2g3 b3a4 d2a2 a4d1 f3f1 d7d3 a2a6 e6d8 a6a3 d1g4 g3f2 d8e6 a3a6 d3d6 a6a2 c5c4 f1b1 g7f6 b1f1 e6f4 a2a6 f4e2 g1h1 e2f4 h1g1 f4e2 g1h1 f6g7 f2c5 d6e6 c5f8 g7g6 a6a3 e2f4 f8c5 g4e2 f1b1 e2d3 b1e1 f4h5 h1g1 h5f6 e1d1 f6e4 c5e3 c6e7 e3c1 e7d5 c1b2 e6b6 b2a1 g6f5 h2h3 g5g4 h3g4 f5g4 d1c1 g4f5 c1d1 d5e3 d1c1 b6g6 a3a2 g6g3 c1e1 e3c2 e1c1"
#define DRAW_ERROR_FEN2 "position startpos moves c2c4 e7e5 b1c3 b8c6 g1f3 g8f6 e2e4 f8c5 f1e2 d7d6 e1g1 c5b4 d2d3 b4c3 b2c3 c8e6 a1b1 a8b8 f3g5 e6d7 e2h5 e8g8 h5f7 f8f7 g5f7 g8f7 c1g5 d7e6 f2f4 f7g8 f4e5 d6e5 d1f3 g8f7 b1b2 f7g6 g5h4 d8e7 f1b1 b7b6 b2f2 b8f8 f3e3 h7h6 b1f1 e7c5 e3c5 b6c5 a2a4 f8f7 f2b2 a7a6 b2b7 f7d7 f1f3 f6e8 b7b2 e8d6 h4e1 g6h7 f3f8 d6f7 f8e8 f7d8 b2d2 e6f7 e8f8 g7g5 e1f2 f7c4 f8f3 d8e6 f3f6 h7g7 f6f3 c4b3 f2g3 b3a4 d2a2 a4d1 f3f1 d7d3 a2a6 e6d8 a6a3 d1g4 g3f2 d8e6 a3a6 d3d6 a6a2 c5c4 f1b1 g7f6 b1f1 e6f4 a2a6 f4e2 g1h1 e2f4 h1g1 f4e2 g1h1 f6g7 f2c5 d6e6 c5f8 g7g6 a6a3 e2f4 f8c5 g4e2 f1b1 e2d3 b1e1 f4h5 h1g1 h5f6 e1d1 f6e4 c5e3 c6e7 e3c1 e7d5 c1b2 e6b6 b2a1 g6f5 h2h3 g5g4 h3g4 f5g4 d1c1 g4f5 c1d1 d5e3 d1c1 b6g6 a3a2 g6g3 c1e1 e3c2 e1c1 c2e3 c1e1 e3c2 e1c1"

#define ILLEGAL_MOVE_FEN "position startpos moves e2e4 e7e6 d2d4 d7d5 b1c3 g8f6 e4e5 f6d7 f2f4 c7c5 g1f3 b8c6 c1e3 a7a6 d1d2 b7b5 f1d3 c8b7 e1g1 f8e7 a1d1 c5d4 f3d4 c6d4 e3d4 b5b4 c3e2 d8c7 d2e3 e8g8 f4f5 e6f5 f1f5 d7c5 e2f4 a8d8 h2h3 f8e8 f5h5 c5d3 e3d3 h7h6 h5f5 g7g6 f5f7 c7c6 e5e6 c6e6 f7g7 g8h8 g7e7 h8g8 f4e6 e8e7 d3g6"
#define ILLEGAL_MOVE_FEN2 "position startpos moves d2d4 d7d5 g1f3 g8f6 c2c4 c7c6 e2e3 c8f5 b1c3 e7e6 f3h4 f5e4 f2f3 e4g6 c1d2 f8e7 h4g6"

using namespace std;

#define LASKER_FEN		"8/k7/3p4/p2P1p2/P2P1P2/8/8/K7 w - - 0 1"
#define MATE_IN_2_FEN	"8/6K1/1p1B1RB1/8/2Q5/2n1kP1N/3b4/4n3 w - - 0 1"
#define ENDGAME_FEN		"8/3r1pp1/4k3/4p3/4P3/4K3/3R1PP1/8 w - - 0 1"
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
	Parse_Position(ILLEGAL_MOVE_FEN, &board);
	Print_Board(&board);
	info.stop_time = 1000000;
	info.depth = 14;
	Search_Position(&board, &info);
	system("PAUSE");
	Parse_Position(DRAW_ERROR_FEN2, &board);
	info.depth = 7;
	Search_Position(&board, &info);
	
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
	
	
	
	system("PAUSE");
}