/* magic.cpp
* Generates magic bits for move generation
* Theo Kanning 1/29/15
*/
#include <stdio.h>
#include <stdlib.h>
#include "globals.h"

U64 occupancyVariation[64][4096] = { 0i64 };
//U64 occupancyAttackSet[64][4096] = { 0i64 };
U64 magicMovesRook[64][4096] = { 0i64 };
U64 magicMovesBishop[64][512] = { 0i64 };


static U64 index_to_uint64(int index, int bits, U64 m);
static U64 rmask(int sq);
static U64 bmask(int sq);
static U64 ratt(int sq, U64 block);
static U64 batt(int sq, U64 block);
static U64 find_magic(int sq, int m, int bishop);


U64 random_uint64_fewbits() {
	return RANDOM_U64() & RANDOM_U64() & RANDOM_U64();
}

int count_1s(U64 b) {
	int r;
	for (r = 0; b; r++, b &= b - 1);
	return r;
}

const int BitTable[64] = {
	63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2,
	51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
	26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
	58, 20, 37, 17, 36, 8
};

int pop_1st_bit(U64 *bb) {
	U64 b = *bb ^ (*bb - 1);
	unsigned int fold = (unsigned)((b & 0xffffffff) ^ (b >> 32));
	*bb &= (*bb - 1);
	return BitTable[(fold * 0x783a9b23) >> 26];
}

//Finds indices of set bits in bb and stores them in array_ptr, all unused values are -1
void Get_Set_Bits(U64 bb, int array_ptr[64])
{
	int index, num = 0;

	//Store index of set bits
	for (index = 0; index < 63; index++)
	{
		if (GET_BIT(bb, index))
		{
			array_ptr[num] = index;
			num++;
		}
	}

	//Fill rest of arrays with 0
	for (index = num; index < 63; index++)
	{
		array_ptr[index] = -1;
	}
}

U64 index_to_uint64(int index, int bits, U64 m) {
	int i, j;
	U64 result = 0ULL;
	for (i = 0; i < bits; i++) {
		j = pop_1st_bit(&m);
		if (index & (1 << i)) result |= (1i64 << j);
	}
	return result;
}

U64 rmask(int sq) {
	U64 result = 0i64;
	int rk = sq / 8, fl = sq % 8, r, f;
	for (r = rk + 1; r <= 6; r++) result |= (1ULL << (fl + r * 8));
	for (r = rk - 1; r >= 1; r--) result |= (1ULL << (fl + r * 8));
	for (f = fl + 1; f <= 6; f++) result |= (1ULL << (f + rk * 8));
	for (f = fl - 1; f >= 1; f--) result |= (1ULL << (f + rk * 8));
	return result;
}

U64 bmask(int sq) {
	U64 result = 0ULL;
	int rk = sq / 8, fl = sq % 8, r, f;
	for (r = rk + 1, f = fl + 1; r <= 6 && f <= 6; r++, f++) result |= (1ULL << (f + r * 8));
	for (r = rk + 1, f = fl - 1; r <= 6 && f >= 1; r++, f--) result |= (1ULL << (f + r * 8));
	for (r = rk - 1, f = fl + 1; r >= 1 && f <= 6; r--, f++) result |= (1ULL << (f + r * 8));
	for (r = rk - 1, f = fl - 1; r >= 1 && f >= 1; r--, f--) result |= (1ULL << (f + r * 8));
	return result;
}

U64 kmask(int sq) {
	U64 result = 0ULL;
	int rk = sq / 8, fl = sq % 8;
	if (rk < RANK_8) result |= (1ULL << (sq + 8)); //Top
	if (rk < RANK_8 && fl < FILE_H) result |= (1ULL << (sq + 9)); //Top right
	if (fl < FILE_H) result |= (1ULL << (sq + 1)); //Right
	if (rk > RANK_1 && fl < FILE_H) result |= (1ULL << (sq - 7)); //Bottom Right
	if (rk > RANK_1) result |= (1ULL << (sq - 8)); //Bottom
	if (rk > RANK_1 && fl > FILE_A) result |= (1ULL << (sq - 9)); //Bottom Left
	if (fl > FILE_A) result |= (1ULL << (sq - 1)); //Left
	if (rk < RANK_8 && fl > FILE_A) result |= (1ULL << (sq + 7)); //Top Left
	return result;
}

U64 nmask(int sq) {
	/*
	*   0  15 0  17 0
	*   6  0  0  0 10
	*   0  0  0  0  0
	*  -10 0  0  0 -6
	*   0 -17 0 -15  0
	*/

	U64 result = 0ULL;
	int rk = sq / 8, fl = sq % 8;
	if (rk < RANK_7 && fl < FILE_H) result |= (1ULL << (sq + 17));
	if (rk < RANK_7 && fl > FILE_A) result |= (1ULL << (sq + 15));

	if (rk < RANK_8 && fl < FILE_G) result |= (1ULL << (sq + 10));
	if (rk < RANK_8 && fl > FILE_B) result |= (1ULL << (sq + 6));

	if (rk > RANK_1 && fl < FILE_G) result |= (1ULL << (sq - 6));
	if (rk > RANK_1 && fl > FILE_B) result |= (1ULL << (sq - 10));
		   
	if (rk > RANK_2 && fl < FILE_H) result |= (1ULL << (sq - 15));
	if (rk > RANK_2 && fl > FILE_A) result |= (1ULL << (sq - 17));
	return result;
}

U64 ratt(int sq, U64 block) {
	U64 result = 0ULL;
	int rk = sq / 8, fl = sq % 8, r, f;
	for (r = rk + 1; r <= 7; r++) {
		result |= (1ULL << (fl + r * 8));
		if (block & (1ULL << (fl + r * 8))) break;
	}
	for (r = rk - 1; r >= 0; r--) {
		result |= (1ULL << (fl + r * 8));
		if (block & (1ULL << (fl + r * 8))) break;
	}
	for (f = fl + 1; f <= 7; f++) {
		result |= (1ULL << (f + rk * 8));
		if (block & (1ULL << (f + rk * 8))) break;
	}
	for (f = fl - 1; f >= 0; f--) {
		result |= (1ULL << (f + rk * 8));
		if (block & (1ULL << (f + rk * 8))) break;
	}
	return result;
}

U64 batt(int sq, U64 block) {
	U64 result = 0ULL;
	int rk = sq / 8, fl = sq % 8, r, f;
	for (r = rk + 1, f = fl + 1; r <= 7 && f <= 7; r++, f++) {
		result |= (1ULL << (f + r * 8));
		if (block & (1ULL << (f + r * 8))) break;
	}
	for (r = rk + 1, f = fl - 1; r <= 7 && f >= 0; r++, f--) {
		result |= (1ULL << (f + r * 8));
		if (block & (1ULL << (f + r * 8))) break;
	}
	for (r = rk - 1, f = fl + 1; r >= 0 && f <= 7; r--, f++) {
		result |= (1ULL << (f + r * 8));
		if (block & (1ULL << (f + r * 8))) break;
	}
	for (r = rk - 1, f = fl - 1; r >= 0 && f >= 0; r--, f--) {
		result |= (1ULL << (f + r * 8));
		if (block & (1ULL << (f + r * 8))) break;
	}
	return result;
}


int transform(U64 b, U64 magic, int bits) {
#if defined(USE_32_BIT_MULTIPLICATIONS)
	return
		(unsigned)((int)b*(int)magic ^ (int)(b >> 32)*(int)(magic >> 32)) >> (32 - bits);
#else
	return (int)((b * magic) >> (64 - bits));
#endif
}

U64 find_magic(int sq, int m, int bishop) {
	U64 mask, b[4096], a[4096], used[4096], magic;
	int i, j, k, n, fail;

	mask = bishop ? bmask(sq) : rmask(sq);
	n = count_1s(mask);

	for (i = 0; i < (1 << n); i++) {
		b[i] = index_to_uint64(i, n, mask);
		a[i] = bishop ? batt(sq, b[i]) : ratt(sq, b[i]);
	}
	for (k = 0; k < 100000000; k++) {
		magic = random_uint64_fewbits();
		if (count_1s((mask * magic) & 0xFF00000000000000ULL) < 6) continue;
		for (i = 0; i < 4096; i++) used[i] = 0ULL;
		for (i = 0, fail = 0; !fail && i < (1 << n); i++) {
			j = transform(b[i], magic, m);
			if (used[j] == 0ULL) used[j] = a[i];
			else if (used[j] != a[i]) fail = 1;
		}
		if (!fail) return magic;
	}
	printf("***Failed***\n");
	return 0ULL;
}

const int R_Bits[64] = {
	12, 11, 11, 11, 11, 11, 11, 12,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	12, 11, 11, 11, 11, 11, 11, 12
};

const int B_Bits[64] = {
	6, 5, 5, 5, 5, 5, 5, 6,
	5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 7, 7, 7, 7, 5, 5,
	5, 5, 7, 9, 9, 7, 5, 5,
	5, 5, 7, 9, 9, 7, 5, 5,
	5, 5, 7, 7, 7, 7, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5,
	6, 5, 5, 5, 5, 5, 5, 6
};

void generateOccupancyVariations(int isRook)
{
	int i, j, bitRef;
	U64 mask;
	int variationCount;
	int setBitsInMask[64];
	int setBitsInIndex[64];
	int bitCount[64] = { 0 };

	for (bitRef = 0; bitRef <= 63; bitRef++)
	{
		mask = isRook ? R_Occ[bitRef] : B_Occ[bitRef];
		Get_Set_Bits(mask, setBitsInMask); //setBitsInMask = Bitboards.getSetBits(mask);
		bitCount[bitRef] = count_1s(mask);
		variationCount = (int)(1i64 << bitCount[bitRef]);
		for (i = 0; i<variationCount; i++)
		{
			occupancyVariation[bitRef][i] = 0i64;

			// find bits set in index "i" and map them to bits in the 64 bit "occupancyVariation"

			Get_Set_Bits(i, setBitsInIndex); //setBitsInIndex = Bitboards.getSetBits(i); // an array of integers showing which bits are set
			for (j = 0; setBitsInIndex[j] != -1; j++)
			{
				occupancyVariation[bitRef][i] |= (1i64 << setBitsInMask[setBitsInIndex[j]]);
			}

			if (isRook)
			{
				for (j = bitRef + 8; j <= 55 && (occupancyVariation[bitRef][i] & (1i64 << j)) == 0; j += 8);
				//if (j >= 0 && j <= 63) occupancyAttackSet[bitRef][i] |= (1i64 << j);
				for (j = bitRef - 8; j >= 8 && (occupancyVariation[bitRef][i] & (1i64 << j)) == 0; j -= 8);
				//if (j >= 0 && j <= 63) occupancyAttackSet[bitRef][i] |= (1i64 << j);
				for (j = bitRef + 1; j % 8 != 7 && j % 8 != 0 && (occupancyVariation[bitRef][i] & (1i64 << j)) == 0; j++);
				//if (j >= 0 && j <= 63) occupancyAttackSet[bitRef][i] |= (1i64 << j);
				for (j = bitRef - 1; j % 8 != 7 && j % 8 != 0 && j >= 0 && (occupancyVariation[bitRef][i] & (1i64 << j)) == 0; j--);
				//if (j >= 0 && j <= 63) occupancyAttackSet[bitRef][i] |= (1i64 << j);
			}
			else
			{
				for (j = bitRef + 9; j % 8 != 7 && j % 8 != 0 && j <= 55 && (occupancyVariation[bitRef][i] & (1i64 << j)) == 0; j += 9);
				//if (j >= 0 && j <= 63) occupancyAttackSet[bitRef][i] |= (1i64 << j);
				for (j = bitRef - 9; j % 8 != 7 && j % 8 != 0 && j >= 8 && (occupancyVariation[bitRef][i] & (1i64 << j)) == 0; j -= 9);
				//if (j >= 0 && j <= 63) occupancyAttackSet[bitRef][i] |= (1i64 << j);
				for (j = bitRef + 7; j % 8 != 7 && j % 8 != 0 && j <= 55 && (occupancyVariation[bitRef][i] & (1i64 << j)) == 0; j += 7);
				//if (j >= 0 && j <= 63) occupancyAttackSet[bitRef][i] |= (1i64 << j);
				for (j = bitRef - 7; j % 8 != 7 && j % 8 != 0 && j >= 8 && (occupancyVariation[bitRef][i] & (1i64 << j)) == 0; j -= 7);
				//if (j >= 0 && j <= 63) occupancyAttackSet[bitRef][i] |= (1i64 << j);
			}
		}
	}
}

void generateMoveDatabase(int isRook)
{
	U64 validMoves;
	int variations, bitCount;
	int bitRef, i, j, magicIndex;

	for (bitRef = 0; bitRef <= 63; bitRef++)
	{
		bitCount = isRook ? count_1s(R_Occ[bitRef]) : count_1s(B_Occ[bitRef]);
		variations = (int)(1i64 << bitCount);

		for (i = 0; i<variations; i++)
		{
			validMoves = 0i64;
			if (isRook)
			{
				magicIndex = (int)((occupancyVariation[bitRef][i] * R_Magic[bitRef]) >> (64 - R_Bits[bitRef]));
				ASSERT(magicIndex < 4096);

				for (j = bitRef + 8; j <= 63; j += 8) { validMoves |= (1i64 << j); if ((occupancyVariation[bitRef][i] & (1i64 << j)) != 0) break; }
				for (j = bitRef - 8; j >= 0; j -= 8) { validMoves |= (1i64 << j); if ((occupancyVariation[bitRef][i] & (1i64 << j)) != 0) break; }
				for (j = bitRef + 1; j % 8 != 0; j++) { validMoves |= (1i64 << j); if ((occupancyVariation[bitRef][i] & (1i64 << j)) != 0) break; }
				for (j = bitRef - 1; j % 8 != 7 && j >= 0; j--) { validMoves |= (1i64 << j); if ((occupancyVariation[bitRef][i] & (1i64 << j)) != 0) break; }

				magicMovesRook[bitRef][magicIndex] = validMoves;
			}
			else
			{
				magicIndex = (int)((occupancyVariation[bitRef][i] * B_Magic[bitRef]) >> (64 - B_Bits[bitRef]));
				ASSERT(magicIndex < 512);

				for (j = bitRef + 9; j % 8 != 0 && j <= 63; j += 9) { validMoves |= (1i64 << j); if ((occupancyVariation[bitRef][i] & (1i64 << j)) != 0) break; }
				for (j = bitRef - 9; j % 8 != 7 && j >= 0; j -= 9) { validMoves |= (1i64 << j); if ((occupancyVariation[bitRef][i] & (1i64 << j)) != 0) break; }
				for (j = bitRef + 7; j % 8 != 7 && j <= 63; j += 7) {
					validMoves |= (1i64 << j);
					if ((occupancyVariation[bitRef][i] & (1i64 << j)) != 0)
						break;
				}
				for (j = bitRef - 7; j % 8 != 0 && j >= 0; j -= 7) {
					validMoves |= (1i64 << j);
					if ((occupancyVariation[bitRef][i] & (1i64 << j)) != 0)
						break;
				}
				magicMovesBishop[bitRef][magicIndex] = validMoves;
			}
		}
	}
}

void Generate_Magic_Numbers(void)
{
	int square;
	printf("const U64 R_Magic[64] = {\n");
	for (square = 0; square < 64; square++)
		printf("  0x%llxULL,\n", find_magic(square, R_Bits[square], 0));
	printf("};\n\n");

	printf("const U64 B_Magic[64] = {\n");
	for (square = 0; square < 64; square++)
		printf("  0x%llxULL,\n", find_magic(square, B_Bits[square], 1));
	printf("};\n\n");
}

void Generate_Occupancy_Masks(void)
{
	int square;
	printf("const U64 R_Occ[64] = {\n");
	for (square = 0; square < 64; square++)
		printf("  0x%llxULL,\n", rmask(square));
	printf("};\n\n");

	printf("const U64 B_Occ[64] = {\n");
	for (square = 0; square < 64; square++)
		printf("  0x%llxULL,\n", bmask(square));
	printf("};\n\n");
}

void Generate_King_Knight_Attack_Masks(void)
{
	int square;
	printf("const U64 magicMovesKnight[64] = {\n");
	for (square = 0; square < 64; square++)
		printf("  0x%llxULL,\n", nmask(square));
	printf("};\n\n");

	printf("const U64 magicMovesKing[64] = {\n");
	for (square = 0; square < 64; square++)
		printf("  0x%llxULL,\n", kmask(square));
	printf("};\n\n");
}

//Generates all magic move databases (bishop and rook)
void Generate_Magic_Moves(void)
{
	/***** Rook *****/
	generateOccupancyVariations(1); //Variations
	generateMoveDatabase(1); //Moves

	/***** Bishop *****/
	generateOccupancyVariations(0);
	generateMoveDatabase(0);
}

/***** Generated Arrays *****/
const U64 R_Magic[64] = {
	0x80008040002010ULL,
	0x40002000401000ULL,
	0x200204008120080ULL,
	0x100041000082100ULL,
	0x200100402002008ULL,
	0x300040001000208ULL,
	0x4400580881100406ULL,
	0x200120020408104ULL,
	0x800080400020ULL,
	0x2a88400444201004ULL,
	0x1001002000110042ULL,
	0x41001001022208ULL,
	0xc001000500100800ULL,
	0x100a004200041008ULL,
	0x819000402004100ULL,
	0x3001002080410002ULL,
	0x840028000408020ULL,
	0x40d0014020004000ULL,
	0x2140808020001000ULL,
	0x1020848008001002ULL,
	0x828008004000880ULL,
	0x20a808002000400ULL,
	0x2000040008419002ULL,
	0x80e00600008c1055ULL,
	0x200400080208000ULL,
	0x8c00210200420080ULL,
	0x40900280200081ULL,
	0x100080080080ULL,
	0x10040080080080ULL,
	0x1444010040400200ULL,
	0x208080400100102ULL,
	0x8050802080004100ULL,
	0x5020004000808000ULL,
	0x8100420082002108ULL,
	0x200200080801000ULL,
	0xd000801001804800ULL,
	0x4081800802800400ULL,
	0x20800400800201ULL,
	0x4240700a04000118ULL,
	0x2400004082000104ULL,
	0x100401080288000ULL,
	0x4040008100210040ULL,
	0x4020040200101000ULL,
	0x8248001000210100ULL,
	0x301001028030004ULL,
	0x8a1c000201004040ULL,
	0x9082000841020044ULL,
	0x5081011088620004ULL,
	0x820800040002080ULL,
	0x2022008040210200ULL,
	0x200010008080ULL,
	0x200801000080080ULL,
	0x3005000511080100ULL,
	0x4000020004008080ULL,
	0x200080110820400ULL,
	0x20108108442200ULL,
	0x2a0c601281410202ULL,
	0x4e00420015002682ULL,
	0x400200422110c882ULL,
	0x40c4a00900900005ULL,
	0x42002004081002ULL,
	0x9133000208040001ULL,
	0x1408081020821104ULL,
	0x8501000200402081ULL,
};

const U64 B_Magic[64] = {
	0x11100908018014ULL,
	0x4140802062110ULL,
	0x50008610400010ULL,
	0x2010908200424000ULL,
	0x44042000002185ULL,
	0x1461010840000852ULL,
	0x8048908820102012ULL,
	0x23220210410400dULL,
	0x1a2200384010400ULL,
	0x81404808008c0041ULL,
	0x2108080810000ULL,
	0x1044040800180ULL,
	0x14102804040aULL,
	0x82000c4420040031ULL,
	0x8504008090109008ULL,
	0x2100802402080400ULL,
	0x20000405901202ULL,
	0x84101030021040ULL,
	0x1898009000802108ULL,
	0x28000402102102ULL,
	0x104020600940028ULL,
	0x202000c4108a044ULL,
	0x4015100480882080ULL,
	0x80a11002078a0102ULL,
	0x2021100004040812ULL,
	0x221042020a80200ULL,
	0xa020470010040088ULL,
	0x4004204010002ULL,
	0x2aa8840101802000ULL,
	0x8410002081040110ULL,
	0x4000942501010800ULL,
	0x5220410090440200ULL,
	0x110101040945d2cULL,
	0x1142000100180ULL,
	0x7100104800041800ULL,
	0x200100820040400ULL,
	0x2004040400c0100ULL,
	0x91111200050040ULL,
	0x80044c0a88004800ULL,
	0x1004210010904ULL,
	0x440a88249e4002ULL,
	0x204512808802018ULL,
	0xa982042208000100ULL,
	0x8001a011021802ULL,
	0xc0202009000880ULL,
	0x9401022802004040ULL,
	0x8503106400210ULL,
	0x2802480041001284ULL,
	0x80820110428400ULL,
	0x4000404450080000ULL,
	0x82201100020ULL,
	0x8408500108480000ULL,
	0x41401002021501ULL,
	0x2a00401022108048ULL,
	0x88204800a10c89ULL,
	0x1114080800509400ULL,
	0x4802802422002ULL,
	0x6d12402081444ULL,
	0x1252208420850ULL,
	0x6400000000420200ULL,
	0x4000480111020200ULL,
	0x42020d4560080240ULL,
	0x400c406488048100ULL,
	0x30201821002020ULL,
};

const U64 R_Occ[64] = {
	0x101010101017eULL,
	0x202020202027cULL,
	0x404040404047aULL,
	0x8080808080876ULL,
	0x1010101010106eULL,
	0x2020202020205eULL,
	0x4040404040403eULL,
	0x8080808080807eULL,
	0x1010101017e00ULL,
	0x2020202027c00ULL,
	0x4040404047a00ULL,
	0x8080808087600ULL,
	0x10101010106e00ULL,
	0x20202020205e00ULL,
	0x40404040403e00ULL,
	0x80808080807e00ULL,
	0x10101017e0100ULL,
	0x20202027c0200ULL,
	0x40404047a0400ULL,
	0x8080808760800ULL,
	0x101010106e1000ULL,
	0x202020205e2000ULL,
	0x404040403e4000ULL,
	0x808080807e8000ULL,
	0x101017e010100ULL,
	0x202027c020200ULL,
	0x404047a040400ULL,
	0x8080876080800ULL,
	0x1010106e101000ULL,
	0x2020205e202000ULL,
	0x4040403e404000ULL,
	0x8080807e808000ULL,
	0x1017e01010100ULL,
	0x2027c02020200ULL,
	0x4047a04040400ULL,
	0x8087608080800ULL,
	0x10106e10101000ULL,
	0x20205e20202000ULL,
	0x40403e40404000ULL,
	0x80807e80808000ULL,
	0x17e0101010100ULL,
	0x27c0202020200ULL,
	0x47a0404040400ULL,
	0x8760808080800ULL,
	0x106e1010101000ULL,
	0x205e2020202000ULL,
	0x403e4040404000ULL,
	0x807e8080808000ULL,
	0x7e010101010100ULL,
	0x7c020202020200ULL,
	0x7a040404040400ULL,
	0x76080808080800ULL,
	0x6e101010101000ULL,
	0x5e202020202000ULL,
	0x3e404040404000ULL,
	0x7e808080808000ULL,
	0x7e01010101010100ULL,
	0x7c02020202020200ULL,
	0x7a04040404040400ULL,
	0x7608080808080800ULL,
	0x6e10101010101000ULL,
	0x5e20202020202000ULL,
	0x3e40404040404000ULL,
	0x7e80808080808000ULL,
};

const U64 B_Occ[64] = {
	0x40201008040200ULL,
	0x402010080400ULL,
	0x4020100a00ULL,
	0x40221400ULL,
	0x2442800ULL,
	0x204085000ULL,
	0x20408102000ULL,
	0x2040810204000ULL,
	0x20100804020000ULL,
	0x40201008040000ULL,
	0x4020100a0000ULL,
	0x4022140000ULL,
	0x244280000ULL,
	0x20408500000ULL,
	0x2040810200000ULL,
	0x4081020400000ULL,
	0x10080402000200ULL,
	0x20100804000400ULL,
	0x4020100a000a00ULL,
	0x402214001400ULL,
	0x24428002800ULL,
	0x2040850005000ULL,
	0x4081020002000ULL,
	0x8102040004000ULL,
	0x8040200020400ULL,
	0x10080400040800ULL,
	0x20100a000a1000ULL,
	0x40221400142200ULL,
	0x2442800284400ULL,
	0x4085000500800ULL,
	0x8102000201000ULL,
	0x10204000402000ULL,
	0x4020002040800ULL,
	0x8040004081000ULL,
	0x100a000a102000ULL,
	0x22140014224000ULL,
	0x44280028440200ULL,
	0x8500050080400ULL,
	0x10200020100800ULL,
	0x20400040201000ULL,
	0x2000204081000ULL,
	0x4000408102000ULL,
	0xa000a10204000ULL,
	0x14001422400000ULL,
	0x28002844020000ULL,
	0x50005008040200ULL,
	0x20002010080400ULL,
	0x40004020100800ULL,
	0x20408102000ULL,
	0x40810204000ULL,
	0xa1020400000ULL,
	0x142240000000ULL,
	0x284402000000ULL,
	0x500804020000ULL,
	0x201008040200ULL,
	0x402010080400ULL,
	0x2040810204000ULL,
	0x4081020400000ULL,
	0xa102040000000ULL,
	0x14224000000000ULL,
	0x28440200000000ULL,
	0x50080402000000ULL,
	0x20100804020000ULL,
	0x40201008040200ULL,
};

const U64 magicMovesKnight[64] = {
	0x20400ULL,
	0x50800ULL,
	0xa1100ULL,
	0x142200ULL,
	0x284400ULL,
	0x508800ULL,
	0xa01000ULL,
	0x402000ULL,
	0x2040004ULL,
	0x5080008ULL,
	0xa110011ULL,
	0x14220022ULL,
	0x28440044ULL,
	0x50880088ULL,
	0xa0100010ULL,
	0x40200020ULL,
	0x204000402ULL,
	0x508000805ULL,
	0xa1100110aULL,
	0x1422002214ULL,
	0x2844004428ULL,
	0x5088008850ULL,
	0xa0100010a0ULL,
	0x4020002040ULL,
	0x20400040200ULL,
	0x50800080500ULL,
	0xa1100110a00ULL,
	0x142200221400ULL,
	0x284400442800ULL,
	0x508800885000ULL,
	0xa0100010a000ULL,
	0x402000204000ULL,
	0x2040004020000ULL,
	0x5080008050000ULL,
	0xa1100110a0000ULL,
	0x14220022140000ULL,
	0x28440044280000ULL,
	0x50880088500000ULL,
	0xa0100010a00000ULL,
	0x40200020400000ULL,
	0x204000402000000ULL,
	0x508000805000000ULL,
	0xa1100110a000000ULL,
	0x1422002214000000ULL,
	0x2844004428000000ULL,
	0x5088008850000000ULL,
	0xa0100010a0000000ULL,
	0x4020002040000000ULL,
	0x400040200000000ULL,
	0x800080500000000ULL,
	0x1100110a00000000ULL,
	0x2200221400000000ULL,
	0x4400442800000000ULL,
	0x8800885000000000ULL,
	0x100010a000000000ULL,
	0x2000204000000000ULL,
	0x4020000000000ULL,
	0x8050000000000ULL,
	0x110a0000000000ULL,
	0x22140000000000ULL,
	0x44280000000000ULL,
	0x88500000000000ULL,
	0x10a00000000000ULL,
	0x20400000000000ULL,
};

const U64 magicMovesKing[64] = {
	0x302ULL,
	0x705ULL,
	0xe0aULL,
	0x1c14ULL,
	0x3828ULL,
	0x7050ULL,
	0xe0a0ULL,
	0xc040ULL,
	0x30203ULL,
	0x70507ULL,
	0xe0a0eULL,
	0x1c141cULL,
	0x382838ULL,
	0x705070ULL,
	0xe0a0e0ULL,
	0xc040c0ULL,
	0x3020300ULL,
	0x7050700ULL,
	0xe0a0e00ULL,
	0x1c141c00ULL,
	0x38283800ULL,
	0x70507000ULL,
	0xe0a0e000ULL,
	0xc040c000ULL,
	0x302030000ULL,
	0x705070000ULL,
	0xe0a0e0000ULL,
	0x1c141c0000ULL,
	0x3828380000ULL,
	0x7050700000ULL,
	0xe0a0e00000ULL,
	0xc040c00000ULL,
	0x30203000000ULL,
	0x70507000000ULL,
	0xe0a0e000000ULL,
	0x1c141c000000ULL,
	0x382838000000ULL,
	0x705070000000ULL,
	0xe0a0e0000000ULL,
	0xc040c0000000ULL,
	0x3020300000000ULL,
	0x7050700000000ULL,
	0xe0a0e00000000ULL,
	0x1c141c00000000ULL,
	0x38283800000000ULL,
	0x70507000000000ULL,
	0xe0a0e000000000ULL,
	0xc040c000000000ULL,
	0x302030000000000ULL,
	0x705070000000000ULL,
	0xe0a0e0000000000ULL,
	0x1c141c0000000000ULL,
	0x3828380000000000ULL,
	0x7050700000000000ULL,
	0xe0a0e00000000000ULL,
	0xc040c00000000000ULL,
	0x203000000000000ULL,
	0x507000000000000ULL,
	0xa0e000000000000ULL,
	0x141c000000000000ULL,
	0x2838000000000000ULL,
	0x5070000000000000ULL,
	0xa0e0000000000000ULL,
	0x40c0000000000000ULL,
};