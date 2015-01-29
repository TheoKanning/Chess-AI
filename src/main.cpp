#include <stdio.h>
#include <iostream>
#include "globals.h"

#define DRAW_ERROR_POS "position startpos moves d2d4 g8f6 c2c4 c7c6 e2e3 d7d6 g1f3 c8g4 f1e2 b8d7 b1c3 e7e5 e1g1 f8e7 d1b3 g4f3 e2f3 d8b6 b3b6 d7b6 d4d5 c6d5 c3d5 b6d5 f3d5 f6d5 c4d5 e5e4 f2f3 e4f3 f1f3 e7f6 e3e4 e8g8 f3b3 f8e8 b3b4 a8b8 c1f4 a7a5 b4b5 e8e4 f4d6 b8e8 d6c5 e4c4 c5a3 f6d4 g1h1 e8c8 b2b3 c4c2 a1d1 c2a2 b5a5 d4c3 d1c1 c8e8 a5c5 a2e2 h2h4 c3f6 h4h5 e2e4 g2g3 f6e7 c5a5 b7b6 a5a7 e7a3 a7a3 e4d4 h5h6 g7h6 a3a6 e8e3 a6b6 e3g3 b6h6 d4d5 b3b4 g3g4 c1g1 g4g6 h6h3 d5d4 b4b5 d4b4 h3h5 b4b2 g1d1 g6b6 d1d5 g8g7 h1g1 b6g6 g1f1 g6f6 f1e1 f6f2 d5g5 g7f6 g5g8 b2e2 e1d1 e2d2 d1e1"
#define DRAW_ERROR_POS2 "position startpos moves d2d4 g8f6 c2c4 c7c6 e2e3 d7d6 g1f3 c8g4 f1e2 b8d7 b1c3 e7e5 e1g1 f8e7 d1b3 g4f3 e2f3 d8b6 b3b6 d7b6 d4d5 c6d5 c3d5 b6d5 f3d5 f6d5 c4d5 e5e4 f2f3 e4f3 f1f3 e7f6 e3e4 e8g8 f3b3 f8e8 b3b4 a8b8 c1f4 a7a5 b4b5 e8e4 f4d6 b8e8 d6c5 e4c4 c5a3 f6d4 g1h1 e8c8 b2b3 c4c2 a1d1 c2a2 b5a5 d4c3 d1c1 c8e8 a5c5 a2e2 h2h4 c3f6 h4h5 e2e4 g2g3 f6e7 c5a5 b7b6 a5a7 e7a3 a7a3 e4d4 h5h6 g7h6 a3a6 e8e3 a6b6 e3g3 b6h6 d4d5 b3b4 g3g4 c1g1 g4g6 h6h3 d5d4 b4b5 d4b4 h3h5 b4b2 g1d1 g6b6 d1d5 g8g7 h1g1 b6g6 g1f1 g6f6 f1e1 f6f2 d5g5 g7f6 g5g8 b2e2 e1d1 e2d2 d1e1 d2e2 e1d1 e2d2 d1e1"

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
	//Set_Passed_Pawn_Rank_Bonuses();
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
		else if (!strncmp(line, "setoption name keps", 18)) {

			int value = 0;
			int index = line[19] - '0';
			sscanf_s(line, "%*s %*s %*s %*s %d", &value);
			printf("Set keps%d to %d\n", index, value);
			king_end_piece_square_tuning_values[index] = value;
			Set_King_End_Values();
		}
		else if (!strncmp(line, "setoption name peps", 18)) {

			int value = 0;
			int index = line[19] - '0';
			sscanf_s(line, "%*s %*s %*s %*s %d", &value);
			printf("Set peps%d to %d\n", index, value);
			pawn_end_piece_square_tuning_values[index] = value;
			Set_Pawn_End_Values();
		}
		else if (!strncmp(line, "setoption name pprb", 18)) {
			float value = 0;
			int index = line[19] - '0';
			sscanf_s(line, "%*s %*s %*s %*s %f", &value);
			printf("Set pprb%d to %f\n", index, value);
			passed_pawn_tuning_parameters[index] = value;
			Set_Passed_Pawn_Rank_Bonuses();
		}
		else if (!strncmp(line, "setoption name fmrg", 18)) {

			int value = 0;
			int index = line[19] - '0';
			sscanf_s(line, "%*s %*s %*s %*s %d", &value);
			printf("Set fmrg%d to %d\n", index, value);
			futility_margins[index] = value;
		}
		else if (!strncmp(line, "quit", 4))	{
			break;
		}
		else if (!strncmp(line, "test", 4))	{
			Parse_Position(DRAW_ERROR_POS, &board);
			//Parse_Fen(LASKER_FEN, &board);
			Print_Board(&board);
			info.stop_time = 100000;
			info.depth = 14;
			Search_Position(&board, &info);
			system("PAUSE");
			break;
		}
	}

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
	
	
	
	
}