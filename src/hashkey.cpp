/* hashkeu.cpp
* contains definitions for creating and changing hashkeys
* Theo kanning 11/28/14 */
#include "globals.h"
#include "stdlib.h"
#include "time.h"

//Returns random 64 bit uint
#define RANDOM_U64()			(((U64)rand() + \
								((U64)rand() << 15) + \
								((U64)rand() << 30) + \
								((U64)rand() << 45) + \
							   (((U64)rand() & 0x0f) << 60)))

//Hashkey data
U64 piece_keys[13][64];//[square][piece]
U64 side_keys[2];
U64 ep_keys[101]; //NO_SQUARE = 100;
U64 castle_keys[16];

int HASH_SIZE = 1000000; //Number of hash entries stored
int HASH_SIZE_MB = 0;

HASH_ENTRY_STRUCT hash_table[1000000];

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
		piece = board->board_array64[index];
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

//Reads hash table and returns pointer if entry is found
int Get_Hash_Entry(U64 hash, int alpha, int beta, int depth, int ply, int * hash_move)
{
	//Get hash data
	HASH_ENTRY_STRUCT *hash_temp = &hash_table[hash % HASH_SIZE];

	if (hash_temp->hash == hash) //If hash keys match
	{
		*hash_move = hash_temp->move; //Store hash move in pointer

		if (hash_temp->depth >= depth) //If depth is greater than or equal to search depth
		{
			//Adjust mate score for ply
			int eval = hash_temp->eval;

			if (IS_MATE(eval)) eval = ADJUST_MATE_SCORE(eval, ply);

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
	else //keys do not match
	{
		*hash_move = 0; //Reset move
	}

	return INVALID;
}
int Get_Hash_Entry(U64 hash, HASH_ENTRY_STRUCT *hash_ptr)
{
	int hash_index = hash % HASH_SIZE;
	//See if hash is a match, if not, end and return zero
	if (hash_table[hash_index].hash != hash)
	{
		//Check next index
		hash_index++;
		if (hash_index >= HASH_SIZE) hash_index = 0;

		if (hash_table[hash_index].hash == hash)
		{
			Copy_Hash_Entry(&hash_table[hash_index], hash_ptr);
			return 1;
		}
		else
		{
			//Remove move field from pointer
			hash_ptr->move = 0;
			return 0;
		}
	}

	//Fill hash_ptr and return 1
	Copy_Hash_Entry(&hash_table[hash_index], hash_ptr);

	return 1;
	
}

//Adds a hash entry to the table, checking if replacement is approriate
void Add_Hash_Entry(HASH_ENTRY_STRUCT *hash_ptr, int ply, SEARCH_INFO_STRUCT *info)
{
	int hash_index = hash_ptr->hash % HASH_SIZE;

	//Adjust mate score for ply
	if (hash_ptr->eval >= MATE_SCORE - MAX_SEARCH_DEPTH) hash_ptr->eval += ply;
	if (hash_ptr->eval <= -MATE_SCORE + MAX_SEARCH_DEPTH) hash_ptr->eval -= ply;

	//If stored entry is empty, replace
	if (hash_table[hash_index].hash == 0)
	{
		Copy_Hash_Entry(hash_ptr, &hash_table[hash_index]);
		return;
	}

	//If stored entry is too old, replace
	if (hash_table[hash_index].age + 3 < info->age)
	{
		Copy_Hash_Entry(hash_ptr, &hash_table[hash_index]);
		return;
	}
	
	//If new entry is exact
	if (hash_ptr->flag == HASH_EXACT)
	{
		//If both are exact and have the same hash, keep deeper entry
		if ((hash_table[hash_index].flag == HASH_EXACT))
		{
			if (hash_ptr->hash == hash_table[hash_index].hash)
			{
				if (hash_ptr->depth >= hash_table[hash_index].depth)
				{
					Copy_Hash_Entry(hash_ptr, &hash_table[hash_index]);
					return;
				}
			}
			else //Hashes not identical, store in index above if empty
			{
				//Increment index
				hash_index++;
				if (hash_index >= HASH_SIZE) hash_index = 0;

				//If spot above is not exact
				if (hash_table[hash_index].flag != HASH_EXACT)
				{
					Copy_Hash_Entry(hash_ptr, &hash_table[hash_index]);
					return;
				}

				//If spot above is old
				if (hash_table[hash_index].age + 3 < info->age)
				{
					Copy_Hash_Entry(hash_ptr, &hash_table[hash_index]);
					return;
				}

				//If hash is identical, and depth is greater or equal
				if (hash_ptr->hash == hash_table[hash_index].hash && hash_ptr->depth >= hash_table[hash_index].depth)
				{
					Copy_Hash_Entry(hash_ptr, &hash_table[hash_index]);
					return;
				}
			}
		}
		else
		{
			//If new entry is exact and stored is not, replace
			Copy_Hash_Entry(hash_ptr, &hash_table[hash_index]);
			return;
		}
	}

	
	//Return if stored is exact and new is not
	if (hash_table[hash_index].flag == HASH_EXACT) return;

	//If new entry has high depth, replace
	if (hash_ptr->depth > hash_table[hash_index].depth)
	{
		Copy_Hash_Entry(hash_ptr, &hash_table[hash_index]);
		return;
	}

	//If entry at index+1 is empty, store there (add later)
	if (hash_table[(hash_index + 1) % HASH_SIZE].hash == 0)
	{
		//Copy_Hash_Entry(hash_ptr, &hash_table[hash_index]);
		return;
	}


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

//Removes entry with the guven hash key
void Remove_Hash_Entry(U64 hash)
{
	int hash_index = hash % HASH_SIZE;

	hash_table[hash_index].age = 0;
	hash_table[hash_index].depth = 0;
	hash_table[hash_index].eval = 0;
	hash_table[hash_index].flag = HASH_EMPTY;
	hash_table[hash_index].hash = 0;
	hash_table[hash_index].move = 0;
}

//Initializes hash table and clears all entries
void Init_Hash_Table(void)
{
	memset(hash_table, 0, sizeof(hash_table));
}