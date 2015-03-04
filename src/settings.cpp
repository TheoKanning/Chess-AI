/* settings.cpp
* Contains functions for changing settings
* Theo Kanning 3/4/15
*/

#include "globals.h"

/***** Settings *****/
int use_SEE = 1;
int use_aspiration_window = 0;
int use_history = 0;
int use_null_window_first = 0;
int use_futility = 1;
int use_late_move_reduction = 1;

//Takes a string and parses settings from it
void Set_Option(char * line)
{
	//King endgame piece-square tables
	if (!strncmp(line, "setoption name keps", 18)) {

		int value = 0;
		int index = line[19] - '0';
		sscanf_s(line, "%*s %*s %*s %*s %d", &value);
		printf("Set keps%d to %d\n", index, value);
		king_end_piece_square_tuning_values[index] = value;
		Set_King_End_Values();
	}
	//Pawn endgame piece square
	else if (!strncmp(line, "setoption name peps", 18)) {

		int value = 0;
		int index = line[19] - '0';
		sscanf_s(line, "%*s %*s %*s %*s %d", &value);
		printf("Set peps%d to %d\n", index, value);
		pawn_end_piece_square_tuning_values[index] = value;
		Set_Pawn_End_Values();
	}
	//Passed pawn rank bonus
	else if (!strncmp(line, "setoption name pprb", 18)) {
		int value = 0;
		int index = line[19] - '0';
		sscanf_s(line, "%*s %*s %*s %*s %d", &value);
		printf("Set pprb%d to %d\n", index, value);
		passed_pawn_rank_bonus[index] = value;
	}
	//Futility margin
	else if (!strncmp(line, "setoption name fmrg", 18)) {
		int value = 0;
		int index = line[19] - '0';
		sscanf_s(line, "%*s %*s %*s %*s %d", &value);
		printf("Set fmrg%d to %d\n", index, value);
		futility_margins[index] = value;
	}
	//Use SEE
	else if (!strncmp(line, "setoption name SEE", 17)) {
		int value = 0;
		sscanf_s(line, "%*s %*s %*s %*s %d", &value);
		printf("Set SEE to %d\n", value);
		use_SEE = value * 10;
	}
	//Aspiration window size
	else if (!strncmp(line, "setoption name aspiration", 24)) {
		int value = 0;
		int index = line[25] - '0';
		sscanf_s(line, "%*s %*s %*s %*s %d", &value);
		printf("Set aspiration%d to %d\n", index, value);
		use_aspiration_window = 1;
		aspiration_windows[index] = value * 10;
	}
	//Null window first
	else if (!strncmp(line, "setoption name null_first", 24)) {
		int value = 0;
		sscanf_s(line, "%*s %*s %*s %*s %d", &value);
		printf("Set null_window_first to %d\n",  value);
		use_null_window_first = value;
	}
	//History
	else if (!strncmp(line, "setoption name history", 21)) {
		int value = 0;
		sscanf_s(line, "%*s %*s %*s %*s %d", &value);
		printf("Set history to %d\n", value);
		use_history = value;
	}
}