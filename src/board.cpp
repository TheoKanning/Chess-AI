/* board.cpp
* Contains definitions of board-related functions 
* Theo Kanning 11/28/14 */
#include "globals.h"
#include <stdio.h>
#include <iostream>

//#include "stdafx.h"
using namespace std;


//Sets a board to the start position and fills all fields
void Init_Board(BOARD_STRUCT *board)
{
	int index,rank,file;
	
	/***** Empty boards *****/
	//8x8 board, set all spaces to empty
	for (index = 0; index < 64; index++)
	{
		board->board_array[index] = EMPTY;
	}

	/**** Adding Pieces *****/

	//Add in pieces to 8x8 board, not in any particularly intelligent fashion
	//Rank 1
	board->board_array[0] = wR;
	board->board_array[1] = wN;
	board->board_array[2] = wB;
	board->board_array[3] = wQ;
	board->board_array[4] = wK;
	board->board_array[5] = wB;
	board->board_array[6] = wN;
	board->board_array[7] = wR;

	//Rank 2
	for (file = FILE_A; file <= FILE_H; file++)
	{
		board->board_array[RANK_FILE_TO_SQUARE(RANK_2,file)] = wP;
	}
	
	//Ranks 3-6 empty
	for (file = FILE_A; file <= FILE_H; file++)
	{
		for (rank = RANK_3; rank <= RANK_6; rank++)
		{
			board->board_array[RANK_FILE_TO_SQUARE(rank, file)] = EMPTY; 
		}
	}

	//Rank 7
	for (file = FILE_A; file <= FILE_H; file++)
	{
		board->board_array[RANK_FILE_TO_SQUARE(RANK_7, file)] = bP;
	}

	//Rank 8
	board->board_array[56] = bR;
	board->board_array[57] = bN;
	board->board_array[58] = bB;
	board->board_array[59] = bQ;
	board->board_array[60] = bK;
	board->board_array[61] = bB;
	board->board_array[62] = bN;
	board->board_array[63] = bR;

	

	board->ep = NO_SQUARE; //ep uses 64 indexing

	//Set each to 1
	board->castle_rights = 15;

	board->side = WHITE;

	board->hply = 0;

	board->move_counter = 0;

	//Pawn bitboards
	Update_Bitboards(board);

	//Piece lists
	Update_Piece_Lists(board);

	Compute_Hash(board);

	board->age = 0;

#ifdef DEBUG
	Check_Board(board);
#endif
}

//Sets a board to a fen position and fills all fields
void Parse_Fen(char *fen, BOARD_STRUCT *board)
{
	int index, rank, file;
	int square = 1; //Square being parsed

	Clear_Undo_List(board);

	//Empty board_array
	for (index = 0; index < 64; index++)
	{
		board->board_array[index] = EMPTY;
	}

	//Read piece data
	while (square <= 64)
	{
		file = FILE_A + ((square - 1) % 8);
		rank = RANK_8 - ((square - 1) / 8);
		index = RANK_FILE_TO_SQUARE(rank, file);
		ASSERT(index >= 0 && index < 64);

		switch (*fen)//Read current character
		{
		//White pieces
		case 'P': board->board_array[index] = wP; break;
		case 'N': board->board_array[index] = wN; break;
		case 'B': board->board_array[index] = wB; break;
		case 'R': board->board_array[index] = wR; break;
		case 'Q': board->board_array[index] = wQ; break;
		case 'K': board->board_array[index] = wK; break;

		//Black pieces
		case 'p': board->board_array[index] = bP; break;
		case 'n': board->board_array[index] = bN; break;
		case 'b': board->board_array[index] = bB; break;
		case 'r': board->board_array[index] = bR; break;
		case 'q': board->board_array[index] = bQ; break;
		case 'k': board->board_array[index] = bK; break;

		//Spaces
		case '/': square--; break;
		case '1': break;
		case '2': square++; break;
		case '3': square+=2; break;
		case '4': square+=3; break;
		case '5': square+=4; break;
		case '6': square+=5; break;
		case '7': square+=6; break;
		case '8': square+=7; break;
		
		}
		square++;
		fen++;
	}
	
	//Next character should  be a space
	ASSERT(*fen == ' ');

	//Side
	fen++;
	if (*fen == 'w')
		board->side = WHITE;
	else
	{
		board->side = BLACK;
	}

	//Next character should  be a space
	fen++;
	ASSERT(*fen == ' ');
	
	//Castling rights
	fen++;
	board->castle_rights = 0;
	while (*fen != ' ') //Continue until space is read
	{
		switch (*fen)
		{
		case 'K': board->castle_rights |= WK_CASTLE; break;
		case 'Q': board->castle_rights |= WQ_CASTLE; break;
		case 'k': board->castle_rights |= BK_CASTLE; break;
		case 'q': board->castle_rights |= BQ_CASTLE; break;
		case '-':  break;
		}
		fen++;
	}
	
	//Next character should  be a space
	ASSERT(*fen == ' ');

	//Ep
	board->ep = NO_SQUARE;
	fen++;
	if (*fen != '-') //If there is an ep square
	{
		file = (int)fen[0] - 97;
		rank = (int)fen[1] - 49;
		ASSERT(ON_BOARD(RANK_FILE_TO_SQUARE(rank, file)));
		board->ep = RANK_FILE_TO_SQUARE(rank, file);
		fen ++; //Skip one extra space
	}
	fen++;

	//Next character should  be a space
	ASSERT(*fen == ' ');

	//50 move counter
	index = 0;
	board->move_counter = 0;
	fen++; //increment to start of counter
	if (fen[1] != ' ')
	{
		board->move_counter += (int)fen[1] - 48; //Add second digit, if any
		board->move_counter += 10*((int)fen[0] - 48); //First digit * 10
		fen += 2;
	}
	else
	{
		board->move_counter += (int)fen[0] - 48; //First digit
		fen++;
	}
	
	//Next character should  be a space
	ASSERT(*fen == ' ');

	//50 move counter
	index = 0;
	board->hply = 0;
	fen++; //increment to start of counter
	if (fen[1] != '\0')
	{
		board->hply += (int)fen[1] - 48; //Add second digit, if any
		board->hply += 10 * ((int)fen[0] - 48); //First digit * 10
		fen += 2;
	}
	else
	{
		board->hply += (int)fen[0] - 48; //First digit
		fen++;
	}

	board->hply *= 2; //Each move is 2 ply
	if (board->side == WHITE) board->hply--;

	board->age = 0;

	Update_Bitboards(board);

	Update_Piece_Lists(board);

	Compute_Hash(board);

#ifdef DEBUG
	Check_Board(board);
#endif
}

//Adds a piece and location to a piece list
void Add_To_Piecelists(int piece, int square, BOARD_STRUCT *board)
{
	ASSERT(ON_BOARD(square));
	if (piece != EMPTY) //Ignore empty pieces
	{
		SET_BIT(board->piece_bitboards[piece], square);
		board->piece_num[piece]++;
	}

	if (IS_WHITE_PIECE(piece))
	{
		SET_BIT(board->side_bitboards[WHITE], square);
		SET_BIT(board->side_bitboards[BOTH], square);

		board->middle_piece_square_score += middle_piece_square_tables[piece][square];
		board->end_piece_square_score += end_piece_square_tables[piece][square];

		if (piece == wP) //Update pawn material
		{
			board->pawn_material[WHITE] += piece_values[piece];
		}
		else if (piece != wK) //Update big material
		{
			board->big_material[WHITE] += piece_values[piece];
		}
	}
	if (IS_BLACK_PIECE(piece))
	{
		SET_BIT(board->side_bitboards[BLACK], square);
		SET_BIT(board->side_bitboards[BOTH], square);

		board->middle_piece_square_score -= middle_piece_square_tables[piece][square];
		board->end_piece_square_score -= end_piece_square_tables[piece][square];

		if (piece == bP) //Update pawn material
		{
			board->pawn_material[BLACK] += piece_values[piece];
		}
		else if (piece != bK) //Update big material
		{
			board->big_material[BLACK] += piece_values[piece];
		}
	}
}

//Removes a piece and location from piece list
//Updates piece_square evaluation
void Remove_From_Piecelists(int piece, int square, BOARD_STRUCT *board)
{
	int found = 0;
	ASSERT(ON_BOARD(square));
	if (piece != EMPTY)
	{
		CLR_BIT(board->piece_bitboards[piece], square); //Remove from bitboards
		board->piece_num[piece]--;
	}

	if (IS_WHITE_PIECE(piece))
	{

		CLR_BIT(board->side_bitboards[WHITE], square);
		CLR_BIT(board->side_bitboards[BOTH], square);

		board->middle_piece_square_score -= middle_piece_square_tables[piece][square];
		board->end_piece_square_score -= end_piece_square_tables[piece][square];

		if (piece == wP) //Update pawn material
		{
			board->pawn_material[WHITE] -= piece_values[piece];
		}
		else if (piece != wK) //Update big material
		{
			board->big_material[WHITE] -= piece_values[piece];
		}
	}
	if (IS_BLACK_PIECE(piece))
	{
		CLR_BIT(board->side_bitboards[BLACK], square);
		CLR_BIT(board->side_bitboards[BOTH], square);

		board->middle_piece_square_score += middle_piece_square_tables[piece][square];
		board->end_piece_square_score += end_piece_square_tables[piece][square];

		if (piece == bP) //Update pawn material
		{
			board->pawn_material[BLACK] -= piece_values[piece];
		}
		else if (piece != bK) //Update big material
		{
			board->big_material[BLACK] -= piece_values[piece];
		}
	}
}


//Updates piecelist and piecenum using data in board_array
void Update_Piece_Lists(BOARD_STRUCT *board)
{
	int index,piece;

	//Reset material and piece square count to 0
	board->big_material[WHITE] = 0;
	board->pawn_material[WHITE] = 0;
	board->big_material[BLACK] = 0;
	board->pawn_material[BLACK] = 0;

	board->end_piece_square_score = 0;
	board->middle_piece_square_score = 0;

	for (int i = 0; i <= bK; i++)
	{
		board->piece_num[i] = 0;
	}

	//Index through all 64 squares and adjust piece list and num accordingly
	for (index = 0; index < 64; index++)
	{
		piece = board->board_array[index];
		if (piece != EMPTY)
		{
			board->piece_num[piece]++;

			if (IS_WHITE_PIECE(piece))
			{
				if (piece == wP) //Update pawn material
				{
					board->pawn_material[WHITE] += piece_values[piece];
				}
				else if (piece != wK) //Update big material
				{
					board->big_material[WHITE] += piece_values[piece];
				}

				board->middle_piece_square_score += middle_piece_square_tables[piece][index];
				board->end_piece_square_score += end_piece_square_tables[piece][index];
			}
			if (IS_BLACK_PIECE(piece))
			{
				if (piece == bP) //Update pawn material
				{
					board->pawn_material[BLACK] += piece_values[piece];
				}
				else if (piece != bK) //Update big material
				{
					board->big_material[BLACK] += piece_values[piece];
				}

				board->middle_piece_square_score -= middle_piece_square_tables[piece][index];
				board->end_piece_square_score -= end_piece_square_tables[piece][index];
			}
		}
	}
}

//Fills pawn bitboards using board_array
void Update_Bitboards(BOARD_STRUCT *board)
{
	int index;
	int piece;
	//Reset bitboards
	board->side_bitboards[WHITE] = 0;
	board->side_bitboards[BLACK] = 0;
	board->side_bitboards[BOTH] = 0;

	for (index = 0; index <= bK; index++) //Clear piece bitboards
	{
		board->piece_bitboards[index] = 0;
	}

	//Loop through all squares and add appropriate data to bitboards
	for (index = 0; index < 64; index++)
	{
		piece = board->board_array[index];

		if (piece != EMPTY)
		{
			SET_BIT(board->piece_bitboards[piece], index); //Update bitboard

			if (IS_WHITE_PIECE(piece)) 	SET_BIT(board->side_bitboards[WHITE], index);
			else SET_BIT(board->side_bitboards[BLACK], index);

			SET_BIT(board->side_bitboards[BOTH], index);
		}
	}
}


//Clears undo list struct in a board
void Clear_Undo_List(BOARD_STRUCT *board)
{
	int index;

	for (index = 0; index < board->undo_list.num; index++)
	{
		//Clear all fields 
		board->undo_list.list[index].castle = 0;
		board->undo_list.list[index].ep = 0;
		board->undo_list.list[index].hash = 0;
		board->undo_list.list[index].move_counter = 0;
		board->undo_list.list[index].move_num = 0;
	}

	board->undo_list.num = 0;
}

//Checks backwards in the undo list struct to see if the current position has occured three times
int Is_Threefold_Repetition(BOARD_STRUCT *board)
{
	int count = 1; //Number of times current hash has occured, starts at 1
	int index = 0;
	int start = board->undo_list.num - 1;
	U64 hash = board->hash_key; //Hash key to search for

	/* Searches backwards until the first irreversible move found.
	* The number of reversible moves is equal to the move counter
	*/
	if (board->move_counter < 3) return 0;

	//could probably be index < move counter, but I'd rather be safe than worry about it
	for (index = 0; index <= board->move_counter; index++)
	{
		if (start - index < 0) return 0;

		if (board->undo_list.list[start - index].hash == hash)//if match is found
		{
			count++;
			if (count >= 3)
			{
				return 1;
			}
		}
	}
	return 0;
}

//Returns 1 if a position has repeated once
int Is_Repetition(BOARD_STRUCT *board)
{
	int index = 0;
	int start = board->undo_list.num - 1;
	U64 hash = board->hash_key; //Hash key to search for

	/* Searches backwards until the first irreversible move found.
	* The number of reversible moves is equal to the move counter
	*/
	if (board->move_counter < 3) return 0;

	//could probably be index < move counter, but I'd rather be safe than worry about it
	for (index = 0; index <= board->move_counter; index++)
	{
		if (start - index < 0) return 0;
		if (board->undo_list.list[start - index].hash == hash) return 1;//if match is found
		
	}
	return 0;
}

//Returns 1 if the board is a material draw
int Is_Material_Draw(BOARD_STRUCT *board)
{
	//Not a draw if a pawn remains
	if (board->pawn_material[WHITE] || board->pawn_material[BLACK]) return 0;

	//Not a draw if one side has at least a queen
	if ((board->big_material[WHITE] >= piece_values[wQ]) || (board->big_material[BLACK] >= piece_values[bQ])) return 0;

	//Return draw if each side has a bishop or less remaining
	if (board->big_material[WHITE] <= piece_values[wB] && board->big_material[BLACK] <= piece_values[wB]) return 1;

	//Return draw if two knights against bare king
	if (board->big_material[WHITE] == 2 * piece_values[wN] && board->big_material[BLACK] == 0) return 1;
	if (board->big_material[BLACK] == 2 * piece_values[bN] && board->big_material[WHITE] == 0) return 1;

	//Assume not a draw
	return 0;
}

//Checks all board values for consistency
//Asserts will fail if anything is incorrect
void Check_Board(BOARD_STRUCT *board)
{
	int square, piece;

	int big_material_temp[2] = { 0 };
	int pawn_material_temp[2] = { 0 };

	int middle_piece_square_temp = 0;
	int end_piece_square_temp = 0;

	int piece_num_temp[13] = { 0 };
	U64 hash_temp = 0;
	U64 pawn_hash_temp = 0;
	U64 pawn_bitboards_temp[3] = { 0 };
	U64 side_bitboards_temp[3] = { 0 };
	U64 piece_bitboards_temp[13] = { 0 };

	//Check board arrays for consistency
	for (square = 0; square < 64; square++)
	{
		piece = board->board_array[square];
		
		/***** Regenerate piece lists *****/
		if (piece != EMPTY)
		{
			SET_BIT(piece_bitboards_temp[piece], square); //Update bitboard
			SET_BIT(side_bitboards_temp[BOTH], square);
			piece_num_temp[piece]++;
		}

		if (IS_WHITE_PIECE(piece))
		{
			if (piece == wP) pawn_material_temp[WHITE] += piece_values[piece];
			else if (piece != wK) big_material_temp[WHITE] += piece_values[piece];

			middle_piece_square_temp += middle_piece_square_tables[piece][square];
			end_piece_square_temp += end_piece_square_tables[piece][square];

			SET_BIT(side_bitboards_temp[WHITE], square);
		}
		if (IS_BLACK_PIECE(piece))
		{
			if (piece == bP) pawn_material_temp[BLACK] += piece_values[piece];
			else if (piece != bK) big_material_temp[BLACK] += piece_values[piece];

			middle_piece_square_temp -= middle_piece_square_tables[piece][square];
			end_piece_square_temp -= end_piece_square_tables[piece][square];

			SET_BIT(side_bitboards_temp[BLACK], square);
		}		
	}

	/***** Bitboards *****/
	ASSERT(side_bitboards_temp[WHITE] == board->side_bitboards[WHITE]);
	ASSERT(side_bitboards_temp[BLACK] == board->side_bitboards[BLACK]);
	ASSERT(side_bitboards_temp[BOTH] == board->side_bitboards[BOTH]);

	for (int index = 0; index < bK; index++)
	{
		ASSERT(piece_bitboards_temp[index] == board->piece_bitboards[index]);
		ASSERT(piece_num_temp[index] == board->piece_num[index]);
	}

	/***** Material Score *****/
	ASSERT(big_material_temp[WHITE] == board->big_material[WHITE]);
	ASSERT(big_material_temp[BLACK] == board->big_material[BLACK]);
	ASSERT(pawn_material_temp[WHITE] == board->pawn_material[WHITE]);
	ASSERT(pawn_material_temp[BLACK] == board->pawn_material[BLACK]);


	/***** Piece Square Score *****/
	ASSERT(middle_piece_square_temp == board->middle_piece_square_score);
	ASSERT(end_piece_square_temp == board->end_piece_square_score);

	/***** Hashkey *****/
	hash_temp = board->hash_key; //Store previous value
	pawn_hash_temp = board->pawn_hash_key;
	Compute_Hash(board); //Recalculate hash
	ASSERT(board->hash_key == hash_temp); //Make sure they match
	ASSERT(board->pawn_hash_key == pawn_hash_temp);
	
	/***** Castling *****/


}


//Prints pieces and useful info to console
void Print_Board(BOARD_STRUCT *board)
{
	int rank, file;

	//Clear_Undo_List(board);

	//Print board
	cout << endl << "    A B C D E F G H" << endl;
	for (rank = RANK_8; rank >= RANK_1; rank--)
	{
		cout  << "   -----------------" << endl; //17 Underscores
		cout << " " << rank + 1 << " ";
		for (file = FILE_A; file <= FILE_H; file++)
		{
			cout  << (char)179; //Bar
			switch (board->board_array[RANK_FILE_TO_SQUARE(rank, file)])
			{
			case EMPTY:
				cout << " ";
				break;
			case wP:
				cout << "P";
				break;
			case wN:
				cout << "N";
				break;
			case wB:
				cout << "B";
				break;
			case wR:
				cout << "R";
				break;
			case wQ:
				cout << "Q";
				break;
			case wK:
				cout << "K";
				break;
			case bP:
				cout << "p";
				break;
			case bN:
				cout << "n";
				break;
			case bB:
				cout << "b";
				break;
			case bR:
				cout << "r";
				break;
			case bQ:
				cout << "q";
				break;
			case bK:
				cout << "k";
				break;
			default:
				break;
			}
		}
		cout << (char)179 << endl; //Bar
	}
	cout << "   -----------------" << endl << endl; //End box

	//Side
	cout << "Side: ";
	if (board->side)
	{
		cout << "Black" << endl;
	}
	else
	{
		cout << "White" << endl;
	}

	//Castling rights
	cout << "Castle: ";
	if (board->castle_rights & WK_CASTLE)
	{
		cout << "K";
	}
	if (board->castle_rights & WQ_CASTLE)
	{
		cout << "Q";
	}
	if (board->castle_rights & BK_CASTLE)
	{
		cout << "k";
	}
	if (board->castle_rights & BQ_CASTLE)
	{
		cout << "q";
	}

	cout << endl; //New line after castle info

	//EP
	if (board->ep == NO_SQUARE)
	{
		cout << "EP: None" << endl;
	}
	else
	{
		cout << "EP: " << board->ep << endl;
	}

	//Hash
	cout << "Hashkey: " << board->hash_key << endl;

	//50 move counter
	cout << "50 Move Count: " << board->move_counter << endl;

	//Ply
	cout << "Hply: " << board->hply << endl;

	//Material score
	cout << "Eval: " << Evaluate_Board(board) / 100.0 << endl;
}

//Prints a bitboards using the same format as the Print_Board function
void Print_Bitboard(U64 bb)
{
	int rank, file;
	//Print board
	cout << endl << "    A B C D E F G H" << endl;
	for (rank = RANK_8; rank >= RANK_1; rank--)
	{
		cout << "   -----------------" << endl; //17 Underscores
		cout << " " << rank + 1 << " ";
		for (file = FILE_A; file <= FILE_H; file++)
		{
			cout << (char)179; //Bar

			if (GET_BIT(bb, RANK_FILE_TO_SQUARE(rank, file)))
			{
				cout << 1;
			}
			else
			{
				cout << " ";
			}

		}
		cout << (char)179 << endl; //Bar
	}
	cout << "   -----------------" << endl << endl; //End box
}
