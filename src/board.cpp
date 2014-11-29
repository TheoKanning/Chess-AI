/* board.cpp
* Contains definitions of board-related functions 
* Theo Kanning 11/28/14 */
#include "globals.h"
#include <stdio.h>
#include <iostream>

using namespace std;

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
	for (file = FILE_A; file <= FILE_H; file++)
	{
		//White
		SET_BIT(board->pawn_bitboards[WHITE], RANK_FILE_TO_SQUARE_64(file, RANK_2));
		//Black
		SET_BIT(board->pawn_bitboards[BLACK], RANK_FILE_TO_SQUARE_64(file, RANK_7));
		//Both
		SET_BIT(board->pawn_bitboards[BOTH], RANK_FILE_TO_SQUARE_64(file, RANK_2));
		SET_BIT(board->pawn_bitboards[BOTH], RANK_FILE_TO_SQUARE_64(file, RANK_7));
	}

	//board->piece_list
	board->piece_list120[wR][0] = A1;
	board->piece_list120[wN][0] = B1;
	board->piece_list120[wB][0] = C1;
	board->piece_list120[wQ][0] = D1;
	board->piece_list120[wK][0] = E1;
	board->piece_list120[wB][1] = F1;
	board->piece_list120[wN][1] = G1;
	board->piece_list120[wR][1] = H1;

	board->piece_list120[wP][0] = A2;
	board->piece_list120[wP][1] = B2;
	board->piece_list120[wP][2] = C2;
	board->piece_list120[wP][3] = D2;
	board->piece_list120[wP][4] = E2;
	board->piece_list120[wP][5] = F2;
	board->piece_list120[wP][6] = G2;
	board->piece_list120[wP][7] = H2;

	board->piece_list120[bP][0] = A7;
	board->piece_list120[bP][1] = B7;
	board->piece_list120[bP][2] = C7;
	board->piece_list120[bP][3] = D7;
	board->piece_list120[bP][4] = E7;
	board->piece_list120[bP][5] = F7;
	board->piece_list120[bP][6] = G7;
	board->piece_list120[bP][7] = H7;

	board->piece_list120[bR][0] = A8;
	board->piece_list120[bN][0] = B8;
	board->piece_list120[bB][0] = C8;
	board->piece_list120[bQ][0] = D8;
	board->piece_list120[bK][0] = E8;
	board->piece_list120[bB][1] = F8;
	board->piece_list120[bN][1] = G8;
	board->piece_list120[bR][1] = H8;

	//Piece-num list
	board->piece_num[wP] = 8;
	board->piece_num[wN] = 2;
	board->piece_num[wB] = 2;
	board->piece_num[wR] = 2;
	board->piece_num[wQ] = 1;
	board->piece_num[wK] = 1;

	board->piece_num[bP] = 8;
	board->piece_num[bN] = 2;
	board->piece_num[bB] = 2;
	board->piece_num[bR] = 2;
	board->piece_num[bQ] = 1;
	board->piece_num[bK] = 1;

	Compute_Hash(board);
}

//Sets a board to a fen position and fills all fields
void Parse_Fen(char *fen, BOARD_STRUCT *board)
{

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
}