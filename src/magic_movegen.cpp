/* magic_movegen
* Generates moves using magic bitboards
* Theo Kanning 1/30/15
*/

#include "globals.h"

U64 Rook_Attacks(U64 occ, int sq)
{
	occ &= R_Occ[sq]; //Remove pieces not along critical lines
	occ *= R_Magic[sq]; //Multiply by magic number
	occ >>= 64 - R_Bits[sq];
	return magicMovesRook[sq][occ];
}

U64 Bishop_Attacks(U64 occ, int sq)
{
	occ &= B_Occ[sq]; //Remove pieces not along critical lines
	occ *= B_Magic[sq]; //Multiply by magic number
	occ >>= 64 - B_Bits[sq];
	return magicMovesBishop[sq][occ];
}

//Add all moves for one slider piece
void Add_Slider_Piece_Moves(int piece, int from, U64 occ, MOVE_LIST_STRUCT *move_list, BOARD_STRUCT *board)
{

}

//Generates all moves using magic bitboards
void Magic_Generate_Moves(BOARD_STRUCT *board, MOVE_LIST_STRUCT *move_list)
{
	int i, capture, from, to;
	const int side = board->side; //Store side to move
	U64 attacks;
	U64 occ = board->side_bitboards[BOTH];

	/***** Clear move list *****/
	for (i = 0; i < move_list->num; i++)
	{
		move_list->list[i].move = 0;
		move_list->list[i].score = 0;
	}

	move_list->num = 0;
	/***** Move list now empty *****/


	ASSERT((side == WHITE) || (side == BLACK));

	if (side == WHITE)
	{
		/***************************************/
		/**************** PAWNS ****************/
		/***************************************/

		/***** Single Pushes *****/
		U64 single_pushes = board->piece_bitboards[wP] << 8; //Shift pawns forward
		single_pushes &= ~occ; //Remove blockers
		U64 double_pushes = (single_pushes & rank_masks[RANK_3]) << 8; //Shift pushes on rank 3
		double_pushes &= ~occ; //Remove blockers

		//Add single push moves
		while (single_pushes)
		{
			to = pop_1st_bit(&single_pushes);
			if (GET_RANK_64(to) == RANK_8)//Promotions
			{
				Add_Move(move_list, SQUARE_64_TO_120(to - 8), SQUARE_64_TO_120(to), wP, 0, QUEEN_PROMOTE, QUEEN_PROMOTE_SCORE, board);
				Add_Move(move_list, SQUARE_64_TO_120(to - 8), SQUARE_64_TO_120(to), wP, 0, ROOK_PROMOTE, UNDER_PROMOTE_SCORE, board);
				Add_Move(move_list, SQUARE_64_TO_120(to - 8), SQUARE_64_TO_120(to), wP, 0, BISHOP_PROMOTE, UNDER_PROMOTE_SCORE, board);
				Add_Move(move_list, SQUARE_64_TO_120(to - 8), SQUARE_64_TO_120(to), wP, 0, KNIGHT_PROMOTE, UNDER_PROMOTE_SCORE, board);
			}
			else //Non-promotions
			{
				Add_Move(move_list, SQUARE_64_TO_120(to - 8), SQUARE_64_TO_120(to), wP, 0, NOT_SPECIAL, 0, board);
			}
		}
		
		/***** Double Pushes *****/
		while (double_pushes)
		{
			to = pop_1st_bit(&double_pushes);
			Add_Move(move_list, SQUARE_64_TO_120(to - 16), SQUARE_64_TO_120(to), wP, 0, NOT_SPECIAL, 0, board);
		}

		/***** Left Captures *****/
		U64 left_captures = (board->piece_bitboards[wP] << 7) & ~file_masks[FILE_H]; //Shift up and left
		U64 left_ep_attacks = left_captures & (1i64 << SQUARE_120_TO_64(board->ep)); //Check if ep square is hit
		left_captures &= board->side_bitboards[BLACK]; //Find valid captures

		while (left_captures)
		{
			to = pop_1st_bit(&left_captures);
			capture = board->board_array64[to];
			ASSERT(capture != EMPTY);
			if (GET_RANK_64(to) == RANK_8) //Promotions
			{
				Add_Move(move_list, SQUARE_64_TO_120(to - 7), SQUARE_64_TO_120(to), wP, capture, QUEEN_PROMOTE, QUEEN_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, wP), board);
				Add_Move(move_list, SQUARE_64_TO_120(to - 7), SQUARE_64_TO_120(to), wP, capture, ROOK_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, wP), board);
				Add_Move(move_list, SQUARE_64_TO_120(to - 7), SQUARE_64_TO_120(to), wP, capture, BISHOP_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, wP), board);
				Add_Move(move_list, SQUARE_64_TO_120(to - 7), SQUARE_64_TO_120(to), wP, capture, KNIGHT_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, wP), board);

			}
			else //Non-promotions
			{
				Add_Move(move_list, SQUARE_64_TO_120(to - 7), SQUARE_64_TO_120(to), wP, capture, NOT_SPECIAL, GET_MMVLVA_SCORE(capture, wP), board);
			}
		}

		/***** Right Captures *****/
		U64 right_captures = (board->piece_bitboards[wP] << 9) & ~file_masks[FILE_A]; //Shift up and left
		U64 right_ep_attacks = right_captures & (1i64 << SQUARE_120_TO_64(board->ep)); //Check if ep square is hit
		right_captures &= board->side_bitboards[BLACK]; //Find valid captures
		

		while (right_captures)
		{
			to = pop_1st_bit(&right_captures);
			capture = board->board_array64[to];
			ASSERT(capture != EMPTY);
			if (GET_RANK_64(to) == RANK_8) //Promotions
			{
				Add_Move(move_list, SQUARE_64_TO_120(to - 9), SQUARE_64_TO_120(to), wP, capture, QUEEN_PROMOTE, QUEEN_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, wP), board);
				Add_Move(move_list, SQUARE_64_TO_120(to - 9), SQUARE_64_TO_120(to), wP, capture, ROOK_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, wP), board);
				Add_Move(move_list, SQUARE_64_TO_120(to - 9), SQUARE_64_TO_120(to), wP, capture, BISHOP_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, wP), board);
				Add_Move(move_list, SQUARE_64_TO_120(to - 9), SQUARE_64_TO_120(to), wP, capture, KNIGHT_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, wP), board);

			}
			else //Non-promotions
			{
				Add_Move(move_list, SQUARE_64_TO_120(to - 9), SQUARE_64_TO_120(to), wP, capture, NOT_SPECIAL, GET_MMVLVA_SCORE(capture, wP), board);
			}
		}

		/***** Ep Captures *****/
		if (left_ep_attacks)
		{
			to = pop_1st_bit(&left_ep_attacks);
			ASSERT(GET_RANK_64(to) == RANK_3)
			Add_Move(move_list, SQUARE_64_TO_120(to - 7), SQUARE_64_TO_120(to), wP, bP, EP_CAPTURE, GET_MMVLVA_SCORE(bP, wP), board);
		}
		if (right_ep_attacks)
		{
			to = pop_1st_bit(&right_ep_attacks);
			ASSERT(GET_RANK_64(to) == RANK_3)
				Add_Move(move_list, SQUARE_64_TO_120(to - 9), SQUARE_64_TO_120(to), wP, bP, EP_CAPTURE, GET_MMVLVA_SCORE(bP, wP), board);
		}


		////Loop through each pawn and test for every kind of move, separate loops for white and black
		//piece = wP;
		//for (i = 0; i < board->piece_num[wP]; i++)
		//{
		//	//Get pawn index120
		//	from120 = board->piece_list120[wP][i];
		//	ASSERT(ON_BOARD_120(from120));

		//	/***** Pushes and Promotions *****/
		//	to120 = from120 + 10;
		//	ASSERT(ON_BOARD_120(to120));

		//	if (board->board_array120[to120] == EMPTY) //If next space is empty, this also means it is on the board
		//	{
		//		/***** Single Pushes and Promotions *****/
		//		//Add as a promotion if this leads to rank 8
		//		if (GET_RANK_120(to120) == RANK_8)
		//		{
		//			Add_Move(move_list, from120, to120, wP, 0, QUEEN_PROMOTE, QUEEN_PROMOTE_SCORE, board);
		//			Add_Move(move_list, from120, to120, wP, 0, ROOK_PROMOTE, UNDER_PROMOTE_SCORE, board);
		//			Add_Move(move_list, from120, to120, wP, 0, BISHOP_PROMOTE, UNDER_PROMOTE_SCORE, board);
		//			Add_Move(move_list, from120, to120, wP, 0, KNIGHT_PROMOTE, UNDER_PROMOTE_SCORE, board);
		//		}
		//		else //If not a promotion
		//		{
		//			Add_Move(move_list, from120, to120, wP, 0, NOT_SPECIAL, 0, board);
		//		}

		//		/***** Double Pushes *****/
		//		to120 = from120 + 20;

		//		//Same as before, first space in front already established as empty
		//		if ((GET_RANK_120(from120) == RANK_2) && (board->board_array120[to120] == EMPTY)) //If pawn is on starting rank, and second square is also empty
		//		{
		//			Add_Move(move_list, from120, to120, wP, 0, NOT_SPECIAL, 0, board);
		//		}
		//	}

		//	/***** Normal and EP Captures *****/
		//	//Check first target square
		//	to120 = from120 + 9;
		//	if (ON_BOARD_120(to120)) //If target square is on board
		//	{
		//		if (IS_BLACK_PIECE(board->board_array120[to120])) //If target has a piece that can be taken
		//		{
		//			capture = board->board_array120[to120];
		//			if (GET_RANK_120(to120) == RANK_8) //If capturing into promotion
		//			{
		//				Add_Move(move_list, from120, to120, wP, capture, QUEEN_PROMOTE, QUEEN_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, wP), board);
		//				Add_Move(move_list, from120, to120, wP, capture, ROOK_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, wP), board);
		//				Add_Move(move_list, from120, to120, wP, capture, BISHOP_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, wP), board);
		//				Add_Move(move_list, from120, to120, wP, capture, KNIGHT_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, wP), board);
		//			}
		//			else
		//			{
		//				Add_Move(move_list, from120, to120, wP, capture, NOT_SPECIAL, GET_MMVLVA_SCORE(capture, wP), board);
		//			}
		//		}
		//		else if (to120 == board->ep) //If target is ep square
		//		{
		//			capture = bP;
		//			Add_Move(move_list, from120, to120, wP, capture, EP_CAPTURE, GET_MMVLVA_SCORE(capture, wP), board);
		//		}
		//	}

		//	//Check second target square
		//	to120 = from120 + 11;
		//	if (ON_BOARD_120(to120)) //If target square is on board
		//	{
		//		if (IS_BLACK_PIECE(board->board_array120[to120])) //If target has a piece that can be taken
		//		{
		//			capture = board->board_array120[to120];
		//			if (GET_RANK_120(to120) == RANK_8) //If capturing into promotion
		//			{
		//				Add_Move(move_list, from120, to120, wP, capture, QUEEN_PROMOTE, QUEEN_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, wP), board);
		//				Add_Move(move_list, from120, to120, wP, capture, ROOK_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, wP), board);
		//				Add_Move(move_list, from120, to120, wP, capture, BISHOP_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, wP), board);
		//				Add_Move(move_list, from120, to120, wP, capture, KNIGHT_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, wP), board);
		//			}
		//			else
		//			{
		//				Add_Move(move_list, from120, to120, wP, capture, NOT_SPECIAL, GET_MMVLVA_SCORE(capture, wP), board);
		//			}
		//		}
		//		else if (to120 == board->ep) //If target is ep square
		//		{
		//			capture = bP;
		//			Add_Move(move_list, from120, to120, wP, capture, EP_CAPTURE, GET_MMVLVA_SCORE(capture, wP), board);
		//		}
		//	}
		//}

		/***************************************/
		/*************** KNIGHTS ***************/
		/***************************************/
		U64 knights_temp = board->piece_bitboards[wN];

		//Loop until no knights remain
		while (knights_temp)
		{
			from = pop_1st_bit(&knights_temp);
			//Calculate attack spaces and remove friendly targets
			attacks = knight_attack_masks[from] & ~board->side_bitboards[WHITE];
			while (attacks)
			{
				to = pop_1st_bit(&attacks);
				//If capture
				if (GET_BIT(board->side_bitboards[BLACK], to)) //If to square is occupied
				{
					Add_Move(move_list, SQUARE_64_TO_120(from), SQUARE_64_TO_120(to), wN, board->board_array64[to], NOT_SPECIAL, GET_MMVLVA_SCORE(board->board_array64[to], wN), board);
				}
				else //Quiet move
				{
					Add_Move(move_list, SQUARE_64_TO_120(from), SQUARE_64_TO_120(to), wN, EMPTY, NOT_SPECIAL, 0, board);
				}
			}
		}
		
		/***************************************/
		/*************** BISHOPS ***************/
		/***************************************/
		U64 bishops_temp = board->piece_bitboards[wB];

		//Loop until no rooks remain
		while (bishops_temp)
		{
			from = pop_1st_bit(&bishops_temp);
			//Calculate attack spaces and remove friendly targets
			attacks = Bishop_Attacks(occ, from) & ~board->side_bitboards[WHITE];
			while (attacks)
			{
				to = pop_1st_bit(&attacks);
				//If capture
				if (GET_BIT(board->side_bitboards[BLACK], to)) //If to square is occupied
				{
					Add_Move(move_list, SQUARE_64_TO_120(from), SQUARE_64_TO_120(to), wB, board->board_array64[to], NOT_SPECIAL, GET_MMVLVA_SCORE(board->board_array64[to], wB), board);
				}
				else //Quiet move
				{
					Add_Move(move_list, SQUARE_64_TO_120(from), SQUARE_64_TO_120(to), wB, EMPTY, NOT_SPECIAL, 0, board);
				}
			}
		}

		/***************************************/
		/**************** ROOKS ****************/
		/***************************************/
		U64 rooks_temp = board->piece_bitboards[wR];

		//Loop until no rooks remain
		while (rooks_temp)
		{
			from = pop_1st_bit(&rooks_temp);
			//Calculate attack spaces and remove friendly targets
			attacks = Rook_Attacks(occ, from);
			attacks &= ~board->side_bitboards[WHITE];
			
			while (attacks)
			{
				to = pop_1st_bit(&attacks);
				//If capture
				if (GET_BIT(board->side_bitboards[BLACK], to)) //If to square is occupied
				{
					Add_Move(move_list, SQUARE_64_TO_120(from), SQUARE_64_TO_120(to), wR, board->board_array64[to], NOT_SPECIAL, GET_MMVLVA_SCORE(board->board_array64[to], wR), board);
				}
				else //Quiet move
				{
					Add_Move(move_list, SQUARE_64_TO_120(from), SQUARE_64_TO_120(to), wR, EMPTY, NOT_SPECIAL, 0, board);
				}
			}
		}

		/***************************************/
		/*************** QUEENS ****************/
		/***************************************/
		
		U64 queens_temp = board->piece_bitboards[wQ];

		//Loop until no rooks remain
		while (queens_temp)
		{
			from = pop_1st_bit(&queens_temp);
			//Calculate attack spaces and remove friendly targets
			attacks = Rook_Attacks(occ, from) | Bishop_Attacks(occ, from);
			attacks &= ~board->side_bitboards[WHITE];

			while (attacks)
			{
				to = pop_1st_bit(&attacks);
				//If capture
				if (GET_BIT(board->side_bitboards[BLACK], to)) //If to square is occupied
				{
					Add_Move(move_list, SQUARE_64_TO_120(from), SQUARE_64_TO_120(to), wQ, board->board_array64[to], NOT_SPECIAL, GET_MMVLVA_SCORE(board->board_array64[to], wQ), board);
				}
				else //Quiet move
				{
					Add_Move(move_list, SQUARE_64_TO_120(from), SQUARE_64_TO_120(to), wQ, EMPTY, NOT_SPECIAL, 0, board);
				}
			}
		}
		

		/***************************************/
		/*************** King *****************/
		/***************************************/

		U64 king_temp = board->piece_bitboards[wK];

		from = pop_1st_bit(&king_temp);
		//Calculate attack spaces and remove friendly targets
		attacks = king_attack_masks[from] & ~board->side_bitboards[WHITE];
		while (attacks)
		{
			to = pop_1st_bit(&attacks);
			//If capture
			if (GET_BIT(board->side_bitboards[BLACK], to)) //If to square is occupied
			{
				Add_Move(move_list, SQUARE_64_TO_120(from), SQUARE_64_TO_120(to), wK, board->board_array64[to], NOT_SPECIAL, GET_MMVLVA_SCORE(board->board_array64[to], wK), board);
			}
			else //Quiet move
			{
				Add_Move(move_list, SQUARE_64_TO_120(from), SQUARE_64_TO_120(to), wK, EMPTY, NOT_SPECIAL, 0, board);
			}
		}
		/***** Castling *****/
		//Kingside
		if ((board->castle_rights & WK_CASTLE))
		{
			//Castling rights active
			if ((board->board_array120[F1] == EMPTY) && (board->board_array120[G1] == EMPTY) //Travel squares are empty
				&& !Under_Attack(E1, BLACK, board) && !Under_Attack(F1, BLACK, board) && !Under_Attack(G1, BLACK, board)) //King does not move through check
			{
				Add_Move(move_list, E1, G1, wK, 0, KING_CASTLE, 0, board); //Always moves to G1	
			}
		}
		//Queenside
		if ((board->castle_rights & WQ_CASTLE))
		{
			//Castling rights active
			if ((board->board_array120[B1] == EMPTY) && (board->board_array120[C1] == EMPTY) && (board->board_array120[D1] == EMPTY) //Travel squares are empty
				&& !Under_Attack(C1, BLACK, board) && !Under_Attack(D1, BLACK, board) && !Under_Attack(E1, BLACK, board)) //King does not move through check
			{
				Add_Move(move_list, E1, C1, wK, 0, QUEEN_CASTLE, 0, board); //Always moves to C1	
			}
		}
		/***** End King Moves *****/

	}
	else //side == BLACK
	{
		/***************************************/
		/**************** PAWNS ****************/
		/***************************************/
		/***** Single Pushes *****/
		U64 single_pushes = board->piece_bitboards[bP] >> 8; //Shift pawns forward
		single_pushes &= ~occ; //Remove blockers
		U64 double_pushes = (single_pushes & rank_masks[RANK_6]) >> 8; //Shift pushes on rank 6
		double_pushes &= ~occ; //Remove blockers

		//Add single push moves
		while (single_pushes)
		{
			to = pop_1st_bit(&single_pushes);
			if (GET_RANK_64(to) == RANK_1)//Promotions
			{
				Add_Move(move_list, SQUARE_64_TO_120(to + 8), SQUARE_64_TO_120(to), wP, 0, QUEEN_PROMOTE, QUEEN_PROMOTE_SCORE, board);
				Add_Move(move_list, SQUARE_64_TO_120(to + 8), SQUARE_64_TO_120(to), wP, 0, ROOK_PROMOTE, UNDER_PROMOTE_SCORE, board);
				Add_Move(move_list, SQUARE_64_TO_120(to + 8), SQUARE_64_TO_120(to), wP, 0, BISHOP_PROMOTE, UNDER_PROMOTE_SCORE, board);
				Add_Move(move_list, SQUARE_64_TO_120(to + 8), SQUARE_64_TO_120(to), wP, 0, KNIGHT_PROMOTE, UNDER_PROMOTE_SCORE, board);
			}
			else //Non-promotions
			{
				Add_Move(move_list, SQUARE_64_TO_120(to + 8), SQUARE_64_TO_120(to), bP, 0, NOT_SPECIAL, 0, board);
			}
		}

		/***** Double Pushes *****/
		while (double_pushes)
		{
			to = pop_1st_bit(&double_pushes);
			Add_Move(move_list, SQUARE_64_TO_120(to + 16), SQUARE_64_TO_120(to), bP, 0, NOT_SPECIAL, 0, board);
		}

		/***** Left Captures *****/
		U64 left_captures = (board->piece_bitboards[bP] >> 9) & ~file_masks[FILE_H]; //Shift down and left
		left_captures &= board->side_bitboards[WHITE]; //Find valid captures
		U64 left_ep_attacks = left_captures & (1i64 << SQUARE_120_TO_64(board->ep)); //Check if ep square is hit

		while (left_captures)
		{
			to = pop_1st_bit(&left_captures);
			capture = board->board_array64[to];
			ASSERT(capture != EMPTY);
			if (GET_RANK_64(to) == RANK_1) //Promotions
			{
				Add_Move(move_list, SQUARE_64_TO_120(to + 9), SQUARE_64_TO_120(to), bP, capture, QUEEN_PROMOTE, QUEEN_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, bP), board);
				Add_Move(move_list, SQUARE_64_TO_120(to + 9), SQUARE_64_TO_120(to), bP, capture, ROOK_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, bP), board);
				Add_Move(move_list, SQUARE_64_TO_120(to + 9), SQUARE_64_TO_120(to), bP, capture, BISHOP_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, bP), board);
				Add_Move(move_list, SQUARE_64_TO_120(to + 9), SQUARE_64_TO_120(to), bP, capture, KNIGHT_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, bP), board);

			}
			else //Non-promotions
			{
				Add_Move(move_list, SQUARE_64_TO_120(to + 9), SQUARE_64_TO_120(to), bP, capture, NOT_SPECIAL, GET_MMVLVA_SCORE(capture, bP), board);
			}
		}

		/***** Right Captures *****/
		U64 right_captures = (board->piece_bitboards[bP] >> 7) & ~file_masks[FILE_A]; //Shift down and right
		right_captures &= board->side_bitboards[WHITE]; //Find valid captures
		U64 right_ep_attacks = right_captures & (1i64 << SQUARE_120_TO_64(board->ep)); //Check if ep square is hit

		while (right_captures)
		{
			to = pop_1st_bit(&right_captures);
			capture = board->board_array64[to];
			ASSERT(capture != EMPTY);
			if (GET_RANK_64(to) == RANK_1) //Promotions
			{
				Add_Move(move_list, SQUARE_64_TO_120(to + 7), SQUARE_64_TO_120(to), bP, capture, QUEEN_PROMOTE, QUEEN_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, bP), board);
				Add_Move(move_list, SQUARE_64_TO_120(to + 7), SQUARE_64_TO_120(to), bP, capture, ROOK_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, bP), board);
				Add_Move(move_list, SQUARE_64_TO_120(to + 7), SQUARE_64_TO_120(to), bP, capture, BISHOP_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, bP), board);
				Add_Move(move_list, SQUARE_64_TO_120(to + 7), SQUARE_64_TO_120(to), bP, capture, KNIGHT_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, bP), board);

			}
			else //Non-promotions
			{
				Add_Move(move_list, SQUARE_64_TO_120(to + 7), SQUARE_64_TO_120(to), bP, capture, NOT_SPECIAL, GET_MMVLVA_SCORE(capture, bP), board);
			}
		}

		/***** Ep Captures *****/
		if (left_ep_attacks)
		{
			to = pop_1st_bit(&left_ep_attacks);
			ASSERT(GET_RANK_64(to) == RANK_6)
				Add_Move(move_list, SQUARE_64_TO_120(to + 9), SQUARE_64_TO_120(to), bP, wP, EP_CAPTURE, GET_MMVLVA_SCORE(wP, bP), board);
		}
		if (right_ep_attacks)
		{
			to = pop_1st_bit(&right_ep_attacks);
			ASSERT(GET_RANK_64(to) == RANK_6)
				Add_Move(move_list, SQUARE_64_TO_120(to + 7), SQUARE_64_TO_120(to), bP, wP, EP_CAPTURE, GET_MMVLVA_SCORE(wP, bP), board);
		}

		/***************************************/
		/*************** KNIGHTS ***************/
		/***************************************/

		U64 knights_temp = board->piece_bitboards[bN];

		//Loop until no rooks remain
		while (knights_temp)
		{
			from = pop_1st_bit(&knights_temp);
			//Calculate attack spaces and remove friendly targets
			attacks = knight_attack_masks[from] & ~board->side_bitboards[BLACK];
			while (attacks)
			{
				to = pop_1st_bit(&attacks);
				//If capture
				if (GET_BIT(board->side_bitboards[WHITE], to)) //If to square is occupied
				{
					Add_Move(move_list, SQUARE_64_TO_120(from), SQUARE_64_TO_120(to), bN, board->board_array64[to], NOT_SPECIAL, GET_MMVLVA_SCORE(board->board_array64[to], bN), board);
				}
				else //Quiet move
				{
					Add_Move(move_list, SQUARE_64_TO_120(from), SQUARE_64_TO_120(to), bN, EMPTY, NOT_SPECIAL, 0, board);
				}
			}
		}

		/***************************************/
		/*************** BISHOPS ***************/
		/***************************************/
		
		U64 bishops_temp = board->piece_bitboards[bB];

		//Loop until no rooks remain
		while (bishops_temp)
		{
			from = pop_1st_bit(&bishops_temp);
			//Calculate attack spaces and remove friendly targets
			attacks = Bishop_Attacks(occ, from) & ~board->side_bitboards[BLACK];
			while (attacks)
			{
				to = pop_1st_bit(&attacks);
				//If capture
				if (GET_BIT(board->side_bitboards[WHITE], to)) //If to square is occupied
				{
					Add_Move(move_list, SQUARE_64_TO_120(from), SQUARE_64_TO_120(to), bB, board->board_array64[to], NOT_SPECIAL, GET_MMVLVA_SCORE(board->board_array64[to], bB), board);
				}
				else //Quiet move
				{
					Add_Move(move_list, SQUARE_64_TO_120(from), SQUARE_64_TO_120(to), bB, EMPTY, NOT_SPECIAL, 0, board);
				}
			}
		}

		/***************************************/
		/**************** ROOKS ****************/
		/***************************************/
		U64 rooks_temp = board->piece_bitboards[bR];

		//Loop until no rooks remain
		while (rooks_temp)
		{
			from = pop_1st_bit(&rooks_temp);
			//Calculate attack spaces and remove friendly targets
			attacks = Rook_Attacks(occ, from) & ~board->side_bitboards[BLACK];
			while (attacks)
			{
				to = pop_1st_bit(&attacks);
				//If capture
				if (GET_BIT(board->side_bitboards[WHITE], to)) //If to square is occupied
				{
					Add_Move(move_list, SQUARE_64_TO_120(from), SQUARE_64_TO_120(to), bR, board->board_array64[to], NOT_SPECIAL, GET_MMVLVA_SCORE(board->board_array64[to], bR), board);
				}
				else //Quiet move
				{
					Add_Move(move_list, SQUARE_64_TO_120(from), SQUARE_64_TO_120(to), bR, EMPTY, NOT_SPECIAL, 0, board);
				}
			}
		}
		

		/***************************************/
		/*************** QUEENS ****************/
		/***************************************/

		U64 queens_temp = board->piece_bitboards[bQ];

		//Loop until no rooks remain
		while (queens_temp)
		{
			from = pop_1st_bit(&queens_temp);
			//Calculate attack spaces and remove friendly targets
			attacks = Rook_Attacks(occ, from) | Bishop_Attacks(occ, from);
			attacks &= ~board->side_bitboards[BLACK];

			while (attacks)
			{
				to = pop_1st_bit(&attacks);
				//If capture
				if (GET_BIT(board->side_bitboards[WHITE], to)) //If to square is occupied
				{
					Add_Move(move_list, SQUARE_64_TO_120(from), SQUARE_64_TO_120(to), bQ, board->board_array64[to], NOT_SPECIAL, GET_MMVLVA_SCORE(board->board_array64[to], bQ), board);
				}
				else //Quiet move
				{
					Add_Move(move_list, SQUARE_64_TO_120(from), SQUARE_64_TO_120(to), bQ, EMPTY, NOT_SPECIAL, 0, board);
				}
			}
		}

		/***************************************/
		/*************** King *****************/
		/***************************************/
		U64 king_temp = board->piece_bitboards[bK];

		from = pop_1st_bit(&king_temp);
		//Calculate attack spaces and remove friendly targets
		attacks = king_attack_masks[from] & ~board->side_bitboards[BLACK];
		while (attacks)
		{
			to = pop_1st_bit(&attacks);
			//If capture
			if (GET_BIT(board->side_bitboards[WHITE], to)) //If to square is occupied
			{
				Add_Move(move_list, SQUARE_64_TO_120(from), SQUARE_64_TO_120(to), bK, board->board_array64[to], NOT_SPECIAL, GET_MMVLVA_SCORE(board->board_array64[to], bK), board);
			}
			else //Quiet move
			{
				Add_Move(move_list, SQUARE_64_TO_120(from), SQUARE_64_TO_120(to), bK, EMPTY, NOT_SPECIAL, 0, board);
			}
		}
		

		/***** Castling *****/
		//Kingside
		if ((board->castle_rights & BK_CASTLE))
		{
			//Castling rights active
			if ((board->board_array120[F8] == EMPTY) && (board->board_array120[G8] == EMPTY) //Travel squares are empty
				&& !Under_Attack(E8, WHITE, board) && !Under_Attack(F8, WHITE, board) && !Under_Attack(G8, WHITE, board)) //King does not move through check
			{
				Add_Move(move_list, E8, G8, bK, 0, KING_CASTLE, 0, board); //Always moves to G8
			}
		}
		//Queenside
		if ((board->castle_rights & BQ_CASTLE))
		{
			//Castling rights active
			if ((board->board_array120[B8] == EMPTY) && (board->board_array120[C8] == EMPTY) && (board->board_array120[D8] == EMPTY) //Travel squares are empty
				&& !Under_Attack(C8, WHITE, board) && !Under_Attack(D8, WHITE, board) && !Under_Attack(E8, WHITE, board)) //King does not move through check
			{
				Add_Move(move_list, E8, C8, bK, 0, QUEEN_CASTLE, 0, board); //Always moves to C8	
			}
		}
		/***** End King Moves *****/
	}
}