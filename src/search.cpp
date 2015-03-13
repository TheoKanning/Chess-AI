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
	int previous_node_count = 0;
	int depth_start_time;
	int window_low, window_high;
	int prev_score = 0;
	PV_LIST_STRUCT pv_list;
	MOVE_STRUCT best_move;

	Clear_PV_List(&pv_list);
	Clear_Search_Info(info);

	board->hply = 0;
	score = 0;

	Clear_History_Data(board);

	for (currentDepth = 1; currentDepth <= info->depth; currentDepth++)
	{		
		info->nodes = 0; //Reset node count befor each iteration
		info->hash_hits = 0;
		info->max_depth = 0;

		depth_start_time = Get_Time_Ms();

		/***** Aspiration Windows *****/
		if (use_aspiration_window)
		{
			window_low = 0;
			window_high = 0;

			while (true)
			{
				score = Alpha_Beta(prev_score - aspiration_windows[window_low], prev_score + aspiration_windows[window_high], currentDepth, PV, DO_NULL, board, info);
				if (score <= prev_score - aspiration_windows[window_low]) //Fail low
				{
					window_low++;
				}
				else if (score >= prev_score + aspiration_windows[window_high]) //Fail High
				{
					window_high++;
				}
				else //within window
				{
					break;
				}
			}
		}
		else //No aspiration window
		{
			//score = Alpha_Beta(-INF, INF, currentDepth, PV, board, info);
			score = Search_Root(-INF, INF, currentDepth, board, info);
		}

		if (info->stopped == 1) {
			break;
		}
		prev_score = score;

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
			printf(" depth %d seldepth %d nodes %ld time %d nps %d ",
				currentDepth, info->max_depth, info->nodes, Get_Time_Ms() - info->start_time, (int)(info->nodes / ((Get_Time_Ms() - depth_start_time + 1) / 1000.0)));
		
		//Print branching factor
		if (previous_node_count != 0)
		{
			//printf("\n\rBranching factor: %f\n\r", info->nodes / (float)previous_node_count);
			//printf("Hash Rate: %d\n", (100 * info->hash_hits / info->hash_probes));
		}
		previous_node_count = info->nodes;

		//Print current pv line
		printf("pv ");
		Print_PV_List(&pv_list);
		printf("\n\n");

		//End if time is one third gone because next depth is unlikely to finish
		//if (info->time_set && info->end_early && (Get_Time_Ms() - info->start_time) >= ((info->stop_time - info->start_time) / 3.0)) break;

		//End search if mate found and full pv shown
		if (IS_MATE(score) && (currentDepth >= ((score > 0) ? MATE_SCORE - score : MATE_SCORE + score))) break; 
		
	}

	printf("bestmove %s\n", UCI_Move_String(&best_move));
	
	info->age++;
	Age_History_Data(board);

	return 1;
}

//Root node search
int Search_Root(int alpha, int beta, int depth, BOARD_STRUCT *board, SEARCH_INFO_STRUCT *info)
{
	int move;
	int best_score = -INF;
	int best_move = 0;
	int score = -MATE_SCORE; //Set in case no moves are available
	MOVE_LIST_STRUCT move_list;
	int current_move;
	int current_move_score;
	HASH_ENTRY_STRUCT hash_entry;
	hash_entry.move = 0;
	int valid = 0;
	int best_move_index = 0;

	info->nodes++;
	//Check for timeout
	if ((info->nodes & 4095) == 0) //every 4096 nodes
	{
		if ((info->time_set) && (Get_Time_Ms() > info->stop_time - 40)) //Could be reduced to 10 ms
		{
			info->stopped = 1;
			return 0;
		}
		ReadInput(info); //Check for input
	}

	/***** Check hash table *****/
	info->hash_probes++;
	int value = Get_Hash_Entry(board->hash_key, alpha, beta, depth, board->hply, &hash_entry.move);
	if (hash_entry.move != 0) info->hash_hits++; //Count hash hit as long as a move if found

	/***** Move generation *****/
	Generate_Moves(board, &move_list);

	if (!Find_PV_Move(hash_entry.move, &move_list)) //If hash move not found
	{
		/***** Internal Iterative Deepening *****/
		/* This funtion is almost never called, but it's an insurance measure just in case*/
		if (depth >= 5)
		{
			Internal_Iterative_Deepening(alpha, beta, depth, &move_list, board, info);
		}
	}


	/***** Search *****/
	for (move = 0; move < move_list.num; move++) //For all moves in list
	{
		/***** Get best move *****/
		Get_Next_Move(move, &move_list);//Moves next move into move_index position
		current_move = move_list.list[move].move;
		current_move_score = move_list.list[move].score;

		if (current_move == 0) break; //End if no moves are available

		//if (current_move == hash_entry.move) continue;

		if (!Make_Move(current_move, board)) continue;//If move is unsuccessful, try next move
		
		/***** Principal Variation Search *****/
		if (move == 0) //If first move, use full window
		{
			score = -Alpha_Beta(-beta, -alpha, depth - 1, PV, DO_NULL, board, info);
		}
		else //If not first move
		{
			score = -Alpha_Beta(-alpha - 1, -alpha, depth - 1, NOT_PV, DO_NULL, board, info); //Full window search
			if (score > alpha)
			{
				score = -Alpha_Beta(-beta, -alpha, depth - 1, PV, DO_NULL, board, info);
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
			Store_Hash_Entry(&hash_entry, board->hply, info);

			//Update best move index
			info->beta_cutoff_index[move]++;

			//if move is not a capture or promotion
			if (current_move_score <= KILLER_MOVE_SCORE)
			{
				Add_Killer_Move(current_move, board); //Store killer move
				//Add_History_Move(current_move, board); //Store move in history array
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
			}
			best_score = score;
			best_move_index = move;
			best_move = current_move;
		}
	} //End looping through all moves

	//Store hash entry
	Fill_Hash_Entry(info->age, depth, best_score, HASH_EXACT, board->hash_key, best_move, &hash_entry);
	Store_Hash_Entry(&hash_entry, board->hply, info);
	
	//Update best_index
	info->best_index[best_move_index]++;

	return best_score;
}

//Alpha beta implementation in negamax search
int Alpha_Beta(int alpha, int beta, int depth, PV_ENUM is_pv, NULL_ENUM do_null, BOARD_STRUCT *board, SEARCH_INFO_STRUCT *info)
{
	int move;
	int moves_searched = 0; //Number of legal moves searched (not futility pruned)
	int moves_made = 0; //Number of legal moves
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
	int checking_move = 0;
	int best_move_index = 0;
	int reduction = 0;

	info->nodes++;
	//Check for timeout
	if ((info->nodes & 4095) == 0) //every 4096 nodes
	{
		if ((info->time_set) && (Get_Time_Ms() > info->stop_time - 40)) //Could be reduced to 10 ms
		{
			info->stopped = 1;
			return 0;
		}
		ReadInput(info); //Check for input
	}

	/***** Check for max depth *****/
	if (board->hply > info->max_depth) info->max_depth = board->hply;
	if (board->hply >= MAX_SEARCH_DEPTH)
	{
		return Evaluate_Board(board);
	}

	/***** Mate Distance Pruning *****/
	int mate_value = MATE_SCORE - board->hply;
	if (alpha < -mate_value) alpha = -mate_value;
	if (beta > mate_value - 1) beta = mate_value - 1;
	if (alpha >= beta) return alpha;

	/***** Draw Detection *****/
	if (board->move_counter >= 100 || Is_Material_Draw(board) || (board->hply && Is_Repetition(board)))	return 0;

	/***** Check Test *****/
	int in_check = In_Check(board->side, board);
	if (in_check) depth++;

	/***** Leaf Node Response *****/
	//Start quiescent search at depth 0
	if (depth <= 0)
	{
		return Quiescent_Search(alpha, beta, board, info);
	}

	/***** Check hash table *****/
	info->hash_probes++;
	int value = Get_Hash_Entry(board->hash_key, alpha, beta, depth, board->hply, &hash_entry.move);
	if (hash_entry.move != 0) info->hash_hits++; //Count hash hit as long as a move if found
	if (value != INVALID) 
	{
		if (!is_pv || (value > alpha && value < beta)) //Only return exact values in pv line
		{
				return value; 
		}
	}
	

	/***** Search Hash Move *****/
	/* Searching the hash move before generating any others can save time */
	/*
	if (hash_entry.move != 0)
	{
		if (Make_Move(hash_entry.move, board)) //If move is legal (it should be)
		{
			mate = 0;
			score = -Alpha_Beta(-beta, -alpha, depth - 1, is_pv, board, info);

			Take_Move(board);

			if (score >= beta)
			{
				Fill_Hash_Entry(info->age, depth, score, HASH_LOWER, board->hash_key, hash_entry.move, &hash_entry);
				Add_Hash_Entry(&hash_entry, board->hply, info);
				return score; //Hash has already been stored here, so no need to save again
			}
			if (score > best_score)
			{
				best_score = score;
				best_move = hash_entry.move;
				if (best_score > alpha) alpha = best_score;
			}
		}
	}
	*/

	/***** Null Move *****/
	if (depth >= 4 
	&& do_null
	&& !is_pv 
	&& board->hply 
	&& !in_check 
	&& (board->big_material[board->side] >= null_move_mat))
	{
		int R = 2;
		if(adapt_null_move) R = max(2, (int)(null_move_depth_mod * depth));


		Make_Null_Move(board);
		score = -Alpha_Beta(-beta, -beta + 1, depth - 1 - R, NOT_PV, DONT_DO_NULL, board, info); //Subtract an additional 2 ply from depth
		Take_Null_Move(board);

		if (score >= beta && !IS_MATE(score)) return score;
	}

	/***** Futility Pruning *****/
	/* Here we determine if this node is elegible for futility pruning */
	if (depth <= 2
		&& use_futility
		&& !is_pv
		&& !in_check
		&& !IS_MATE(alpha) //Not searching for a mate
		&& (Evaluate_Board(board) + futility_margins[depth] <= alpha))
		f_prune_allowed = 1;

	/***** Move generation *****/
	Generate_Moves(board, &move_list);
	
	if (!Find_PV_Move(hash_entry.move, &move_list)) //If hash move not found
	{
		/***** Internal Iterative Deepening *****/
		/* This funtion is almost never called, but it's an insurance measure just in case*/
		if (is_pv
			&& depth >= 5
			&& do_null)
		{
			Internal_Iterative_Deepening(alpha, beta, depth, &move_list, board, info);
		}
	}
	Find_Best_Recapture(&move_list, board);

	/***** Search *****/
	for (move = 0; move < move_list.num; move++) //For all moves in list
	{
		/***** Get best move *****/
		Get_Next_Move(move, &move_list);//Moves next move into move_index position
		current_move = move_list.list[move].move;
		current_move_score = move_list.list[move].score;

		if (current_move == 0) break; //End if no moves are available

		//if (current_move == hash_entry.move) continue;

		if (!Make_Move(current_move, board)) continue;//If move is unsuccessful, try next move
		
		mate = 0; //A move has been made
		moves_made++;

		//See if current move leads to check, important for pruning and reductions
		if((f_prune_allowed || moves_made >= LATE_MOVE_NUM) && CAN_REDUCE(current_move)) checking_move = In_Check(board->side, board);
		else checking_move = 0;

		/***** Futility Pruning *****/
		if (f_prune_allowed
			&&  !IS_CAPTURE(current_move)
			&& !IS_PROMOTION(current_move)
			&& !checking_move) 
		{
			if (depth <= 2)
			{
				Take_Move(board);
				continue;
			}
			else //Razoring
			{
				score = -Alpha_Beta(-alpha - 1, -alpha, 1, NOT_PV, DO_NULL, board, info); //Null window search at depth 1
				if (score <= alpha)
				{
					Take_Move(board);
					continue;
				}
			}
		}

		moves_searched++; //Move will be searched

		/***** Principal Variation Search *****/
		if (move == 0) //If first move, use full window
		{
			score = -Alpha_Beta(-beta, -alpha, depth - 1, is_pv, DO_NULL, board, info);
		}
		else //If not first move
		{
			/***** Late move reduction *****/
			/* Does a reduced search if the move is eligible, otherwise goes straight into PVS search */
			/* Moves that raise alpha are re-searched in PVS */
			if (moves_made >= LATE_MOVE_NUM
				&& use_late_move_reduction
				&& !in_check
				&& (!is_pv || use_lmr_in_pv)
				&& CAN_REDUCE(current_move)
				&& depth >= REDUCTION_LIMIT
				&& !IS_KILLER(move_list.list[move].score)
				&& !checking_move)
			{
				if (use_extra_lmr)
				{
					double reduction_temp = sqrt(depth * lmr_depth_mod) + sqrt(move * lmr_move_mod);
					if (is_pv) reduction_temp *= lmr_pv_mod;
					reduction = (int)reduction_temp; //Floor
				}
				else reduction = 1;

				int new_depth = max(1,depth - reduction - 1);

				//Null Window search
				score = -Alpha_Beta(-alpha - 1, -alpha, new_depth, NOT_PV, DO_NULL, board, info); 
			}
			else
			{
				score = alpha + 1; //Set score above alpha to automatically trigger PVS next
			}

			/***** Principal Variation Search *****/
			if (score > alpha) //Continue if reduced search is improvement
			{
				//Look for refutation in null window
				score = -Alpha_Beta(-alpha - 1, -alpha, depth - 1, NOT_PV, DO_NULL, board, info); //Null window search

				//If move improves alpha but does not cause a cutoff, and if not in a null search already
				if ((is_pv || !only_research_in_pv) && alpha < score && beta > score && (beta - alpha > 1))
				{
					score = -Alpha_Beta(-beta, -alpha, depth - 1, PV, DO_NULL, board, info);
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
			Store_Hash_Entry(&hash_entry, board->hply, info);

			//Update best move index
			info->beta_cutoff_index[move]++;

			//if move is not a capture or promotion
			if (current_move_score <= KILLER_MOVE_SCORE)
			{				 
				Add_Killer_Move(current_move, board); //Store killer move
				Add_History_Move(current_move, depth, board); //Store move in history array
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
			}
			best_score = score;
			best_move_index = move;
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
			Store_Hash_Entry(&hash_entry, board->hply, info);
			return best_score; //Losing checkmate
		}
		//Store in table
		Fill_Hash_Entry(info->age, depth, 0, HASH_EXACT, board->hash_key, 0, &hash_entry);
		Store_Hash_Entry(&hash_entry, board->hply, info);
		return 0; //Draw
	}

	/***** Futility Check *****/
	/* If all moves have been pruned (moves_searched == 0) set best_score to alpha */
	if (moves_searched == 0) best_score = alpha;

	//For exact and upper nodes
	if (best_score > alpha_orig)
	{
		//Store hash entry
		Fill_Hash_Entry(info->age, depth, best_score, HASH_EXACT, board->hash_key, best_move, &hash_entry);
		Store_Hash_Entry(&hash_entry, board->hply, info);
	}
	else
	{
		//Store hash entry
		Fill_Hash_Entry(info->age, depth, best_score, HASH_UPPER, board->hash_key, best_move, &hash_entry);
		Store_Hash_Entry(&hash_entry, board->hply, info);
	}

	//Update best_index
	info->best_index[best_move_index]++;
	
	return best_score;
}

//Quiescent search to find positions suitable for evaluation
int Quiescent_Search(int alpha, int beta, BOARD_STRUCT *board, SEARCH_INFO_STRUCT *info)
{
	int move;
	MOVE_LIST_STRUCT move_list;
	int next_move;
	int score = Evaluate_Board(board);
	int best_score = score;
	
	info->nodes++;

	if ((info->nodes & 4095) == 0) //every 4096 nodes
	{
		if (info->time_set && (Get_Time_Ms() > info->stop_time - 40))
		{
			info->stopped = 1;
			return 0;
		}
		ReadInput(info); //Check for input
	}

	//Check for max depth
	if (board->hply >= MAX_SEARCH_DEPTH)
	{
		return Evaluate_Board(board);
	}


	if (score >= beta) return score;
	if (score > best_score) best_score = score;
	if (score > alpha) alpha = score;


	Generate_Capture_Promote_Moves(board, &move_list);
	if(quiescent_SEE) Set_Quiescent_SEE_Scores(&move_list, board);

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
			return score; //Beta cutoff
		}
		if (score > best_score) best_score = score;
		if (score > alpha)
		{
			alpha = score;
		}
	}
	return best_score;
}


//Searches every move in the list, and sorts them according to their scores 
void Internal_Iterative_Deepening(int alpha, int beta, int depth, MOVE_LIST_STRUCT *move_list, BOARD_STRUCT *board, SEARCH_INFO_STRUCT *info)
{
	int iid_depth = depth / 2; //Search at half depth

	U64 hash = board->hash_key;

	for (int i = 0; i < move_list->num; i++)
	{
		//Make move
		if (Make_Move(move_list->list[i].move, board))
		{
			move_list->list[i].score = -Alpha_Beta(-beta, -alpha, iid_depth, PV, DO_NULL, board, info);

			Take_Move(board);
		}
		else //Move not successful
		{
			move_list->list[i].score = -10000; //Search these last
		}
	}

	ASSERT(hash == board->hash_key);
}