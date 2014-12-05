/* board.cpp
* Contains definitions of board-related functions 
* Theo Kanning 11/28/14 */
#include "globals.h"
#include <stdio.h>
#include <iostream>

using namespace std;

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
}

//Sets a board to a fen position and fills all fields
void Parse_Fen(char *fen, BOARD_STRUCT *board)
{
	int index, rank, file;
	int square = 1; //Square being parsed

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

	Update_Bitboards(board);

	Update_Piece_Lists(board);

	Update_Board_Array_120(board);

	Compute_Hash(board);


}

//Updates piecelist120 and piecenum using data in board_array64
void Update_Piece_Lists(BOARD_STRUCT *board)
{
	int index,piece;
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
		}
	}
}


//Fills pawn bitboards using board_array64
void Update_Bitboards(BOARD_STRUCT *board)
{
	int index;
	for (index = 0; index < 64; index++)
	{
		if (board->board_array64[index] == wP) //White pawn
		{
			SET_BIT(board->pawn_bitboards[WHITE], index); //Update bitboard
			SET_BIT(board->pawn_bitboards[BOTH], index); //Update bitboard
		}
		else if (board->board_array64[index] == bP) //White pawn
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

//Prints pieces and useful info to console
void Print_Board(BOARD_STRUCT *board)
{
	int rank, file;
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
	cout << "EP: " << board->ep<< endl;

	//Hash
	cout << "Hashkey: " << board->hash_key << endl;

	//50 move counter
	cout << "50 Move Count: " << board->move_counter << endl;

	//Ply
	cout << "Hply: " << board->hply << endl;
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
