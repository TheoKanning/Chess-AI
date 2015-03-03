/* history.cpp
* Contains functions for maintaining and using history heuristic
* Theo Kanning 1/2/15
*/

#include "globals.h"

//Adds a move to the history array, checking if it creates a new maximum
void Add_History_Move(int move, BOARD_STRUCT *board)
{
	int from = GET_FROM_SQ(move);
	int to = GET_TO_SQ(move);

	board->history[from][to]++; //increment spot in array

	//Increase the max if applicable
	if (board->history[from][to] > board->history_max)
	{
		board->history_max = board->history[from][to];
	}
}

//Searches a movelist and finds history moves
void Find_History_Moves(MOVE_LIST_STRUCT *move_list, BOARD_STRUCT *board)
{
	//ASSERT(board->history_max < 100);

	int index, move_num;
	int to, from;

	if (board->history_max == 0) return;

	//Loop through movelist and add history value to score
	for (index = 0; index < move_list->num; index++)
	{
		if (move_list->list[index].score < HISTORY_SCORE_MAX) //If move is not already scored higher
		{
			move_num = move_list->list[index].move;
			from = GET_FROM_SQ(move_num);
			to = GET_TO_SQ(move_num);
			move_list->list[index].score += board->history[from][to] * HISTORY_SCORE_MAX  / board->history_max;
		}
	}
}

//Clears history data before beginning a search
void Clear_History_Data(BOARD_STRUCT *board)
{
	int index1, index2;

	//Set all values to zero
	for (index1 = 0; index1 < 64; index1++)
	{
		for (index2 = 0; index2 < 64; index2++)
		{
			board->history[index1][index2] = 0;
		}
	}

	board->history_max = 0; //Reset max value
}