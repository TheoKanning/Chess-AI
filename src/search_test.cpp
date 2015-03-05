/* search_test.cpp
* Functions for testing search features in a variety of positions
* Theo Kanning 2/5/15
*/

#include "globals.h"

#define NUM_POSITIONS			53
#define TEST_TIME				1000 //Time (ms) to search each position

char *fens[NUM_POSITIONS] = {
	"rnbqkb1r/pp1p1ppp/2p5/4P3/2B5/8/PPP1NnPP/RNBQK2R w KQkq - 0 6",
	"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 6",
	"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
	"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",
	"1k1r4/pp1b1R2/3q2pp/4p3/ 2B5/4Q3/PPP2B2/2K5 b - - 0 0",
	"3r1k2/4npp1/1ppr3p/p6P/P2PPPP1/1NR5/5K2/2R5 w - - 0 0",
	"2q1rr1k/3bbnnp/p2p1pp1/2pPp3/PpP1P1P1/1P2BNNP/2BQ1PRK/7R b - - 0 0",
	"rnbqkb1r/p3pppp/1p6/2ppP3/3N4/2P5/PPP1QPPP/R1B1KB1R w KQkq - - 0 0",
	"r1b2rk1/2q1b1pp/p2ppn2/1p6/3QP3/1BN1B3/PPP3PP/R4RK1 w - - 0 0",
	"2r3k1/pppR1pp1/4p3/4P1P1/5P2/1P4K1/P1P5/8 w - - 0 0",
	"1nk1r1r1/pp2n1pp/4p3/q2pPp1N/b1pP1P2/B1P2R2/2P1B1PP/R2Q2K1 w - - 0 0",
	"4b3/p3kp2/6p1/3pP2p/2pP1P2/4K1P1/P3N2P/8 w - - 0 0",
	"2kr1bnr/pbpq4/2n1pp2/3p3p/3P1P1B/2N2N1Q/PPP3PP/2KR1B1R w - - 0 0",
	"3rr1k1/pp3pp1/1qn2np1/8/3p4/PP1R1P2/2P1NQPP/R1B3K1 b - - 0 0",
	"2r1nrk1/p2q1ppp/bp1p4/n1pPp3/P1P1P3/2PBB1N1/4QPPP/R4RK1 w - - 0 0",
	"r3r1k1/ppqb1ppp/8/4p1NQ/8/2P5/PP3PPP/R3R1K1 b - - 0 0",
	"r2q1rk1/4bppp/p2p4/2pP4/3pP3/3Q4/PP1B1PPP/R3R1K1 w - - 0 0",
	"rnb2r1k/pp2p2p/2pp2p1/q2P1p2/8/1Pb2NP1/PB2PPBP/R2Q1RK1 w - - 0 0",
	"2r3k1/1p2q1pp/2b1pr2/p1pp4/6Q1/1P1PP1R1/P1PN2PP/5RK1 w - - 0 0",
	"r1bqkb1r/4npp1/p1p4p/1p1pP1B1/8/1B6/PPPN1PPP/R2Q1RK1 w kq - - 0 0",
	"r2q1rk1/1ppnbppp/p2p1nb1/3Pp3/2P1P1P1/2N2N1P/PPB1QP2/R1B2RK1 b - - 0 0",
	"r1bq1rk1/pp2ppbp/2np2p1/2n5/P3PP2/N1P2N2/1PB3PP/R1B1QRK1 b - - 0 0",
	"3rr3/2pq2pk/p2p1pnp/8/2QBPP2/1P6/P5PP/4RRK1 b - - 0 0",
	"r4k2/pb2bp1r/1p1qp2p/3pNp2/3P1P2/2N3P1/PPP1Q2P/2KRR3 w - - 0 0",
	"3rn2k/ppb2rpp/2ppqp2/5N2/2P1P3/1P5Q/PB3PPP/3RR1K1 w - - 0 0",
	"2r2rk1/1bqnbpp1/1p1ppn1p/pP6/N1P1P3/P2B1N1P/1B2QPP1/R2R2K1 b - - 0 0",
	"r1bqk2r/pp2bppp/2p5/3pP3/P2Q1P2/2N1B3/1PP3PP/R4RK1 b kq - - 0 0",
	"r2qnrnk/p2b2b1/1p1p2pp/2pPpp2/1PP1P3/PRNBB3/3QNPPP/5RK1 w - - 0 0",
	"rn1qkb1r/pp2pppp/5n2/3p1b2/3P4/2N1P3/PP3PPP/R1BQKBNR w KQkq - 0 1",
	"rn1qkb1r/pp2pppp/5n2/3p1b2/3P4/1QN1P3/PP3PPP/R1B1KBNR b KQkq - 1 1",
	"r1bqk2r/ppp2ppp/2n5/4P3/2Bp2n1/5N1P/PP1N1PP1/R2Q1RK1 b kq - 1 10",
	"r1bqrnk1/pp2bp1p/2p2np1/3p2B1/3P4/2NBPN2/PPQ2PPP/1R3RK1 w - - 1 12",
	"rnbqkb1r/ppp1pppp/5n2/8/3PP3/2N5/PP3PPP/R1BQKBNR b KQkq - 3 5",
	"rnbq1rk1/pppp1ppp/4pn2/8/1bPP4/P1N5/1PQ1PPPP/R1B1KBNR b KQ - 1 5",
	"r4rk1/3nppbp/bq1p1np1/2pP4/8/2N2NPP/PP2PPB1/R1BQR1K1 b - - 1 12",
	"rn1qkb1r/pb1p1ppp/1p2pn2/2p5/2PP4/5NP1/PP2PPBP/RNBQK2R w KQkq c6 1 6",
	"r1bq1rk1/1pp2pbp/p1np1np1/3Pp3/2P1P3/2N1BP2/PP4PP/R1NQKB1R b KQ - 1 9",
	"rnbqr1k1/1p3pbp/p2p1np1/2pP4/4P3/2N5/PP1NBPPP/R1BQ1RK1 w - - 1 11",
	"rnbqkb1r/pppp1ppp/5n2/4p3/4PP2/2N5/PPPP2PP/R1BQKBNR b KQkq f3 1 3",
	"r1bqk1nr/pppnbppp/3p4/8/2BNP3/8/PPP2PPP/RNBQK2R w KQkq - 2 6",
	"rnbq1b1r/ppp2kpp/3p1n2/8/3PP3/8/PPP2PPP/RNBQKB1R b KQ d3 1 5",
	"rnbqkb1r/pppp1ppp/3n4/8/2BQ4/5N2/PPP2PPP/RNB2RK1 b kq - 1 6",
	"r2q1rk1/2p1bppp/p2p1n2/1p2P3/4P1b1/1nP1BN2/PP3PPP/RN1QR1K1 w - - 1 12",
	"r1bqkb1r/2pp1ppp/p1n5/1p2p3/3Pn3/1B3N2/PPP2PPP/RNBQ1RK1 b kq - 2 7",
	"r2qkbnr/2p2pp1/p1pp4/4p2p/4P1b1/5N1P/PPPP1PP1/RNBQ1RK1 w kq - 1 8",
	"r1bqkb1r/pp3ppp/2np1n2/4p1B1/3NP3/2N5/PPP2PPP/R2QKB1R w KQkq e6 1 7",
	"rn1qk2r/1b2bppp/p2ppn2/1p6/3NP3/1BN5/PPP2PPP/R1BQR1K1 w kq - 5 10",
	"r1b1kb1r/1pqpnppp/p1n1p3/8/3NP3/2N1B3/PPP1BPPP/R2QK2R w KQkq - 3 8",
	"r1bqnr2/pp1ppkbp/4N1p1/n3P3/8/2N1B3/PPP2PPP/R2QK2R b KQ - 2 11",
	"r3kb1r/pp1n1ppp/1q2p3/n2p4/3P1Bb1/2PB1N2/PPQ2PPP/RN2K2R w KQkq - 3 11",
	"r1bq1rk1/pppnnppp/4p3/3pP3/1b1P4/2NB3N/PPP2PPP/R1BQK2R w KQ - 3 7",
	"r2qkbnr/ppp1pp1p/3p2p1/3Pn3/4P1b1/2N2N2/PPP2PPP/R1BQKB1R w KQkq - 2 6",
	"rn2kb1r/pp2pppp/1qP2n2/8/6b1/1Q6/PP1PPPBP/RNB1K1NR b KQkq - 1 6"
};

//Main loop that calls searches of all smaller functions
void Search_Test(void)
{
	BOARD_STRUCT board;
	SEARCH_INFO_STRUCT info;
	int test_start_time = Get_Time_Ms();
	int depth_finish_time = 0;
	long total_nps = 0;
	int pos_nodes;
	float pos_branching_factor;
	int pos_hit_rate;
	float best_index = 0;
	float beta_index = 0;
	float best_index_total = 0;
	float beta_index_total = 0;
	int prev_nodes;
	int depth;

	float total_branching_factor = 0;
	float total_depth = 0;
	int total_hit_rate = 0;


	printf("\n\nSearch Test Started\n%d positions\n%d seconds per position\n", NUM_POSITIONS, TEST_TIME / 1000);

	//Loop through all positions
	for (int pos = 0; pos < NUM_POSITIONS; pos++)
	{
		//Parse position
		Parse_Fen(fens[pos], &board);

		//Prepare for iterative deepening loop
		Clear_Pawn_Hash_Table();
		Clear_Hash_Table();
		Clear_Search_Info(&info);
		Clear_History_Data(&board);
		board.hply = 0;

		info.start_time = Get_Time_Ms();
		info.stop_time = info.start_time + TEST_TIME;

		//Variables to store for this position
		pos_nodes = 0;
		pos_branching_factor = 0;
		pos_hit_rate = 0;
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
				pos_hit_rate = (100 * info.hash_hits) / (info.hash_probes);
				depth_finish_time = Get_Time_Ms();
			}
		}

		//Print results after position
		printf("Position %d:\nDepth:%d Nodes:%d Nps:%d Branching:%f HR:%d\n", pos, depth - 1, pos_nodes, 1000 * (pos_nodes / ((depth_finish_time - info.start_time))), pos_branching_factor / (depth - 1), pos_hit_rate);
		total_branching_factor += pos_branching_factor / (depth - 1);
		total_depth += depth - 1;
		total_nps += 1000 * (pos_nodes / ((depth_finish_time - info.start_time)));
		total_hit_rate += pos_hit_rate;
		Calc_Move_Index_Data(&info, &best_index, &beta_index);
		best_index_total += best_index;
		beta_index_total += beta_index;
		printf("Average Indices: Best Move:%f Beta Cutoff:%f\n", best_index, beta_index);

	}

	//Summary of all positions
	printf("\nSummary Averages:\nDepth:%f NPS:%d Branching:%f HR:%d\n", total_depth / NUM_POSITIONS, total_nps / NUM_POSITIONS, total_branching_factor / NUM_POSITIONS, total_hit_rate / NUM_POSITIONS);
	printf("Average Indices: Best Move:%f Beta Cutoff:%f\n\n", best_index_total / NUM_POSITIONS, beta_index_total / NUM_POSITIONS);
}