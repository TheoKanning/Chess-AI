/* movemacros.h
* Contains macros for making moves that would otherwise clutter globals.h
* Theo Kanning 12/2/14
*/



//Shifts to corresponding fields
#define fromShift				0
#define toShift					7
#define pieceShift				14
#define	captureShift			18
#define specialShift			22

//Special flag values
#define NOT_SPECIAL				0
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
#define IS_NOT_SPECIAL(x)		(GET_SPECIAL(x) == NOT_SPECIAL)
#define IS_EP_CAPTURE(x)		(GET_SPECIAL(x) == EP_CAPTURE)
#define IS_KING_CASTLE(x)		(GET_SPECIAL(x) == KING_CASTLE)
#define IS_QUEEN_CASTLE(x)		(GET_SPECIAL(x) == QUEEN_CASTLE)
#define IS_PROMOTION(x)			(GET_SPECIAL(x) >= QUEEN_PROMOTE)
#define IS_QUEEN_PROMOTION(x)	(GET_SPECIAL(x) == QUEEN_PROMOTE)
#define IS_ROOK_PROMOTION(x)	(GET_SPECIAL(x) == ROOK_PROMOTE)
#define IS_BISHOP_PROMOTION(x)	(GET_SPECIAL(x) == BISHOP_PROMOTE)
#define IS_KNIGHT_PROMOTION(x)	(GET_SPECIAL(x) == KNIGHT_PROMOTE)

//Set macros
#define SET_FROM_SQ(x,sq)		CLEAR_FROM_SQ(x); x |= (sq << fromShift); //Clear, then or with shifted data
#define SET_TO_SQ(x,sq)			CLEAR_TO_SQ(x); x |= (sq << toShift);
#define SET_PIECE(x,p)			CLEAR_PIECE(x); x |= (p << pieceShift);
#define SET_CAPTURE(x,p)		CLEAR_CAPTURE(x); x |= (p << captureShift);
#define SET_SPECIAL(x,s)		CLEAR_SPECIAL(x); x |= (s << specialShift);


//Clear macros
#define CLEAR_FROM_SQ(x)		((x &= ~(fromMask << fromShift))) //And with bitMask complements
#define CLEAR_TO_SQ(x)			((x &= ~(toMask << toShift)))
#define CLEAR_PIECE(x)			((x &= ~(pieceMask << pieceShift)))
#define CLEAR_CAPTURE(x)		((x &= ~(captureMask << captureShift)))
#define CLEAR_SPECIAL(x)		((x &= ~(specialMask << specialShift)))

//Moves scores
#define PV_SCORE				10000
#define QUEEN_PROMOTE_SCORE		1000
#define KNIGHT_PROMOTE_SCORE	500
#define CAPTURE_SCORE			1000	//Number to add to make capture scores higher or lower than other heuristics
#define GET_MMVLVA_SCORE(x,y)	(CAPTURE_SCORE + MMV_SCORES[x] + LVA_SCORES[y])

const int MMV_SCORES[13] = { 0, 10, 20, 20, 30, 40, 50, 10, 20, 20, 30, 40, 50 }; //MVV Scores are multiples of ten
const int LVA_SCORES[13] = { 0, 5, 4, 4, 3, 2, 1, 5, 4, 4, 3, 2, 1 }; //LVA Scores are less important

//Arrays for move generation
#define NUM_KNIGHT_MOVES		8
#define NUM_BISHOP_MOVES		4
#define NUM_ROOK_MOVES			4
#define NUM_QUEEN_MOVES			8
#define NUM_KING_MOVES			8

const int KNIGHT_MOVES[8] = { -21, -19, -12, -8, 8, 12, 19, 21 };
const int BISHOP_MOVES[4] = { -11, -9, 11, 9 };
const int ROOK_MOVES[4] = { -10, -1, 1, 10 };
const int QUEEN_MOVES[8] = { -11, -10, -9, -1, 1, 9, 10, 11 };
const int KING_MOVES[8] = { -11, -10, -9, -1, 1, 9, 10, 11 };

#define HASH_IN(x,y)			(x^=y)//Keep separate macros for readability later
#define HASH_OUT(x,y)			(x^=y)