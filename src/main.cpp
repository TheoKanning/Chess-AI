#include <stdio.h>
#include <iostream>
#include "globals.h"

using namespace std;
BOARD_STRUCT board;


int main()
{
	cout << PROGRAM_NAME << " version " << VERSION_NO << endl << AUTHOR << endl;	
	Init_Hashkeys();
	Init_Board(&board);
	Print_Board(&board);
	system("PAUSE");
}