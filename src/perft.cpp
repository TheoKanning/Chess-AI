/* perft.cpp
* Contains functions and variables for perft testing
* Theo Kanning 12/12/14
*/

#include "globals.h"
#include "time.h"

using namespace std;

int leaves; //Number of leaf nodes found

//Call search function at each depth until reaching limit
int Perft_Test(char *fen, int depth, BOARD_STRUCT *board)
{
	int i;
	int start;

	Parse_Fen(fen, board);

	Print_Board(board);

	for (i = 1; i <= depth; i++)
	{
		start = Get_Time_Ms(); //Get start time
		leaves = 0;//Reset leaf count
		Search(board, i);
		cout << "Normal: Search Depth: " << i << " Leaf Nodes: " << leaves;
		cout << " Nodes/Sec: " << leaves / ((Get_Time_Ms() - start + 1) / 1000.0);
		cout << endl;
	}
	return 1;
}

//Searches to a given depth
int Search(BOARD_STRUCT *board, int depth)
{
	MOVE_LIST_STRUCT move_list;
	int index;

	if (depth == 0) //End of search
	{
		leaves++; //increment leaf count
		return 1;
	}

	//Generate all moves
	Magic_Generate_Moves(board, &move_list);

	//For each move
	for (index = 0; index < move_list.num; index++)
	{
		if (Make_Move(move_list.list[index].move, board)) //If make move is successful
		{
			Search(board, depth - 1);
			Take_Move(board);
		}
	}
	return 1;
}
