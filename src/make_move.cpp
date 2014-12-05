/*makemove.c
* Contains functions for making and taking moves from the board, and updating all list and array data
* Theo Kanning 11/30/14 
*/
#include "globals.h"

using namespace std;

//Hashkey data
extern U64 piece_keys[64][12];//[square][piece]
extern U64 side_keys[2];
extern U64 ep_keys[101]; //NO_SQUARE = 100;
extern U64 castle_keys[16];


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
	int from120, to120, from64, to64, piece, capture, side;
	int move_num = move->move;

	U64 hash = board->hash_key;

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


	/***** Quiet Moves and Captures *****/
	if (IS_NOT_SPECIAL(move_num))
	{
		/***** board_array120[120] *****/
		//Remove from square
		ASSERT(board->board_array120[from120] == piece);
		board->board_array120[from120] = EMPTY;
		
		
		//Add to square
		ASSERT(board->board_array120[to120] == capture);
		board->board_array120[to120] = piece;

		/***** board_array64[64] *****/
		//Remove from square
		board->board_array64[from64] = EMPTY;
		//Add to square
		board->board_array64[to64] = piece;

		//Update hashkey
		HASH_OUT(hash, piece_keys[piece][from64]); //From square
		HASH_IN(hash, piece_keys[EMPTY][from64]);

		HASH_OUT(hash, piece_keys[capture][to64]); //From square
		HASH_IN(hash, piece_keys[piece][to64]);

		/***** En Passant Square *****/
		HASH_OUT(hash, ep_keys[board->ep]); //Remove to add later
		board->ep = NO_SQUARE;
		if (piece == wP)
		{
			if (from120 + 20 == to120) board->ep = from120 + 10; //If double push
		}
		else if (piece == bP)
		{
			if (from120 - 20 == to120) board->ep = from120 - 10; //If double push
		}
		HASH_IN(hash, ep_keys[board->ep]); //Re add ep key


		/***** Piece Lists *****/
		Remove_From_Piecelists(piece, from120, board);
		if (capture != EMPTY) Remove_From_Piecelists(capture, to120, board);
		Add_To_Piecelists(piece, to120, board);
		//These functions automatically update material score
		
		/***** Pawn Bitboards *****/
		if (piece == wP)
		{
			CLR_BIT(board->pawn_bitboards[WHITE], from120);
			CLR_BIT(board->pawn_bitboards[BOTH], from120);
			SET_BIT(board->pawn_bitboards[WHITE], to120);
			SET_BIT(board->pawn_bitboards[BOTH], to120);
		}
		else if (piece == bP)
		{
			CLR_BIT(board->pawn_bitboards[BLACK], from120);
			CLR_BIT(board->pawn_bitboards[BOTH], from120);
			SET_BIT(board->pawn_bitboards[BLACK], to120);
			SET_BIT(board->pawn_bitboards[BOTH], to120);
		}

		if (capture == wP)
		{
			CLR_BIT(board->pawn_bitboards[WHITE], to120);
			CLR_BIT(board->pawn_bitboards[BOTH], to120);
		}
		else if (capture == bP)
		{
			CLR_BIT(board->pawn_bitboards[BLACK], to120);
			CLR_BIT(board->pawn_bitboards[BOTH], to120);
		}
	}
	else if (IS_EP_CAPTURE(move_num))
	{

	}
	else if (IS_KING_CASTLE(move_num))
	{

	}
	else if (IS_QUEEN_CASTLE(move_num))
	{

	}
	else if (IS_QUEEN_PROMOTION(move_num))
	{

	}
	else if (IS_ROOK_PROMOTION(move_num))
	{

	}
	else if (IS_BISHOP_PROMOTION(move_num))
	{

	}
	else if (IS_KNIGHT_PROMOTION(move_num))
	{

	}
	else
	{
		ASSERT(1 == 0); //This should never be called
	}

	/***** Updates regardless of move type *****/

	//side (Incremented at end of function)
	board->side ^= 1; //Toggle

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
			board->castle_rights &= ~WK_CASTLE;
			HASH_OUT(hash, castle_keys[WK_CASTLE]);
		}
	}
	if (board->castle_rights & WQ_CASTLE)
	{
		if (piece == wK || board->board_array120[A1] != wR)
		{
			board->castle_rights &= ~WQ_CASTLE;
			HASH_OUT(hash, castle_keys[WQ_CASTLE]);
		}
	}
	if (board->castle_rights & BK_CASTLE)
	{
		if (piece == bK || board->board_array120[H8] != bR)
		{
			board->castle_rights &= ~BK_CASTLE;
			HASH_OUT(hash, castle_keys[BK_CASTLE]);
		}
	}
	if (board->castle_rights & BQ_CASTLE)
	{
		if (piece == bK || board->board_array120[A8] != bR)
		{
			board->castle_rights &= ~BQ_CASTLE;
			HASH_OUT(hash, castle_keys[BQ_CASTLE]);
		}
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