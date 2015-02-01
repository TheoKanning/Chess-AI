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
	int i, from120, to120, piece, capture, from, to;
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

		//Loop through each pawn and test for every kind of move, separate loops for white and black
		piece = wP;
		for (i = 0; i < board->piece_num[wP]; i++)
		{
			//Get pawn index120
			from120 = board->piece_list120[wP][i];
			ASSERT(ON_BOARD_120(from120));

			/***** Pushes and Promotions *****/
			to120 = from120 + 10;
			ASSERT(ON_BOARD_120(to120));

			if (board->board_array120[to120] == EMPTY) //If next space is empty, this also means it is on the board
			{
				/***** Single Pushes and Promotions *****/
				//Add as a promotion if this leads to rank 8
				if (GET_RANK_120(to120) == RANK_8)
				{
					Add_Move(move_list, from120, to120, wP, 0, QUEEN_PROMOTE, QUEEN_PROMOTE_SCORE, board);
					Add_Move(move_list, from120, to120, wP, 0, ROOK_PROMOTE, UNDER_PROMOTE_SCORE, board);
					Add_Move(move_list, from120, to120, wP, 0, BISHOP_PROMOTE, UNDER_PROMOTE_SCORE, board);
					Add_Move(move_list, from120, to120, wP, 0, KNIGHT_PROMOTE, UNDER_PROMOTE_SCORE, board);
				}
				else //If not a promotion
				{
					Add_Move(move_list, from120, to120, wP, 0, NOT_SPECIAL, 0, board);
				}

				/***** Double Pushes *****/
				to120 = from120 + 20;

				//Same as before, first space in front already established as empty
				if ((GET_RANK_120(from120) == RANK_2) && (board->board_array120[to120] == EMPTY)) //If pawn is on starting rank, and second square is also empty
				{
					Add_Move(move_list, from120, to120, wP, 0, NOT_SPECIAL, 0, board);
				}
			}

			/***** Normal and EP Captures *****/
			//Check first target square
			to120 = from120 + 9;
			if (ON_BOARD_120(to120)) //If target square is on board
			{
				if (IS_BLACK_PIECE(board->board_array120[to120])) //If target has a piece that can be taken
				{
					capture = board->board_array120[to120];
					if (GET_RANK_120(to120) == RANK_8) //If capturing into promotion
					{
						Add_Move(move_list, from120, to120, wP, capture, QUEEN_PROMOTE, QUEEN_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, wP), board);
						Add_Move(move_list, from120, to120, wP, capture, ROOK_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, wP), board);
						Add_Move(move_list, from120, to120, wP, capture, BISHOP_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, wP), board);
						Add_Move(move_list, from120, to120, wP, capture, KNIGHT_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, wP), board);
					}
					else
					{
						Add_Move(move_list, from120, to120, wP, capture, NOT_SPECIAL, GET_MMVLVA_SCORE(capture, wP), board);
					}
				}
				else if (to120 == board->ep) //If target is ep square
				{
					capture = bP;
					Add_Move(move_list, from120, to120, wP, capture, EP_CAPTURE, GET_MMVLVA_SCORE(capture, wP), board);
				}
			}

			//Check second target square
			to120 = from120 + 11;
			if (ON_BOARD_120(to120)) //If target square is on board
			{
				if (IS_BLACK_PIECE(board->board_array120[to120])) //If target has a piece that can be taken
				{
					capture = board->board_array120[to120];
					if (GET_RANK_120(to120) == RANK_8) //If capturing into promotion
					{
						Add_Move(move_list, from120, to120, wP, capture, QUEEN_PROMOTE, QUEEN_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, wP), board);
						Add_Move(move_list, from120, to120, wP, capture, ROOK_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, wP), board);
						Add_Move(move_list, from120, to120, wP, capture, BISHOP_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, wP), board);
						Add_Move(move_list, from120, to120, wP, capture, KNIGHT_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, wP), board);
					}
					else
					{
						Add_Move(move_list, from120, to120, wP, capture, NOT_SPECIAL, GET_MMVLVA_SCORE(capture, wP), board);
					}
				}
				else if (to120 == board->ep) //If target is ep square
				{
					capture = bP;
					Add_Move(move_list, from120, to120, wP, capture, EP_CAPTURE, GET_MMVLVA_SCORE(capture, wP), board);
				}
			}
		}
		/***** End Pawn Moves *****/

		/***************************************/
		/*************** KNIGHTS ***************/
		/***************************************/
		U64 knights_temp = board->piece_bitboards[wN];

		//Loop until no rooks remain
		while (knights_temp)
		{
			from = pop_1st_bit(&knights_temp);
			//Calculate attack spaces and remove friendly targets
			attacks = magicMovesKnight[from] & ~board->side_bitboards[WHITE];
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
		attacks = magicMovesKing[from] & ~board->side_bitboards[WHITE];
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

		//Loop through each pawn and test for every kind of move
		piece = bP;
		for (i = 0; i < board->piece_num[bP]; i++)
		{
			//Get pawn index120
			from120 = board->piece_list120[bP][i];
			ASSERT(ON_BOARD_120(from120));

			/***** Pushes and Promotions *****/
			to120 = from120 - 10;
			ASSERT(ON_BOARD_120(to120));

			if (board->board_array120[to120] == EMPTY) //If next space is empty, this also means it is on the board
			{
				/***** Single Pushes and Promotions *****/
				//Add as a promotion if this leads to rank 1
				if (GET_RANK_120(to120) == RANK_1)
				{
					Add_Move(move_list, from120, to120, bP, 0, QUEEN_PROMOTE, QUEEN_PROMOTE_SCORE, board);
					Add_Move(move_list, from120, to120, bP, 0, ROOK_PROMOTE, UNDER_PROMOTE_SCORE, board);
					Add_Move(move_list, from120, to120, bP, 0, BISHOP_PROMOTE, UNDER_PROMOTE_SCORE, board);
					Add_Move(move_list, from120, to120, bP, 0, KNIGHT_PROMOTE, UNDER_PROMOTE_SCORE, board);
				}
				else //If not a promotion
				{
					Add_Move(move_list, from120, to120, bP, 0, NOT_SPECIAL, 0, board);
				}

				/***** Double Pushes *****/
				to120 = from120 - 20;

				//Same as before, first space in front already established as empty
				if ((GET_RANK_120(from120) == RANK_7) && (board->board_array120[to120] == EMPTY)) //If pawn is on starting rank, and second square is also empty
				{
					Add_Move(move_list, from120, to120, bP, 0, NOT_SPECIAL, 0, board);
				}
			}

			/***** Normal and EP Captures *****/
			//Check first target square
			to120 = from120 - 9;
			if (ON_BOARD_120(to120)) //If target square is on board
			{
				if (IS_WHITE_PIECE(board->board_array120[to120])) //If target has a piece that can be taken
				{
					capture = board->board_array120[to120];
					if (GET_RANK_120(to120) == RANK_1) //If capturing into promotion
					{
						Add_Move(move_list, from120, to120, bP, capture, QUEEN_PROMOTE, QUEEN_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, bP), board);
						Add_Move(move_list, from120, to120, bP, capture, ROOK_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, bP), board);
						Add_Move(move_list, from120, to120, bP, capture, BISHOP_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, bP), board);
						Add_Move(move_list, from120, to120, bP, capture, KNIGHT_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, bP), board);
					}
					else
					{
						Add_Move(move_list, from120, to120, bP, capture, NOT_SPECIAL, GET_MMVLVA_SCORE(capture, bP), board);
					}
				}
				else if (to120 == board->ep) //If target is ep square
				{
					capture = wP;
					Add_Move(move_list, from120, to120, bP, capture, EP_CAPTURE, GET_MMVLVA_SCORE(capture, bP), board);
				}
			}

			//Check second target square
			to120 = from120 - 11;
			if (ON_BOARD_120(to120)) //If target square is on board
			{
				if (IS_WHITE_PIECE(board->board_array120[to120])) //If target has a piece that can be taken
				{
					capture = board->board_array120[to120];
					if (GET_RANK_120(to120) == RANK_1) //If capturing into promotion
					{
						Add_Move(move_list, from120, to120, bP, capture, QUEEN_PROMOTE, QUEEN_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, bP), board);
						Add_Move(move_list, from120, to120, bP, capture, ROOK_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, bP), board);
						Add_Move(move_list, from120, to120, bP, capture, BISHOP_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, bP), board);
						Add_Move(move_list, from120, to120, bP, capture, KNIGHT_PROMOTE, UNDER_PROMOTE_SCORE + GET_MMVLVA_SCORE(capture, bP), board);
					}
					else
					{
						Add_Move(move_list, from120, to120, bP, capture, NOT_SPECIAL, GET_MMVLVA_SCORE(capture, bP), board);
					}
				}
				else if (to120 == board->ep) //If target is ep square
				{
					capture = wP;
					Add_Move(move_list, from120, to120, bP, capture, EP_CAPTURE, GET_MMVLVA_SCORE(capture, bP), board);
				}
			}
		}
		/***** End Pawn Moves *****/

		/***************************************/
		/*************** KNIGHTS ***************/
		/***************************************/

		U64 knights_temp = board->piece_bitboards[bN];

		//Loop until no rooks remain
		while (knights_temp)
		{
			from = pop_1st_bit(&knights_temp);
			//Calculate attack spaces and remove friendly targets
			attacks = magicMovesKnight[from] & ~board->side_bitboards[BLACK];
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
		attacks = magicMovesKing[from] & ~board->side_bitboards[BLACK];
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