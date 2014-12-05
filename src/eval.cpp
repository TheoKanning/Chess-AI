/***** eval.cpp
* Contains evaluation function for board state
* Theo Kanning 12/5/14
*/
#include "globals.h"


void Evaluate_Board(BOARD_STRUCT *board)
{
	board->eval_score = board->material;
}