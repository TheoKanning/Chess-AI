/* search.c
* Contains iterative deepening, alpha-beta and other search functions
* Theo Kanning 12/10/14
*/
#include "globals.h"
#include "time.h"
#include "windows.h"
#include "crtdbg.h"

using namespace std;

typedef enum
{
	NOT_PV,PV
};


//Searches a given position using board and search info 
int Search_Position(BOARD_STRUCT *board, SEARCH_INFO_STRUCT *info)
{
	int currentDepth, score;
	int previous_node_count = 0;
	int depth_start_time;
	PV_LIST_STRUCT pv_list;
	MOVE_STRUCT best_move;

	Clear_PV_List(&pv_list);

	board->hply = 0;

	//Reset stop flag
	info->stopped = 0;
	info->nodes = 0;

	info->null_available = 1;

	Clear_History_Data(board);

	for (currentDepth = 1; currentDepth <= info->depth; ++currentDepth)
	{		
		info->nodes = 0; //Reset node count befor each iteration
		depth_start_time = Get_Time_Ms();

		score = Alpha_Beta(-INF, INF, currentDepth, PV, board, info);

		if (info->stopped == 1) {
			break;
		}

		/***** Get PV Line *****/
		Get_PV_Line(currentDepth, &pv_list, board);
		if (pv_list.list[0].move != 0) Copy_Move(&pv_list.list[0], &best_move); //Store best move found

		//Print search info
		printf("info ");
		if (IS_MATE(score))
		{
			printf("score mate %d", (score > 0) ? (MATE_SCORE - score) / 2 + 1 : -(MATE_SCORE + score) / 2 - 1);
		}
		else
		{
			printf("score cp %d", score);
		}
		
		//Remaining info
			printf(" depth %d nodes %ld time %d nps %d ",
				currentDepth, info->nodes, Get_Time_Ms() - info->start_time, (int)(info->nodes / ((Get_Time_Ms() - depth_start_time + 1) / 1000.0)));
		
		//Print branching factor
		if (previous_node_count != 0)
		{
			//printf("\n\rBranching factor: %f\n\r", info->nodes / (float)previous_node_count);
		}
		previous_node_count = info->nodes;

		//Print current pv line
		printf("pv ");
		Print_PV_List(&pv_list);
		printf("\n\n");

		//End if time is one third gone because next depth is unlikely to finish
		if ((Get_Time_Ms() - info->start_time) >= ((info->stop_time - info->start_time) / 3.0)) info->stopped = 1;

		//if (IS_MATE(score)) break; //End search if mate found
		if (IS_MATE(score) && (currentDepth >= ((score > 0) ? MATE_SCORE - score : MATE_SCORE + score))) break; //End search if mate found
		
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
		score = Alpha_Beta(-INF, INF, i, NOT_PV, board, info);
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
int Alpha_Beta(int alpha, int beta, int depth, int is_pv, BOARD_STRUCT *board, SEARCH_INFO_STRUCT *info)
{
	int move;
	int alpha_orig = alpha;
	int best_score = -INF;
	int best_move = 0;
	int score = -MATE_SCORE; //Set in case no moves are available
	int mate = 1; //If no legal moves are found
	int f_prune_allowed = 0; //If futility pruning is allowed at this node
	MOVE_LIST_STRUCT move_list;
	int current_move;
	int current_move_score;
	HASH_ENTRY_STRUCT hash_entry;
	hash_entry.move = 0;
	int valid = 0;

	info->nodes++;
	//Check for timeout
	if ((info->nodes & 4095) == 0) //every 4096 nodes
	{
		if (Get_Time_Ms() > info->stop_time - 20) //Could be reduced to 10 ms
		{
			info->stopped = 1;
			return 0;
		}
	}

	/***** Draw Detection *****/
	if (board->move_counter >= 100 || (board->total_material == 0) || (board->hply && Is_Repetition(board))) //)
	{
		return 0;
	}

	/***** Check Test *****/
	int in_check = In_Check(board->side, board);
	if (in_check) depth++;

	/***** Leaf Node Response *****/
	//If at depth 0, extend if in check, start quiescent search if not
	if (depth <= 0)
	{
		return Quiescent_Search(alpha, beta, board, info);
	}

	/***** Check hash table *****/
	int value = Get_Hash_Entry(board->hash_key, alpha, beta, depth, board->hply, &hash_entry.move);
	if (value != INVALID) 
	{
		if (!is_pv || (value > alpha && value < beta)) //Only return exact values in pv line
		{
			//if ((!is_pv && board->hply > 2) || !Draw_Error_Found(hash_entry.move, board)) //Search for draw errors if searching higher depths, or if in pv line
			//{
				info->hash_hits++;
				return value; 
			//}
		}
	}
	
	/***** Null Move *****/
	if (depth >= 4 
	&& info->null_available 
	&& !is_pv 
	&& board->hply 
	&& !in_check 
	&& board->total_material >= 2000)
	{
		info->null_available = 0;
		Make_Null_Move(board);
		score = -Alpha_Beta(-beta, -beta + 1, depth - 3, NOT_PV, board, info); //Subtract an additional 2 ply from depth
		Take_Null_Move(board);
		info->null_available = 1;

		if (score >= beta) return score;
	}

	/***** Futility Pruning *****/
	/* Here we determine if this node is elegible for futility pruning */
	int fmargin[4] = { 0, 300, 500, 900 };

	if (depth <= 2
		&& !is_pv
		&& !in_check
		&& abs(alpha) < 9000 //Not searching for a mate
		&& Evaluate_Board(board) + fmargin[depth] <= alpha)
		f_prune_allowed = 1;

	/***** Move generation and sorting *****/
	Generate_Moves(board, &move_list);
	Find_PV_Move(hash_entry.move, &move_list); //Find hash move if available

	for (move = 0; move < move_list.num; move++) //For all moves in list
	{
		/***** Get best move *****/
		Get_Next_Move(move, &move_list);//Moves next move into move_index position
		current_move = move_list.list[move].move;
		current_move_score = move_list.list[move].score;

		if (current_move == 0) break; //End if no moves are available

		if (!Make_Move(current_move, board)) continue;//If move is unsuccessful, try next move

		mate = 0; //A move has been made

		//See if current move leads to check, important for pruning and reductions
		int checking_move = In_Check(board->side, board);

		/***** Futility Pruning *****/
		if (f_prune_allowed
			&&  !IS_CAPTURE(current_move)
			&& !IS_PROMOTION(current_move)
			&& !checking_move) {
			Take_Move(board);
			continue;
		}

		/***** Principal Variation Search *****/
		if (move == 0) //If first move, use full window
		{
			score = -Alpha_Beta(-beta, -alpha, depth - 1, is_pv, board, info);
		}
		else //If not first move
		{
			/***** Late move reduction *****/
			/* Does a reduced search if the move is eligible, otherwise goes straight into PVS search */
			/* Moves that raise alpha are re-searched in PVS */
			if (move >= LATE_MOVE_NUM 
			&& !in_check 
			&& CAN_REDUCE(current_move) 
			&& depth >= REDUCTION_LIMIT
			&& !IS_KILLER(move_list.list[move].score)
			&& !checking_move)
			{
				score = -Alpha_Beta(-alpha - 1, -alpha, depth - 1 - LATE_MOVE_REDUCTION, NOT_PV, board, info); //Null window search
			}
			else
			{
				score = alpha + 1; //Set score above alpha to automatically trigger PVS next
			}

			if (score > alpha) //Continue with PVS search
			{
				score = -Alpha_Beta(-alpha - 1, -alpha, depth - 1, NOT_PV, board, info); //Null window search

				//If move improves alpha but does not cause a cutoff, and if not in a null search already
				if (alpha < score && beta > score && (beta - alpha > 1))
				{
					score = -Alpha_Beta(-beta, -alpha, depth - 1, is_pv, board, info);
				}
			}
		}
		

		Take_Move(board);

		//Check if search ended while searching
		if (info->stopped)
		{
			return 0;
		}

		//Update beta, storing hash entry if cutoff is found
		if (score >= beta)
		{
			//Store hash entry
			Fill_Hash_Entry(info->age, depth, score, HASH_LOWER, board->hash_key, current_move, &hash_entry);
			Add_Hash_Entry(&hash_entry, board->hply, info);

			//if move is not a capture or promotion
			if (current_move_score <= KILLER_MOVE_SCORE)
			{				 
				Add_Killer_Move(current_move, board); //Store killer move
				Add_History_Move(current_move, board); //Store move in history array
			}
			return score; //Beta cutoff
		}
		//Update best score and best move for hash entry later
		if (score > best_score)
		{
			//Update alpha
			if (score > alpha)
			{
				alpha = score;
				//Add_History_Move(current_move, board); //Store move in history array
			}
			best_score = score;
			best_move = current_move;
		}

	} //End looping through all moves

	/***** Mate detection *****/
	if (mate) //No moves found
	{
		if (in_check)
		{
			//Store in table
			best_score = -MATE_SCORE + board->hply;
			Fill_Hash_Entry(info->age, depth, best_score, HASH_EXACT, board->hash_key, 0, &hash_entry);
			Add_Hash_Entry(&hash_entry, board->hply, info);
			return best_score; //Losing checkmate
		}
		//Store in table
		Fill_Hash_Entry(info->age, depth, 0, HASH_EXACT, board->hash_key, 0, &hash_entry);
		Add_Hash_Entry(&hash_entry, board->hply, info);
		return 0; //Draw
	}

	//For exact and upper nodes
	if (best_score > alpha_orig)
	{
		//Store hash entry
		Fill_Hash_Entry(info->age, depth, best_score, HASH_EXACT, board->hash_key, best_move, &hash_entry);
		Add_Hash_Entry(&hash_entry, board->hply, info);
	}
	else
	{
		//Store hash entry
		Fill_Hash_Entry(info->age, depth, best_score, HASH_UPPER, board->hash_key, best_move, &hash_entry);
		Add_Hash_Entry(&hash_entry, board->hply, info);
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

	if ((info->nodes & 4095) == 0) //every 4096 nodes
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
		Get_Next_Capture_Move(move, &move_list);
		next_move = move_list.list[move].move;

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

//Performs the given move at a position, checks for a draw, the performs all moves at that position and checks for draws
int Draw_Error_Found(int move, BOARD_STRUCT *board)
{
	int count, next_move;
	int draw = 0;
	MOVE_LIST_STRUCT move_list;
	HASH_ENTRY_STRUCT hash_entry;
	int ply = board->hply;
	U64 hash = board->hash_key;

	//Make hash move
	if (!Make_Move(move, board)) return 0;
	//Check first ply
	
	if (board->move_counter >= 100 || Is_Threefold_Repetition(board))
	{
		Take_Move(board);
		ASSERT(ply == board->hply);
		ASSERT(hash == board->hash_key);
		return 1;
	}

	//Check all possible opponent moves for draws, starting with hash move
	Generate_Moves(board, &move_list);
	//Find hash move
	Get_Hash_Entry(board->hash_key, 0, 0, 0, board->hply, &hash_entry.move);
	Find_PV_Move(hash_entry.move, &move_list);

	//Search all moves in sorted order, return 1 if draw is found
	count = 0;

	while( count < move_list.num && !draw)
	{
		Get_Next_Move(count, &move_list);
		next_move = move_list.list[count].move;

		if (Make_Move(next_move, board))
		{
			if (board->move_counter >= 100 || Is_Threefold_Repetition(board))
			{
				draw = 1;
			}
			
			if (board->hply != (ply + 2))
			{
				printf("board->hply: %d, ply %d\n\r", board->hply, ply);
			}

			ASSERT(board->hply == (ply + 2));

			Take_Move(board);
		}
		count++;
	}

	Take_Move(board);

	ASSERT(ply == board->hply);
	ASSERT(hash == board->hash_key);

	return draw;
}