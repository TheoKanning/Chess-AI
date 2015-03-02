/***** eval.cpp
* Contains evaluation function for board state
* Theo Kanning 12/5/14
*/
#include "globals.h"

#define ENDGAME_MATERIAL		2000
#define START_MATERIAL			8000
#define START_BIG_MATERIAL		6400

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
	int total_big_material = board->big_material[BLACK] + board->big_material[BLACK];
	int total_material = total_big_material + board->pawn_material[WHITE] + board->pawn_material[BLACK];
	int material_diff = (board->big_material[WHITE] + board->pawn_material[WHITE]) - (board->big_material[BLACK] + board->pawn_material[BLACK]);
	int winning_pawn_num = (material_diff > 0) ? board->piece_num[wP] : board->piece_num[bP]; //Number of pawns on side winning in material

	/* Material score, adjusted for total material on board */
	int material_score = material_diff;// +(material_diff * winning_pawn_num * (START_MATERIAL - total_material)) / (START_BIG_MATERIAL * (winning_pawn_num + 1));
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
	score += Get_Pawn_And_King_Score(board);

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


//Returns pawn evaluation score
int Get_Pawn_Eval_Score(BOARD_STRUCT *board)
{
	int index, sq64;
	int score = 0;
	const U64 white_pawns = board->piece_bitboards[wP];
	const U64 black_pawns = board->piece_bitboards[bP];
	const U64 all_pawns = white_pawns | black_pawns;

	//Check hash table for stored score (added later)

	//White pawns
	U64 temp = white_pawns;
	while(temp) //loop through all white pawns
	{
		sq64 = pop_1st_bit(&temp); //pawn location in 64 square array

		//if no black pawns in passed mask area
		if ((black_pawns & white_passed_masks[sq64]) == 0) score += passed_pawn_rank_bonus[GET_RANK_64(sq64)];
		
		//if no white pawns in isolated mask area
		if ((white_pawns & isolated_masks[sq64]) == 0) score += ISOLATED_PAWN_SCORE;

		//If a white pawn is found in the same file
		if ((white_pawns & doubled_masks[sq64]) != 0) score -= DOUBLED_PAWN_SCORE;
	}


	//Black pawns
	temp = black_pawns;
	while (temp) //loop through all black pawns
	{
		sq64 = pop_1st_bit(&temp); //pawn location in 64 square array

		//if no white pawns in passed mask area
		if ((white_pawns & black_passed_masks[sq64]) == 0)	score -= passed_pawn_rank_bonus[RANK_8 - GET_RANK_64(sq64)];
		
		//if no black pawns in isolated mask area
		if ((black_pawns & isolated_masks[sq64]) == 0) score -= ISOLATED_PAWN_SCORE;

		//If a black pawn is found in the same file
		if ((black_pawns & doubled_masks[sq64]) != 0) score += DOUBLED_PAWN_SCORE;
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

	U64 temp = board->piece_bitboards[wK];
	int white_king_square = pop_1st_bit(&temp);
	temp = board->piece_bitboards[bK];
	int black_king_square = pop_1st_bit(&temp);

	//White king on kingside
	if (GET_FILE_64(white_king_square) > FILE_E && GET_RANK_64(white_king_square) == RANK_1) {

		if (board->board_array64[F2] == wP)  white_score += PAWN_SHIELD_SCORE;
		else if (board->board_array64[F3] == wP)  white_score += PAWN_SHIELD_SCORE / 2;

		if (board->board_array64[G2] == wP)  white_score += PAWN_SHIELD_SCORE;
		else if (board->board_array64[G3] == wP)  white_score += PAWN_SHIELD_SCORE / 2;

		if (board->board_array64[H2] == wP)  white_score += PAWN_SHIELD_SCORE;
		else if (board->board_array64[H3] == wP)  white_score += PAWN_SHIELD_SCORE / 2;
	}
	else if (GET_FILE_64(white_king_square) < FILE_D && GET_RANK_64(white_king_square) == RANK_1) {

		if (board->board_array64[A2] == wP)  white_score += PAWN_SHIELD_SCORE;
		else if (board->board_array64[A3] == wP)  white_score += PAWN_SHIELD_SCORE / 2;

		if (board->board_array64[B2] == wP)  white_score += PAWN_SHIELD_SCORE;
		else if (board->board_array64[B3] == wP)  white_score += PAWN_SHIELD_SCORE / 2;

		if (board->board_array64[C2] == wP)  white_score += PAWN_SHIELD_SCORE;
		else if (board->board_array64[C3] == wP)  white_score += PAWN_SHIELD_SCORE / 2;
	}
	

	//Black king on kingside
	if (GET_FILE_64(black_king_square) > FILE_E && GET_RANK_64(black_king_square) == RANK_8) {

		if (board->board_array64[F7] == bP)  black_score += PAWN_SHIELD_SCORE;
		else if (board->board_array64[F6] == bP)  black_score += PAWN_SHIELD_SCORE / 2;

		if (board->board_array64[G7] == bP)  black_score += PAWN_SHIELD_SCORE;
		else if (board->board_array64[G6] == bP)  black_score += PAWN_SHIELD_SCORE / 2;

		if (board->board_array64[H7] == bP)  black_score += PAWN_SHIELD_SCORE;
		else if (board->board_array64[H6] == bP)  black_score += PAWN_SHIELD_SCORE / 2;
	}
	else if (GET_FILE_64(black_king_square) < FILE_D && GET_RANK_64(black_king_square) == RANK_8) {

		if (board->board_array64[A7] == bP)  black_score += PAWN_SHIELD_SCORE;
		else if (board->board_array64[A6] == bP)  black_score += PAWN_SHIELD_SCORE / 2;

		if (board->board_array64[B7] == bP)  black_score += PAWN_SHIELD_SCORE;
		else if (board->board_array64[B6] == bP)  black_score += PAWN_SHIELD_SCORE / 2;

		if (board->board_array64[C7] == bP)  black_score += PAWN_SHIELD_SCORE;
		else if (board->board_array64[C6] == bP)  black_score += PAWN_SHIELD_SCORE / 2;
	}

	return white_score - black_score;

}

//Returns evaluation for pawns and kings, checks hash table for hit
int Get_Pawn_And_King_Score(BOARD_STRUCT *board)
{
	//Check hash table
	int hash_score = Get_Pawn_Hash_Entry(board->pawn_hash_key);
	//if (hash_score != INVALID) return hash_score;

	//Calculate scores and store hash data
	int score = Get_Pawn_Eval_Score(board);

	score += Get_King_Safety_Score(board);

	Add_Pawn_Hash_Entry(score, board->pawn_hash_key);

	return score;
}