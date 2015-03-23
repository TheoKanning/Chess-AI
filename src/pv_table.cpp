/* pv_table.cpp
* Contains functions for finding pv moves in a list and printing the pv table
* Theo Kanning 12/14/14
*/
#include "globals.h"

using namespace std;

//Gets pv line by moving through hash table
void Get_PV_Line(int depth, PV_LIST_STRUCT *pv_list, BOARD_STRUCT *board)
{
	HASH_ENTRY_STRUCT hash_entry;
	int valid = 0;
	
	Clear_PV_List(pv_list);

	Get_Hash_Entry(board->hash_key, 0, 0, 0, board->hply, &hash_entry.move);
	int move = hash_entry.move;
	int count = 0;

	while ((move != 0) && (count < depth)) {

		ASSERT(count < MAX_SEARCH_DEPTH);

		if (Make_Move(move, board))
		{
			pv_list->list[count].move = move;
			count++;
		}
		else {
			break;
		}

		//Stop after 3-fold rep, 50 move, or material draw detected
		if (Is_Threefold_Repetition(board) || board->move_counter >= 100 || Is_Material_Draw(board)) break;

		//Get next hash move
		move = 0;
		Get_Hash_Entry(board->hash_key, 0, 0, 0, board->hply, &move);
		
	}

	while (board->hply > 0) {
		Take_Move(board);
	}

	pv_list->num = count;

}

//Empties a PV list
void Clear_PV_List(PV_LIST_STRUCT *pv)
{
	int index;

	for (index = 0; index < MAX_SEARCH_DEPTH; index++)
	{
		pv->list[index].move = 0;
		pv->list[index].score = 0;
	}
	pv->in_pv_line = 0;
	pv->num = 0;
}

//Finds pv move in list, returns 0 if not found
int Find_PV_Move(int move_num, MOVE_LIST_STRUCT *move_list)
{
	int index;
	if (move_num == 0) return 0;

	for (index = 0; index < move_list->num; index++)
	{
		if (move_list->list[index].move == move_num) //if move matches pv move
		{
			move_list->list[index].score = PV_SCORE; //Set mvoe score to PV_SCORE
			return 1;
		}
	}
	return 0;//Move not found
}


//Print PV list with final eval score
void Print_PV_List(PV_LIST_STRUCT *pv_list)
{
	int i;

	for (i = 0; i < pv_list->num; i++)
	{
		Print_Move(&pv_list->list[i]);
		cout << " ";
	}
}

