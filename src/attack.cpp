/* attack
* Determines if a given space is under attack
* Theo Kanning 2/1/15
*/

#include "globals.h"

U64 between[64][64] = { 0i64 }; //Masks for squares in between pairs of squares


//Returns 1 if square is under attack by given side
int Under_Attack_Fast(int sq, int side, BOARD_STRUCT *board)
{
	U64 pawns, knights, bishops, rooks, king;
	int from;
	U64 occ = board->side_bitboards[BOTH];

	if (side == WHITE)
	{
		//Pawns
		pawns = board->piece_bitboards[wP];
		if (bpawn_attack_masks[sq] & pawns) return 1;

		//Knights
		knights = board->piece_bitboards[wN];
		if (knight_attack_masks[sq] & knights) return 1;

		//Bishops
		bishops = board->piece_bitboards[wB] | board->piece_bitboards[wQ];
		bishops &= bishop_attack_masks[sq];

		//Check for blockers
		while (bishops)
		{
			from = pop_1st_bit(&bishops);
			if ((between[sq][from] & occ) == 0) return 1; //Return 1 if no blockers are found
		}

		//Rooks
		rooks = board->piece_bitboards[wR] | board->piece_bitboards[wQ];
		rooks &= rook_attack_masks[sq];

		//Check for blockers
		while (rooks)
		{
			from = pop_1st_bit(&rooks);
			if ((between[sq][from] & occ) == 0) return 1; //Return 1 if no blockers are found
		}

		//King
		king = board->piece_bitboards[wK];
		if (king & king_attack_masks[sq]) return 1;
	}
	else //side == BLACK
	{
		//Pawns
		pawns = board->piece_bitboards[bP];
		if (wpawn_attack_masks[sq] & pawns) return 1;

		//Knights
		knights = board->piece_bitboards[bN];
		if (knight_attack_masks[sq] & knights) return 1;

		//Bishops
		bishops = board->piece_bitboards[bB] | board->piece_bitboards[bQ];
		bishops &= bishop_attack_masks[sq];

		//Check for blockers
		while (bishops)
		{
			from = pop_1st_bit(&bishops);
			if ((between[sq][from] & occ) == 0) return 1; //Return 1 if no blockers are found
		}

		//Rooks
		rooks = board->piece_bitboards[bR] | board->piece_bitboards[bQ];
		rooks &= rook_attack_masks[sq];

		//Check for blockers
		while (rooks)
		{
			from = pop_1st_bit(&rooks);
			if ((between[sq][from] & occ) == 0) return 1; //Return 1 if no blockers are found
		}

		//King
		king = board->piece_bitboards[bK];
		if (king & king_attack_masks[sq]) return 1;
	}

	return 0; //If nothing found, return 0
}


//Sees if the specified square is under attack by the given side
//Returns 1 if under attack, else 0
int Under_Attack(int target120, int side, BOARD_STRUCT *board)
{
	int piece, direction, slider_num, from120;

	ASSERT(ON_BOARD_120(target120));

	if (side == WHITE)
	{
		//Pawns
		if (board->board_array120[target120 - 9] == wP) return 1; //Check both directions
		if (board->board_array120[target120 - 11] == wP) return 1;

		//Knights
		for (direction = 0; direction < NUM_KNIGHT_MOVES; direction++)
		{
			if (board->board_array120[target120 + KNIGHT_MOVES[direction]] == wN) return 1; //Check all knight sources, return 1 if found
		}

		//Bishops and queens
		for (direction = 0; direction < NUM_BISHOP_MOVES; direction++)
		{
			for (slider_num = 1; slider_num < 8; slider_num++)
			{
				from120 = target120 + slider_num*BISHOP_MOVES[direction];
				if (!ON_BOARD_120(from120))
				{
					break; //End this direction
				}
				else //Continue searching in this direction
				{
					piece = board->board_array120[from120];
					if ((piece == wB) || (piece == wQ))
					{
						return 1;
					}
					else if (piece != EMPTY) //Piece in the way
					{
						break; //End direction
					}
				}
			}
		}

		//Rooks and queens
		for (direction = 0; direction < NUM_ROOK_MOVES; direction++)
		{
			for (slider_num = 1; slider_num < 8; slider_num++)
			{
				from120 = target120 + slider_num*ROOK_MOVES[direction];
				if (!ON_BOARD_120(from120))
				{
					break; //End this direction
				}
				else //Continue searching in this direction
				{
					piece = board->board_array120[from120];
					if ((piece == wR) || (piece == wQ))
					{
						return 1;
					}
					else if (piece != EMPTY) //Piece in the way
					{
						break; //End direction
					}
				}
			}
		}

		//King
		for (direction = 0; direction < NUM_KING_MOVES; direction++)
		{
			if (board->board_array120[target120 + KING_MOVES[direction]] == wK) return 1; //Check all knight sources, return 1 if found
		}

	}
	else //side == BLACK
	{
		//Pawns
		if (board->board_array120[target120 + 9] == bP) return 1; //Check both directions
		if (board->board_array120[target120 + 11] == bP) return 1;

		//Knights
		for (direction = 0; direction < NUM_KNIGHT_MOVES; direction++)
		{
			if (board->board_array120[target120 + KNIGHT_MOVES[direction]] == bN) return 1; //Check all knight sources, return 1 if found
		}

		//Bishops and queens
		for (direction = 0; direction < NUM_BISHOP_MOVES; direction++)
		{
			for (slider_num = 1; slider_num < 8; slider_num++)
			{
				from120 = target120 + slider_num*BISHOP_MOVES[direction];
				if (!ON_BOARD_120(from120))
				{
					break; //End this direction
				}
				else //Continue searching in this direction
				{
					piece = board->board_array120[from120];
					if ((piece == bB) || (piece == bQ))
					{
						return 1;
					}
					else if (piece != EMPTY) //Piece in the way
					{
						break; //End direction
					}
				}
			}
		}

		//Rooks and queens
		for (direction = 0; direction < NUM_ROOK_MOVES; direction++)
		{
			for (slider_num = 1; slider_num < 8; slider_num++)
			{
				from120 = target120 + slider_num*ROOK_MOVES[direction];
				if (!ON_BOARD_120(from120))
				{
					break; //End this direction
				}
				else //Continue searching in this direction
				{
					piece = board->board_array120[from120];
					if ((piece == bR) || (piece == bQ))
					{
						return 1;
					}
					else if (piece != EMPTY) //Piece in the way
					{
						break; //End direction
					}
				}
			}
		}

		//King
		for (direction = 0; direction < NUM_KING_MOVES; direction++)
		{
			if (board->board_array120[target120 + KING_MOVES[direction]] == bK) return 1; //Check all knight sources, return 1 if found
		}
	}

	return 0; //If no attacks were found

}

//Returns 1 if the current side's king is in check
int In_Check(int side, BOARD_STRUCT *board)
{
	int king_square = 0;
	int result;
	if (side == WHITE)
	{
		king_square = board->piece_list120[wK][0];
	}
	else
	{
		king_square = board->piece_list120[bK][0];
	}

	return Under_Attack_Fast(SQUARE_120_TO_64(king_square), side ^ 1, board);

	result = Under_Attack(king_square, side ^ 1, board);
	if (result != Under_Attack_Fast(SQUARE_120_TO_64(king_square), side ^ 1, board))
	{
		Print_Board(board);
		printf("Under Attack values do not match\n");
		printf("Side %d Normal: %d    Fast: %d\n", side, result, Under_Attack_Fast(SQUARE_120_TO_64(king_square), side ^ 1, board));
		Under_Attack_Fast(SQUARE_120_TO_64(king_square), side ^ 1, board);
		system("PAUSE");
	}
	return result;
}


//Fills between array
void Generate_Between_Squares(void)
{
	int start, f_rank, f_file, t_rank, t_file, b_rank, b_file;

	for (start = 0; start < 64; start++)
	{
		f_rank = GET_RANK_64(start);
		f_file = GET_FILE_64(start);

		//Rook moves up
		t_file = f_file;
		for (t_rank = f_rank + 1; t_rank <= RANK_8; t_rank++)
		{
			for (b_rank = f_rank + 1; b_rank < t_rank; b_rank++)
			{
				SET_BIT(between[start][RANK_FILE_TO_SQUARE_64(t_rank, t_file)], RANK_FILE_TO_SQUARE_64(b_rank, t_file));
			}
		}
		//Rook moves right
		t_rank = f_rank;
		for (t_file = f_file + 1; t_file <= FILE_H; t_file++)
		{
			for (b_file = f_file + 1; b_file < t_file; b_file++)
			{
				SET_BIT(between[start][RANK_FILE_TO_SQUARE_64(t_rank, t_file)], RANK_FILE_TO_SQUARE_64(t_rank, b_file));
			}
		}
		//Rook moves down
		t_file = f_file;
		for (t_rank = f_rank - 1; t_rank >= RANK_1; t_rank--)
		{
			for (b_rank = f_rank - 1; b_rank > t_rank; b_rank--)
			{
				SET_BIT(between[start][RANK_FILE_TO_SQUARE_64(t_rank, t_file)], RANK_FILE_TO_SQUARE_64(b_rank, t_file));
			}
		}
		//Rook moves left
		t_rank = f_rank;
		for (t_file = f_file - 1; t_file >= FILE_A; t_file--)
		{
			for (b_file = f_file - 1; b_file > t_file; b_file--)
			{
				SET_BIT(between[start][RANK_FILE_TO_SQUARE_64(t_rank, t_file)], RANK_FILE_TO_SQUARE_64(t_rank, b_file));
			}
		}
		//Bishop moves up and right
		for (t_file = f_file + 1, t_rank = f_rank + 1; t_file <= FILE_H && t_rank <= RANK_8; t_file++, t_rank++)
		{
			for (b_file = f_file + 1, b_rank = f_rank + 1; b_file < t_file, b_rank < t_rank; b_file++, b_rank++)
			{
				SET_BIT(between[start][RANK_FILE_TO_SQUARE_64(t_rank, t_file)], RANK_FILE_TO_SQUARE_64(b_rank, b_file));
			}
		}
		//Bishop moves up and left
		for (t_file = f_file - 1, t_rank = f_rank + 1; t_file >= FILE_A && t_rank <= RANK_8; t_file--, t_rank++)
		{
			for (b_file = f_file - 1, b_rank = f_rank + 1; b_file > t_file, b_rank < t_rank; b_file--, b_rank++)
			{
				SET_BIT(between[start][RANK_FILE_TO_SQUARE_64(t_rank, t_file)], RANK_FILE_TO_SQUARE_64(b_rank, b_file));
			}
		}
		//Bishop moves down and right
		for (t_file = f_file + 1, t_rank = f_rank - 1; t_file <= FILE_H && t_rank >= RANK_1; t_file++, t_rank--)
		{
			for (b_file = f_file + 1, b_rank = f_rank - 1; b_file < t_file, b_rank > t_rank; b_file++, b_rank--)
			{
				SET_BIT(between[start][RANK_FILE_TO_SQUARE_64(t_rank, t_file)], RANK_FILE_TO_SQUARE_64(b_rank, b_file));
			}
		}
		//Bishop moves down and left
		if (start == 56)
		{
			int x = 1;
		}
		for (t_file = f_file - 1, t_rank = f_rank - 1; t_file >= FILE_A && t_rank >= RANK_1; t_file--, t_rank--)
		{
			for (b_file = f_file - 1, b_rank = f_rank - 1; b_file > t_file, b_rank > t_rank; b_file--, b_rank--)
			{
				SET_BIT(between[start][RANK_FILE_TO_SQUARE_64(t_rank, t_file)], RANK_FILE_TO_SQUARE_64(b_rank, b_file));
			}
		}
		
	}
}