/*makemove.c
* Contains functions for making and taking moves from the board, and updating all list and array data
* Theo Kanning 11/30/14 
*/
#include "globals.h"

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

//Shifts to corresponding fields
#define fromShift				0
#define toShift					7
#define pieceShift				14
#define	captureShift			18
#define specialShift			22

//Special flag values
#define QUIET					0
#define EP_CAPTURE				1
#define KING_CASTLE				2
#define QUEEN_CASTLE			3
#define QUEEN_PROMOTE			4
#define ROOK_PROMOTE			5
#define BISHOP_PROMOTE			6
#define KNIGHT_PROMOTE			7

//Masks for reading and clearing bits
#define fromMask				0x7f
#define toMask					0x7f
#define pieceMask				0x0f
#define captureMask				0x0f
#define specialMask				0x07

//Read macros
#define GET_FROM_SQ(x)			((x >> fromShift) & fromMask) //Shift down and then read with mask
#define GET_TO_SQ(x)			((x >> toShift) & toMask)
#define GET_PIECE(x)			((x >> pieceShift) & pieceMask)
#define GET_CAPTURE(x)			((x >> captureShift) & captureMask)
#define GET_SPECIAL(x)			((x >> specialShift) & specialMask)

//Set macros
#define SET_FROM_SQ(x,sq)		CLEAR_FROM_SQ(x); x |= (sq << fromShift); //Clear, then or with shifted data
#define SET_TO_SQ(x,sq)			CLEAR_TO_SQ(x); x |= (sq << toShift);
#define SET_PIECE(x,p)			CLEAR_PIECE(x); x |= (p << pieceShift);
#define SET_CAPTURE(x,p)		CLEAR_CAPTURE(x); x |= (p << captureShift);
#define SET_SPECIAL(x,s)		CLEAR_SPECIAL(x); x |= (s << specialShift);


//Clear macros
#define CLEAR_FROM_SQ(x)		((x &= (~fromMask << fromShift))) //And with bitMask complements
#define CLEAR_TO_SQ(x)			((x &= (~toMask << toShift)))
#define CLEAR_PIECE(x)			((x &= (~pieceMask << pieceShift)))
#define CLEAR_CAPTURE(x)		((x &= (~captureMask << captureShift)))
#define CLEAR_SPECIAL(x)		((x &= (~specialMask << specialShift)))

//Creates integer from move data and stores in movelist
void Add_Move(MOVE_LIST_STRUCT *move_list, int from, int to, int piece, int capture, int special, int score)
{
	int temp = 0;
	
	//Check all fields within bounds
	ASSERT(ON_BOARD_120(from));
	ASSERT(ON_BOARD_120(to));
	ASSERT((piece >= wP) && (piece <= bK)); //Piece is not empty
	ASSERT((piece >= EMPTY) && (piece <= bK)); //Piece can be empty
	ASSERT((special >= QUIET) && (special <= KNIGHT_PROMOTE));

	//Set all fields
	SET_FROM_SQ(temp, from);
	SET_TO_SQ(temp, to);
	SET_PIECE(temp, piece);
	SET_CAPTURE(temp, capture);
	SET_SPECIAL(temp, special);

	//Store in list and update counter
	move_list->list[move_list->num].move = temp; 
	move_list->list[move_list->num].score = score;
	move_list->num++; //Increment counter
}

//Prints all moves in standard chess format
void Print_Movelist(MOVE_LIST_STRUCT *movelist)
{
	int index, rank, file;

	for (index = 0; index < movelist->num; index++) //Iterate through all moves stored
	{

	}
}