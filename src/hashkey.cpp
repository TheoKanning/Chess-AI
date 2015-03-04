/* hashkeu.cpp
* contains definitions for creating and changing hashkeys
* Theo kanning 11/28/14 */
#include "globals.h"
#include "stdlib.h"
#include "time.h"

//Hashkey data
U64 piece_keys[13][64];//[square][piece]
U64 side_keys[2];
U64 ep_keys[101]; //NO_SQUARE = 100;
U64 castle_keys[16];

#define DUAL_HASH_SIZE 500000 //Number of hash entries stored
int HASH_SIZE_MB = 0;

HASH_ENTRY_STRUCT dual_hash_table[2][DUAL_HASH_SIZE];

static void Copy_Hash_Entry(HASH_ENTRY_STRUCT *ptr1, HASH_ENTRY_STRUCT *ptr2);

//Generates all hashkeys, called before board initialization
void Init_Hashkeys(void)
{
	int index, index2;

	//Set rand seed
	srand(time(NULL));
	//Piece keys
	for (index = 0; index < 64; index++)
	{
		for (index2 = 0; index2 < 13; index2++)
		{
			piece_keys[index2][index] = RANDOM_U64();
		}
	}

	//Side keys
	side_keys[WHITE] = RANDOM_U64();
	side_keys[BLACK] = RANDOM_U64();

	//EP keys
	for (index = 0; index < 101; index++)
	{
		ep_keys[index] = RANDOM_U64();
	}

	//Castle keys
	for (index = 0; index < 12; index++)
	{
		castle_keys[index] = RANDOM_U64();
	}
}


//Takes a pointer to a board object and updates its hash_key and pawn_hash_key fields
void Compute_Hash(BOARD_STRUCT *board)
{
	int index;
	int piece;
	board->hash_key = 0; //Reset key
	board->pawn_hash_key = 0;

	//Piece keys
	for (index = 0; index < 64; index++)
	{
		piece = board->board_array[index];
		HASH_IN(board->hash_key, piece_keys[piece][index]);//Add the hash value of the piece at each index
		if (IS_KING(piece) || IS_PAWN(piece)) HASH_IN(board->pawn_hash_key, piece_keys[piece][index]); //Add only pawn and king moves into pawn hash
	}

	//Side keys
	HASH_IN(board->hash_key,side_keys[board->side]);

	//Ep keys
	ASSERT(board->ep >= 0);
	ASSERT(board->ep <= NO_SQUARE);
	HASH_IN(board->hash_key, ep_keys[board->ep]);

	//Castle keys
	HASH_IN(board->hash_key, castle_keys[board->castle_rights]);
}

//Adds an entry to the two-tiered hash table
void Add_Dual_Hash_Entry(HASH_ENTRY_STRUCT *hash_ptr, int ply, SEARCH_INFO_STRUCT *info)
{
	int hash_index = hash_ptr->hash % DUAL_HASH_SIZE;

	//Adjust mate score for ply
	if (hash_ptr->eval >= MATE_SCORE - MAX_SEARCH_DEPTH && hash_ptr->eval <= MATE_SCORE) hash_ptr->eval += ply;
	if (hash_ptr->eval <= -MATE_SCORE + MAX_SEARCH_DEPTH && hash_ptr->eval >= -MATE_SCORE) hash_ptr->eval -= ply;

	//Replace first slot if deeper or newer
	if (dual_hash_table[0][hash_index].depth < hash_ptr->depth || dual_hash_table[0][hash_index].age < hash_ptr->age)
	{
		Copy_Hash_Entry(hash_ptr, &dual_hash_table[0][hash_index]);
	}
	else //Copy into second slot
	{
		Copy_Hash_Entry(hash_ptr, &dual_hash_table[1][hash_index]);
	}
}

//Returns the value of probing the dual hash table
int Get_Dual_Hash_Entry(U64 hash, int alpha, int beta, int depth, int ply, int * hash_move)
{
	//Get hash data
	HASH_ENTRY_STRUCT *hash_temp = &dual_hash_table[0][hash % DUAL_HASH_SIZE];

	//Try first slot
	if (hash_temp->hash == hash) //If hash keys match
	{
		*hash_move = hash_temp->move; //Store hash move in pointer

		if (hash_temp->depth >= depth) //If depth is greater than or equal to search depth
		{
			//Adjust mate score for ply
			int eval = hash_temp->eval;

			if (eval >= MATE_SCORE - MAX_SEARCH_DEPTH) eval -= ply;
			else if (eval <= -MATE_SCORE + MAX_SEARCH_DEPTH) eval += ply;

			if (hash_temp->flag == HASH_EXACT)
			{
				return eval;
			}
			else if (hash_temp->flag == HASH_UPPER && (eval <= alpha))
			{
				return eval;
			}
			else if (hash_temp->flag == HASH_LOWER && (eval >= beta))
			{
				return eval;
			}
			//return INVALID;
		}
	}
	
	//Check second slot
	hash_temp = &dual_hash_table[1][hash % DUAL_HASH_SIZE];
	if (hash_temp->hash == hash) //If hash keys match
	{
		*hash_move = hash_temp->move; //Store hash move in pointer

		if (hash_temp->depth >= depth) //If depth is greater than or equal to search depth
		{
			//Adjust mate score for ply
			int eval = hash_temp->eval;

			if (eval >= MATE_SCORE - MAX_SEARCH_DEPTH) eval -= ply;
			else if (eval <= -MATE_SCORE + MAX_SEARCH_DEPTH) eval += ply;

			if (hash_temp->flag == HASH_EXACT)
			{
				return eval;
			}
			else if (hash_temp->flag == HASH_UPPER && (eval <= alpha))
			{
				return eval;
			}
			else if (hash_temp->flag == HASH_LOWER && (eval >= beta))
			{
				return eval;
			}
		}
	}
	
	return INVALID;
}


//Fills a hash entry with the given parameters
void Fill_Hash_Entry(int age, int depth, int eval, int flag, U64 hash, int move, HASH_ENTRY_STRUCT *hash_ptr)
{
	//Copy parameters into fields
	hash_ptr->age = age;
	hash_ptr->depth = depth;
	hash_ptr->eval = eval;
	hash_ptr->flag = flag;
	hash_ptr->hash = hash;
	hash_ptr->move = move;
}

//Copies hash struct data from pointer 1 to pointer 2
void Copy_Hash_Entry(HASH_ENTRY_STRUCT *ptr1, HASH_ENTRY_STRUCT *ptr2)
{
	//Copy ptr1 data into ptr 2
	ptr2->age = ptr1->age;
	ptr2->depth = ptr1->depth;
	ptr2->eval = ptr1->eval;
	ptr2->flag = ptr1->flag;
	ptr2->hash = ptr1->hash;
	ptr2->move = ptr1->move;
}


//Initializes hash table and clears all entries
void Clear_Hash_Table(void)
{
	memset(dual_hash_table, 0, sizeof(dual_hash_table));
}