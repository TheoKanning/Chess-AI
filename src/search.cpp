/* search.c
* Contains iterative deepening, alpha-beta and other search functions
* Theo Kanning 12/10/14
*/
#include "globals.h"
#include "time.h"
#include "windows.h"

using namespace std;
//Searches a given position using board ans search info
int Search_Position(BOARD_STRUCT *board, SEARCH_INFO_STRUCT *info)
{
	int currentDepth, score;
	PV_LIST_STRUCT pv_list;
	MOVE_STRUCT best_move;

	//Reset stop flag
	info->stopped = 0;
	info->nodes = 0;

	for (currentDepth = 1; currentDepth <= info->depth; ++currentDepth) {

		pv_list.in_pv_line = 1;

		score = Alpha_Beta(-INF, INF, currentDepth, &pv_list, board, info);

		if (info->stopped == TRUE) {
			break;
		}

		Copy_Move(&pv_list.list[0], &best_move); //Store best move found

		//Print search info
			printf("info score cp %d depth %d nodes %ld time %d ",
				score, currentDepth, info->nodes, Get_Time_Ms() - info->start_time);
		
		//Print current pv line
		
		printf("pv ");
		Print_PV_List(&pv_list);
		printf("\n");
		

		//printf("Hits:%d Overwrite:%d NewWrite:%d Cut:%d\nOrdering %.2f NullCut:%d\n",pos->HashTable->hit,pos->HashTable->overWrite,pos->HashTable->newWrite,pos->HashTable->cut,
		//(info->fhf/info->fh)*100,info->nullCut);
	}

	
	printf("bestmove %s\n", UCI_Move_String(&best_move));
	
	return 1;
}


//Calls search functions at increasing depths
int Iterative_Deepening(int depth, BOARD_STRUCT *board, SEARCH_INFO_STRUCT *info)
{
	int i, score;
	time_t start, stop;
	PV_LIST_STRUCT PV_list;

	for (i = 1; i <= depth; i++) //Search at each depth
	{
		PV_list.in_pv_line = 1; //Call root node in PV line
		info->nodes = 0;
		time(&start);
		score = Alpha_Beta(-INF, INF, i, &PV_list, board, info);
		time(&stop);
		
		//Print Data
		cout << "Depth:" << i << " ";
		Print_PV_List(&PV_list);
		cout << " Score:" << score/100.0 << " Time:" << stop - start << " seconds" << endl;
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
int Alpha_Beta(int alpha, int beta, int depth, PV_LIST_STRUCT *pv_list_ptr, BOARD_STRUCT *board, SEARCH_INFO_STRUCT *info)
{
	int move;
	int score = -INF; //Set in case no moves are available
	int mate = 1; //If no legal moves are found
	MOVE_LIST_STRUCT move_list;
	PV_LIST_STRUCT pv_list;
	pv_list.num = 0;
	pv_list.in_pv_line = 0;

	info->nodes++;
	//Check for timeout
	if ((info->nodes % 1000) == 0) //every 1000 nodes
	{
		if (Get_Time_Ms() > info->stop_time - 50)
		{
			info->stopped = 1;
			return 0;
		}
	}

	//Check for 50 move rule and threefold repetition
	if (board->move_counter >= 100)
	{
		return 0;
	}


	if (depth == 0 && In_Check(board->side, board))
	{
		depth++;
	}
	else if (depth == 0 )
	{
		pv_list_ptr->num = 0;
		return Quiescent_Search( alpha, beta, board, info);
	}

	Generate_Moves(board, &move_list);
	//If in pv line, find PV move
	if (pv_list_ptr->in_pv_line) Find_PV_Move(&pv_list_ptr->list[0], &move_list);
	Sort_Moves(&move_list);

	for (move = 0; move < move_list.num; move++) //For all moves in list
	{
		if (Make_Move(&move_list.list[move], board)) //If move is successful
		{
			//if move score == PV_SCORE
			//Fill local PV table with last n-1 moves
			// Set is_pv to 1
			//Call alpha beta
			//Reset pv table values for rest of search
			if (move == 0 && move_list.list[move].score == PV_SCORE)
			{
				memcpy(pv_list.list, pv_list_ptr->list + 1, (pv_list_ptr->num - 1)* sizeof(MOVE_STRUCT)); //Copy remaining moves into pointer list
				pv_list.in_pv_line = 1;
				pv_list.num = pv_list_ptr->num - 1;
				score = -Alpha_Beta(-beta, -alpha, depth - 1, &pv_list, board, info);
				pv_list.in_pv_line = 0; //Not in PV line for rest of moves
			}
			else
			{
				score = -Alpha_Beta(-beta, -alpha, depth - 1, &pv_list, board, info);
			}
			mate = 0; //A move has been made
			Take_Move(board);

			if (info->stopped)
			{
				return 0;
			}

			//Update alpha and beta
			if (score >= beta)
			{
				return beta; //Beta cutoff
			}
			if (score > alpha)
			{
				alpha = score;
				//Store PV move 
				Copy_Move(&move_list.list[move], &pv_list_ptr->list[0]); //Copy first move into pv list
				memcpy(pv_list_ptr->list + 1, pv_list.list, pv_list.num * sizeof(MOVE_STRUCT)); //Copy remaining moves into pointer list
				pv_list_ptr->num = pv_list.num + 1;
			}
		}
	}

	if (mate) //No moves found
	{
		if (In_Check(board->side, board))
		{
			return -INF; //Losing checkmate
		}
		return 0; //Draw
	}
	return alpha;
}

//Quiescent search to find positions suitable for evaluation
int Quiescent_Search(int alpha, int beta, BOARD_STRUCT *board, SEARCH_INFO_STRUCT *info)
{
	int move;
	MOVE_LIST_STRUCT move_list;
	int score = Evaluate_Board(board);
	
	info->nodes++;

	if ((info->nodes % 1000) == 0) //every 1000 nodes
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
	//If in pv line, find PV move
	Sort_Moves(&move_list);
	Get_Capture_Moves(&move_list);

	for (move = 0; move < move_list.num; move++) //For all moves in list
	{
		if (Make_Move(&move_list.list[move], board)) //If move is successful
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