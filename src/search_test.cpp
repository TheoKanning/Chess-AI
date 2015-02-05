/* search_test.cpp
* Functions for testing search features in a variety of positions
* Theo Kanning 2/5/15
*/

#include "globals.h"

#define NUM_POSITIONS			6
#define TEST_TIME				10000 //Time (ms) to search each position

char *fens[NUM_POSITIONS] = {
	"rnbqkb1r/pp1p1ppp/2p5/4P3/2B5/8/PPP1NnPP/RNBQK2R w KQkq - 0 6",
	"rnbqkb1r/pp1p1ppp/2p5/4P3/2B5/8/PPP1NnPP/RNBQK2R w KQkq - 0 6",
	"rnbqkb1r/pp1p1ppp/2p5/4P3/2B5/8/PPP1NnPP/RNBQK2R w KQkq - 0 6",
	"rnbqkb1r/pp1p1ppp/2p5/4P3/2B5/8/PPP1NnPP/RNBQK2R w KQkq - 0 6",
	"rnbqkb1r/pp1p1ppp/2p5/4P3/2B5/8/PPP1NnPP/RNBQK2R w KQkq - 0 6",
	"rnbqkb1r/pp1p1ppp/2p5/4P3/2B5/8/PPP1NnPP/RNBQK2R w KQkq - 0 6"
};

//Main loop that calls searches of all smaller functions
void Search_Test(void)
{
	BOARD_STRUCT board;
	SEARCH_INFO_STRUCT info;
	int test_start_time = Get_Time_Ms();
	long total_nodes = 0;
	int pos_nodes;
	float pos_branching_factor;
	int prev_nodes;
	int depth;

	printf("\n\nSearch Test Started\n%d positions\n%d seconds per position\n", NUM_POSITIONS, TEST_TIME / 1000);

	//Loop through all positions
	for (int pos = 0; pos < NUM_POSITIONS; pos++)
	{
		//Parse position
		Parse_Fen(fens[pos], &board);

		//Prepare for iterative deepening loop
		Clear_Pawn_Hash_Table();
		Clear_Hash_Table();
		board.hply = 0;
		info.stopped = 0;
		info.nodes = 0;
		info.hash_hits = 0;
		info.start_time = Get_Time_Ms();
		info.stop_time = info.start_time + TEST_TIME;

		//Variables to store for this position
		pos_nodes = 0;
		pos_branching_factor = 0;
		prev_nodes = 0;

		//Iterative deepening
		for (depth = 1; depth < MAX_SEARCH_DEPTH; depth++)
		{
			info.nodes = 0;

			Alpha_Beta(-INF, INF, depth, 1, &board, &info);

			if (info.stopped)//Print results if finished
			{
				break;  
			}
			else //Update data at start next depth
			{
				pos_nodes += info.nodes;
				if (prev_nodes != 0) pos_branching_factor += (info.nodes / (1.0*prev_nodes));
				prev_nodes = info.nodes;
			}
		}

		//Print results after position
		printf("Position %d: Depth:%d Nodes:%d Nps:%d Branching:%f\n", pos, depth - 1, pos_nodes, pos_nodes / ((Get_Time_Ms() - info.start_time) / 1000), pos_branching_factor / (depth - 1));
	}
}