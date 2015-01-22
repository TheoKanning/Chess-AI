/* tuning.cpp
* Contains arrays and functions for setting parameters via tuning functions
* Theo Kanning 1/19/14 */

#include "globals.h"

int king_end_piece_square_tuning_values[8] = { -50, -30, -30, -30, -10, 10, 20, 40 }; //Starting values
int pawn_end_piece_square_tuning_values[7] = { 0, 0, 10, 20, 30, 40, 50 }; //Starting values
int passed_pawn_rank_bonus[8] = { 0, 0, 0, 5, 10, 15, 20, 0};

static int king_end_tuning_zones[64] =
{
	0, 1, 2, 2, 2, 2, 1, 0,
	1, 3, 4, 4, 4, 4, 3, 1,
	2, 4, 5, 6, 6, 5, 4, 2,
	2, 4, 6, 7, 7, 6, 4, 2,
	2, 4, 6, 7, 7, 6, 4, 2,
	2, 4, 5, 6, 6, 5, 4, 2,
	1, 3, 4, 4, 4, 4, 3, 1,
	0, 1, 2, 2, 2, 2, 1, 0
};

static int pawn_end_tuning_zones[64] =
{
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2,
	3, 3, 3, 3, 3, 3, 3, 3,
	4, 4, 4, 4, 4, 4, 4, 4,
	5, 5, 5, 5, 5, 5, 5, 5,
	6, 6, 6, 6, 6, 6, 6, 6,	
	0, 0, 0, 0, 0, 0, 0, 0
};

//Sets king endgame values to those in the array
void Set_King_End_Values(void)
{
	int index;
	/***** Set values in white king array *****/
	for (index = 0; index < 64; index++)
	{
		end_piece_square_tables[wK][index] = king_end_piece_square_tuning_values[king_end_tuning_zones[index]];
		end_piece_square_tables[bK][index] = king_end_piece_square_tuning_values[king_end_tuning_zones[63 - index]];
	}
}

//Sets pawn endgame values to those in the array
void Set_Pawn_End_Values(void)
{
	int index;
	/***** Set values in white pawn array *****/
	for (index = 0; index < 64; index++)
	{
		end_piece_square_tables[wP][index] = pawn_end_piece_square_tuning_values[pawn_end_tuning_zones[index]];
		end_piece_square_tables[bP][index] = pawn_end_piece_square_tuning_values[pawn_end_tuning_zones[63 - index]];
	}
}