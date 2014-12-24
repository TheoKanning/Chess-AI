/* movelist.cpp
* Contains functions for sorting and accessing movelists
* Theo Kanning 12/23/14 
*/

#include "globals.h"

//Finds highest scoring move in list, sets its score to -1, then returns its move integer
int Get_Next_Move(MOVE_LIST_STRUCT *move_list)
{
	int high_score = -1;
	int high_score_index = -1;
	int index;

	for (index = 0; index < move_list->num; index++)
	{
		//If move is highest so far
		if (move_list->list[index].score > high_score)
		{
			high_score = move_list->list[index].score;
			high_score_index = index;
		}
	}

	//Return integer and clear move from list
	if (high_score_index != -1) //If move found
	{
		move_list->list[high_score_index].score = -1; //Clear score so move can't be found again
		return move_list->list[high_score_index].move;
	}
	else
	{
		return 0;
	}
}

//Finds highest scoring capture move in list, sets its score to -1, then returns its move integer
int Get_Next_Capture_Move(MOVE_LIST_STRUCT *move_list)
{
	int high_score = CAPTURE_SCORE - 1;
	int high_score_index = -1;
	int index;

	for (index = 0; index < move_list->num; index++)
	{
		//If move is highest so far
		if (move_list->list[index].score > high_score)
		{
			high_score = move_list->list[index].score;
			high_score_index = index;
		}
	}

	//Return integer and clear move from list
	if (high_score_index != -1) //If move found
	{
		move_list->list[high_score_index].score = -1; //Clear score so move can't be found again
		return move_list->list[high_score_index].move;
	}
	else
	{
		return 0;
	}
}

//Sort moves according to score using comb sort algorithm
void Sort_Moves(MOVE_LIST_STRUCT *move_list)
{
	int width = 10; //Distance between compared values
	int done = 0; //Records whether or not the list is fully sorted
	int index; //Index of start point
	MOVE_STRUCT temp; //Temporary move structure

	while (!done)
	{
		if (width == 1) done = 1; //Only enable ending once minimum width is reached

		for (index = 0; index < move_list->num - width; index++)
		{
			if (move_list->list[index].score < move_list->list[index + width].score)
			{
				//Swap moves
				Copy_Move(&move_list->list[index], &temp); //1 -> temp
				Copy_Move(&move_list->list[index + width], &move_list->list[index]); //2 -> 1
				Copy_Move(&temp, &move_list->list[index + width]); //temp -> 2
				done = 0;
			}
		}
		if (width > 1) width--;
	}
}

//Removes all non-captures, returns 0 if no captures are found
int Get_Capture_Moves(MOVE_LIST_STRUCT *move_list)
{
	int index;
	int count = 0;//Number of captures found
	for (index = 0; index < move_list->num; index++)
	{
		if (move_list->list[index].score >= CAPTURE_SCORE) //If move is a capture
		{
			count++;
		}
		else
		{
			move_list->list[index].score = 0;
			move_list->list[index].move = 0;
		}
	}

	move_list->num = count;
	if (count == 0) return 0;
	return 1;
}

//Copies move 1 into move 2 pointer
void Copy_Move(MOVE_STRUCT *move1, MOVE_STRUCT *move2)
{
	move2->move = move1->move;
	move2->score = move1->score;
}