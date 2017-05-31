#include "uciPro.h"
#include "Board.h"
#include "Search.h"
#include <thread>
#include <string.h>
#define NAME "ChessProj"
#define AUTHOR "LTY"
#define INPUTSIZE 5000
static uint8_t startFlag;
class SThread{

	char * input;
	Board * board;
public:
	SThread(char * i, Board* b){
		input =i;
		board =b;

	}

	void run(){

		parseGo(input, *board);
		finishFlag=1;

	}

	void signal(){
        std::this_thread::sleep_for(std::chrono::seconds(2));
        while (!finishFlag){
			
            printf("Searching depth%d... current nodes:%llu Time:%llu\n", destDepth,nodes,GameTime::getMs()-timeInfo.startSearch );
            std::this_thread::sleep_for(std::chrono::seconds(1));
        
        }
        return;
    }

};

void parseGo(char * input , Board & board){
	printf("input:%s\n",input);
	int depth = -1, time =-1, inc=0, movetime =-1, movestogo=30;
	char *command=0;
	if (strstr(input,"infinite")) {
		;
	} 

	if ((command=strstr(input,"binc") )&& board.side ==Black ){
		inc = atoi(command + 5);
	}
	if ( (command=strstr(input,"winc") ) && board.side ==White ){
		inc = atoi(command + 5);
	}

	if ((command=strstr(input,"wtime") ) && board.side ==White ) {
		time = atoi(command + 6);
	} 

	if ((command=strstr(input,"btime")) && board.side ==Black ) {
		time = atoi(command + 6);
	}   
	if ((command=strstr(input,"movestogo") )) {
		movestogo = atoi(command + 10);
	} 

	if ((command=strstr(input,"movetime") )) {
		movetime = atoi(command + 9);
	}

	if ((command=strstr(input,"depth") )) {
		depth = atoi(command + 6);
	} 

	if(movetime != -1) {
		time = movetime;
		movestogo = 1;
	}
	timeInfo.startSearch=GameTime::getMs();

	if(time != -1) {
		timeInfo.useTimer=1;

		time /= movestogo;
		time -= 50;	//for safety	
		timeInfo.stopSearch= timeInfo.startSearch + time + inc;
	}else{
		timeInfo.useTimer=0;
	}

	if(depth == -1) {
		depth = MAXDEPTH;
	}
	printf("time:%d start:%llu stop:%llu depth:%d timeset:%d\n",
		time,timeInfo.startSearch,timeInfo.stopSearch,depth,timeInfo.useTimer);
	startFlag=1;
	searchMove(depth,board);


}




void UciStart(){
	char input[INPUTSIZE];
	setbuf(stdin,NULL);
	setbuf(stdout,NULL);
	
	printf("====Welcome===%s\n",VERSION);
	printf("To play in Console, please type \"play\"\n");
	

	Board board;

	SThread sthread(input,&board);
	while(1){
		memset(input,0,INPUTSIZE*sizeof(char));
		if (!fgets(input,INPUTSIZE,stdin))
			continue;

processing:;
		if (input[0]=='\n')
			continue;
		if (!strncmp(input,"ucinewgame",10)){
			board.parseFEN(startFEN);
			bestTable.clear();
			board.printBoard();
			continue;
		}
		if (!strncmp(input,"position",8)){
			parsePosition(input,board);
			//board.printBoard();
			continue;
		}
		if (!strncmp(input,"isready",7)){
			printf("readyok\n");
			continue;
		}
		if (!strncmp(input,"quit",4)){
			break;
		}
		if (!strncmp(input,"play",4)){
			
			//board.parseFEN("3Rrq2/3Q2kp/p4p2/8/1p3PPK/1P1P3P/1PP5/8 b - - 9 49 ");
		    board.startGame();
			return;
		}
		if (!strncmp(input,"uci",3)){
			printf("id name %s\n", NAME);
			printf("id author %s\n", AUTHOR);
			printf("uciok\n");
			continue;
		}
		if (!strncmp(input,"go",2)){
			finishFlag =0; startFlag=0;
			std::thread searchT (&SThread::run,&sthread);
			std::thread signalT (&SThread::signal,&sthread);
			while(!startFlag){
				std::this_thread::yield();
			}
			while(!finishFlag ){

				memset(input,0,INPUTSIZE*sizeof(char));
				fgets(input,INPUTSIZE,stdin);
				if (!strncmp(input,"stop",4)){
					timeInfo.isStop=1;
					break;
				}
				

			}
			searchT.join();
			signalT.join();
			

			goto processing;
		}



	}

}




void parsePosition(char input[], Board & board){
	input+=9;
	if (!strncmp(input,"fen",3)){
		board.parseFEN((input +4));

	}else if (!strncmp (input,"startpos",8)){
		board.parseFEN(startFEN);


	}else {

		return;
	}
	char * moves = strstr(input,"moves");
	if (moves)
		moves+=6;

	//printf("%s\n", moves);
	while (moves){
		int i =0;
		
		if (!(i=board.parseMove(moves))){
			//printf("no Move \n");
			break;
		}
		moves+=i+1;

	}
	
	board.printBoard();



}




