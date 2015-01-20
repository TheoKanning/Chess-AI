/* board.cpp
* Contains definitions of board-related functions 
* Theo Kanning 11/28/14 */
#include "globals.h"
#include <stdio.h>
#include <iostream>

//#include "stdafx.h"
using namespace std;

extern int piece_values[13];

//Private functions
void Update_Board_Array_120(BOARD_STRUCT *board);

//Sets a board to the start position and fills all fields
void Init_Board(BOARD_STRUCT *board)
{
	int index,rank,file;
	/***** Empty boards *****/
	//12x10 board, set all squares to OFF_BOARD or EMPTY
	for (index = 0; index < 120; index++)
	{
		if (ON_BOARD_120(index))
		{
			board->board_array120[index] = EMPTY;
		}
		else
		{
			board->board_array120[index] = OFF_BOARD;
		}
	}

	//8x8 board, set all spaces to empty
	for (index = 0; index < 64; index++)
	{
		board->board_array64[index] = EMPTY;
	}

	/**** Adding Pieces *****/

	//Add in pieces to 8x8 board, not in any particularly intelligent fashion
	//Rank 1
	board->board_array64[0] = wR;
	board->board_array64[1] = wN;
	board->board_array64[2] = wB;
	board->board_array64[3] = wQ;
	board->board_array64[4] = wK;
	board->board_array64[5] = wB;
	board->board_array64[6] = wN;
	board->board_array64[7] = wR;

	//Rank 2
	for (file = FILE_A; file <= FILE_H; file++)
	{
		board->board_array64[RANK_FILE_TO_SQUARE_64(RANK_2,file)] = wP;
	}
	
	//Ranks 3-6 empty
	for (file = FILE_A; file <= FILE_H; file++)
	{
		for (rank = RANK_3; rank <= RANK_6; rank++)
		{
			board->board_array64[RANK_FILE_TO_SQUARE_64(rank, file)] = EMPTY; 
		}
	}

	//Rank 7
	for (file = FILE_A; file <= FILE_H; file++)
	{
		board->board_array64[RANK_FILE_TO_SQUARE_64(RANK_7, file)] = bP;
	}

	//Rank 8
	board->board_array64[56] = bR;
	board->board_array64[57] = bN;
	board->board_array64[58] = bB;
	board->board_array64[59] = bQ;
	board->board_array64[60] = bK;
	board->board_array64[61] = bB;
	board->board_array64[62] = bN;
	board->board_array64[63] = bR;

	//Copy pieces to 10x12 board
	for (index = 0; index < 64; index++)
	{
		board->board_array120[SQUARE_64_TO_120(index)] = board->board_array64[index];
	}

	board->ep = NO_SQUARE; //ep uses 120 indexing

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

	Evaluate_Board(board);

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

	//Empty board_array64
	for (index = 0; index < 64; index++)
	{
		board->board_array64[index] = EMPTY;
	}

	//Read piece data
	while (square <= 64)
	{
		file = FILE_A + ((square - 1) % 8);
		rank = RANK_8 - ((square - 1) / 8);
		index = RANK_FILE_TO_SQUARE_64(rank, file);
		ASSERT(index >= 0 && index < 64);

		switch (*fen)//Read current character
		{
		//White pieces
		case 'P': board->board_array64[index] = wP; break;
		case 'N': board->board_array64[index] = wN; break;
		case 'B': board->board_array64[index] = wB; break;
		case 'R': board->board_array64[index] = wR; break;
		case 'Q': board->board_array64[index] = wQ; break;
		case 'K': board->board_array64[index] = wK; break;

		//Black pieces
		case 'p': board->board_array64[index] = bP; break;
		case 'n': board->board_array64[index] = bN; break;
		case 'b': board->board_array64[index] = bB; break;
		case 'r': board->board_array64[index] = bR; break;
		case 'q': board->board_array64[index] = bQ; break;
		case 'k': board->board_array64[index] = bK; break;

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
		ASSERT(ON_BOARD_120(SQUARE_64_TO_120(RANK_FILE_TO_SQUARE_64(rank, file))));
		board->ep = SQUARE_64_TO_120(RANK_FILE_TO_SQUARE_64(rank, file));
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

	Update_Board_Array_120(board);

	Compute_Hash(board);

	Evaluate_Board(board);

#ifdef DEBUG
	Check_Board(board);
#endif
}

//Adds a piece and location to a piece list
void Add_To_Piecelists(int piece, int index120, BOARD_STRUCT *board)
{
	ASSERT(ON_BOARD_120(index120));
	if (piece != EMPTY) //Ignore empty pieces
	{
		board->piece_list120[piece][board->piece_num[piece]] = index120;
		board->piece_num[piece]++;
		SET_BIT(board->piece_bitboards[piece], SQUARE_120_TO_64(index120));
	}

	if (IS_WHITE_PIECE(piece))
	{
		board->material += piece_values[piece];
		board->white_material += piece_values[piece];
		board->middle_piece_square_score += middle_piece_square_tables[piece][SQUARE_120_TO_64(index120)];
		board->end_piece_square_score += end_piece_square_tables[piece][SQUARE_120_TO_64(index120)];
	}
	if (IS_BLACK_PIECE(piece))
	{
		board->material -= piece_values[piece];
		board->black_material += piece_values[piece];
		board->middle_piece_square_score -= middle_piece_square_tables[piece][SQUARE_120_TO_64(index120)];
		board->end_piece_square_score -= end_piece_square_tables[piece][SQUARE_120_TO_64(index120)];
	}
	if ((piece != wK) && (piece != bK))	board->total_material += piece_values[piece];
}

//Removes a piece and location from piece list
//Updates piece_square evaluation
void Remove_From_Piecelists(int piece, int index120, BOARD_STRUCT *board)
{
	int i;
	int found = 0;
	ASSERT(ON_BOARD_120(index120));
	if (piece != EMPTY)
	{
		for (i = 0; i < board->piece_num[piece] - 1; i++) //Loop through available info
		{
			if (board->piece_list120[piece][i] == index120) found = 1; //Begin shifting indices down

			if (found) //If the index has been found
			{
				board->piece_list120[piece][i] = board->piece_list120[piece][i + 1]; //Shift values down one index
			}
		}

		ASSERT(found || (board->piece_list120[piece][board->piece_num[piece]-1] == index120)); //Index was either found early or in last position
		board->piece_list120[piece][board->piece_num[piece]-1] = 0; //Remove last index

		board->piece_num[piece]--;

		CLR_BIT(board->piece_bitboards[piece], SQUARE_120_TO_64(index120)); //Remove from bitboards
	}

	if (IS_WHITE_PIECE(piece))
	{
		board->material -= piece_values[piece];
		board->white_material -= piece_values[piece];
		board->middle_piece_square_score -= middle_piece_square_tables[piece][SQUARE_120_TO_64(index120)];
		board->end_piece_square_score -= end_piece_square_tables[piece][SQUARE_120_TO_64(index120)];
	}
	if (IS_BLACK_PIECE(piece))
	{
		board->material += piece_values[piece];
		board->black_material -= piece_values[piece];
		board->middle_piece_square_score += middle_piece_square_tables[piece][SQUARE_120_TO_64(index120)];
		board->end_piece_square_score += end_piece_square_tables[piece][SQUARE_120_TO_64(index120)];
	}
	if ((piece != wK) && (piece != bK))	board->total_material -= piece_values[piece];
}


//Updates piecelist120 and piecenum using data in board_array64
void Update_Piece_Lists(BOARD_STRUCT *board)
{
	int index,piece;

	//Reset material and piece square count to 0
	board->material = 0;
	board->white_material = 0;
	board->black_material = 0;
	board->total_material = 0;
	board->end_piece_square_score = 0;
	board->middle_piece_square_score = 0;

	//Set all values to zero
	for (piece = 0; piece <= bK; piece++)
	{
		for (index = 0; index < 10; index++)
		{
			board->piece_list120[piece][index] = 0;	
		}
		board->piece_num[piece] = 0;
	}

	//Index through all 64 squares and adjust piece list and num accordingly
	for (index = 0; index < 64; index++)
	{
		piece = board->board_array64[index];
		if (piece != EMPTY)
		{
			board->piece_list120[piece][board->piece_num[piece]] = SQUARE_64_TO_120(index); //Store 10x12 index in list
			board->piece_num[piece]++; //Increment piece count

			if (IS_WHITE_PIECE(piece))
			{
				board->material += piece_values[piece];
				board->white_material += piece_values[piece];
				board->middle_piece_square_score += middle_piece_square_tables[piece][index];
				board->end_piece_square_score += end_piece_square_tables[piece][index];
			}
			if (IS_BLACK_PIECE(piece))
			{
				board->material -= piece_values[piece];
				board->black_material += piece_values[piece];
				board->middle_piece_square_score -= middle_piece_square_tables[piece][index];
				board->end_piece_square_score -= end_piece_square_tables[piece][index];
			}
			if ((piece != wK) && (piece != bK))	board->total_material += piece_values[piece];
		}
	}
}

//Fills pawn bitboards using board_array64
void Update_Bitboards(BOARD_STRUCT *board)
{
	int index;
	int piece;
	//Reset bitboards
	board->pawn_bitboards[WHITE] = 0;
	board->pawn_bitboards[BLACK] = 0;
	board->pawn_bitboards[BOTH] = 0;

	for (index = 0; index <= bK; index++) //Clear piece bitboards
	{
		board->piece_bitboards[index] = 0;
	}

	//Loop through all squares and add appropriate data to bitboards
	for (index = 0; index < 64; index++)
	{
		piece = board->board_array64[index];

		if (piece != EMPTY)
		{
			SET_BIT(board->piece_bitboards[piece], index); //Update bitboard
		}

		if (piece == wP) //White pawn
		{
			SET_BIT(board->pawn_bitboards[WHITE], index); //Update bitboard
			SET_BIT(board->pawn_bitboards[BOTH], index); //Update bitboard
		}
		else if (piece == bP) //White pawn
		{
			SET_BIT(board->pawn_bitboards[BLACK], index); //Update bitboard
			SET_BIT(board->pawn_bitboards[BOTH], index); //Update bitboard
		}
	}

}
//Updates board_array120 from a filled board_array_64 list
void Update_Board_Array_120(BOARD_STRUCT *board)
{
	int index;
	for (index = 0; index < 64; index++)
	{
		board->board_array120[SQUARE_64_TO_120(index)] = board->board_array64[index];
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
	if (board->move_counter < 4) return 0;

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


//Checks all board values for consistency
//Asserts will fail if anything is incorrect
void Check_Board(BOARD_STRUCT *board)
{
	int index64, index120, piece, i, j;
	int material_temp = 0;
	int white_material_temp = 0;
	int black_material_temp = 0;
	int total_material_temp = 0;
	int middle_piece_square_temp = 0;
	int end_piece_square_temp = 0;
	int piece_square_temp = 0;
	int piecelist_temp[13][10] = { 0 };
	int piece_num_temp[13] = { 0 };
	U64 hash_temp = 0;
	U64 pawn_hash_temp = 0;
	U64 pawn_bitboards_temp[3] = { 0 };
	U64 piece_bitboards_temp[13] = { 0 };

	//Check board arrays for consistency
	for (index64 = 0; index64 < 64; index64++)
	{
		piece = board->board_array64[index64];
		index120 = SQUARE_64_TO_120(index64);
		ASSERT(board->board_array120[index120] == piece); //Check if both arrays have identical data

		/***** Regenerate piece lists *****/
		if (piece != EMPTY)
		{
			piecelist_temp[piece][piece_num_temp[piece]] = index120; //Store index
			piece_num_temp[piece]++; //Increment count
			SET_BIT(piece_bitboards_temp[piece], index64); //Update bitboard
		}
		if (piece == wP)
		{
			SET_BIT(pawn_bitboards_temp[WHITE], index64);
			SET_BIT(pawn_bitboards_temp[BOTH], index64);
		}
		else if (piece == bP)
		{
			SET_BIT(pawn_bitboards_temp[BLACK], index64);
			SET_BIT(pawn_bitboards_temp[BOTH], index64);
		}
		if (IS_WHITE_PIECE(piece))
		{
			material_temp += piece_values[piece];
			white_material_temp += piece_values[piece];
			middle_piece_square_temp += middle_piece_square_tables[piece][index64];
			end_piece_square_temp += end_piece_square_tables[piece][index64];
		}
		if (IS_BLACK_PIECE(piece))
		{
			material_temp -= piece_values[piece];
			black_material_temp += piece_values[piece];
			middle_piece_square_temp -= middle_piece_square_tables[piece][index64];
			end_piece_square_temp -= end_piece_square_tables[piece][index64];
		}
		if ((piece != wK) && (piece != bK)) total_material_temp += piece_values[piece];
		
	}

	/***** Verify Piece Lists *****/
	for (piece = 0; piece <= bK; piece++)
	{
		ASSERT(piece_num_temp[piece] == board->piece_num[piece]); //Check piece num array for consistency
	}

	//Make sure unused piece list indices are zero
	for (piece = 1; piece <= bK; piece++)
	{
		for (i = piece_num_temp[piece]; i < 10; i++)
		{
			ASSERT(board->piece_list120[piece][i] == 0); //Make sure unused indices are zero
		}
	}

	//Make sure piece list indices are correct
	for (piece = 0; piece <= bK; piece++)
	{
		for (i = 0; i < piece_num_temp[piece]; i++) //Index to look for
		{
			for (j = 0; j <= piece_num_temp[piece]; j++)//If last index reached, return error
			{
				ASSERT(j < piece_num_temp[piece]); //Make sure that last index has not been reached
				if (piecelist_temp[piece][i] == board->piece_list120[piece][j]) break;
			}
		}
	}

	/***** Bitboards *****/
	ASSERT(pawn_bitboards_temp[WHITE] == board->pawn_bitboards[WHITE]);
	ASSERT(pawn_bitboards_temp[BLACK] == board->pawn_bitboards[BLACK]);
	ASSERT(pawn_bitboards_temp[BOTH] == board->pawn_bitboards[BOTH]);
	for (int index = 0; index < bK; index++)
	{
		ASSERT(piece_bitboards_temp[index] == board->piece_bitboards[index]);
	}

	/***** Material Score *****/
	ASSERT(material_temp == board->material);
	ASSERT(total_material_temp == board->total_material);
	ASSERT(white_material_temp == board->white_material);
	ASSERT(black_material_temp == board->black_material);

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
			switch (board->board_array64[RANK_FILE_TO_SQUARE_64(rank, file)])
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
	cout << "Eval: " << board->eval_score / 100.0 << endl;
}

//Prints all three pawn bitboards using the same format as the Print_Board function
void Print_Bitboards(BOARD_STRUCT *board)
{
	int rank, file, color;
	char* names[3] = { "White", "Black", "Both" };
	//Print board
	for (color = WHITE; color <= BOTH; color++)
	{
		cout << names[color] << endl;
		cout << endl << "    A B C D E F G H" << endl;
		for (rank = RANK_8; rank >= RANK_1; rank--)
		{
			cout << "   -----------------" << endl; //17 Underscores
			cout << " " << rank + 1 << " ";
			for (file = FILE_A; file <= FILE_H; file++)
			{
				cout << (char)179; //Bar

				if (GET_BIT(board->pawn_bitboards[color], RANK_FILE_TO_SQUARE_64(rank, file)))
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
}
