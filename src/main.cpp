#include <stdio.h>
#include <iostream>
#include "globals.h"

#define ERR_POS "position startpos moves c2c4 e7e5 b1c3 b8c6 g1f3 g8f6 d2d4 e5d4 f3d4 f8b4 d4c6 b4c3 b2c3 d7c6 d1d8 e8d8 f2f3 c8e6 e2e4 d8c8 c1f4 c8b8 f1e2 c6c5 f4e3 f6d7 e1g1 d7e5 e3c5 e6c4 e2c4 e5c4 f1d1 h8g8 d1d4 c4e5 f3f4 e5g6 g2g3 g8e8 d4d7 g6h8 a1d1 b7b6 c5d4 e8e4 d4g7 b8b7 f4f5 e4e2 d7d8 e2e8 d8d4 e8e2 d4h4 a8e8 h4h7 e2e1 d1e1 e8e1 g1f2 e1a1 g7h8"

/* 

2014-12-17 23:40:53.179-->2:position startpos moves c2c4 e7e5 b1c3 b8c6 g1f3 g8f6 d2d4 e5d4 f3d4 f8b4 d4c6 b4c3 b2c3 d7c6 d1d8 e8d8 f2f3 c8e6 e2e4 d8c8 c1f4 c8b8 f1e2 c6c5 f4e3 f6d7 e1g1 d7e5 e3c5 e6c4 e2c4 e5c4 f1d1 h8g8 d1d4 c4e5 f3f4 e5g6 g2g3 g8e8 d4d7 g6h8 a1d1 b7b6 c5d4 e8e4 d4g7 b8b7 f4f5 e4e2 d7d8 e2e8 d8d4 e8e2 d4h4 a8e8 h4h7 e2e1 d1e1 e8e1 g1f2 e1a1 g7h8
2014-12-17 23:40:53.179-->2:go wtime 47116 btime 22857 winc 1000 binc 1000
2014-12-17 23:40:53.181<--2:    A B C D E F G H
2014-12-17 23:40:53.183<--2:   -----------------
2014-12-17 23:40:53.185<--2: 8 ³ ³ ³ ³ ³ ³ ³ ³B³
2014-12-17 23:40:53.187<--2:   -----------------
2014-12-17 23:40:53.189<--2: 7 ³p³k³p³ ³ ³p³ ³R³
2014-12-17 23:40:53.191<--2:   -----------------
2014-12-17 23:40:53.193<--2: 6 ³ ³p³ ³ ³ ³ ³ ³ ³
2014-12-17 23:40:53.195<--2:   -----------------
2014-12-17 23:40:53.197<--2: 5 ³ ³ ³ ³ ³ ³P³ ³ ³
2014-12-17 23:40:53.199<--2:   -----------------
2014-12-17 23:40:53.201<--2: 4 ³ ³ ³ ³ ³ ³ ³ ³ ³
2014-12-17 23:40:53.203<--2:   -----------------
2014-12-17 23:40:53.205<--2: 3 ³ ³ ³P³ ³ ³ ³P³ ³
2014-12-17 23:40:53.207<--2:   -----------------
2014-12-17 23:40:53.209<--2: 2 ³P³ ³ ³ ³ ³K³ ³P³
2014-12-17 23:40:53.209<--2:   -----------------
2014-12-17 23:40:53.211<--2: 1 ³r³ ³ ³ ³ ³ ³ ³ ³
2014-12-17 23:40:53.213<--2:   -----------------
2014-12-17 23:40:53.215<--2:Side: Black
2014-12-17 23:40:53.217<--2:Castle: 
2014-12-17 23:40:53.219<--2:EP: None
2014-12-17 23:40:53.221<--2:Hashkey: 18070127746767201836
2014-12-17 23:40:53.223<--2:50 Move Count: 0
2014-12-17 23:40:53.225<--2:Hply: 0
2014-12-17 23:40:53.227<--2:Eval: 0
2014-12-17 23:40:53.229<--2:Seen Go..
2014-12-17 23:40:53.231<--2:time:711 start:125466 stop:127177 depth:64 timeset:1
2014-12-17 23:40:53.233<--2:info score cp -290 depth 1 nodes 39 time 0 pvRxa2 
2014-12-17 23:40:53.236<--2:info score cp -320 depth 2 nodes 122 time 2 pvRxa2 Kg1 
2014-12-17 23:40:53.239<--2:ERROR!! Assert board->hash_key == hash_temp failed
2014-12-17 23:40:53.241<--2: on line 471
2014-12-17 23:40:53.243<--2: in file c:\users\theo\documents\chess_ai\dildozer-chess-ai\dildozer\board.cpp
2014-12-17 23:41:17.171-->2:stop
2014-12-17 23:41:17.197<--2:Press any key to continue . . . 
2014-12-17 23:41:17.198<--2:ERROR!! Assert board->board_array120[from120] != EMPTY failed
2014-12-17 23:41:17.198<--2: on line 478
2014-12-17 23:41:17.199<--2: in file c:\users\theo\documents\chess_ai\dildozer-chess-ai\dildozer\make_move.cpp
2014-12-17 23:41:17.242<--2:Press any key to continue . . . 
2014-12-17 23:41:17.263<--2:ERROR!! Assert board->hash_key == hash failed
2014-12-17 23:41:17.284<--2: on line 456
2014-12-17 23:41:17.305<--2: in file c:\users\theo\documents\chess_ai\dildozer-chess-ai\dildozer\make_move.cpp
2014-12-17 23:41:17.305<--2:Press any key to continue . . . 
2014-12-17 23:41:17.326<--2:ERROR!! Assert ON_BOARD_120(from120) failed
2014-12-17 23:41:17.347<--2: on line 350
2014-12-17 23:41:17.368<--2: in file c:\users\theo\documents\chess_ai\dildozer-chess-ai\dildozer\make_move.cpp
2014-12-17 23:41:17.388<--2:Press any key to continue . . . 
2014-12-17 23:41:17.409<--2:ERROR!! Assert ON_BOARD_120(to120) failed
2014-12-17 23:41:17.409<--2: on line 351
2014-12-17 23:41:17.430<--2: in file c:\users\theo\documents\chess_ai\dildozer-chess-ai\dildozer\make_move.cpp
2014-12-17 23:41:17.451<--2:Press any key to continue . . . 
2014-12-17 23:41:17.451<--2:ERROR!! Assert (EMPTY < piece) && (piece <= bK) failed
2014-12-17 23:41:17.451<--2: on line 352
2014-12-17 23:41:17.472<--2: in file c:\users\theo\documents\chess_ai\dildozer-chess-ai\dildozer\make_move.cpp

*/



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
	Perft_Test("3K4/8/8/8/8/8/4p3/2k2R2 b - - 0 1", 6, &board);
	
	
	/*
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
	*/
	
	
	system("PAUSE");
}