#include <stdio.h>
#include <iostream>
#include "globals.h"

using namespace std;
BOARD_STRUCT board;


int main()
{
	Init_Board(&board);
	Print_Board(&board);
	system("PAUSE");
}