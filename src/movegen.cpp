/* movegen.c
* Contains all functions for generating a move list given a board pointer
* Thep Kanning 12/2/14
*/

#include "globals.h"

void Generate_Moves(BOARD_STRUCT *board, MOVE_LIST_STRUCT *move_list)
{
	int i, j, from120, to120, piece, capture, slide_num;
	const int side = board->side; //Store side to move

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

		for (i = 0; i < board->piece_num[wN]; i++) //Loop through all available white knights
		{
			from120 = board->piece_list120[wN][i];
			ASSERT(ON_BOARD_120(from120));
			//Cycle through all directions, adding move if on board, and adding capture if applicable
			for (j = 0; j < NUM_KNIGHT_MOVES; j++)
			{
				//Calculate target square
				to120 = from120 + KNIGHT_MOVES[j]; 

				//See if target square is on the board
				if (ON_BOARD_120(to120))
				{
					//Move if square is empty, or capture if it contains a black piece
					if (board->board_array120[to120] == EMPTY)
					{
						//Add quiet move
						Add_Move(move_list, from120, to120, wN, 0, NOT_SPECIAL, 0, board);
					}
					else if (IS_BLACK_PIECE(board->board_array120[to120]))
					{
						//Add capture
						capture = board->board_array120[to120];
						Add_Move(move_list, from120, to120, wN, capture, NOT_SPECIAL, GET_MMVLVA_SCORE(capture, wN), board);
					}
				}
			}
		}
		/***** End Knight Moves *****/

		/***************************************/
		/*************** BISHOPS ***************/
		/***************************************/

		for (i = 0; i < board->piece_num[wB]; i++) //Loop through all available white bishops
		{
			from120 = board->piece_list120[wB][i];
			ASSERT(ON_BOARD_120(from120));
			//Cycle through all directions, adding move if on board, and adding capture if applicable
			for (j = 0; j < NUM_BISHOP_MOVES; j++)
			{
				//Increase slide distance, adding moves until off board or hitting another piece
				for (slide_num = 1; slide_num < 8; slide_num++)
				{
					//Calculate target square based on direction and slide number
					to120 = from120 + slide_num * BISHOP_MOVES[j];

					//See if target square is on the board, if not, break loop
					if (ON_BOARD_120(to120))
					{
						//Move if square is empty, capture and break if it contains a black piece, break if white piece
						if (board->board_array120[to120] == EMPTY)
						{
							//Add quiet move
							Add_Move(move_list, from120, to120, wB, 0, NOT_SPECIAL, 0, board);
						}
						else if (IS_BLACK_PIECE(board->board_array120[to120]))
						{
							//Add capture
							capture = board->board_array120[to120];
							Add_Move(move_list, from120, to120, wB, capture, NOT_SPECIAL, GET_MMVLVA_SCORE(capture, wB), board);
							break;
						}
						else if (IS_WHITE_PIECE(board->board_array120[to120]))
						{
							break; //Stop sliding
						}
					}
					else
					{
						break; //Stop sliding in this direction
					}
				}
			}
		}
		/***** End Bishop Moves *****/

		/***************************************/
		/**************** ROOKS ****************/
		/***************************************/

		for (i = 0; i < board->piece_num[wR]; i++) //Loop through all available white rooks
		{
			from120 = board->piece_list120[wR][i];
			ASSERT(ON_BOARD_120(from120));

			//Cycle through all directions, adding move if on board, and adding capture if applicable
			for (j = 0; j < NUM_ROOK_MOVES; j++)
			{
				//Increase slide distance, adding moves until off board or hitting another piece
				for (slide_num = 1; slide_num < 8; slide_num++)
				{
					//Calculate target square based on direction and slide number
					to120 = from120 + slide_num * ROOK_MOVES[j];

					//See if target square is on the board, if not, break loop
					if (ON_BOARD_120(to120))
					{
						//Move if square is empty, capture and break if it contains a black piece, break if white piece
						if (board->board_array120[to120] == EMPTY)
						{
							//Add quiet move
							Add_Move(move_list, from120, to120, wR, 0, NOT_SPECIAL, 0, board);
						}
						else if (IS_BLACK_PIECE(board->board_array120[to120]))
						{
							//Add capture
							capture = board->board_array120[to120];
							Add_Move(move_list, from120, to120, wR, capture, NOT_SPECIAL, GET_MMVLVA_SCORE(capture, wR), board);
							break;
						}
						else if (IS_WHITE_PIECE(board->board_array120[to120]))
						{
							break; //Stop sliding
						}
					}
					else
					{
						break; //Stop sliding in this direction
					}
				}
			}
		}
		/***** End Rook Moves *****/

		/***************************************/
		/*************** QUEENS ****************/
		/***************************************/

		for (i = 0; i < board->piece_num[wQ]; i++) //Loop through all available white queens
		{
			from120 = board->piece_list120[wQ][i];
			ASSERT(ON_BOARD_120(from120));

			//Cycle through all directions, adding move if on board, and adding capture if applicable
			for (j = 0; j < NUM_QUEEN_MOVES; j++)
			{
				//Increase slide distance, adding moves until off board or hitting another piece
				for (slide_num = 1; slide_num < 8; slide_num++)
				{
					//Calculate target square based on direction and slide number
					to120 = from120 + slide_num * QUEEN_MOVES[j];

					//See if target square is on the board, if not, break loop
					if (ON_BOARD_120(to120))
					{
						//Move if square is empty, capture and break if it contains a black piece, break if white piece
						if (board->board_array120[to120] == EMPTY)
						{
							//Add quiet move
							Add_Move(move_list, from120, to120, wQ, 0, NOT_SPECIAL, 0, board);
						}
						else if (IS_BLACK_PIECE(board->board_array120[to120]))
						{
							//Add capture
							capture = board->board_array120[to120];
							Add_Move(move_list, from120, to120, wQ, capture, NOT_SPECIAL, GET_MMVLVA_SCORE(capture, wQ), board);
							break;
						}
						else if (IS_WHITE_PIECE(board->board_array120[to120]))
						{
							break; //Stop sliding
						}
					}
					else
					{
						break; //Stop sliding in this direction
					}
				}
			}
		}
		/***** End Queen Moves *****/

		/***************************************/
		/*************** King *****************/
		/***************************************/

		from120 = board->piece_list120[wK][0];
		ASSERT(ON_BOARD_120(from120));

		/***** Normal Moves and Captures *****/
		//Cycle through all directions, adding move if on board, and adding capture if applicable
		for (j = 0; j < NUM_KING_MOVES; j++)
		{
			//Calculate target square based on direction and slide number
			to120 = from120 + KING_MOVES[j];

			//See if target square is on the board
			if (ON_BOARD_120(to120))
			{
				//Move if square is empty, capture if it contains a black piece
				if (board->board_array120[to120] == EMPTY)
				{
					//Add quiet move
					Add_Move(move_list, from120, to120, wK, 0, NOT_SPECIAL, 0, board);
				}
				else if (IS_BLACK_PIECE(board->board_array120[to120]))
				{
					//Add capture
					capture = board->board_array120[to120];
					Add_Move(move_list, from120, to120, wK, capture, NOT_SPECIAL, GET_MMVLVA_SCORE(capture, wK), board);
				}
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

		for (i = 0; i < board->piece_num[bN]; i++) //Loop through all available black knights
		{
			from120 = board->piece_list120[bN][i];
			ASSERT(ON_BOARD_120(from120));
			//Cycle through all directions, adding move if on board, and adding capture if applicable
			for (j = 0; j < NUM_KNIGHT_MOVES; j++)
			{
				//Calculate target square
				to120 = from120 + KNIGHT_MOVES[j];

				//See if target square is on the board
				if (ON_BOARD_120(to120))
				{
					//Move if square is empty, or capture if it contains a black piece
					if (board->board_array120[to120] == EMPTY)
					{
						//Add quiet move
						Add_Move(move_list, from120, to120, bN, 0, NOT_SPECIAL, 0, board);
					}
					else if (IS_WHITE_PIECE(board->board_array120[to120]))
					{
						//Add capture
						capture = board->board_array120[to120];
						Add_Move(move_list, from120, to120, bN, capture, NOT_SPECIAL, GET_MMVLVA_SCORE(capture, bN), board);
					}
				}
			}
		}
		/***** End Knight Moves *****/

		/***************************************/
		/*************** BISHOPS ***************/
		/***************************************/

		for (i = 0; i < board->piece_num[bB]; i++) //Loop through all available black bishops
		{
			from120 = board->piece_list120[bB][i];
			ASSERT(ON_BOARD_120(from120));
			//Cycle through all directions, adding move if on board, and adding capture if applicable
			for (j = 0; j < NUM_BISHOP_MOVES; j++)
			{
				//Increase slide distance, adding moves until off board or hitting another piece
				for (slide_num = 1; slide_num < 8; slide_num++)
				{
					//Calculate target square based on direction and slide number
					to120 = from120 + slide_num * BISHOP_MOVES[j];

					//See if target square is on the board, if not, break loop
					if (ON_BOARD_120(to120))
					{
						//Move if square is empty, capture and break if it contains a black piece, break if white piece
						if (board->board_array120[to120] == EMPTY)
						{
							//Add quiet move
							Add_Move(move_list, from120, to120, bB, 0, NOT_SPECIAL, 0, board);
						}
						else if (IS_WHITE_PIECE(board->board_array120[to120]))
						{
							//Add capture
							capture = board->board_array120[to120];
							Add_Move(move_list, from120, to120, bB, capture, NOT_SPECIAL, GET_MMVLVA_SCORE(capture, bB), board);
							break;
						}
						else if (IS_BLACK_PIECE(board->board_array120[to120]))
						{
							break; //Stop sliding
						}
					}
					else
					{
						break; //Stop sliding in this direction
					}
				}
			}
		}
		/***** End Bishop Moves *****/

		/***************************************/
		/**************** ROOKS ****************/
		/***************************************/

		for (i = 0; i < board->piece_num[bR]; i++) //Loop through all available black rooks
		{
			from120 = board->piece_list120[bR][i];
			ASSERT(ON_BOARD_120(from120));

			//Cycle through all directions, adding move if on board, and adding capture if applicable
			for (j = 0; j < NUM_ROOK_MOVES; j++)
			{
				//Increase slide distance, adding moves until off board or hitting another piece
				for (slide_num = 1; slide_num < 8; slide_num++)
				{
					//Calculate target square based on direction and slide number
					to120 = from120 + slide_num * ROOK_MOVES[j];

					//See if target square is on the board, if not, break loop
					if (ON_BOARD_120(to120))
					{
						//Move if square is empty, capture and break if it contains a black piece, break if white piece
						if (board->board_array120[to120] == EMPTY)
						{
							//Add quiet move
							Add_Move(move_list, from120, to120, bR, 0, NOT_SPECIAL, 0, board);
						}
						else if (IS_WHITE_PIECE(board->board_array120[to120]))
						{
							//Add capture
							capture = board->board_array120[to120];
							Add_Move(move_list, from120, to120, bR, capture, NOT_SPECIAL, GET_MMVLVA_SCORE(capture, bR), board);
							break;
						}
						else if (IS_BLACK_PIECE(board->board_array120[to120]))
						{
							break; //Stop sliding
						}
					}
					else
					{
						break; //Stop sliding in this direction
					}
				}
			}
		}
		/***** End Rook Moves *****/

		/***************************************/
		/*************** QUEENS ****************/
		/***************************************/

		for (i = 0; i < board->piece_num[bQ]; i++) //Loop through all available black queens
		{
			from120 = board->piece_list120[bQ][i];
			ASSERT(ON_BOARD_120(from120));

			//Cycle through all directions, adding move if on board, and adding capture if applicable
			for (j = 0; j < NUM_QUEEN_MOVES; j++)
			{
				//Increase slide distance, adding moves until off board or hitting another piece
				for (slide_num = 1; slide_num < 8; slide_num++)
				{
					//Calculate target square based on direction and slide number
					to120 = from120 + slide_num * QUEEN_MOVES[j];

					//See if target square is on the board, if not, break loop
					if (ON_BOARD_120(to120))
					{
						//Move if square is empty, capture and break if it contains a black piece, break if white piece
						if (board->board_array120[to120] == EMPTY)
						{
							//Add quiet move
							Add_Move(move_list, from120, to120, bQ, 0, NOT_SPECIAL, 0, board);
						}
						else if (IS_WHITE_PIECE(board->board_array120[to120]))
						{
							//Add capture
							capture = board->board_array120[to120];
							Add_Move(move_list, from120, to120, bQ, capture, NOT_SPECIAL, GET_MMVLVA_SCORE(capture, bQ), board);
							break;
						}
						else if (IS_BLACK_PIECE(board->board_array120[to120]))
						{
							break; //Stop sliding
						}
					}
					else
					{
						break; //Stop sliding in this direction
					}
				}
			}
		}
		/***** End Queen Moves *****/

		/***************************************/
		/*************** King *****************/
		/***************************************/

		from120 = board->piece_list120[bK][0];
		ASSERT(ON_BOARD_120(from120));

		/***** Normal Moves and Captures *****/
		//Cycle through all directions, adding move if on board, and adding capture if applicable
		for (j = 0; j < NUM_KING_MOVES; j++)
		{
			//Calculate target square based on direction and slide number
			to120 = from120 + KING_MOVES[j];

			//See if target square is on the board
			if (ON_BOARD_120(to120))
			{
				//Move if square is empty, capture if it contains a black piece
				if (board->board_array120[to120] == EMPTY)
				{
					//Add quiet move
					Add_Move(move_list, from120, to120, bK, 0, NOT_SPECIAL, 0, board);
				}
				else if (IS_WHITE_PIECE(board->board_array120[to120]))
				{
					//Add capture
					capture = board->board_array120[to120];
					Add_Move(move_list, from120, to120, bK, capture, NOT_SPECIAL, GET_MMVLVA_SCORE(capture, bK), board);
				}
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

	if (side == WHITE)
	{
		king_square = board->piece_list120[wK][0];
	}
	else
	{
		king_square = board->piece_list120[bK][0];
	}

	return Under_Attack(king_square, side^1, board);
}