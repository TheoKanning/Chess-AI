/***** eval.cpp
* Contains evaluation function for board state
* Theo Kanning 12/5/14
*/
#include "globals.h"

#define ENDGAME_MATERIAL		2000
#define START_MATERIAL			8000
#define START_BIG_MATERIAL		6400

#define PASSED_PAWN_SCORE		20
#define ISOLATED_PAWN_SCORE	   -15
#define DOUBLED_PAWN_SCORE		5	//This is counted once for each pawn

#define PAWN_SHIELD_SCORE		10	//Score for each pawn in front of the king

#define BISHOP_PAIR				50	//Bonus in cp for having bishop pair	
#define KNIGHT_PAIR			   -20	//Penalty in cp for having knight pair
#define ROOK_PAIR			   -20	//Penalty in cp for having rook pair

//Pawn evaluation masks
U64 white_passed_masks[64];
U64 black_passed_masks[64];
U64 isolated_masks[64];
U64 doubled_masks[64];

int Evaluate_Board(BOARD_STRUCT *board)
{
	int score = 0;
	int total_big_material = board->white_big_material + board->black_big_material;
	int total_material = total_big_material + board->white_pawn_material + board->black_pawn_material;
	int material_diff = (board->white_big_material + board->white_pawn_material) - (board->black_big_material + board->black_pawn_material);
	int winning_pawn_num = (material_diff > 0) ? board->piece_num[wP] : board->piece_num[bP]; //Number of pawns on side winning in material

	/* Material score, adjusted for total material on board */
	int material_score = material_diff + (material_diff * winning_pawn_num * (START_MATERIAL - total_material)) / (START_BIG_MATERIAL * (winning_pawn_num + 1));
	score += material_score;

	/* Piece square table score */
	score += (total_big_material * board->middle_piece_square_score + (START_BIG_MATERIAL - total_big_material) * board->end_piece_square_score) / START_BIG_MATERIAL;

	/* Pair bonuses */
	if (board->piece_num[wB] >= 2) score += BISHOP_PAIR;
	if (board->piece_num[bB] >= 2) score -= BISHOP_PAIR;

	if (board->piece_num[wN] >= 2) score += KNIGHT_PAIR;
	if (board->piece_num[bN] >= 2) score -= KNIGHT_PAIR;

	if (board->piece_num[wR] >= 2) score += ROOK_PAIR;
	if (board->piece_num[bR] >= 2) score -= ROOK_PAIR;

	/* Pawn structure score */
	score += Get_Pawn_Eval_Score(board);

	/* King Safety */
	score += Get_King_Safety_Score(board);


	/* Low material correction 
	* Divide score in cases of drawish endings (add later) 
	*/

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
	int total = board->white_big_material + board->white_pawn_material + board->black_big_material + board->black_pawn_material;

	//Calculate game period
	float phase;
	if (total <= ENDGAME_MATERIAL)
	{
		phase = 1;
	}
	else
	{
		phase = 1 - ((float)(total - ENDGAME_MATERIAL)) / (START_MATERIAL - ENDGAME_MATERIAL);
	}

	return (int)((1 - phase)*board->middle_piece_square_score + phase*board->end_piece_square_score);

	int mid = 0, end = 0;

	//Loop through every square and add the approriate score
	for (piece = wP; piece <= bK; piece++)
	{
		for (piece_num = 0; piece_num < board->piece_num[piece]; piece_num++)
		{
			index64 = SQUARE_120_TO_64(board->piece_list120[piece][piece_num]);
			score += Get_Piece_Square_Score(index64, piece, phase);
			mid += (IS_WHITE_PIECE(piece) ? 1 : -1) * middle_piece_square_tables[piece][index64];
			end += (IS_WHITE_PIECE(piece) ? 1 : -1) * end_piece_square_tables[piece][index64];
		}
	}
	ASSERT(board->middle_piece_square_score == mid);
	ASSERT(board->end_piece_square_score == end);

	return score;
}

//Returns the piece square table score of a piece in a given position
int Get_Piece_Square_Score(int index64, int piece, float phase)
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
		if ((board->pawn_bitboards[BLACK] & white_passed_masks[sq64]) == 0) score += passed_pawn_rank_bonus[GET_RANK_64(sq64)];

		//if no white pawns in isolated mask area
		if ((board->pawn_bitboards[WHITE] & isolated_masks[sq64]) == 0) score += ISOLATED_PAWN_SCORE;

		//If a white pawn is found in the same file
		if ((board->pawn_bitboards[WHITE] & doubled_masks[sq64]) != 0) score -= DOUBLED_PAWN_SCORE;

	}


	//Black pawns
	for (index = 0; index < board->piece_num[bP]; index++) //loop through all black pawns
	{
		sq64 = SQUARE_120_TO_64(board->piece_list120[bP][index]); //pawn location in 64 square array

		//if no white pawns in passed mask area
		if ((board->pawn_bitboards[WHITE] & black_passed_masks[sq64]) == 0) score -= passed_pawn_rank_bonus[RANK_8- GET_RANK_64(sq64)];

		//if no black pawns in isolated mask area
		if ((board->pawn_bitboards[BLACK] & isolated_masks[sq64]) == 0) score -= ISOLATED_PAWN_SCORE;

		//If a black pawn is found in the same file
		if ((board->pawn_bitboards[BLACK] & doubled_masks[sq64]) != 0) score += DOUBLED_PAWN_SCORE;
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
		doubled_masks[index] = 0;
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

				//Doubled mask
				//If file is the same and rank is different
				if (file == file_index && rank != rank_index)
				{
					SET_BIT(doubled_masks[index], RANK_FILE_TO_SQUARE_64(rank_index, file_index));
				}
			}
		}
	}

}

//Looks at pawn shielding around each king and returns white - black score
int Get_King_Safety_Score(BOARD_STRUCT *board)
{
	int white_score = 0;
	int black_score = 0;

	int white_king_square = board->piece_list120[wK][0];
	int black_king_square = board->piece_list120[bK][0];

	//White king on kingside
	if (GET_FILE_120(white_king_square) > FILE_E && GET_RANK_120(white_king_square) == RANK_1) {

		if (board->board_array120[F2] == wP)  white_score += PAWN_SHIELD_SCORE;
		else if (board->board_array120[F3] == wP)  white_score += PAWN_SHIELD_SCORE / 2;

		if (board->board_array120[G2] == wP)  white_score += PAWN_SHIELD_SCORE;
		else if (board->board_array120[G3] == wP)  white_score += PAWN_SHIELD_SCORE / 2;

		if (board->board_array120[H2] == wP)  white_score += PAWN_SHIELD_SCORE;
		else if (board->board_array120[H3] == wP)  white_score += PAWN_SHIELD_SCORE / 2;
	}
	else if (GET_FILE_120(white_king_square) < FILE_D && GET_RANK_120(white_king_square) == RANK_1) {

		if (board->board_array120[A2] == wP)  white_score += PAWN_SHIELD_SCORE;
		else if (board->board_array120[A3] == wP)  white_score += PAWN_SHIELD_SCORE / 2;

		if (board->board_array120[B2] == wP)  white_score += PAWN_SHIELD_SCORE;
		else if (board->board_array120[B3] == wP)  white_score += PAWN_SHIELD_SCORE / 2;

		if (board->board_array120[C2] == wP)  white_score += PAWN_SHIELD_SCORE;
		else if (board->board_array120[C3] == wP)  white_score += PAWN_SHIELD_SCORE / 2;
	}
	

	//Black king on kingside
	if (GET_FILE_120(black_king_square) > FILE_E && GET_RANK_120(black_king_square) == RANK_8) {

		if (board->board_array120[F7] == bP)  black_score += PAWN_SHIELD_SCORE;
		else if (board->board_array120[F6] == bP)  black_score += PAWN_SHIELD_SCORE / 2;

		if (board->board_array120[G7] == bP)  black_score += PAWN_SHIELD_SCORE;
		else if (board->board_array120[G6] == bP)  black_score += PAWN_SHIELD_SCORE / 2;

		if (board->board_array120[H7] == bP)  black_score += PAWN_SHIELD_SCORE;
		else if (board->board_array120[H6] == bP)  black_score += PAWN_SHIELD_SCORE / 2;
	}
	else if (GET_FILE_120(black_king_square) < FILE_D && GET_RANK_120(black_king_square) == RANK_8) {

		if (board->board_array120[A7] == bP)  black_score += PAWN_SHIELD_SCORE;
		else if (board->board_array120[A6] == bP)  black_score += PAWN_SHIELD_SCORE / 2;

		if (board->board_array120[B7] == bP)  black_score += PAWN_SHIELD_SCORE;
		else if (board->board_array120[B6] == bP)  black_score += PAWN_SHIELD_SCORE / 2;

		if (board->board_array120[C7] == bP)  black_score += PAWN_SHIELD_SCORE;
		else if (board->board_array120[C6] == bP)  black_score += PAWN_SHIELD_SCORE / 2;
	}

	return white_score - black_score;

}