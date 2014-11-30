/*makemove.c
* Contains functions for making and taking moves from the board, and updating all list and array data
* Theo Kanning 11/30/14 
*/

/* Move data structure
32 bit integer
bits [0:6] from square index 120
bits [7:13] to square index 120
bits [14:17] moved piece
bits [18:21] captured piece, if any
bit  [22] en-passant capture
bit  [23] kingside castle
bit  [24] queenside castle
bit  [25] promotion to queen
bit  [26] promotion to rook
bit  [27] promotion to bishop
bit  [28] promotion to knight

*/

//Shifts to corresponding fields
#define fromShift				0
#define toShift					7
#define pieceShift				14
#define	capturedShift			18
#define epShift					22
#define kcShift					23
#define qcShift					24
#define pqShift					25
#define prShift					26
#define pbShift					27
#define pnShift					28

