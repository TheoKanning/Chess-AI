/* search.c
* Contains iterative deepening, alpha-beta and other search functions
* Theo Kanning 12/10/14
*/
#include "globals.h"
#include "time.h"

using namespace std;

//Calls search functions at increasing depths
int Iterative_Deepening(int depth, BOARD_STRUCT *board)
{
	int i, score;
	time_t start, stop;
	PV_LIST_STRUCT PV_list;
	

	for (i = 1; i <= depth; i++) //Search at each depth
	{
		PV_list.in_pv_line = 1; //Call root node in PV line
		time(&start);
		score = Alpha_Beta(-INF, INF, i, &PV_list, board);
		time(&stop);
		
		//Print Data
		cout << "Depth:" << i << " ";
		Print_PV_List(&PV_list);
		cout << " Score:" << score/100.0 << " Time:" << stop - start << " seconds" << endl;
	}
	return 1;
}

//Alpha beta implementation in negamax search
int Alpha_Beta(int alpha, int beta, int depth, PV_LIST_STRUCT *pv_list_ptr, BOARD_STRUCT *board)
{
	int move;
	int score = -INF; //Set in case no moves are available
	int mate = 1; //If no legal moves are found
	MOVE_LIST_STRUCT move_list;
	PV_LIST_STRUCT pv_list;
	pv_list.num = 0;
	pv_list.in_pv_line = 0;

	if (depth == 0)
	{
		pv_list_ptr->num = 0;
		return Evaluate_Board(board); //Quiescent search later
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
				score = -Alpha_Beta(-beta, -alpha, depth - 1, &pv_list, board);
				pv_list.in_pv_line = 0; //Not in PV line for rest of moves
			}
			else 
			{
				score = -Alpha_Beta(-beta, -alpha, depth - 1, &pv_list, board);
			}
			mate = 0; //A move has been made
			Take_Move(board);
		}
		if (score >= beta)
		{
			return score; //Beta cutoff
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

