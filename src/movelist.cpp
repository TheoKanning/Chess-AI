/* movelist.cpp
* Contains functions for sorting and accessing movelists
* Theo Kanning 12/23/14 
*/

#include "globals.h"

using namespace std;

//Finds highest scoring move in list, sets its score to -1, then returns its move integer
int Get_Next_Move(MOVE_LIST_STRUCT *move_list, int *score_ptr)
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
		*score_ptr = move_list->list[high_score_index].score;
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

//Creates integer from move data and stores in move_list
void Add_Move(MOVE_LIST_STRUCT *move_list, int from, int to, int piece, int capture, int special, int score, BOARD_STRUCT *board)
{
	int temp = 0;

	//Check all fields within bounds
	ASSERT(ON_BOARD_120(from));
	ASSERT(ON_BOARD_120(to));
	ASSERT((piece >= wP) && (piece <= bK)); //Piece is not empty
	ASSERT((piece >= EMPTY) && (piece <= bK)); //Piece can be empty
	ASSERT((special >= NOT_SPECIAL) && (special <= KNIGHT_PROMOTE));

	//Set all fields
	SET_FROM_SQ(temp, from);
	SET_TO_SQ(temp, to);
	SET_PIECE(temp, piece);
	SET_CAPTURE(temp, capture);
	SET_SPECIAL(temp, special);

	//Store in list and update counter
	move_list->list[move_list->num].move = temp;
	
	//Update score using killer and history heuristics
	if (score != 0) //Skip if move has an MVVLVA or promote score
	{
		move_list->list[move_list->num].score = score;
	}

	else if (temp == board->the_killers[board->hply][0])//if move matches first killer move
	{
		move_list->list[move_list->num].score = KILLER_MOVE_SCORE;
	}
	else if (temp == board->the_killers[board->hply][1])//if move matches second killer move
	{
		move_list->list[move_list->num].score = KILLER_MOVE_SCORE - 1;
	}
	else //If nothing else, add score of zero
	{
		move_list->list[move_list->num].score = 0;
	}
	/*
	else if (board->history_max > 0)//Add history score 
	{
		move_list->list[move_list->num].score = board->history[from][to] * HISTORY_SCORE_MAX / (1.0 * board->history_max);
		if (move_list->list[move_list->num].score > HISTORY_SCORE_MAX)
		{
			printf("Higher than bound score: %d history val:%d max:%d\n\r", move_list->list[move_list->num].score, board->history[from][to], board->history_max);
		}
	}
	*/
	move_list->num++; //Increment counter
}

//Prints all moves in standard chess format
void Print_Move_List(MOVE_LIST_STRUCT *move_list)
{
	int index;

	cout << endl << "Move List: " << endl;
	cout << "Moves found: " << move_list->num << endl;

	for (index = 0; index < move_list->num; index++) //Iterate through all moves stored
	{
		cout << index << " ";
		Print_Move(&move_list->list[index]);

		cout << " Score: " << move_list->list[index].score << endl;
	}
}