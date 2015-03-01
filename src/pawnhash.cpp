/* pawnhash
* Contains all functions for accessing pawn hash table
* Theo Kanning 2/2/15
*/

#include "globals.h"

typedef struct{
	int score;
	U64 hash;
}PAWN_HASH_ENTRY_STRUCT;

int hash_size = 65536;

PAWN_HASH_ENTRY_STRUCT pawn_hash_table[65536];

//Stores an entry in the table
void Add_Pawn_Hash_Entry(int score, U64 hash)
{
	int hash_index = hash & hash_size;

	pawn_hash_table[hash_index].hash = hash;
	pawn_hash_table[hash_index].score = score;
}

//Gets an entry from the table
int Get_Pawn_Hash_Entry(U64 hash)
{
	int hash_index = hash & hash_size;
	if (pawn_hash_table[hash_index].hash == hash) return pawn_hash_table[hash_index].score;

	return INVALID;
}

//Clears pawn hash table
void Clear_Pawn_Hash_Table(void)
{
	memset(pawn_hash_table, 0, sizeof(pawn_hash_table));
}