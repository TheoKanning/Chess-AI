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
		time(&start);
		score = Alpha_Beta(-INF, INF, i, &PV_list, board);
		time(&stop);
		
		//Print Data
		Print_PV_List(&PV_list);
		cout << " Score:" << score << " Time:" << stop - start << " seconds" << endl;
	}
	return 1;
}

//Alpha beta implementation in negamax search
int Alpha_Beta(int alpha, int beta, int depth, PV_LIST_STRUCT *pv_list_ptr, BOARD_STRUCT *board)
{
	int move;;
	int score = -INF; //Set in case no moves are available
	int mate = 1; //If no legal moves are found
	MOVE_LIST_STRUCT move_list;
	PV_LIST_STRUCT pv_list;
	pv_list.num = 0;

	if (depth == 0)
	{
		pv_list_ptr->num = 0;
		return Evaluate_Board(board); //Quiescent search later
	}

	Generate_Moves(board, &move_list);

	for (move = 0; move < move_list.num; move++) //For all moves in list
	{
		if (Make_Move(&move_list.list[move], board)) //If move is successful
		{
			score = -Alpha_Beta(-beta, -alpha, depth - 1, &pv_list, board);
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

//Print PV list with final eval score
void Print_PV_List(PV_LIST_STRUCT *pv_list)
{
	int i;

	cout << "PV List: ";
	for (i = 0; i < pv_list->num; i++)
	{
		Print_Move(&pv_list->list[i]);
		cout << " ";
	}
}