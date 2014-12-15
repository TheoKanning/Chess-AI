/* pv_table.cpp
* Contains functions for finding pv moves in a list and printing the pv table
* Theo Kanning 12/14/14
*/
#include "globals.h"

using namespace std;

//Empties a PV list
void Clear_PV_List(PV_LIST_STRUCT *pv)
{
	int index;

	for (index = 0; index < pv->num; index++)
	{
		pv->list[index].move = 0;
		pv->list[index].score = 0;
	}
	pv->in_pv_line = 0;
	pv->num = 0;
}

//Finds pv move in list, returns 0 if not found
int Find_PV_Move(MOVE_STRUCT *move, MOVE_LIST_STRUCT *move_list)
{
	int index;

	for (index = 0; index < move_list->num; index++)
	{
		if (move_list->list[index].move == move->move) //if move matches pv move
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