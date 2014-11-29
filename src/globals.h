/* globals.h
* Theo Kanning
* Contains all global definitions of functions, macros, and data types
*/

#define DEBUG //Define debug mode for full assert function

#define PROGRAM_NAME "Chess-AI"
#define AUTHOR	"Theo Kanning"
#define VERSION_NO	0.1

#ifndef DEBUG
        #define ASSERT(x)
#else
#define ASSERT(x) \
               if (! (x)) \
               { \
                  cout << "ERROR!! Assert " << #x << " failed\n"; \
                  cout << " on line " << __LINE__  << "\n"; \
                  cout << " in file " << __FILE__ << "\n";  \
				  system("PAUSE"); \
			   }
#endif

typedef unsigned long long U64; //64 bit integer

#define MAX_PLY						1028 //Maximum depth for searching
#define MAX_SEARCH_DEPTH			64   //Max search depth, arbitrary

enum PIECE_NAME_ENUM
{
	EMPTY,wP,wN,wB,wR,wQ,Wk,bP,bN,bB,bQ,bK
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
	A8 = 91, B8, C8, D8, E8, F8, G8, H8, OFF_BOARD
};

typedef struct
{
	int board_array[10][12];

	int ep; 
	int side;
	int castle_rights;

	int hply; //total moves taken so far

	int piece_num[12]; //Stores the number of each type of piece
	int piece_list[12][10]; //[Piece][instance] contains index of pieces on board, empty pieces are OFF_BOARD

	U64 pawn_bitboards[3]; //One for white, black, and both

	U64 hash_key;





} BOARD_STRUCT;