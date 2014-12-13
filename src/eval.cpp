/***** eval.cpp
* Contains evaluation function for board state
* Theo Kanning 12/5/14
*/
#include "globals.h"


int Evaluate_Board(BOARD_STRUCT *board)
{
	int score = 0;

	board->eval_score = board->material;
	score = board->material;

	if (board->side == WHITE)
	{
		return score;
	}
	else
	{
		return -score;
	}
}