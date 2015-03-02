/* globals.h
* Theo Kanning
* Contains all global definitions of functions, macros, and data types
*/
#include "math.h"
#include <iostream>
#include "move_macros.h"

/***** Global Macros *****/

//#define DEBUG //Define debug mode for full assert function

#define PROGRAM_NAME "Chess-AI"
#define AUTHOR	"Theo Kanning"
#define VERSION_NO	1.40

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

//Returns random 64 bit uint
#define RANDOM_U64()			(((U64)rand() + \
								((U64)rand() << 15) + \
								((U64)rand() << 30) + \
								((U64)rand() << 45) + \
							   (((U64)rand() & 0x0f) << 60)))

#define MAX_PLY						1028 //Maximum depth for searching
#define MAX_SEARCH_DEPTH			64   //Max search depth, arbitrary
#define MAX_MOVE_LIST_LENGTH		218 //Maximum moves in any position
#define INF							100000 //Large enough number to be infinite
#define INVALID						1000001 //Larger than infinity, always out of bounds
#define MATE_SCORE					15000
#define IS_MATE(x)					(abs(x) >= MATE_SCORE - MAX_SEARCH_DEPTH && abs(x) <= MATE_SCORE)
#define ADJUST_MATE_SCORE(score,ply)		((score < 0) ? -MATE_SCORE + ply : MATE_SCORE -ply)

#define START_FEN		"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define ON_BOARD_120(x)				(ON_BOARD_64(SQUARE_120_TO_64(x)))
#define ON_BOARD_64(x)				(x >= 0 && x < 64)

#define SQUARE_120_TO_64(x)			(square_120_to_64[x])
#define SQUARE_64_TO_120(x)			(square_64_to_120[x])

#define RANK_FILE_TO_SQUARE_64(r,f)	(f+8*r)
#define GET_RANK_64(x)				(x>>3)
#define GET_FILE_64(x)				(x&7)
#define GET_RANK_120(x)				GET_RANK_64(SQUARE_120_TO_64(x))
#define GET_FILE_120(x)				GET_FILE_64(SQUARE_120_TO_64(x))

#define SET_BIT(x,y)				(x |= (1i64<<y))
#define CLR_BIT(x,y)				(x &= ~(1i64<<y))
#define GET_BIT(x,y)				(x & (1i64<<y))

#define IS_WHITE_PIECE(x)			(is_white[x]) //((x > EMPTY) && (x <= wK)) 
#define IS_BLACK_PIECE(x)			(is_black[x]) //(x > wK) && (x <= bK))
#define COLOR(x)					(color[x])
#define IS_PAWN(x)					(is_pawn[x]) //((x == wP) || (x == bP))
#define IS_KNIGHT(x)				(is_knight[x]) //((x == wN) || (x == bN))
#define IS_BISHOP(x)				(is_bishop[x]) //((x == wB) || (x == bB))
#define IS_ROOK(x)					(is_rook[x]) //((x == wR) || (x == bR))
#define IS_QUEEN(x)					(is_queen[x]) //((x == wQ) || (x == bQ))
#define IS_KING(x)					(is_king[x]) //((x == wK) || (x == bK))

//Late move reduction macros
#define CAN_REDUCE(move)			(!IS_CAPTURE(move) && !IS_PROMOTION(move)) //Returns 1 if move is not a promotion or capture
#define REDUCTION_LIMIT				3 //Minimum depth to consider reductions
#define LATE_MOVE_NUM				5 //First move number to consider reducing, 6th move = 5
#define LATE_MOVE_REDUCTION			1 //Number of ply to shorten late move searches


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
	A1, B1, C1, D1, E1, F1, G1, H1,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A8, B8, C8, D8, E8, F8, G8, H8, OFF_BOARD,NO_SQUARE
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
	MOVE_STRUCT list[MAX_MOVE_LIST_LENGTH];
	int num;
}MOVE_LIST_STRUCT;

typedef struct
{
	int move_counter;
	int ep;
	int castle;
	int move_num;
	U64 hash; //Hash before move

}UNDO_STRUCT;

typedef struct
{
	UNDO_STRUCT list[MAX_PLY];
	int num;
}UNDO_LIST_STRUCT;

typedef struct
{
	MOVE_STRUCT list[MAX_SEARCH_DEPTH];
	int num;
	int in_pv_line;
}PV_LIST_STRUCT;

typedef struct
{
	int start_time;
	int stop_time;
	int time_set;
	int end_early; //Enable ending early if using normal clock
	int stopped;
	long nodes;

	int depth;
	int max_depth; //Max depth reached in normal search
	int null_available;

	int age; //Number of irreversible moves made
	long hash_probes;
	long hash_hits;

	int best_index[MAX_MOVE_LIST_LENGTH];
	int beta_cutoff_index[MAX_MOVE_LIST_LENGTH];

	int quit;
	
}SEARCH_INFO_STRUCT;

typedef struct
{
	U64 hash;
	int depth;
	int flag;
	int eval;
	int age; //number of irreversible moves made
	int move;
}HASH_ENTRY_STRUCT;

typedef struct
{
	int num_entries;
	HASH_ENTRY_STRUCT *table;
}HASH_TABLE_STRUCT;

typedef struct
{
	//int board_array120[120];
	int board_array64[64]; 

	int ep; //64 index
	int side;
	int castle_rights;

	int hply; //total moves taken so far
	int move_counter; //100 move counter

	int piece_num[13]; //Stores the number of each type of piece
	int piece_list120[13][10]; //[Piece][instance] contains index of pieces on board, empty cells are 0, which is off the 120 board

	int phase; //Calculated at beginning of eval function

	int big_material[2]; //Non-pawn material
	int pawn_material[2];

	int middle_piece_square_score;
	int end_piece_square_score;

	int eval_score; //Overall evaluation score

	U64 piece_bitboards[13]; //One for each piece type
	U64 side_bitboards[3]; //One for white, black, and both

	UNDO_LIST_STRUCT undo_list;

	int the_killers[MAX_SEARCH_DEPTH][2];

	int history[120][120];
	int history_max;

	U64 hash_key;
	U64 pawn_hash_key;
	int age; //number of irreversible moves made

} BOARD_STRUCT;


/***** Global Functions *****/
//attack
extern int Under_Attack(int sq, int side, BOARD_STRUCT *board);
extern int In_Check(int side, BOARD_STRUCT *board);
extern U64 between[64][64]; 
extern void Generate_Between_Squares(void);

//attack masks
extern const U64 knight_attack_masks[64];
extern const U64 king_attack_masks[64];
extern const U64 wpawn_attack_masks[64];
extern const U64 bpawn_attack_masks[64];
extern const U64 rook_attack_masks[64];
extern const U64 bishop_attack_masks[64];
extern void Generate_King_Knight_Attack_Masks(void);
extern void Generate_Pawn_Attack_Masks(void);
extern void Generate_Rook_Bishop_Attack_Masks(void);

//board
extern void Init_Board(BOARD_STRUCT *board);
extern void Update_Piece_Lists(BOARD_STRUCT *board);
extern void Update_Bitboards(BOARD_STRUCT *board);
extern void Add_To_Piecelists(int piece, int index64, BOARD_STRUCT *board);
extern void Remove_From_Piecelists(int piece, int index64, BOARD_STRUCT *board);
extern void Parse_Fen(char *fen, BOARD_STRUCT *board);
extern void Clear_Undo_List(BOARD_STRUCT *board);
extern int  Is_Threefold_Repetition(BOARD_STRUCT *board);
extern int Is_Repetition(BOARD_STRUCT *board);
extern int Is_Material_Draw(BOARD_STRUCT *board);
extern void Check_Board(BOARD_STRUCT *board);
extern void Print_Board(BOARD_STRUCT *board);
extern void Print_Bitboard(U64 bb);

//data
extern int use_SEE;
extern int use_aspiration_window;
extern int use_dual_hash;
extern int use_futility;
extern int use_late_move_reduction;
extern int square_120_to_64[120];
extern int square_64_to_120[64];
extern char* file_names;
extern char* rank_names;
extern char* piece_names;
extern int is_white[13];
extern int is_black[13];
extern int color[13];
extern int is_pawn[13];
extern int is_knight[13];
extern int is_bishop[13];
extern int is_rook[13];
extern int is_queen[13];
extern int is_king[13];
extern const U64 rank_masks[8];
extern const U64 file_masks[8];
extern int futility_margins[4];
extern int aspiration_windows[4];
extern int piece_values[13];
extern int passed_pawn_rank_bonus[8];
extern short middle_piece_square_tables[13][64];
extern short end_piece_square_tables[13][64];

//eval
extern int Evaluate_Board(BOARD_STRUCT *board);
extern int Get_Board_Piece_Square_Score(BOARD_STRUCT *board);
extern int Get_Piece_Square_Score(int index64, int piece, float phase);
extern int Get_Pawn_Eval_Score(BOARD_STRUCT *board);
extern int Get_King_Safety_Score(BOARD_STRUCT *board);
extern int Get_Pawn_And_King_Score(BOARD_STRUCT *board);
extern void Init_Pawn_Masks(void);

//hashkeys
extern void Init_Hashkeys(void);
extern void Clear_Hash_Table(void);
extern void Compute_Hash(BOARD_STRUCT *board);
extern void Add_Hash_Entry(HASH_ENTRY_STRUCT *hash_ptr, int ply, SEARCH_INFO_STRUCT *info);
extern void Remove_Hash_Entry(U64 hash);
extern int  Get_Hash_Entry(U64 hash, int alpha, int beta, int depth, int ply, int * hash_move);
extern void Add_Dual_Hash_Entry(HASH_ENTRY_STRUCT *hash_ptr, int ply, SEARCH_INFO_STRUCT *info);
extern int  Get_Dual_Hash_Entry(U64 hash, int alpha, int beta, int depth, int ply, int * hash_move);
extern void Fill_Hash_Entry(int age, int depth, int eval, int flag, U64 hash, int move, HASH_ENTRY_STRUCT *hash_ptr);

//history
extern void Add_History_Move(int move, BOARD_STRUCT *board);
extern void Find_History_Moves(MOVE_LIST_STRUCT *move_list, BOARD_STRUCT *board);
extern void Clear_History_Data(BOARD_STRUCT *board);

//input
extern int Get_Time_Ms(void);
extern void ReadInput(SEARCH_INFO_STRUCT *info);

//killers
extern void Find_Killer_Moves(MOVE_LIST_STRUCT *move_list, BOARD_STRUCT *board);
extern void Add_Killer_Move(int move, BOARD_STRUCT *board);

//magic_data
extern const U64 R_Magic[64];
extern const U64 B_Magic[64];
extern const U64 R_Occ[64];
extern const U64 B_Occ[64];
extern const int R_Bits[64];
extern const int B_Bits[64];
extern int count_1s(U64 b);
extern const int BitTable[64];
extern int pop_1st_bit(U64 *bb); //Make inline
extern int transform(U64 b, U64 magic, int bits);
extern U64 magicMovesRook[64][4096];
extern U64 magicMovesBishop[64][512];
extern void Generate_Occupancy_Masks(void);
extern void Generate_Magic_Numbers(void);
extern void Generate_Magic_Moves(void);

//magic_movegen
extern U64 Rook_Attacks(U64 occ, int sq);
extern U64 Bishop_Attacks(U64 occ, int sq);
extern void Magic_Generate_Moves(BOARD_STRUCT *board, MOVE_LIST_STRUCT *list);
extern void Magic_Generate_Capture_Promote_Moves(BOARD_STRUCT *board, MOVE_LIST_STRUCT *move_list);

//makemove
extern int Make_Move(int move_num, BOARD_STRUCT *board);
extern void Take_Move(BOARD_STRUCT *board);
extern int Make_Null_Move(BOARD_STRUCT *board);
extern void Take_Null_Move(BOARD_STRUCT *board);
extern int Is_Checking_Move(int move_num, BOARD_STRUCT *board);
extern void Print_Move(MOVE_STRUCT *move);
extern char* UCI_Move_String(MOVE_STRUCT *move);

//movegen

//movelist
extern void Sort_Moves(MOVE_LIST_STRUCT *move_list);
extern void Copy_Move(MOVE_STRUCT *move1, MOVE_STRUCT *move2);
extern int Get_Capture_Moves(MOVE_LIST_STRUCT *move_list); 
extern void Get_Next_Move(int num, MOVE_LIST_STRUCT *move_list); 
extern void Get_Next_Capture_Move(int num, MOVE_LIST_STRUCT *move_list);
extern void Add_Move(MOVE_LIST_STRUCT *move_list, int from, int to, int piece, int capture, int special, int score, BOARD_STRUCT *board);
extern int Movelists_Identical(MOVE_LIST_STRUCT *ptr1, MOVE_LIST_STRUCT *ptr2);
extern void Clear_Movelist(MOVE_LIST_STRUCT *ptr);
extern void Print_Move_List(MOVE_LIST_STRUCT *move_list);

//pawn_hash_table
extern void Add_Pawn_Hash_Entry(int score, U64 hash);
extern int Get_Pawn_Hash_Entry(U64 hash);
extern void Clear_Pawn_Hash_Table(void);

//perft
extern int Perft_Test(char *fen, int depth, BOARD_STRUCT *board);
extern int Search(BOARD_STRUCT *board, int depth);

//pv_table
extern void Clear_PV_List(PV_LIST_STRUCT *pv);
extern int Find_PV_Move(int move_num, MOVE_LIST_STRUCT *move_list);
extern void Print_PV_List(PV_LIST_STRUCT *pv_list);
extern void Get_PV_Line(int depth, PV_LIST_STRUCT *pv_list, BOARD_STRUCT *board);

//search
extern int Iterative_Deepening(int depth, BOARD_STRUCT *board, SEARCH_INFO_STRUCT *info);
extern int Alpha_Beta(int alpha, int beta, int depth, int is_pv, BOARD_STRUCT *board, SEARCH_INFO_STRUCT *info);
extern int Quiescent_Search(int alpha, int beta, BOARD_STRUCT *board, SEARCH_INFO_STRUCT *info);
extern int Search_Position(BOARD_STRUCT *board, SEARCH_INFO_STRUCT *info);
extern void Internal_Iterative_Deepening(int alpha, int beta, int depth, MOVE_LIST_STRUCT *move_list, BOARD_STRUCT *board, SEARCH_INFO_STRUCT *info);

//search_info
extern void Clear_Search_Info(SEARCH_INFO_STRUCT *info);
extern void Print_Move_Index_Data(SEARCH_INFO_STRUCT *info);


//search_test
extern void Search_Test(void);

//see
extern int Static_Exchange_Evaluation(int move, BOARD_STRUCT *board);
extern void Set_Quiescent_SEE_Scores(MOVE_LIST_STRUCT *move_list, BOARD_STRUCT *board);

//tuning
extern int king_end_piece_square_tuning_values[8];
extern int pawn_end_piece_square_tuning_values[7];
extern float passed_pawn_tuning_parameters[3];
extern void Set_King_End_Values(void);
extern void Set_Pawn_End_Values(void);
extern void Set_Passed_Pawn_Rank_Bonuses(void);

//uci
extern void Parse_Go(char* line, SEARCH_INFO_STRUCT *info, BOARD_STRUCT *board);
extern void Parse_Position(char* lineIn, BOARD_STRUCT *board);
extern int Parse_Move(char *ptrChar, BOARD_STRUCT *board);
extern void Uci_Loop(BOARD_STRUCT *pos, SEARCH_INFO_STRUCT *info);



