/***** eval.cpp
* Contains evaluation function for board state
* Theo Kanning 12/5/14
*/
#include "globals.h"


int Evaluate_Board(BOARD_STRUCT *board)
{
	int score = 0;

	//Material score
	score = board->material;

	//Piece square table score
	score += board->piece_square_score;

	//Pawn structure score
	//Add later

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

//Returns the piece square table score of a piece in a given position
int Get_Piece_Square_Score(int index64, int piece)
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
		return king_piece_square_table[63 - index64];
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
		return -king_piece_square_table[index64];
	default:
		ASSERT(1 == 2);
	}
	return 0;
}