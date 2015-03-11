/* settings.cpp
* Contains functions for changing settings
* Theo Kanning 3/4/15
*/

#include "globals.h"

/***** Settings *****/
int use_SEE = 0;
int quiescent_SEE = 1; //Use SEE for pruing in quiescence search
int use_aspiration_window = 0;
int use_history = 0;
int only_research_in_pv = 0;

/* LMR */
int use_lmr_in_pv = 1;
int use_extra_lmr = 1;
int lmr_depth_mod = 2;
int lmr_move_mod = 8;

/* Null Move*/
int null_move_R = 2; //Amount to reduce null move search
int adapt_null_move = 0; //Use adaptive reduction based on depth
int mult_null_move = 0; //Whether or not to do a null move search within a null move search

/* Always on */
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
		use_SEE = value;
	}
	//Use SEE in quiescent search
	else if (!strncmp(line, "setoption name quiescent_SEE", 27)) {
		int value = 0;
		sscanf_s(line, "%*s %*s %*s %*s %d", &value);
		printf("Set quiescent_SEE to %d\n", value);
		quiescent_SEE = value;
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
	//History
	else if (!strncmp(line, "setoption name history", 21)) {
		int value = 0;
		sscanf_s(line, "%*s %*s %*s %*s %d", &value);
		printf("Set history to %d\n", value);
		use_history = value;
	}
	//LMR in pv nodes
	else if (!strncmp(line, "setoption name lmr_in_pv", 23)) {
		int value = 0;
		sscanf_s(line, "%*s %*s %*s %*s %d", &value);
		printf("Set lmr_in_pv to %d\n", value);
		use_lmr_in_pv = value;
	}
	//Extra lmr
	else if (!strncmp(line, "setoption name extra_lmr", 23)) {
		int value = 0;
		sscanf_s(line, "%*s %*s %*s %*s %d", &value);
		printf("Set extra_lmr to %d\n", value);
		use_extra_lmr = value;
	}
	//LMR depth modifier
	else if (!strncmp(line, "setoption name lmr_depth_mod", 27)) {
		int value = 0;
		sscanf_s(line, "%*s %*s %*s %*s %d", &value);
		printf("Set lmr_depth_mod to %d\n", value);
		lmr_depth_mod = value;
	}
	//LMR move modifier
	else if (!strncmp(line, "setoption name lmr_move_mod", 26)) {
		int value = 0;
		sscanf_s(line, "%*s %*s %*s %*s %d", &value);
		printf("Set lmr_move_mod to %d\n", value);
		lmr_move_mod = value;
	}
	//Null move depth reduction
	else if (!strncmp(line, "setoption name null_move_R", 25)) {
		int value = 0;
		sscanf_s(line, "%*s %*s %*s %*s %d", &value);
		printf("Set null_move_R to %d\n", value);
		null_move_R = value;
	}
	//Null move depth reduction based on search depth
	else if (!strncmp(line, "setoption name adapt_null_move", 29)) {
		int value = 0;
		sscanf_s(line, "%*s %*s %*s %*s %d", &value);
		printf("Set adapt_null_move to %d\n", value);
		adapt_null_move = value;
	}
	//Null move within null move search
	else if (!strncmp(line, "setoption name mult_null_move", 28)) {
		int value = 0;
		sscanf_s(line, "%*s %*s %*s %*s %d", &value);
		printf("Set mult_null_move to %d\n", value);
		mult_null_move = value;
	}
	//Only research after null windows in pv
	else if (!strncmp(line, "setoption name only_research_in_pv", 33)) {
		int value = 0;
		sscanf_s(line, "%*s %*s %*s %*s %d", &value);
		printf("Set only_research_in_pv to %d\n", value);
		only_research_in_pv = value;
	}
}