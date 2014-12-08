/*makemove.c
* Contains functions for making and taking moves from the board, and updating all list and array data
* Theo Kanning 11/30/14 
*/
#include "globals.h"

using namespace std;

//Hashkey data
extern U64 piece_keys[13][64];//[square][piece]
extern U64 side_keys[2];
extern U64 ep_keys[101]; //NO_SQUARE = 100;
extern U64 castle_keys[16];

static void Move_Piece(int from120, int to120, BOARD_STRUCT *board);
static void Capture_Piece(int index120, BOARD_STRUCT *board);


/* Move data structure
32 bit integer
bits [0:6] from square index 120
bits [7:13] to square index 120
bits [14:17] moved piece
bits [18:21] captured piece, if any
bit  [22:24] Special flags
	0 0 0 Quiet move
	0 0 1 Ep capture
	0 1 0 King side castle
	0 1 1 Queen side castle
	1 0 0 Promote to queen
	1 0 1 Promote to rook
	1 1 0 Promote to bishop
	1 1 1 Promote to knight
*/

void Make_Move(MOVE_STRUCT *move, BOARD_STRUCT *board)
{
	int from120, to120, from64, to64, piece, capture, side, castle_temp, ep_capture120, ep_capture64;
	int move_num = move->move;

	from120 = GET_FROM_SQ(move_num);
	to120 = GET_TO_SQ(move_num);
	from64 = SQUARE_120_TO_64(from120);
	to64 = SQUARE_120_TO_64(to120);

	piece = GET_PIECE(move_num);
	capture = GET_CAPTURE(move_num);
	side = board->side;

	ASSERT(ON_BOARD_120(from120));
	ASSERT(ON_BOARD_120(to120));
	ASSERT((EMPTY < piece) && (piece <= bK));
	ASSERT((EMPTY <= capture) && (capture <= bK));

	ASSERT(board->board_array120[from120] == piece);
	ASSERT(board->board_array120[to120] == capture);

	/***** Quiet Moves and Captures *****/
	if (IS_NOT_SPECIAL(move_num))
	{
		Move_Piece(from120, to120, board);
		
		/***** En Passant Square *****/
		HASH_OUT(board->hash_key, ep_keys[board->ep]); //Remove to add later
		board->ep = NO_SQUARE;
		if (piece == wP)
		{
			if (from120 + 20 == to120) board->ep = from120 + 10; //If double push
		}
		else if (piece == bP)
		{
			if (from120 - 20 == to120) board->ep = from120 - 10; //If double push
		}
		HASH_IN(board->hash_key, ep_keys[board->ep]); //Re add ep key
	}
	else if (IS_EP_CAPTURE(move_num))
	{
		HASH_OUT(board->hash_key, ep_keys[board->ep]);
		board->ep = NO_SQUARE; //Reset ep square
		HASH_IN(board->hash_key, ep_keys[board->ep]);

		if (side == WHITE)
		{
			ep_capture120 = to120 - 10; //Square with captured pawn on it
			capture = bP;
		}
		else
		{
			ep_capture120 = to120 + 10;
			capture = wP;
		}
		ep_capture64 = SQUARE_120_TO_64(ep_capture120);

			/***** board_array120[120] *****/
			//Remove from square
			ASSERT(board->board_array120[from120] == piece);
			board->board_array120[from120] = EMPTY;

			//Add to square
			ASSERT(board->board_array120[to120] == EMPTY);
			board->board_array120[to120] = piece;

			//Remove capture pawn
			ASSERT(board->board_array120[ep_capture120] == capture); //Black pawn in capture square
			board->board_array120[ep_capture120] = EMPTY; //Remove capture

			/***** board_array64[64] *****/
			//Remove from square
			board->board_array64[from64] = EMPTY;
			//Add to square
			board->board_array64[to64] = piece;
			//Remove captured pawn
			board->board_array64[ep_capture64] = EMPTY; //Remove capture

			//Update hashkey
			HASH_OUT(board->hash_key, piece_keys[piece][from64]); //From square
			HASH_IN(board->hash_key, piece_keys[EMPTY][from64]);

			HASH_OUT(board->hash_key, piece_keys[EMPTY][to64]); //From square
			HASH_IN(board->hash_key, piece_keys[piece][to64]);

			HASH_OUT(board->hash_key, piece_keys[capture][ep_capture64]); //Remove captured pawn
			HASH_IN(board->hash_key, piece_keys[EMPTY][ep_capture64]);


			/***** Piece Lists *****/
			Remove_From_Piecelists(piece, from120, board);
			Add_To_Piecelists(piece, to120, board);
			Remove_From_Piecelists(capture, ep_capture120, board);
			//These functions automatically update material score

			/***** Pawn Bitboards *****/
			CLR_BIT(board->pawn_bitboards[WHITE], from64);
			CLR_BIT(board->pawn_bitboards[BOTH], from64);

			SET_BIT(board->pawn_bitboards[WHITE], to64);
			SET_BIT(board->pawn_bitboards[BOTH], to64);

			if (capture == bP)
			{
				CLR_BIT(board->pawn_bitboards[BLACK], ep_capture64);
				CLR_BIT(board->pawn_bitboards[BOTH], ep_capture64);
			}
			else
			{
				CLR_BIT(board->pawn_bitboards[WHITE], ep_capture64);
				CLR_BIT(board->pawn_bitboards[BOTH], ep_capture64);
			}
	}
	else if (IS_KING_CASTLE(move_num))
	{
		if (piece == wK)
		{
			ASSERT(from120 == E1);
			ASSERT(to120 == G1);
			ASSERT(board->board_array120[to120] == EMPTY);

			//Move king
			board->board_array120[from120] = EMPTY; //Remove from E1
			board->board_array64[from64] = EMPTY;
			Remove_From_Piecelists(wK, from120, board);
			HASH_OUT(board->hash_key, piece_keys[wK][from64]);
			HASH_IN(board->hash_key, piece_keys[EMPTY][from64]);

			board->board_array120[to120] = wK; //Add to G1
			board->board_array64[to64] = wK;
			Add_To_Piecelists(wK, to120, board);
			HASH_OUT(board->hash_key, piece_keys[EMPTY][to64]);
			HASH_IN(board->hash_key, piece_keys[wK][to64]);

			//Move rook
			board->board_array120[H1] = EMPTY; //Remove from H1
			board->board_array64[SQUARE_120_TO_64(H1)] = EMPTY; 
			Remove_From_Piecelists(wR, H1, board);
			HASH_OUT(board->hash_key, piece_keys[wR][SQUARE_120_TO_64(H1)]);
			HASH_IN(board->hash_key, piece_keys[EMPTY][SQUARE_120_TO_64(H1)]);

			board->board_array120[F1] = wR; //Add to F1
			board->board_array64[SQUARE_120_TO_64(F1)] = wR; 
			Add_To_Piecelists(wR, F1, board);
			HASH_OUT(board->hash_key, piece_keys[EMPTY][SQUARE_120_TO_64(F1)]);
			HASH_IN(board->hash_key, piece_keys[wR][SQUARE_120_TO_64(F1)]);

			//Update castling rights and hash
			HASH_OUT(board->hash_key, castle_keys[board->castle_rights]); //Remove old hashkey
			board->castle_rights &= ~WK_CASTLE; //Remove kingside rights
			board->castle_rights &= ~WQ_CASTLE; //Remove queenside rights
			HASH_IN(board->hash_key, castle_keys[board->castle_rights]); //Add new hashkey

		}
		else if (piece == bK)
		{
			ASSERT(from120 == E8);
			ASSERT(to120 == G8);
			ASSERT(board->board_array120[to120] == EMPTY);

			//Move king
			board->board_array120[from120] = EMPTY; //Remove from E8
			board->board_array64[from64] = EMPTY;
			Remove_From_Piecelists(bK, from120, board);
			HASH_OUT(board->hash_key, piece_keys[bK][from64]);
			HASH_IN(board->hash_key, piece_keys[EMPTY][from64]);

			board->board_array120[to120] = bK; //Add to G8
			board->board_array64[to64] = bK;
			Add_To_Piecelists(bK, to120, board);
			HASH_OUT(board->hash_key, piece_keys[EMPTY][to64]);
			HASH_IN(board->hash_key, piece_keys[bK][to64]);

			//Move rook
			board->board_array120[H8] = EMPTY; //Remove from H8
			board->board_array64[SQUARE_120_TO_64(H8)] = EMPTY;
			Remove_From_Piecelists(bR, H8, board);
			HASH_OUT(board->hash_key, piece_keys[bR][SQUARE_120_TO_64(H8)]);
			HASH_IN(board->hash_key, piece_keys[EMPTY][SQUARE_120_TO_64(H8)]);

			board->board_array120[F8] = bR; //Add to F8
			board->board_array64[SQUARE_120_TO_64(F8)] = bR;
			Add_To_Piecelists(bR, F8, board);
			HASH_OUT(board->hash_key, piece_keys[EMPTY][SQUARE_120_TO_64(F8)]);
			HASH_IN(board->hash_key, piece_keys[bR][SQUARE_120_TO_64(F8)]);

			//Update castling rights and hash
			HASH_OUT(board->hash_key, castle_keys[board->castle_rights]); //Remove old hashkey
			board->castle_rights &= ~BK_CASTLE; //Remove kingside rights
			board->castle_rights &= ~BQ_CASTLE; //Remove queenside rights
			HASH_IN(board->hash_key, castle_keys[board->castle_rights]); //Add new hashkey
		}
	}
	else if (IS_QUEEN_CASTLE(move_num))
	{
		if (piece == wK)
		{
			ASSERT(from120 == E1);
			ASSERT(to120 == C1);
			ASSERT(board->board_array120[to120] == EMPTY);

			//Move king
			board->board_array120[from120] = EMPTY; //Remove from E1
			board->board_array64[from64] = EMPTY;
			Remove_From_Piecelists(wK, from120, board);
			HASH_OUT(board->hash_key, piece_keys[wK][from64]);
			HASH_IN(board->hash_key, piece_keys[EMPTY][from64]);

			board->board_array120[to120] = wK; //Add to C1
			board->board_array64[to64] = wK;
			Add_To_Piecelists(wK, to120, board);
			HASH_OUT(board->hash_key, piece_keys[EMPTY][to64]);
			HASH_IN(board->hash_key, piece_keys[wK][to64]);

			//Move rook
			board->board_array120[A1] = EMPTY; //Remove from A1
			board->board_array64[SQUARE_120_TO_64(A1)] = EMPTY;
			Remove_From_Piecelists(wR, A1, board);
			HASH_OUT(board->hash_key, piece_keys[wR][SQUARE_120_TO_64(A1)]);
			HASH_IN(board->hash_key, piece_keys[EMPTY][SQUARE_120_TO_64(A1)]);

			board->board_array120[D1] = wR; //Add to D1
			board->board_array64[SQUARE_120_TO_64(D1)] = wR;
			Add_To_Piecelists(wR, D1, board);
			HASH_OUT(board->hash_key, piece_keys[EMPTY][SQUARE_120_TO_64(D1)]);
			HASH_IN(board->hash_key, piece_keys[wR][SQUARE_120_TO_64(D1)]);

			//Update castling rights and hash
			HASH_OUT(board->hash_key, castle_keys[board->castle_rights]); //Remove old hashkey
			board->castle_rights &= ~WK_CASTLE; //Remove kingside rights
			board->castle_rights &= ~WQ_CASTLE; //Remove queenside rights
			HASH_IN(board->hash_key, castle_keys[board->castle_rights]); //Add new hashkey

		}
		else if (piece == bK)
		{
			ASSERT(from120 == E8);
			ASSERT(to120 == C8);
			ASSERT(board->board_array120[to120] == EMPTY);

			//Move king
			board->board_array120[from120] = EMPTY; //Remove from E8
			board->board_array64[from64] = EMPTY;
			Remove_From_Piecelists(bK, from120, board);
			HASH_OUT(board->hash_key, piece_keys[bK][from64]);
			HASH_IN(board->hash_key, piece_keys[EMPTY][from64]);

			board->board_array120[to120] = bK; //Add to C8
			board->board_array64[to64] = bK;
			Add_To_Piecelists(bK, to120, board);
			HASH_OUT(board->hash_key, piece_keys[EMPTY][to64]);
			HASH_IN(board->hash_key, piece_keys[bK][to64]);

			//Move rook
			board->board_array120[A8] = EMPTY; //Remove from A8
			board->board_array64[SQUARE_120_TO_64(A8)] = EMPTY;
			Remove_From_Piecelists(bR, A8, board);
			HASH_OUT(board->hash_key, piece_keys[bR][SQUARE_120_TO_64(A8)]);
			HASH_IN(board->hash_key, piece_keys[EMPTY][SQUARE_120_TO_64(A8)]);

			board->board_array120[D8] = bR; //Add to D8
			board->board_array64[SQUARE_120_TO_64(D8)] = bR;
			Add_To_Piecelists(bR, D8, board);
			HASH_OUT(board->hash_key, piece_keys[EMPTY][SQUARE_120_TO_64(D8)]);
			HASH_IN(board->hash_key, piece_keys[bR][SQUARE_120_TO_64(D8)]);

			//Update castling rights and hash
			HASH_OUT(board->hash_key, castle_keys[board->castle_rights]); //Remove old hashkey
			board->castle_rights &= ~BK_CASTLE; //Remove kingside rights
			board->castle_rights &= ~BQ_CASTLE; //Remove queenside rights
			HASH_IN(board->hash_key, castle_keys[board->castle_rights]); //Add new hashkey
		}
	}
	else if (IS_QUEEN_PROMOTION(move_num))
	{
		if (side == WHITE)
		{
			ASSERT(GET_RANK_64(from64) == RANK_7);
			ASSERT(GET_RANK_64(to64) == RANK_8);
			ASSERT(piece == wP);
			
			//Remove pawn
			board->board_array120[from120] = EMPTY; 
			board->board_array64[from64] = EMPTY;
			Remove_From_Piecelists(wP, from120, board);
			HASH_OUT(board->hash_key, piece_keys[wP][from64]);
			HASH_IN(board->hash_key, piece_keys[EMPTY][from64]);

			//Add queen
			board->board_array120[to120] = wQ; //Add to C8
			board->board_array64[to64] = wQ;
			Add_To_Piecelists(wQ, to120, board);
			HASH_OUT(board->hash_key, piece_keys[capture][to64]);
			HASH_IN(board->hash_key, piece_keys[wQ][to64]);

			//Remove captured piece from lists
			Remove_From_Piecelists(capture, to120, board);

			//Bitboards
			CLR_BIT(board->pawn_bitboards[WHITE], from64);
			CLR_BIT(board->pawn_bitboards[BOTH], from64);
		}
		else
		{
			ASSERT(GET_RANK_64(from64) == RANK_2);
			ASSERT(GET_RANK_64(to64) == RANK_1);
			ASSERT(piece == bP);

			//Remove pawn
			board->board_array120[from120] = EMPTY;
			board->board_array64[from64] = EMPTY;
			Remove_From_Piecelists(bP, from120, board);
			HASH_OUT(board->hash_key, piece_keys[bP][from64]);
			HASH_IN(board->hash_key, piece_keys[EMPTY][from64]);

			//Add queen
			board->board_array120[to120] = bQ; //Add to C8
			board->board_array64[to64] = bQ;
			Add_To_Piecelists(bQ, to120, board);
			HASH_OUT(board->hash_key, piece_keys[capture][to64]);
			HASH_IN(board->hash_key, piece_keys[bQ][to64]);

			//Remove captured piece from lists
			Remove_From_Piecelists(capture, to120, board);

			//Bitboards
			CLR_BIT(board->pawn_bitboards[BLACK], from64);
			CLR_BIT(board->pawn_bitboards[BOTH], from64);
		}
	}
	else if (IS_ROOK_PROMOTION(move_num))
	{
		if (side == WHITE)
		{
			ASSERT(GET_RANK_64(from64) == RANK_7);
			ASSERT(GET_RANK_64(to64) == RANK_8);
			ASSERT(piece == wP);

			//Remove pawn
			board->board_array120[from120] = EMPTY;
			board->board_array64[from64] = EMPTY;
			Remove_From_Piecelists(wP, from120, board);
			HASH_OUT(board->hash_key, piece_keys[wP][from64]);
			HASH_IN(board->hash_key, piece_keys[EMPTY][from64]);

			//Add rook
			board->board_array120[to120] = wR; //Add to C8
			board->board_array64[to64] = wR;
			Add_To_Piecelists(wR, to120, board);
			HASH_OUT(board->hash_key, piece_keys[capture][to64]);
			HASH_IN(board->hash_key, piece_keys[wR][to64]);

			//Remove captured piece from lists
			Remove_From_Piecelists(capture, to120, board);

			//Bitboards
			CLR_BIT(board->pawn_bitboards[WHITE], from64);
			CLR_BIT(board->pawn_bitboards[BOTH], from64);
		}
		else
		{
			ASSERT(GET_RANK_64(from64) == RANK_2);
			ASSERT(GET_RANK_64(to64) == RANK_1);
			ASSERT(piece == bP);

			//Remove pawn
			board->board_array120[from120] = EMPTY;
			board->board_array64[from64] = EMPTY;
			Remove_From_Piecelists(bP, from120, board);
			HASH_OUT(board->hash_key, piece_keys[bP][from64]);
			HASH_IN(board->hash_key, piece_keys[EMPTY][from64]);

			//Add rook
			board->board_array120[to120] = bR; //Add to C8
			board->board_array64[to64] = bR;
			Add_To_Piecelists(bR, to120, board);
			HASH_OUT(board->hash_key, piece_keys[capture][to64]);
			HASH_IN(board->hash_key, piece_keys[bR][to64]);

			//Remove captured piece from lists
			Remove_From_Piecelists(capture, to120, board);

			//Bitboards
			CLR_BIT(board->pawn_bitboards[BLACK], from64);
			CLR_BIT(board->pawn_bitboards[BOTH], from64);
		}
	}
	else if (IS_BISHOP_PROMOTION(move_num))
	{
		if (side == WHITE)
		{
			ASSERT(GET_RANK_64(from64) == RANK_7);
			ASSERT(GET_RANK_64(to64) == RANK_8);
			ASSERT(piece == wP);

			//Remove pawn
			board->board_array120[from120] = EMPTY;
			board->board_array64[from64] = EMPTY;
			Remove_From_Piecelists(wP, from120, board);
			HASH_OUT(board->hash_key, piece_keys[wP][from64]);
			HASH_IN(board->hash_key, piece_keys[EMPTY][from64]);

			//Add bishop
			board->board_array120[to120] = wB; //Add to C8
			board->board_array64[to64] = wB;
			Add_To_Piecelists(wB, to120, board);
			HASH_OUT(board->hash_key, piece_keys[capture][to64]);
			HASH_IN(board->hash_key, piece_keys[wB][to64]);

			//Remove captured piece from lists
			Remove_From_Piecelists(capture, to120, board);

			//Bitboards
			CLR_BIT(board->pawn_bitboards[WHITE], from64);
			CLR_BIT(board->pawn_bitboards[BOTH], from64);
		}
		else
		{
			ASSERT(GET_RANK_64(from64) == RANK_2);
			ASSERT(GET_RANK_64(to64) == RANK_1);
			ASSERT(piece == bP);

			//Remove pawn
			board->board_array120[from120] = EMPTY;
			board->board_array64[from64] = EMPTY;
			Remove_From_Piecelists(bP, from120, board);
			HASH_OUT(board->hash_key, piece_keys[bP][from64]);
			HASH_IN(board->hash_key, piece_keys[EMPTY][from64]);

			//Add bishop
			board->board_array120[to120] = bB; //Add to C8
			board->board_array64[to64] = bB;
			Add_To_Piecelists(bB, to120, board);
			HASH_OUT(board->hash_key, piece_keys[capture][to64]);
			HASH_IN(board->hash_key, piece_keys[bB][to64]);

			//Remove captured piece from lists
			Remove_From_Piecelists(capture, to120, board);

			//Bitboards
			CLR_BIT(board->pawn_bitboards[BLACK], from64);
			CLR_BIT(board->pawn_bitboards[BOTH], from64);
		}
	}
	else if (IS_KNIGHT_PROMOTION(move_num))
	{
		if (side == WHITE)
		{
			ASSERT(GET_RANK_64(from64) == RANK_7);
			ASSERT(GET_RANK_64(to64) == RANK_8);
			ASSERT(piece == wP);

			//Remove pawn
			board->board_array120[from120] = EMPTY;
			board->board_array64[from64] = EMPTY;
			Remove_From_Piecelists(wP, from120, board);
			HASH_OUT(board->hash_key, piece_keys[wP][from64]);
			HASH_IN(board->hash_key, piece_keys[EMPTY][from64]);

			//Add knight
			board->board_array120[to120] = wN; //Add to C8
			board->board_array64[to64] = wN;
			Add_To_Piecelists(wN, to120, board);
			HASH_OUT(board->hash_key, piece_keys[capture][to64]);
			HASH_IN(board->hash_key, piece_keys[wN][to64]);

			//Remove captured piece from lists
			Remove_From_Piecelists(capture, to120, board);

			//Bitboards
			CLR_BIT(board->pawn_bitboards[WHITE], from64);
			CLR_BIT(board->pawn_bitboards[BOTH], from64);
		}
		else
		{
			ASSERT(GET_RANK_64(from64) == RANK_2);
			ASSERT(GET_RANK_64(to64) == RANK_1);
			ASSERT(piece == bP);

			//Remove pawn
			board->board_array120[from120] = EMPTY;
			board->board_array64[from64] = EMPTY;
			Remove_From_Piecelists(bP, from120, board);
			HASH_OUT(board->hash_key, piece_keys[bP][from64]);
			HASH_IN(board->hash_key, piece_keys[EMPTY][from64]);

			//Add knight
			board->board_array120[to120] = bN; //Add to C8
			board->board_array64[to64] = bN;
			Add_To_Piecelists(bN, to120, board);
			HASH_OUT(board->hash_key, piece_keys[capture][to64]);
			HASH_IN(board->hash_key, piece_keys[bN][to64]);

			//Remove captured piece from lists
			Remove_From_Piecelists(capture, to120, board);

			//Bitboards
			CLR_BIT(board->pawn_bitboards[BLACK], from64);
			CLR_BIT(board->pawn_bitboards[BOTH], from64);
		}
	}
	else
	{
		ASSERT(1 == 0); //This should never be called
	}

	/***** Updates regardless of move type *****/

	//side (Incremented at end of function)
	HASH_OUT(board->hash_key,side_keys[board->side]);
	board->side ^= 1; //Toggle
	HASH_IN(board->hash_key, side_keys[board->side]);

	//hply; //total moves taken so far
	board->hply++;

	//move_counter; //100 move counter
	if ((piece == wP) || (piece == bP) || (capture != EMPTY))
	{
		board->move_counter = 0; //if pawn move or capture
	}
	else
	{
		board->move_counter++;
	}

	//castle_rights;
	//Check if rights are available, then test conditions to prevent hashing twice
	if (board->castle_rights & WK_CASTLE )
	{
		if (piece == wK || board->board_array120[H1] != wR)
		{
			castle_temp = board->castle_rights;
			HASH_OUT(board->hash_key, castle_keys[castle_temp]);
			board->castle_rights &= ~WK_CASTLE;
			HASH_IN(board->hash_key, castle_keys[board->castle_rights]);
		}
	}
	if (board->castle_rights & WQ_CASTLE)
	{
		if (piece == wK || board->board_array120[A1] != wR)
		{
			castle_temp = board->castle_rights;
			HASH_OUT(board->hash_key, castle_keys[castle_temp]);
			board->castle_rights &= ~WQ_CASTLE;
			HASH_IN(board->hash_key, castle_keys[board->castle_rights]);
		}
	}
	if (board->castle_rights & BK_CASTLE)
	{
		if (piece == bK || board->board_array120[H8] != bR)
		{
			castle_temp = board->castle_rights;
			HASH_OUT(board->hash_key, castle_keys[castle_temp]);
			board->castle_rights &= ~BK_CASTLE;
			HASH_IN(board->hash_key, castle_keys[board->castle_rights]);
		}
	}
	if (board->castle_rights & BQ_CASTLE)
	{
		if (piece == bK || board->board_array120[A8] != bR)
		{
			castle_temp = board->castle_rights;
			HASH_OUT(board->hash_key, castle_keys[castle_temp]);
			board->castle_rights &= ~BQ_CASTLE;
			HASH_IN(board->hash_key, castle_keys[board->castle_rights]);
		}
	}
	/***** Check test *****/
	//If king under attack
	//take move

	/***** Evaluate *****/
	Evaluate_Board(board);

#ifdef DEBUG
	Check_Board(board);
#endif

}

//Moves one piece, updating all lists, bitboards, and hashes
void Move_Piece(int from120, int to120, BOARD_STRUCT *board)
{
	int from64, to64, piece, capture;

	ASSERT(board->board_array120[from120] != EMPTY);
	ASSERT(ON_BOARD_120(from120));
	ASSERT(ON_BOARD_120(to120));

	from64 = SQUARE_120_TO_64(from120);
	to64 = SQUARE_120_TO_64(to120);
	piece = board->board_array120[from120];
	capture = board->board_array120[to120];

	//Remove captured piece from piecelists, remove before updating arrays
	if (capture != EMPTY) Capture_Piece(to120, board);

	//Clear from square
	board->board_array120[from120] = EMPTY; 
	board->board_array64[from64] = EMPTY;
	Remove_From_Piecelists(piece, from120, board);
	HASH_OUT(board->hash_key, piece_keys[piece][from64]);
	HASH_IN(board->hash_key, piece_keys[EMPTY][from64]);

	//Add to new square
	board->board_array120[to120] = piece; 
	board->board_array64[to64] = piece;
	Add_To_Piecelists(piece, to120, board);
	HASH_OUT(board->hash_key, piece_keys[EMPTY][to64]); //Capture already removed earlier
	HASH_IN(board->hash_key, piece_keys[piece][to64]);

	//Update pawn bitboards
	if (piece == wP)
	{
		CLR_BIT(board->pawn_bitboards[WHITE], from64);
		CLR_BIT(board->pawn_bitboards[BOTH], from64);
		SET_BIT(board->pawn_bitboards[WHITE], to64);
		SET_BIT(board->pawn_bitboards[BOTH], to64);
	}
	else if (piece == bP)
	{
		CLR_BIT(board->pawn_bitboards[BLACK], from64);
		CLR_BIT(board->pawn_bitboards[BOTH], from64);
		SET_BIT(board->pawn_bitboards[BLACK], to64);
		SET_BIT(board->pawn_bitboards[BOTH], to64);
	}
}

//Removes a piece from the board, updating all lists, bitboards, and hashes
void Capture_Piece(int index120, BOARD_STRUCT *board)
{
	int index64, piece;

	ASSERT(board->board_array120[index120] != EMPTY);
	ASSERT(ON_BOARD_120(index120));

	index64 = SQUARE_120_TO_64(index120);
	piece = board->board_array120[index120];

	//Clear from square
	board->board_array120[index120] = EMPTY;
	board->board_array64[index64] = EMPTY;
	Remove_From_Piecelists(piece, index120, board);
	HASH_OUT(board->hash_key, piece_keys[piece][index64]);
	HASH_IN(board->hash_key, piece_keys[EMPTY][index64]);

	//Update pawn bitboards
	if (piece == wP)
	{
		CLR_BIT(board->pawn_bitboards[WHITE], index64);
		CLR_BIT(board->pawn_bitboards[BOTH], index64);
	}
	else if (piece == bP)
	{
		CLR_BIT(board->pawn_bitboards[BLACK], index64);
		CLR_BIT(board->pawn_bitboards[BOTH], index64);
	}
}

//Creates integer from move data and stores in movelist
void Add_Move(MOVE_LIST_STRUCT *movelist, int from, int to, int piece, int capture, int special, int score)
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
	movelist->list[movelist->num].move = temp; 
	movelist->list[movelist->num].score = score;
	movelist->num++; //Increment counter
}

//Prints all moves in standard chess format
void Print_Movelist(MOVE_LIST_STRUCT *movelist)
{
	int index, rank, file, piece, square, from;
	int move_num, move_score; //Integer for move

	cout << endl << "Movelist: " << endl;
	cout << "Moves found: " << movelist->num << endl;

	for (index = 0; index < movelist->num; index++) //Iterate through all moves stored
	{
		cout << index << " ";
		move_num = movelist->list[index].move; //Store move in temp variable
		move_score = movelist->list[index].score;

		//Get moving piece
		piece = GET_PIECE(move_num);

		//Get rank and file of ending position
		from = GET_FROM_SQ(move_num);
		square = GET_TO_SQ(move_num);
		ASSERT(ON_BOARD_120(square));
		rank = GET_RANK_120(square);
		file = GET_FILE_120(square);

		/***** Print *****/
		//Castle
		if (IS_KING_CASTLE(move_num))
		{
			cout << "O-O";
		}
		else if (IS_QUEEN_CASTLE(move_num))
		{
			cout << "O-O-O";
		}
		else
		{
			if ((piece != wP) && (piece != bP)) //If piece is not a pawn
			{
				cout << piece_names[piece]; //Piece
			}
			else if (GET_CAPTURE(move_num) != 0) //If piece is a pawn, and a capture occured
			{
				cout << file_names[GET_FILE_120(from)];
			}

			if (GET_CAPTURE(move_num) != 0) cout << "x"; //If Capture occured

			cout << file_names[file]; //File and rank
			cout << rank_names[rank];

			if (IS_PROMOTION(move_num)) //Promoted piece
			{
				if (IS_QUEEN_PROMOTION(move_num)) cout << "Q";
				if (IS_ROOK_PROMOTION(move_num)) cout << "R";
				if (IS_BISHOP_PROMOTION(move_num)) cout << "B";
				if (IS_KNIGHT_PROMOTION(move_num)) cout << "N";
			}
		}
		cout << " Score: " << move_score << endl;
	}
}