#include "definition.h"
#include "Search.h"
#include "Board.h"
#include "MoveNode.h"
#include "uciPro.h"
#include <thread>
#define GINPUTSIZE 5000
static int vsMode, playerSide, npcDepth,npcTime, gameState, startflag;

static void printHelp();
static void parseSearch(char *input, Board &board);
static void moveInstruction(char input[], Board & board);
static void vsChange(char input[]);
int checkState(int gameState , int side);
class GameThread{

	char * input;
	Board * board;
public:
	MoveNode bestmove;
	GameThread(char * i, Board* b){
		input =i;
		board =b;

	}

	void run(){

		parseSearch(input, *board);
		finishFlag=1;
		board->printBoard();
		printf("critical command ready\n");

	}
	void npcRun(){



		printf("============NPC turn ============\n");
		timeInfo.useTimer=1;
		timeInfo.startSearch=GameTime::getMs();
		timeInfo.isStop=0;
		timeInfo.stopSearch=timeInfo.startSearch+npcTime;
		bestmove = searchMove(npcDepth,*board);
        finishFlag=1;
		board->performMove(bestmove);
		board->printBoard();
		printf("v.s.Mode %s (PlayerSide %s npcDepth %d npcTimeLimit %dmsec)\n",vsMode?"On":"Off",playerSide?"Black":"White",npcDepth,npcTime);
		printf("critical command ready\n");
		gameState=board->getState();
		
		if (checkState(gameState , board->side))
			return;
		printf("===========Your turn ============\n");

		
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


void Board::startGame(){


	char input[GINPUTSIZE]; playerSide=White;
	vsMode=1; npcDepth =MAXDEPTH;npcTime =10000;
	GameThread gThread(input,this);
	
	
	parseFEN(startFEN);
	printBoard();
	printf("type \"help\" for help\n");
	printf("v.s.Mode %s (PlayerSide %s npcDepth %d npcTime %dmsec)\n",vsMode?"On":"Off",playerSide?"Black":"White",npcDepth,npcTime);
//	printf("CommandReady\n");
	while(true){
		gameState=getState();
        


		//npc turn
		if (side !=playerSide && vsMode && gameState==0){
			finishFlag =0;
            std::thread signalT(&GameThread::signal,&gThread);
			std::thread searchT (&GameThread::npcRun,&gThread);
			while(!finishFlag ){

				fgets(input,GINPUTSIZE,stdin);
				if (!strncmp(input,"s",1)){
					timeInfo.isStop=1;
					break;
				}

			}
			signalT.detach();
			searchT.join();
            
			goto processing;

		}

		//=============player turn
		printf("critical command ready\n");
		fgets(input,GINPUTSIZE,stdin);

		//=============		
processing:;



		if (!strncmp(input,"search",6)){
			finishFlag =0; startflag=0;
			std::thread searchT (&GameThread::run,&gThread);

            std::thread signalT(&GameThread::signal,&gThread);
			while(!startflag){
				std::this_thread::yield();
			}
			while(!finishFlag ){


				fgets(input,GINPUTSIZE,stdin);
				if (!strncmp(input,"s",1)){
					timeInfo.isStop=1;
					break;
				}

			}
			signalT.detach();
			searchT.join();
            
			goto processing;


		}
		if (!strncmp(input,"position",8)){
			parsePosition(input,*this);
			printBoard();

			continue;
		}


		if (!strncmp(input,"move",4)){
			moveInstruction(input,*this);
			printBoard();
			continue;
		}
		if (!strncmp(input,"help",4)){
			printHelp();
			
			continue;
		}

		if (!strncmp(input,"quit",4)){
			break;
		}

		if (!strncmp(input,"undo",4)){
			if (vsMode){
				undoMove();
				undoMove();
			}else
			{
				undoMove();
			}
			printBoard();
			continue;
		}
		if (!strncmp(input,"new",3)){
			
			parseFEN(startFEN);
			bestTable.clear();
			printBoard();

			continue;
		}
		if (!strncmp(input,"vs",2)){
			vsChange(input);
            printf("v.s.Mode %s (PlayerSide %s npcDepth %d npcTime %dmsec)\n",vsMode?"On":"Off",playerSide?"Black":"White",npcDepth,npcTime);

			continue;
		}
		if (!strncmp(input,"bit",3)){
			printBitBoard(pawnBitBoard[White]);
			printBitBoard(pawnBitBoard[Black]);
			continue;
		}
        if (!strncmp(input,"print",5)){
			printBoard();
			continue;
		}
        if (!strncmp(input,"eva",3)){
			printf("score %d\n", evaluation());
			
			continue;
		}

		if (!strncmp(input,"key",3)){
			printf("%llxu\n",hashcode);
			continue;
		}
		if (!strncmp(input,"clearh",6)){
			bestTable.clear();
			printf("cleared\n");
			continue;
		}
		if (!strncmp (input,"validate",8)){
			char * move=input +9;
			if (!validateMove(move)){//fail
				printf("invalid Move\n");
			}else {
				printf("valid Move\n");
			}
		}
		/*if (!strncmp(input,"nullm",5)){
			performNull();
			printBoard();
			continue;
		}
		if (!strncmp(input,"unull",5)){
			undoNull();
			printBoard();
			continue;
		}
		if (!strncmp(input,"draw",4)){
			if (getState()==THREEFOLD){
				printf("Draw Accepted\n");
				parseFEN(startFEN);
				bestTable.clear();
				printBoard();
			}else {
				printf ("cannot\n");

			}
			continue;
		}*/
	}


}


static void parseSearch(char *input, Board &board){

	int depth = -1, time =-1;
	char *command=0;




	if ((command=strstr(input,"timelimit") )) {
		time = atoi(command + 10);
	}

	if ((command=strstr(input,"depth") )) {
		depth = atoi(command + 6);
	} 


	timeInfo.startSearch=GameTime::getMs();

	if(time != -1) {
		timeInfo.useTimer=1;	
		time -= 50;	//for safety	
		timeInfo.stopSearch= timeInfo.startSearch + time;
	}else{
		timeInfo.useTimer=0;
	}

	if(depth == -1) {
		depth = MAXDEPTH;
	}
	printf("timelimit:%d depth:%d usetimer:%d\n",
		time,depth,timeInfo.useTimer);
	startflag=1;
	searchMove(depth,board);



}

static void moveInstruction(char input[], Board & board){
	char * move=input +5;

	if (checkState(gameState , board.side))
		return;

	if (!board.parseMove(move)){//fail
		printf("Invalid Move\n");
	}
	gameState=board.getState();
	if (checkState(gameState , board.side))
		return;

}

int checkState(int gameState , int side){
	switch (gameState){
		case MATESTATE:
			printf("critical state checkmate %s lose\n", side?"Black":"White");
			return 1;
			break;
		case FIFTYDRAW:
			printf("critical state draw Fifty Move Rule\n");
			return 1;
			break;
		case THREEFOLD:
			printf("critical state draw threefold\n");
			return 1;
			break;
		case STALEMATE:
			printf("critical state draw Stalemate\n");
			return 1;
			break;
		case MATDRAW:
			printf("critical state draw Insufficient Materials\n");
			return 1;
			break;
		default:
			return 0;
    }
}

static void vsChange(char input[]){
	char * ptr=input +3;
	if (!strncmp(ptr,"on",2))
		vsMode=1;
	if (!strncmp(ptr,"off",3)){
		vsMode=0;
	}
	if ((ptr=strstr(ptr,"side"))){
		if (!strncmp(ptr+5,"white",5)){
			playerSide=0;
		}
		if (!strncmp(ptr+5,"black",5)){
			playerSide=1;
		}
	}

	if ((ptr=strstr(input,"timelimit") )) {
		if (!(npcTime = atoi(ptr + 10))){
			printf("Invalid\n");
			npcTime=10000;
		}
	}

	if ((ptr=strstr(input,"depth") )) {
        int tDepth;
		if(!(tDepth = atoi(ptr + 6))){
			printf("Invalid\n");
			
		}
        else {
            if (npcDepth > MAXDEPTH){
                npcDepth = MAXDEPTH;
                
            }else {
                npcDepth = tDepth;
                
            }
        }
	}


}

static void printHelp(){
	printf("\nInstruction==========\n");
	printf("new game:\n\tnew\n");
	printf("Moving a piece:\n\tmove a2a3\n");
	printf("\tpromotion:move a7a8q (queen), move a7a8r (rook), ...b (bishop), ...n(knight)\n");
	//=============
	printf("Search move:\n\tsearch depth 5\n");
	printf("\tsearch\n");
	printf("\tsearch depth 5 timelimit 5000\n");
	printf("\ttype \"s\" to stop searching \n");
	//==========
	printf("undo move:\n");
	printf("\tundo\n");
    
	printf("set vs Mode:\n");
	printf("\tvs [on/off] side [white/black]\n");
	printf("\tvs side [white/black]\n");
	printf("\tvs depth 7 timelimit 1000  (the depth and timelimit for the npc)\n");
    
	printf("print the board:\t print\n");
	
    
	printf("set Position:\n");
	printf("\tposition [fen  | startpos ]  moves  a2a4 a8a7 ...\n");
	printf("quit:\n\tquit\n");
	printf("========================\n");

}