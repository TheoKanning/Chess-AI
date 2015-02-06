/* see
* Contains all static exchange evaluation functions
* Theo Kanning 2/3/14
*/

#include "globals.h"
#include "stdlib.h"

//Returns bitboard with least valuable attacker index filled, stores piece in pointer
//Returns piece of given color, assumes there is at least one attacker
U64 Least_Valuable_Attacker(int color, U64 attackers, int *piece_ptr, BOARD_STRUCT *board)
{
	int piece, i;
	U64 match;

	if (color == WHITE) piece = wP;
	else piece = bP;

	//Find matching piece
	while (!(match = board->pawn_bitboards[piece] & attackers))
	{
		piece++;
	}

	//Store piece type
	*piece_ptr = piece;

	i = pop_1st_bit(&match);

	return 1i64 << i;
}

//Returns static exchange evaluation score
int Static_Exchange_Evaluation(int move, BOARD_STRUCT *board)
{
	if (IS_EP_CAPTURE(move)) return 0; //Handle ep captures later

	int attacker = GET_PIECE(move);
	int square = SQUARE_120_TO_64(GET_TO_SQ(move));

	int score[32] = { 0 };

	const U64 all_rooks = (board->piece_bitboards[wR] | board->piece_bitboards[wQ] | board->piece_bitboards[bR] | board->piece_bitboards[bQ]);
	const U64 all_bishops = board->piece_bitboards[wB] | board->piece_bitboards[wQ] | board->piece_bitboards[bB] | board->piece_bitboards[bQ];

	U64 all_pieces = board->side_bitboards[BOTH];

	//Remove attacking piece
	all_pieces &= ~(1i64 << SQUARE_120_TO_64(GET_FROM_SQ(move)));

	U64 all_attackers = ((bpawn_attack_masks[square] & board->piece_bitboards[wP]) |
		(wpawn_attack_masks[square] & board->piece_bitboards[bP]) |
		(knight_attack_masks[square] & (board->piece_bitboards[wN] | board->piece_bitboards[bN])) |
		(Bishop_Attacks(all_pieces, square) & (board->piece_bitboards[wB] | board->piece_bitboards[bB] | board->piece_bitboards[wQ] | board->piece_bitboards[bQ])) |
		(Rook_Attacks(all_pieces, square) & (board->piece_bitboards[wR] | board->piece_bitboards[bR] | board->piece_bitboards[wQ] | board->piece_bitboards[bQ])) |
		(king_attack_masks[square] & (board->piece_bitboards[wK] | board->piece_bitboards[bK])));

	//Remove attacking piece
	all_attackers &= ~(1i64 << SQUARE_120_TO_64(GET_FROM_SQ(move)));
	
	//Store initial score
	score[0] = piece_values[GET_CAPTURE(move)];

	//Return if no new attackers found
	if (!(all_attackers & board->side_bitboards[board->side ^ 1]))
	{
		return score[0];
	}

	int depth = 1;
	int side = board->side ^ 1;

	do
	{
		//Calculate new score
		score[depth] = piece_values[attacker] - score[depth - 1];

		//Exit if bad capture
		if (score[depth] < 0 && score[depth - 1] < 0) break;

		//Get next attacker, we already no there is at least one
		U64 attacker_mask = Least_Valuable_Attacker(side, all_attackers, &attacker, board);

		depth++;
		side ^= 1;

		all_pieces &= ~attacker_mask;

		// Update attacker list due to uncovered attacks
		all_attackers |= ((Rook_Attacks(all_pieces, square) & all_rooks) |
			(Bishop_Attacks(all_pieces, square) & all_bishops));

		all_attackers &= all_pieces;

		if (!(all_attackers & board->side_bitboards[side])) {
			break;
		}

		if (IS_KING(attacker)) {
			// This is needed in case both kings can attack the target square.
			// If only one piece can attack the target square, see() still returns correct
			// result even without this condition.
			score[depth++] = piece_values[attacker];
			break;
		}


	} while (true);

	// Propagate scores back
	while (--depth) {
		score[depth - 1] = -(score[depth] > -score[depth-1] ? score[depth] : -score[depth-1]);
	}

	return score[0];
}

//Reads a move list and sets the score of all capture moves to their SEE score, and all quiet moves to -INF
void Set_Quiescent_SEE_Scores(MOVE_LIST_STRUCT *move_list, BOARD_STRUCT *board)
{
	int move_num;
	for (int i = 0; i < move_list->num; i++)
	{
		move_num = move_list->list[i].move;
		if (IS_PROMOTION(move_num))
		{
			continue;
		}
		else if (IS_CAPTURE(move_num))
		{
			move_list->list[i].score = CAPTURE_SCORE + Static_Exchange_Evaluation(move_num, board);
		}
	}
}