/* search.c
* Contains iterative deepening, alpha-beta and other search functions
* Theo Kanning 12/10/14
*/
#include "globals.h"
#include "time.h"
#include "windows.h"
#include "crtdbg.h"

using namespace std;

//Searches a given position using board and search info 
int Search_Position(BOARD_STRUCT *board, SEARCH_INFO_STRUCT *info)
{
	int currentDepth, score;
	PV_LIST_STRUCT pv_list;
	MOVE_STRUCT best_move;

	Clear_PV_List(&pv_list);

	board->hply = 0;

	//Reset stop flag
	info->stopped = 0;
	info->nodes = 0;

	for (currentDepth = 1; currentDepth <= info->depth; ++currentDepth) 
	{		
		score = Alpha_Beta(-INF, INF, currentDepth, board, info);

		if (info->stopped == 1) {
			break;
		}

		Get_PV_Line(currentDepth, &pv_list, board);

		Copy_Move(&pv_list.list[0], &best_move); //Store best move found

		//Print search info
			printf("info score cp %d depth %d nodes %ld time %d ",
				score, currentDepth, info->nodes, Get_Time_Ms() - info->start_time);
		
		//Print current pv line
		
		printf("pv ");
		Print_PV_List(&pv_list);
		printf("\n");
		
		if (pv_list.num < currentDepth)
		{
			Get_PV_Line(currentDepth, &pv_list, board);
		}

		//printf("Hits:%d Overwrite:%d NewWrite:%d Cut:%d\nOrdering %.2f NullCut:%d\n",pos->HashTable->hit,pos->HashTable->overWrite,pos->HashTable->newWrite,pos->HashTable->cut,
		//(info->fhf/info->fh)*100,info->nullCut);
	}

	
	printf("bestmove %s\n", UCI_Move_String(&best_move));
	
	info->age++;

	return 1;
}


//Calls search functions at increasing depths
int Iterative_Deepening(int depth, BOARD_STRUCT *board, SEARCH_INFO_STRUCT *info)
{
	int i, score;
	score = 0;
	time_t start, stop;
	PV_LIST_STRUCT pv_list;

	info->start_time = Get_Time_Ms();
	info->stop_time = 10000000;
	info->stopped = 0;

	for (i = 1; i <= depth; i++) //Search at each depth
	{
		info->nodes = 0;
		info->hash_hits = 0;
		time(&start);
		score = Alpha_Beta(-INF, INF, i, board, info);
		time(&stop);
		
		//Print Data
		cout << "Depth:" << i << " ";
		Get_PV_Line(i, &pv_list, board);
		Print_PV_List(&pv_list);
		cout << " Score:" << score/100.0 << " Time:" << stop - start << " seconds Nodes: " << info->nodes << " Hits: " << info->hash_hits << endl;
		cout << "Nodes/sec: " << info->nodes * 1000 / (Get_Time_Ms() - info->start_time) << endl;
	}
	//If checkmate found
	if (score == INF)
	{
		cout << "Black Checkmated" << endl;
	}
	else if (score == -INF)
	{
		cout << "White Checkmated" << endl;
	}
	return 1;
}

//Alpha beta implementation in negamax search
int Alpha_Beta(int alpha, int beta, int depth, BOARD_STRUCT *board, SEARCH_INFO_STRUCT *info)
{
	int move;
	int alpha_orig = alpha;
	int best_score = -INF;
	int best_move = 0;
	int score = -INF; //Set in case no moves are available
	int mate = 1; //If no legal moves are found
	MOVE_LIST_STRUCT move_list;
	int next_move;
	HASH_ENTRY_STRUCT hash_entry;
	hash_entry.move = 0;
	int valid = 0;


	info->nodes++;
	//Check for timeout
	if ((info->nodes % 5000) == 0) //every 5000 nodes
	{
		if (Get_Time_Ms() > info->stop_time - 50)
		{
			info->stopped = 1;
			return 0;
		}
	}

	//Check for 50 move rule and threefold repetition
	if (board->move_counter >= 100 || Is_Threefold_Repetition(board) || (board->total_material == 0))
	{
		return 0;
	}

	//If at depth 0, extend if in check, start quiescent search if not
	if (depth == 0)
	{
		if (In_Check(board->side, board))
		{
			depth++;
		}
		else
		{
			return Quiescent_Search(alpha, beta, board, info);
		}
	}


	//Check hash table 
	valid = Get_Hash_Entry(board->hash_key, &hash_entry);
	//If match is found and at greater depth
	if (valid && (hash_entry.depth > depth))
	{
		//Check if stored move results in a draw
		//Check for draw
		if (Make_Move(hash_entry.move, board))
		{
			int three_rep = Is_Threefold_Repetition(board);
			Take_Move(board); //take move before removing entry
			
			if(three_rep)
			{
				//Remove hash entry from table
				Remove_Hash_Entry(board->hash_key);

				//Clear local hash entry
				hash_entry.move = 0;
				hash_entry.flag = HASH_EMPTY;
			}
		}

		info->hash_hits++;
		if (hash_entry.flag == HASH_EXACT)
		{
			return hash_entry.eval;
		}
		else if (hash_entry.flag == HASH_LOWER)
		{
			alpha = max(alpha, hash_entry.eval);
		}
		else if (hash_entry.flag == HASH_UPPER)
		{
			beta = min(beta, hash_entry.eval);
		}
		//Check for cutoff
		if (alpha >= beta)
		{
			return hash_entry.eval;
		}
	}


	Generate_Moves(board, &move_list);
	//Find hash move if available
	Find_PV_Move(hash_entry.move, &move_list);

	for (move = 0; move < move_list.num; move++) //For all moves in list
	{
		next_move = Get_Next_Move(&move_list);

		if (next_move == 0) break;

		if (Make_Move(next_move, board)) //If move is successful
		{
			score = -Alpha_Beta(-beta, -alpha, depth - 1, board, info);
			
			mate = 0; //A move has been made
			Take_Move(board);

			if (info->stopped)
			{
				return 0;
			}

			//Update beta, storing hash entry if cutoff is found
			if (score >= beta)
			{
				//Store hash entry
				Fill_Hash_Entry(info->age, depth, score, HASH_LOWER, board->hash_key, next_move, &hash_entry);
				Add_Hash_Entry(&hash_entry, info);
				return beta; //Beta cutoff
			}
			//Update alpha, storing hash if improvement is found
			if (score > alpha)
			{
				alpha = score;
			}
			//Update best score and best move for hash entry later
			if (score > best_score)
			{
				best_score = score;
				best_move = next_move;
			}
		}
	}

	/***** Mate detection *****/
	if (mate) //No moves found
	{
		if (In_Check(board->side, board))
		{
			return -INF + board->hply; //Losing checkmate
		}
		return 0; //Draw
	}

	//For exact and upper nodes
	if (best_score > alpha_orig)
	{
		//Store hash entry
		Fill_Hash_Entry(info->age, depth, best_score, HASH_EXACT, board->hash_key, best_move, &hash_entry);
		Add_Hash_Entry(&hash_entry, info);
	}
	else
	{
		//Store hash entry
		Fill_Hash_Entry(info->age, depth, best_score, HASH_UPPER, board->hash_key, best_move, &hash_entry);
		Add_Hash_Entry(&hash_entry, info);
	}

	
	return alpha;
}

//Quiescent search to find positions suitable for evaluation
int Quiescent_Search(int alpha, int beta, BOARD_STRUCT *board, SEARCH_INFO_STRUCT *info)
{
	int move;
	MOVE_LIST_STRUCT move_list;
	int next_move;
	int score = Evaluate_Board(board);
	
	info->nodes++;

	if ((info->nodes % 5000) == 0) //every 5000 nodes
	{
		if (Get_Time_Ms() > info->stop_time - 50)
		{
			info->stopped = 1;
			return 0;
		}
	}


	if (score >= beta) return beta;
	if (alpha < score) alpha = score;

	Generate_Moves(board, &move_list);

	for (move = 0; move < move_list.num; move++) //For all moves in list
	{
		next_move = Get_Next_Capture_Move(&move_list);

		if (next_move == 0) break; 

		if (Make_Move(next_move, board)) //If move is successful
		{
			score = -Quiescent_Search(-beta, -alpha,  board, info);
			Take_Move(board);
		}
		if (info->stopped)
		{
			return 0;
		}
		if (score >= beta)
		{
			return beta; //Beta cutoff
		}
		if (score > alpha)
		{
			alpha = score;
		}
	}

	return alpha;
}

//Returns current time in ms
int Get_Time_Ms(void)
{
	return clock() * 1000 / CLOCKS_PER_SEC;
}