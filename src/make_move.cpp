/*makemove.c
* Contains functions for making and taking moves from the board, and updating all list and array data
* Theo Kanning 11/30/14 
*/
#include "globals.h"

using namespace std;

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