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

int HASH_SIZE = 10000; //Number of hash entries stored
int HASH_SIZE_MB = 0;

HASH_ENTRY_STRUCT hash_table[10000];

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
int Get_Hash_Entry(U64 hash, HASH_ENTRY_STRUCT *hash_ptr)
{
	int hash_index = hash % HASH_SIZE;
	
	//See if hash is a match, if not, end and return zero
	if (hash_table[hash_index].hash != hash)	return 0;

	//Fill hash_ptr and return 1
	Copy_Hash_Entry(&hash_table[hash_index], hash_ptr);

	return 1;
}

//Adds a hash entry to the table, checking if replacement is approriate
void Add_Hash_Entry(HASH_ENTRY_STRUCT *hash_ptr, SEARCH_INFO_STRUCT *info)
{
	int hash_index = hash_ptr->hash % HASH_SIZE;

	//Check criteria and return if one is not met
	
	//If stored entry is empty, replace
	if (hash_table[hash_index].hash == 0)
	{
		Copy_Hash_Entry(hash_ptr, &hash_table[hash_index]);
		return;
	}


	//If stored entry is too old, replace
	if (hash_table[hash_index].age < info->age)
	{
		Copy_Hash_Entry(hash_ptr, &hash_table[hash_index]);
		return;
	}

	//If both entries are exact, keep entry with higher depth
	if ((hash_table[hash_index].flag == HASH_EXACT) && (hash_ptr->age == HASH_EXACT))
	{
		//Copy if stored entry has lower or equal depth, then return
		if (hash_table[hash_index].depth <= hash_ptr->depth)	Copy_Hash_Entry(hash_ptr, &hash_table[hash_index]);
		return;
	}

	//If new entry is exact and stored is not, replace
	if ((hash_ptr->age == HASH_EXACT))
	{
		Copy_Hash_Entry(hash_ptr, &hash_table[hash_index]);
		return;
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

