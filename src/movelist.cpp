/* movelist.cpp
* Contains functions for sorting and accessing movelists
* Theo Kanning 12/23/14 
*/

#include "globals.h"

using namespace std;

//Finds nth highest scoring move in list, move it to the front, then returns its move integer
void Get_Next_Move(int num, MOVE_LIST_STRUCT *move_list)
{
	int high_index = num;
	int high_score = -INF;
	for (int i = num; i < move_list->num; i++)
	{
		if (move_list->list[i].score > high_score)
		{
			high_score = move_list->list[i].score;
			high_index = i; //Store new high index
		}
	}

	//Move high scoring move to nth position so it won't be searched again
	MOVE_STRUCT temp;
	Copy_Move(&move_list->list[num], &temp);
	Copy_Move(&move_list->list[high_index], &move_list->list[num]);
	Copy_Move(&temp, &move_list->list[high_index]);
}

//Finds highest scoring capture move in list, sets its score to -1, then returns its move integer
void Get_Next_Capture_Move(int num, MOVE_LIST_STRUCT *move_list)
{
	int high_index = num;
	int high_score = -INF;

	for (int i = num; i < move_list->num; i++)
	{
		if ((move_list->list[i].score > high_score))
		{
			high_score = move_list->list[i].score;
			high_index = i; //Store new high index
		}
	}

	if (move_list->list[high_index].score >= CAPTURE_SCORE) //Subtract 10000 to include negative see scores
	{
		//Move high scoring move to nth position so it won't be searched again
		MOVE_STRUCT temp;
		Copy_Move(&move_list->list[num], &temp);
		Copy_Move(&move_list->list[high_index], &move_list->list[num]);
		Copy_Move(&temp, &move_list->list[high_index]);
	}
	else
	{
		move_list->list[num].move = 0; //Remove next move
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
	
	//If capture
	if (IS_CAPTURE(temp))
	{
		//move_list->list[move_list->num].score = score;
		
		int see = Static_Exchange_Evaluation(temp, board); //Calculate SEE score
		if (see > 0)
		{
			move_list->list[move_list->num].score = WINNING_CAPTURE_SCORE + see;
		}
		else if (see < 0)
		{
			move_list->list[move_list->num].score = LOSING_CAPTURE_SCORE + see;
		}
		else
		{
			move_list->list[move_list->num].score = CAPTURE_SCORE + see;
		}
	}
	else if (IS_PROMOTION(temp)) //Promotion
	{
		move_list->list[move_list->num].score = score;//Add parameter score
	}
	else //Check heuristics for scoring
	{
		move_list->list[move_list->num].score = 0;

		//Killer moves
		if (temp == board->the_killers[board->hply][0])//if move matches first killer move
		{
			move_list->list[move_list->num].score = KILLER_MOVE_SCORE;
		}
		else if (temp == board->the_killers[board->hply][1])//if move matches second killer move
		{
			move_list->list[move_list->num].score = KILLER_MOVE_SCORE - 1;
		}
		else if (board->hply >= 2) //If killer moves from ply - 2 are available
		{
			if (temp == board->the_killers[board->hply - 2][0])//if move matches first killer move
			{
				move_list->list[move_list->num].score = KILLER_MOVE_SCORE - 2;
			}
			else if (temp == board->the_killers[board->hply - 2][1])//if move matches second killer move
			{
				move_list->list[move_list->num].score = KILLER_MOVE_SCORE - 3;
			}
		}

		//Piece square ordering
		if (move_list->list[move_list->num].score == 0) //If score is still zero
		{
			move_list->list[move_list->num].score = middle_piece_square_tables[piece][SQUARE_120_TO_64(to)] - middle_piece_square_tables[piece][SQUARE_120_TO_64(from)];
		}
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

//Returns 1 if movelists are identical, otherwise 0
int Movelists_Identical(MOVE_LIST_STRUCT *ptr1, MOVE_LIST_STRUCT *ptr2)
{
	ASSERT(ptr1->num == ptr2->num)
	
	int move, score, found;

	//For each move in list 1, search all of list 2 to find an exact match
	for (int i = 0; i < ptr1->num; i++)
	{
		move = ptr1->list[i].move;
		score = ptr1->list[i].score;
		found = 0;
		for (int j = 0; j < ptr2->num; j++)
		{
			if (move == ptr2->list[j].move && score == ptr2->list[j].score) found = 1;
		}
		if (!found)
		{
			Print_Move(&ptr1->list[i]);
			printf(" not found!\n");
			return 0;
		}
	}
	return 1;
}

//Clears a movelist
void Clear_Movelist(MOVE_LIST_STRUCT *ptr)
{
	ptr->num = 0;
	memset(ptr->list, 0, MAX_MOVE_LIST_LENGTH*sizeof(MOVE_STRUCT));
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