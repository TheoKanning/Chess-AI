/***** eval.cpp
* Contains evaluation function for board state
* Theo Kanning 12/5/14
*/
#include "globals.h"

#define ENDGAME_MATERIAL 2000
#define START_MATERIAL	 8000

#define PASSED_PAWN_SCORE 20
#define ISOLATED_PAWN_SCORE	-15

//Pawn evaluation masks
U64 white_passed_masks[64];
U64 black_passed_masks[64];
U64 isolated_masks[64];

int Evaluate_Board(BOARD_STRUCT *board)
{
	int score = 0;

	//Material score
	score = board->material;

	//Piece square table score
	score += Get_Board_Piece_Square_Score(board);

	//Pawn structure score
	score += Get_Pawn_Eval_Score(board);

	//Copy into board
	board->eval_score = score;

	if (board->side == WHITE)
	{
		return score;
	}
	else
	{
		return -score;
	}
}

//Loops through every piece and adds the total piece square contribution to the evaluation function
int Get_Board_Piece_Square_Score(BOARD_STRUCT *board)
{
	int index64;
	int piece;
	int piece_num;
	int score = 0;

	//Calculate game period
	float phase;
	if (board->total_material <= ENDGAME_MATERIAL)
	{
		phase = 1;
	}
	else
	{
		phase = 1 - ((float)(board->total_material - ENDGAME_MATERIAL)) / (START_MATERIAL - ENDGAME_MATERIAL);
	}

	//Loop through every square and add the approriate score
	for (piece = wP; piece <= bK; piece++)
	{
		for (piece_num = 0; piece_num < board->piece_num[piece]; piece_num++)
		{
			index64 = SQUARE_120_TO_64(board->piece_list120[piece][piece_num]);
			score += Get_Piece_Square_Score(index64, piece, phase, board);
		}
	}
	/*
	for (index64 = 0; index64 < 64; index64++)
	{
		piece = board->board_array64[index64];

		if (piece != EMPTY) score += Get_Piece_Square_Score(index64, piece, phase, board);
	}
	*/


	return score;
}

//Returns the piece square table score of a piece in a given position
int Get_Piece_Square_Score(int index64, int piece, float phase, BOARD_STRUCT *board)
{
	switch (piece)
	{
	case EMPTY:
		return 0;
	case wP:
		return pawn_piece_square_table[63 - index64];
	case wN:
		return knight_piece_square_table[63 - index64];
	case wB:
		return bishop_piece_square_table[63 - index64];
	case wR:
		return rook_piece_square_table[63 - index64];
	case wQ:
		return queen_piece_square_table[63 - index64];
	case wK:
		return (int)((1 - phase)*king_piece_square_table[63 - index64] + phase*king_endgame_piece_square_table[63 - index64]);
	case bP:
		return -pawn_piece_square_table[index64];
	case bN:
		return -knight_piece_square_table[index64];
	case bB:
		return -bishop_piece_square_table[index64];
	case bR:
		return -rook_piece_square_table[index64];
	case bQ:
		return -queen_piece_square_table[index64];
	case bK:
		return -(int)((1 - phase)*king_piece_square_table[index64] + phase*king_endgame_piece_square_table[index64]);
	default:
		ASSERT(1 == 2);
	}
	return 0;
}

//Returns pawn evaluation score
int Get_Pawn_Eval_Score(BOARD_STRUCT *board)
{
	int index, sq64;
	int score = 0;
	//Check hash table for stored score (added later)

	//White pawns
	for (index = 0; index < board->piece_num[wP]; index++) //loop through all white pawns
	{
		sq64 = SQUARE_120_TO_64(board->piece_list120[wP][index]); //pawn location in 64 square array

		//if no black pawns in passed mask area
		if ((board->pawn_bitboards[BLACK] & white_passed_masks[sq64]) == 0) score += PASSED_PAWN_SCORE;

		//if no white pawns in isolated mask area
		if ((board->pawn_bitboards[WHITE] & isolated_masks[sq64]) == 0) score += ISOLATED_PAWN_SCORE;

	}


	//Black pawns
	for (index = 0; index < board->piece_num[bP]; index++) //loop through all black pawns
	{
		sq64 = SQUARE_120_TO_64(board->piece_list120[bP][index]); //pawn location in 64 square array

		//if no white pawns in passed mask area
		if ((board->pawn_bitboards[WHITE] & black_passed_masks[sq64]) == 0) score -= PASSED_PAWN_SCORE;

		//if no black pawns in isolated mask area
		if ((board->pawn_bitboards[BLACK] & isolated_masks[sq64]) == 0) score -= ISOLATED_PAWN_SCORE;
	}

	//Store in hash table (added later)

	return score;
}

//Initializes pawn eval masks
void Init_Pawn_Masks(void)
{
	int index,rank,file,rank_index,file_index;

	//Clear all masks;
	for (index = 0; index < 64; index++)
	{
		white_passed_masks[index] = 0;
		black_passed_masks[index] = 0;
		isolated_masks[index] = 0;
	}

	//For each square on the board
	for (index = 0; index < 64; index++)
	{
		rank = GET_RANK_64(index);
		file = GET_FILE_64(index);
		for (rank_index = RANK_1; rank_index <= RANK_8; rank_index++)
		{
			for (file_index = FILE_A; file_index <= FILE_H; file_index++)
			{
				//white passed mask
				//If file is neighboring and space is above index
				if (abs(file - file_index) <= 1 && rank_index > rank)
				{
					SET_BIT(white_passed_masks[index], RANK_FILE_TO_SQUARE_64(rank_index, file_index));
				}

				//black passed mask
				//If file is neighboring and space is below index
				if (abs(file - file_index) <= 1 && rank_index < rank)
				{
					SET_BIT(black_passed_masks[index], RANK_FILE_TO_SQUARE_64(rank_index, file_index));
				}

				//isolated mask
				//If file is neighboring and space is not equal to index
				if (abs(file - file_index) <= 1 && (file != file_index || rank != rank_index))
				{
					SET_BIT(isolated_masks[index], RANK_FILE_TO_SQUARE_64(rank_index, file_index));
				}
			}
		}
	}

}