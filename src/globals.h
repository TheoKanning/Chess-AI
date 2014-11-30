/* globals.h
* Theo Kanning
* Contains all global definitions of functions, macros, and data types
*/
#include "math.h"
#include <iostream>

/***** Global Macros *****/

#define DEBUG //Define debug mode for full assert function

#define PROGRAM_NAME "Chess-AI"
#define AUTHOR	"Theo Kanning"
#define VERSION_NO	0.2

#ifndef DEBUG
        #define ASSERT(x)
#else
#define ASSERT(x) \
               if (! (x)) \
               { \
                  std::cout << "ERROR!! Assert " << #x << " failed\n"; \
                  std::cout << " on line " << __LINE__  << "\n"; \
                  std::cout << " in file " << __FILE__ << "\n";  \
				  system("PAUSE"); \
			   }
#endif



#define MAX_PLY						1028 //Maximum depth for searching
#define MAX_SEARCH_DEPTH			64   //Max search depth, arbitrary

#define ON_BOARD_120(x)				((21 <= x && x <= 28) || (31 <= x && x <= 38) || (41 <= x && x <= 48) || (51 <= x && x <= 58) || \
									 (61 <= x && x <= 68) || (71 <= x && x <= 78) || (81 <= x && x <= 88) || (91 <= x && x <= 98))

#define SQUARE_120_TO_64(x)			((x%10 -1 + 8*((x-21)/10)))
#define SQUARE_64_TO_120(x)			((x + 21 + 2*((x/8))))

#define RANK_FILE_TO_SQUARE_64(r,f)	(f+8*r)
#define GET_RANK_64(x)				(x>>3)
#define GET_FILE_64(x)				(x&7)

#define SET_BIT(x,y)				(x |= (1i64<<y))
#define CLR_BIT(x,y)				(x &= ~(1i64<<y))
#define GET_BIT(x,y)				(x & (1i64<<y))

/***** Global structures and typedefs *****/

typedef unsigned long long U64; //64 bit integer


enum PIECE_NAME_ENUM
{
	EMPTY,wP,wN,wB,wR,wQ,wK,bP,bN,bB,bR,bQ,bK
};

enum COLOR_ENUM
{
	WHITE,BLACK,BOTH //BOTH is used for bitboards
};

enum SPACE_ENUM //Space indices using 10x12 board
{
	A1 = 21, B1, C1, D1, E1, F1, G1, H1,
	A2 = 31, B2, C2, D2, E2, F2, G2, H2,
	A3 = 41, B3, C3, D3, E3, F3, G3, H3,
	A4 = 51, B4, C4, D4, E4, F4, G4, H4,
	A5 = 61, B5, C5, D5, E5, F5, G5, H5,
	A6 = 71, B6, C6, D6, E6, F6, G6, H6,
	A7 = 81, B7, C7, D7, E7, F7, G7, H7,
	A8 = 91, B8, C8, D8, E8, F8, G8, H8, OFF_BOARD,NO_SQUARE
};

enum RANK_ENUM
{
	RANK_1,RANK_2,RANK_3,RANK_4,RANK_5,RANK_6,RANK_7,RANK_8,NO_RANK
};

enum FILE_ENUM
{
	FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, NO_FILE
};

enum CASTLE_ENUM
{
	WK_CASTLE = 1, WQ_CASTLE = 2, BK_CASTLE = 4, BQ_CASTLE = 8
};

typedef struct
{
	int move; //32 bit move stores all necessary data 
	int score;
}MOVE_STRUCT;

typedef struct
{
	int board_array120[120];
	int board_array64[64]; 

	int ep; //120 index
	int side;
	int castle_rights;

	int hply; //total moves taken so far
	int move_counter; //100 move counter

	int piece_num[13]; //Stores the number of each type of piece
	int piece_list120[13][10]; //[Piece][instance] contains index of pieces on board, empty cells are 0, which is off the 120 board

	U64 pawn_bitboards[3]; //One for white, black, and both

	U64 hash_key;

} BOARD_STRUCT;


/***** Global Functions *****/
//board
extern void Init_Board(BOARD_STRUCT *board);
extern void Update_Piece_Lists(BOARD_STRUCT *board);
extern void Update_Bitboards(BOARD_STRUCT *board);
extern void Parse_Fen(char *fen, BOARD_STRUCT *board);
extern void Print_Board(BOARD_STRUCT *board);
extern void Print_Bitboards(BOARD_STRUCT *board);

//hashkeys
extern void Init_Hashkeys(void);
extern void Compute_Hash(BOARD_STRUCT *board);