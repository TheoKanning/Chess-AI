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