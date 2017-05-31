#include"definition.h"
#include "Board.h"
#include "MoveNode.h"
#include "uciPro.h"
#include <stdio.h>


#include "Search.h"
#include "hash.h"

#define UCI_MODE

//#define TEST_MODE

static void genInit(){
	bestTable.clear();
	initData();
	initHashData();
}


int main(){
	genInit();

	Board board;
	setbuf(stdin,NULL);
	setbuf(stdout,NULL);

	board.startGame();
	
	return 0;
#ifdef UCI_MODE
	//UciStart();
	//board.startGame();
#else
#ifdef TEST_MODE
	//perft
	Board a;
	a.parseFEN(case3);
    test(a,6);
#endif
#endif

	
	

	//return 0;
}


