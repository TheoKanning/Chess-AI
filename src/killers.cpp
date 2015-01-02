/* killers.cpp
* Contains all functions and definitions for storing and finding killer moves
* Theo Kanning 12/29/14
*/

#include "globals.h"

//Adds a move to the killer list at the goven ply, ply parameter may be redundant
void Add_Killer_Move(int move, BOARD_STRUCT *board)
{
	int ply = board->hply;
	//Automatically shift
	//board->the_killers[ply][2].move = board->the_killers[ply][1].move;
	board->the_killers[ply][1].move = board->the_killers[ply][0].move;
	board->the_killers[ply][0].move = move;
	return;
	

	/*
	//if score is higher than first killer
	if (score > board->the_killers[ply][0].score)
	{
		//Move first to second
		board->the_killers[ply][1].score = board->the_killers[ply][0].score;
		board->the_killers[ply][1].move = board->the_killers[ply][0].move;

		//Store new move in first position
		board->the_killers[ply][0].score = score;
		board->the_killers[ply][0].move = move;
		return;
	}
	else if (score > board->the_killers[ply][1].score)
	{
		board->the_killers[ply][1].score = score;
		board->the_killers[ply][1].move = move;
	}
	*/
}

//Finds killer moves (if any) in movelist and sets their scores higher
void Find_Killer_Moves(MOVE_LIST_STRUCT *move_list, BOARD_STRUCT *board)
{
	int killer_index, killer_move, move_index;
	int ply = board->hply;

	//Loop through each killer move, subtracting the move index from the final score to weight first killers higher
	for (killer_index = 0; killer_index < 2; killer_index++)
	{
		//Get killer move from board array
		killer_move = board->the_killers[ply][killer_index].move;

		if (killer_move == 0) break; //End first loop if no more killer moves are available

		for (move_index = 0; move_index < move_list->num; move_index++)
		{
			//If the move found matches the killer move and is not already scored higher
			if (move_list->list[move_index].move == killer_move && move_list->list[move_index].score < KILLER_MOVE_SCORE)
			{
				move_list->list[move_index].score = KILLER_MOVE_SCORE - killer_index; //Subtract killer index to give previous killer priority
				break;
			}
		}
	}
	//Check for killer moves at ply -2
	ply -= 2;
	if (ply < 0) return;
	//Loop through each killer move, subtracting the move index from the final score to weight first killers higher
	for (killer_index = 0; killer_index < 2; killer_index++)
	{
		//Get killer move from board array
		killer_move = board->the_killers[ply][killer_index].move;

		if (killer_move == 0) return; //End if no more killer moves are available

		for (move_index = 0; move_index < move_list->num; move_index++)
		{
			//If the move found matches the killer move and is not already scored higher
			if (move_list->list[move_index].move == killer_move && move_list->list[move_index].score < KILLER_MOVE_SCORE - 2)
			{
				move_list->list[move_index].score = KILLER_MOVE_SCORE - killer_index - 2; //Subtract killer index to give previous killer priority
				break;
			}
		}
	}
}