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

static void Move_Piece(int from, int to, BOARD_STRUCT *board);
static void Remove_Piece(int square, BOARD_STRUCT *board);
static void Add_Piece(int piece, int square, BOARD_STRUCT *board);

/* Move data structure
32 bit integer
bits [0:6] from square index 64
bits [7:13] to square index 64
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

int Make_Move(int move_num, BOARD_STRUCT *board)
{
	int from, to, piece, capture, side, castle_temp, ep_capture;

	if (move_num == 0) return 0;

	from = GET_FROM_SQ(move_num);
	to = GET_TO_SQ(move_num);

	piece = GET_PIECE(move_num);
	capture = GET_CAPTURE(move_num);
	side = board->side;

	ASSERT((!side && IS_WHITE_PIECE(piece)) || (side && IS_BLACK_PIECE(piece)));
	ASSERT(ON_BOARD(from));
	ASSERT(ON_BOARD(to));
	ASSERT((EMPTY < piece) && (piece <= bK));
	ASSERT((EMPTY <= capture) && (capture <= bK));

	ASSERT(board->board_array[from] == piece);
	ASSERT(board->board_array[to] == capture || IS_EP_CAPTURE(move_num));

	/***** Update Undo Move Structure *****/
	board->undo_list.list[board->undo_list.num].move_num = move_num;
	board->undo_list.list[board->undo_list.num].move_counter = board->move_counter;
	board->undo_list.list[board->undo_list.num].ep = board->ep;
	board->undo_list.list[board->undo_list.num].castle = board->castle_rights;
	board->undo_list.list[board->undo_list.num].hash = board->hash_key;
	board->undo_list.num++;

	/***** En Passant Square *****/
	HASH_OUT(board->hash_key, ep_keys[board->ep]); //Remove to add later
	board->ep = NO_SQUARE;

	/***** Quiet Moves and Captures *****/
	if (IS_NOT_SPECIAL(move_num))
	{
		Move_Piece(from, to, board);


		if (piece == wP)
		{
			if (from + 16 == to) board->ep = from + 8; //If double push
		}
		else if (piece == bP)
		{
			if (from - 16 == to) board->ep = from - 8; //If double push
		}
		//HASH_IN(board->hash_key, ep_keys[board->ep]); //Re add ep key
	}
	else if (IS_EP_CAPTURE(move_num))
	{
		if (side == WHITE)
		{
			ep_capture = to - 8; //Square with captured pawn on it
			capture = bP;
		}
		else
		{
			ep_capture = to + 8;
			capture = wP;
		}

		ASSERT(board->board_array[from] == piece);
		ASSERT(board->board_array[to] == EMPTY);
		ASSERT(board->board_array[ep_capture] == capture);

		Move_Piece(from, to, board); //Move active pawn
		Remove_Piece(ep_capture, board); //Remove captured pawn

		//HASH_OUT(board->hash_key, ep_keys[board->ep]);
		//board->ep = NO_SQUARE; //Reset ep square
		//HASH_IN(board->hash_key, ep_keys[board->ep]);
	}
	else if (IS_KING_CASTLE(move_num))
	{
		if (piece == wK)
		{
			ASSERT(from == E1);
			ASSERT(to == G1);
			ASSERT(board->board_array[to] == EMPTY);
			ASSERT(board->castle_rights & WK_CASTLE);

			//Move king
			Move_Piece(E1, G1, board);

			//Move rook
			Move_Piece(H1, F1, board);

			//Update castling rights and hash
			HASH_OUT(board->hash_key, castle_keys[board->castle_rights]); //Remove old hashkey
			board->castle_rights &= ~WK_CASTLE; //Remove kingside rights
			board->castle_rights &= ~WQ_CASTLE; //Remove queenside rights
			HASH_IN(board->hash_key, castle_keys[board->castle_rights]); //Add new hashkey

		}
		else if (piece == bK)
		{
			ASSERT(from == E8);
			ASSERT(to == G8);
			ASSERT(board->board_array[to] == EMPTY);
			ASSERT(board->castle_rights & BK_CASTLE);

			//Move king
			Move_Piece(E8, G8, board);

			//Move rook
			Move_Piece(H8, F8, board);

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
			ASSERT(from == E1);
			ASSERT(to == C1);
			ASSERT(board->board_array[to] == EMPTY);
			ASSERT(board->castle_rights & WQ_CASTLE);

			//Move king
			Move_Piece(E1, C1, board);

			//Move rook
			Move_Piece(A1, D1, board);

			//Update castling rights and hash
			HASH_OUT(board->hash_key, castle_keys[board->castle_rights]); //Remove old hashkey
			board->castle_rights &= ~WK_CASTLE; //Remove kingside rights
			board->castle_rights &= ~WQ_CASTLE; //Remove queenside rights
			HASH_IN(board->hash_key, castle_keys[board->castle_rights]); //Add new hashkey

		}
		else if (piece == bK)
		{
			ASSERT(from == E8);
			ASSERT(to == C8);
			ASSERT(board->board_array[to] == EMPTY);
			ASSERT(board->castle_rights & BQ_CASTLE);

			//Move king
			Move_Piece(E8, C8, board);

			//Move rook
			Move_Piece(A8, D8, board);

			//Update castling rights and hash
			HASH_OUT(board->hash_key, castle_keys[board->castle_rights]); //Remove old hashkey
			board->castle_rights &= ~BK_CASTLE; //Remove kingside rights
			board->castle_rights &= ~BQ_CASTLE; //Remove queenside rights
			HASH_IN(board->hash_key, castle_keys[board->castle_rights]); //Add new hashkey
		}
	}
	else if (IS_QUEEN_PROMOTION(move_num) || IS_ROOK_PROMOTION(move_num) || IS_BISHOP_PROMOTION(move_num) || IS_KNIGHT_PROMOTION(move_num))
	{
		if (side == WHITE)
		{
			ASSERT(GET_RANK(from) == RANK_7);
			ASSERT(GET_RANK(to) == RANK_8);
			ASSERT(piece == wP);

			//Remove pawn
			Remove_Piece(from, board);

			//Remove captured piece, if any
			if (capture != EMPTY) Remove_Piece(to, board);

			//Add piece
			if (IS_QUEEN_PROMOTION(move_num)) Add_Piece(wQ, to, board);
			if (IS_ROOK_PROMOTION(move_num)) Add_Piece(wR, to, board);
			if (IS_BISHOP_PROMOTION(move_num)) Add_Piece(wB, to, board);
			if (IS_KNIGHT_PROMOTION(move_num)) Add_Piece(wN, to, board);
		}
		else
		{
			ASSERT(GET_RANK(from) == RANK_2);
			ASSERT(GET_RANK(to) == RANK_1);
			ASSERT(piece == bP);

			//Remove pawn
			Remove_Piece(from, board);

			//Remove captured piece, if any
			if (capture != EMPTY) Remove_Piece(to, board);

			//Add promoted piece
			if (IS_QUEEN_PROMOTION(move_num)) Add_Piece(bQ, to, board);
			if (IS_ROOK_PROMOTION(move_num)) Add_Piece(bR, to, board);
			if (IS_BISHOP_PROMOTION(move_num)) Add_Piece(bB, to, board);
			if (IS_KNIGHT_PROMOTION(move_num)) Add_Piece(bN, to, board);
		}
	}
	else
	{
		ASSERT(1 == 0); //This should never be called
	}

	/***** Updates regardless of move type *****/

	//side (Incremented at end of function)
	HASH_OUT(board->hash_key, side_keys[board->side]);
	board->side ^= 1; //Toggle
	HASH_IN(board->hash_key, side_keys[board->side]);

	//Re add ep key
	HASH_IN(board->hash_key, ep_keys[board->ep]);

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
	if (board->castle_rights & WK_CASTLE)
	{
		if (piece == wK || board->board_array[H1] != wR)
		{
			castle_temp = board->castle_rights;
			HASH_OUT(board->hash_key, castle_keys[castle_temp]);
			board->castle_rights &= ~WK_CASTLE;
			HASH_IN(board->hash_key, castle_keys[board->castle_rights]);
		}
	}
	if (board->castle_rights & WQ_CASTLE)
	{
		if (piece == wK || board->board_array[A1] != wR)
		{
			castle_temp = board->castle_rights;
			HASH_OUT(board->hash_key, castle_keys[castle_temp]);
			board->castle_rights &= ~WQ_CASTLE;
			HASH_IN(board->hash_key, castle_keys[board->castle_rights]);
		}
	}
	if (board->castle_rights & BK_CASTLE)
	{
		if (piece == bK || board->board_array[H8] != bR)
		{
			castle_temp = board->castle_rights;
			HASH_OUT(board->hash_key, castle_keys[castle_temp]);
			board->castle_rights &= ~BK_CASTLE;
			HASH_IN(board->hash_key, castle_keys[board->castle_rights]);
		}
	}
	if (board->castle_rights & BQ_CASTLE)
	{
		if (piece == bK || board->board_array[A8] != bR)
		{
			castle_temp = board->castle_rights;
			HASH_OUT(board->hash_key, castle_keys[castle_temp]);
			board->castle_rights &= ~BQ_CASTLE;
			HASH_IN(board->hash_key, castle_keys[board->castle_rights]);
		}
	}

	/***** Check test *****/
	//If king under attack
	if (In_Check(side, board))
	{
		Take_Move(board);
		return 0;
	}


#ifdef DEBUG
	Check_Board(board);
#endif

	return 1;
}

//Undoes most recent move stored in board history
void Take_Move(BOARD_STRUCT *board)
{
	int from, to, piece, capture, side, ep_capture;

	//Read data from undo struct
	int move_num = board->undo_list.list[board->undo_list.num - 1].move_num;
	int move_counter = board->undo_list.list[board->undo_list.num - 1].move_counter;
	int ep = board->undo_list.list[board->undo_list.num - 1].ep;
	int castle = board->undo_list.list[board->undo_list.num - 1].castle;
	U64 hash = board->undo_list.list[board->undo_list.num - 1].hash;



	from = GET_FROM_SQ(move_num);
	to = GET_TO_SQ(move_num);

	piece = GET_PIECE(move_num);
	capture = GET_CAPTURE(move_num);

	//Toggle side first, side now represents the side of the move that is being undone
	HASH_OUT(board->hash_key, side_keys[board->side]);
	board->side ^= 1; //Toggle
	HASH_IN(board->hash_key, side_keys[board->side]);
	side = board->side; //Side of move being undones

	ASSERT(ON_BOARD(from));
	ASSERT(ON_BOARD(to));
	ASSERT((EMPTY < piece) && (piece <= bK));
	ASSERT((EMPTY <= capture) && (capture <= bK));


	/***** Quiet Moves and Captures *****/
	if (IS_NOT_SPECIAL(move_num))
	{
		Move_Piece(to, from, board);
		if (capture != EMPTY) Add_Piece(capture, to, board);
	}

	else if (IS_EP_CAPTURE(move_num))
	{

		if (side == WHITE)
		{
			ep_capture = to - 8; //Square with captured pawn on it
			capture = bP;
		}
		else
		{
			ep_capture = to + 8;
			capture = wP;
		}

		Move_Piece(to, from, board); //Move active pawn
		Add_Piece(capture, ep_capture, board); //Remove captured pawn

	}
	else if (IS_KING_CASTLE(move_num))
	{
		if (piece == wK)
		{
			//Move king
			Move_Piece(G1, E1, board);

			//Move rook
			Move_Piece(F1, H1, board);
		}
		else if (piece == bK)
		{
			//Move king
			Move_Piece(G8, E8, board);

			//Move rook
			Move_Piece(F8, H8, board);
		}
	}
	else if (IS_QUEEN_CASTLE(move_num))
	{
		if (piece == wK)
		{
			//Move king
			Move_Piece(C1, E1, board);

			//Move rook
			Move_Piece(D1, A1, board);
		}
		else if (piece == bK)
		{
			//Move king
			Move_Piece(C8, E8, board);

			//Move rook
			Move_Piece(D8, A8, board);
		}
	}
	else if (IS_QUEEN_PROMOTION(move_num) || IS_ROOK_PROMOTION(move_num) || IS_BISHOP_PROMOTION(move_num) || IS_KNIGHT_PROMOTION(move_num))
	{
		//Remove promoted piece
		Remove_Piece(to, board);

		//Add pawn
		if (side == WHITE)  Add_Piece(wP, from, board);
		if (side == BLACK)  Add_Piece(bP, from, board);

		//Add captured piece, if any
		if (capture != EMPTY) Add_Piece(capture, to, board);
	}
	else
	{
		ASSERT(1 == 0); //This should never be called
	}

	/***** Updates regardless of move type *****/

	//side changed at beginning of function

	//hply; //total moves taken so far
	board->hply--;

	//move_counter; //100 move counter
	board->move_counter = move_counter;

	//Ep square
	HASH_OUT(board->hash_key, ep_keys[board->ep]);
	board->ep = ep; //Enter value from undo
	HASH_IN(board->hash_key, ep_keys[board->ep]);

	//castle_rights;
	HASH_OUT(board->hash_key, castle_keys[board->castle_rights]);
	board->castle_rights = castle;
	HASH_IN(board->hash_key, castle_keys[board->castle_rights]);

	ASSERT(board->hash_key == hash);

	//Remove undo structure from list
	board->undo_list.list[board->undo_list.num - 1].move_num = 0;
	board->undo_list.list[board->undo_list.num - 1].move_counter = 0;
	board->undo_list.list[board->undo_list.num - 1].ep = 0;
	board->undo_list.list[board->undo_list.num - 1].castle = 0;
	board->undo_list.list[board->undo_list.num - 1].hash = 0;
	board->undo_list.num--;


#ifdef DEBUG
	Check_Board(board);
#endif
}

//Switches side without moving a piece
int Make_Null_Move(BOARD_STRUCT *board)
{	

	/***** Update Undo Move Structure *****/
	board->undo_list.list[board->undo_list.num].move_num = 0;
	board->undo_list.list[board->undo_list.num].move_counter = board->move_counter;
	board->undo_list.list[board->undo_list.num].ep = board->ep;
	board->undo_list.list[board->undo_list.num].castle = board->castle_rights;
	board->undo_list.list[board->undo_list.num].hash = board->hash_key;
	board->undo_list.num++;

	/***** En Passant Square *****/
	HASH_OUT(board->hash_key, ep_keys[board->ep]); 
	board->ep = NO_SQUARE;
	HASH_IN(board->hash_key, ep_keys[board->ep]);

	//side 
	HASH_OUT(board->hash_key,side_keys[board->side]);
	board->side ^= 1; //Toggle
	HASH_IN(board->hash_key, side_keys[board->side]);

	//hply; //total moves taken so far
	board->hply++;

	board->move_counter++;	

#ifdef DEBUG
	Check_Board(board);
#endif

	return 1;
}

//Undoes null move
void Take_Null_Move(BOARD_STRUCT *board)
{
	//Read data from undo struct
	ASSERT(board->undo_list.list[board->undo_list.num - 1].move_num == 0);

	int move_counter = board->undo_list.list[board->undo_list.num-1].move_counter;
	int ep = board->undo_list.list[board->undo_list.num-1].ep;
	int castle = board->undo_list.list[board->undo_list.num-1].castle;
	U64 hash = board->undo_list.list[board->undo_list.num-1].hash;


	//Side
	HASH_OUT(board->hash_key, side_keys[board->side]);
	board->side ^= 1; //Toggle
	HASH_IN(board->hash_key, side_keys[board->side]);
	
	//hply; //total moves taken so far
	board->hply--;

	//move_counter; //100 move counter
	board->move_counter = move_counter;

	//Ep square
	HASH_OUT(board->hash_key, ep_keys[board->ep]);
	board->ep = ep; //Enter value from undo
	HASH_IN(board->hash_key, ep_keys[board->ep]);

	//castle_rights;
	HASH_OUT(board->hash_key, castle_keys[board->castle_rights]);
	board->castle_rights = castle;
	HASH_IN(board->hash_key, castle_keys[board->castle_rights]);
	
	ASSERT(board->hash_key == hash);

	//Remove undo structure from list
	board->undo_list.list[board->undo_list.num - 1].move_num = 0;
	board->undo_list.list[board->undo_list.num - 1].move_counter = 0;
	board->undo_list.list[board->undo_list.num - 1].ep = 0;
	board->undo_list.list[board->undo_list.num - 1].castle = 0;
	board->undo_list.list[board->undo_list.num - 1].hash = 0;
	board->undo_list.num--;


#ifdef DEBUG
	Check_Board(board);
#endif
}


//Moves one piece, updating all lists, bitboards, and hashes
void Move_Piece(int from, int to, BOARD_STRUCT *board)
{
	int piece, capture;

	ASSERT(board->board_array[from] != EMPTY);
	ASSERT(ON_BOARD(from));
	ASSERT(ON_BOARD(to));

	piece = board->board_array[from];
	capture = board->board_array[to];

	//Remove captured piece from piecelists, remove before updating arrays
	if (capture != EMPTY) Remove_Piece(to, board);

	//Clear from square
	board->board_array[from] = EMPTY;
	Remove_From_Piecelists(piece, from, board);
	HASH_OUT(board->hash_key, piece_keys[piece][from]);
	HASH_IN(board->hash_key, piece_keys[EMPTY][from]);

	//Add to new square
	board->board_array[to] = piece;
	Add_To_Piecelists(piece, to, board);

	//Update hash key
	HASH_OUT(board->hash_key, piece_keys[EMPTY][to]); //Capture already removed earlier
	HASH_IN(board->hash_key, piece_keys[piece][to]);

	//Update pawn_hash_key
	if (IS_PAWN(piece) || IS_KING(piece))
	{
		HASH_OUT(board->pawn_hash_key, piece_keys[piece][from]);
		HASH_IN(board->pawn_hash_key, piece_keys[piece][to]);
	}
}

//Removes a piece from the board, updating all lists, bitboards, and hashes
void Remove_Piece(int square, BOARD_STRUCT *board)
{
	int piece;

	ASSERT(board->board_array[square] != EMPTY);

	piece = board->board_array[square];

	//Clear from square
	board->board_array[square] = EMPTY;
	Remove_From_Piecelists(piece, square, board);
	HASH_OUT(board->hash_key, piece_keys[piece][square]);
	HASH_IN(board->hash_key, piece_keys[EMPTY][square]);

	//Update pawn_hash_key
	if (IS_PAWN(piece) || IS_KING(piece))
	{
		HASH_OUT(board->pawn_hash_key, piece_keys[piece][square]);
	}
}
//Adds piece to board after promotion
void Add_Piece(int piece, int square, BOARD_STRUCT *board)
{

	ASSERT(board->board_array[square] == EMPTY);
	ASSERT(ON_BOARD(square));

	//Clear from square
	board->board_array[square] = piece;
	Add_To_Piecelists(piece, square, board);
	HASH_OUT(board->hash_key, piece_keys[EMPTY][square]);
	HASH_IN(board->hash_key, piece_keys[piece][square]);

	//Update pawn_hash_key
	if (IS_PAWN(piece) || IS_KING(piece))
	{
		HASH_IN(board->pawn_hash_key, piece_keys[piece][square]);
	}
}

//Returns 1 if the move generates check
int Is_Checking_Move(int move_num, BOARD_STRUCT *board)
{
	int in_check = 0;
	//Make move, see if in check, take move
	if(!Make_Move(move_num, board)) return 0;

	in_check = In_Check(board->side, board);

	Take_Move(board);

	return in_check;

}

//Prints individual move without score data
void Print_Move(MOVE_STRUCT *move)
{
	int move_num, piece, from, square, rank, file;

	move_num = move->move; //Store move in temp variable

	//Get moving piece
	piece = GET_PIECE(move_num);

	//Get rank and file of ending position
	from = GET_FROM_SQ(move_num);
	square = GET_TO_SQ(move_num);
	ASSERT(ON_BOARD(square));
	rank = GET_RANK(square);
	file = GET_FILE(square);

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
			cout << file_names[GET_FILE(from)];
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
}


//Prints a move in UCI format
char* UCI_Move_String(MOVE_STRUCT *move)
{
	static char MvStr[6];
	int move_num = move->move;

	int from_file = GET_FILE(GET_FROM_SQ(move_num));
	int from_rank = GET_RANK(GET_FROM_SQ(move_num));
	int to_file = GET_FILE(GET_TO_SQ(move_num));
	int to_rank = GET_RANK(GET_TO_SQ(move_num));

	
	//Get prmotion char, if any
	if (IS_PROMOTION(move_num)) {
		char pchar = 'q';
		if (IS_KNIGHT_PROMOTION(move_num)) {
			pchar = 'n';
		}
		else if (IS_ROOK_PROMOTION(move_num))  {
			pchar = 'r';
		}
		else if (IS_BISHOP_PROMOTION(move_num))  {
			pchar = 'b';
		}
		sprintf_s(MvStr, "%c%c%c%c%c", ('a' + from_file), ('1' + from_rank), ('a' + to_file), ('1' + to_rank), pchar);
	}
	else {
		sprintf_s(MvStr, "%c%c%c%c", ('a' + from_file), ('1' + from_rank), ('a' + to_file), ('1' + to_rank));
	}

	return MvStr;
}