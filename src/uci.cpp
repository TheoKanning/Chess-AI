/* uci.cpp
* Contains functions for interfacing with uci engine
* Theo Kanning 
#include "stdio/.h"
*/

#include "globals.h"
#include "string.h"

#define INPUTBUFFER 400 * 6

// go depth 6 wtime 180000 btime 100000 binc 1000 winc 1000 movetime 1000 movestogo 40
void Parse_Go(char* line, SEARCH_INFO_STRUCT *info, BOARD_STRUCT *board) {

	int depth = -1, movestogo = 30, movetime = -1;
	int time = -1, inc = 0;
	char *ptr = NULL;
	info->time_set = 0;

	if ((ptr = strstr(line, "infinite"))) {
		;
	}

	if ((ptr = strstr(line, "binc")) && board->side == BLACK) {
		inc = atoi(ptr + 5);
	}

	if ((ptr = strstr(line, "winc")) && board->side == WHITE) {
		inc = atoi(ptr + 5);
	}

	if ((ptr = strstr(line, "wtime")) && board->side == WHITE) {
		time = atoi(ptr + 6);
	}

	if ((ptr = strstr(line, "btime")) && board->side == BLACK) {
		time = atoi(ptr + 6);
	}

	if ((ptr = strstr(line, "movestogo"))) {
		movestogo = atoi(ptr + 10);
	}

	if ((ptr = strstr(line, "movetime"))) {
		movetime = atoi(ptr + 9);
	}

	if ((ptr = strstr(line, "depth"))) {
		depth = atoi(ptr + 6);
	}

	if (movetime != -1) {
		time = movetime;
		movestogo = 1;
	}

	info->start_time = Get_Time_Ms();
	info->depth = depth;

	if (time != -1) {
		info->time_set = 1;
		time /= movestogo;
		time -= 50;
		info->stop_time = info->start_time + time + inc;
	}

	if (depth == -1) {
		info->depth = MAX_SEARCH_DEPTH;
	}

	printf("time:%d start:%d stop:%d depth:%d timeset:%d\n", time, info->start_time, info->stop_time, info->depth, info->time_set);

	Search_Position(board, info);
}

// position fen fenstr
// position startpos
// ... moves e2e4 e7e5 b7b8q
void Parse_Position(char* lineIn, BOARD_STRUCT *board) {

	Clear_Undo_List(board);

	lineIn += 9;
	char *ptrChar = lineIn;

	if (strncmp(lineIn, "startpos", 8) == 0){
		Parse_Fen(START_FEN, board);
	}
	else {
		ptrChar = strstr(lineIn, "fen");
		if (ptrChar == NULL) {
			Parse_Fen(START_FEN, board);
		}
		else {
			ptrChar += 4;
			Parse_Fen(ptrChar, board);
		}
	}
	//Adding additional moves
	ptrChar = strstr(lineIn, "moves");
	int parsed_move;
	if (ptrChar != NULL) {
		ptrChar += 6;
		while (*ptrChar) {
			parsed_move = Parse_Move(ptrChar, board);
			if (parsed_move == 0) break;
			Make_Move(parsed_move, board);
			while (*ptrChar && *ptrChar != ' ') ptrChar++;
			ptrChar++;
		}
	}

	board->hply = 0;
}

void Uci_Loop(BOARD_STRUCT *board, SEARCH_INFO_STRUCT *info) {


	setvbuf(stdin, NULL, _IONBF, NULL);
	setvbuf(stdout, NULL, _IONBF, NULL);

	char line[INPUTBUFFER];
	printf("id name %s\n", PROGRAM_NAME);
	printf("id author %s\n", AUTHOR);
	//printf("option name Hash type spin default 64 min 4 max 2048\n");
	printf("uciok\n");

	int MB = 64;

	while (1) {
		memset(&line[0], 0, sizeof(line));
		fflush(stdout);
		if (!fgets(line, INPUTBUFFER, stdin))
			continue;

		if (line[0] == '\n')
			continue;

		if (!strncmp(line, "isready", 7)) {
			printf("readyok\n");
			continue;
		}
		else if (!strncmp(line, "position", 8)) {
			Parse_Position(line, board);
		}
		else if (!strncmp(line, "ucinewgame", 10)) {
			Parse_Position("position startpos\n", board);
		}
		else if (!strncmp(line, "go", 2)) {
			printf("Seen Go..\n");
			Parse_Go(line, info, board);
		}
		else if (!strncmp(line, "quit", 4)) {
			info->quit = 1;
			break;
		}
		else if (!strncmp(line, "uci", 3)) {
			printf("id name %s\n", PROGRAM_NAME);
			printf("id author %s\n", AUTHOR);
			printf("uciok\n");
		}
		else if (!strncmp(line, "debug", 4)) {
			//DebugAnalysisTest(debug, info);
			break;
		}
		/*
		else if (!strncmp(line, "setoption name Hash value ", 26)) {
			sscanf_s(line, "%*s %*s %*s %*s %d", &MB);
			if (MB < 4) MB = 4;
			if (MB > 2048) MB = 2048;
			printf("Set Hash to %d MB\n", MB);
			InitHashTable(pos->HashTable, MB);
		}
		*/
		if (info->quit) break;
	}
}

//Parses a move from the uci and returns the move integer
int Parse_Move(char *ptrChar, BOARD_STRUCT *board) {

	Check_Board(board);

	if (ptrChar[1] > '8' || ptrChar[1] < '1') return 0;
	if (ptrChar[3] > '8' || ptrChar[3] < '1') return 0;
	if (ptrChar[0] > 'h' || ptrChar[0] < 'a') return 0;
	if (ptrChar[2] > 'h' || ptrChar[2] < 'a') return 0;

	int from64 = RANK_FILE_TO_SQUARE_64((int)(ptrChar[1] - '1'), (int)(ptrChar[0] - 'a'));
	int to64 = RANK_FILE_TO_SQUARE_64((int)(ptrChar[3] - '1'), (int)(ptrChar[2] - 'a'));
	int from120 = SQUARE_64_TO_120(from64);
	int to120 = SQUARE_64_TO_120(to64);

	ASSERT(ON_BOARD_64(from64) && ON_BOARD_64(to64));

	MOVE_LIST_STRUCT move_list;
	Generate_Moves(board, &move_list);

	int index = 0;
	int move_num = 0;
	int PromPce = EMPTY;

	for (index = 0; index < move_list.num; index++) {
		move_num = move_list.list[index].move;
		if (GET_FROM_SQ(move_num) == from120 && GET_TO_SQ(move_num) == to120) 
		{
			if (IS_PROMOTION(move_num)) 
			{
				if (IS_ROOK_PROMOTION(move_num) && ptrChar[4] == 'r') {
					return move_num;
				}
				else if (IS_BISHOP_PROMOTION(move_num) && ptrChar[4] == 'b') {
					return move_num;
				}
				else if (IS_QUEEN_PROMOTION(move_num) && ptrChar[4] == 'q') {
					return move_num;
				}
				else if (IS_KNIGHT_PROMOTION(move_num) && ptrChar[4] == 'n') {
					return move_num;
				}
				continue;
			}
			return move_num;
		}
	}

	return 0;
}