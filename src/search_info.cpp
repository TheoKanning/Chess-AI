/* search_info
* Contains functions for maintaining information during search
* Theo Kanning 2/6/15
*/

#include "globals.h"

//Clears search info before a new search
void Clear_Search_Info(SEARCH_INFO_STRUCT *info)
{
	//Reset stop flag
	info->stopped = 0;
	info->nodes = 0;
	info->null_available = 1;

	info->hash_hits = 0;
	info->hash_probes = 0;

	memset(info->best_index, 0, MAX_MOVE_LIST_LENGTH*sizeof(int));
	memset(info->beta_cutoff_index, 0, MAX_MOVE_LIST_LENGTH*sizeof(int));
}

//Prints move index info
void Print_Move_Index_Data(SEARCH_INFO_STRUCT *info)
{
	//Calculate average best move index
	float best_temp = 0;
	float beta_temp = 0;
	int best_total = 0;
	int beta_total = 0;

	for (int i = 0; i < MAX_MOVE_LIST_LENGTH; i++)
	{
		best_temp += info->best_index[i] * i;
		best_total += info->best_index[i];
		beta_temp += info->beta_cutoff_index[i] * i;
		beta_total += info->beta_cutoff_index[i];
	}

	printf("Average Indices: Best Move:%f Beta Cutoff:%f\n", best_temp / best_total, beta_temp/beta_total);
}