
#include "definition.h"
#include "evaluationData.h"
#include "hash.h"
#include "Board.h"
#include "MoveNode.h"
#include "Search.h"
#include <string>
#include <string.h>
#include <stdio.h>

//global

int destDepth; static bool pvSearched,pvSet; unsigned long long nodes;
static const int C = 3, M=9 , R=3;
double fh ,fhf; long muCut, histCut, nullCut,futCut;
int mateDepth; int finishFlag;
MoveNode pvList [30][30]; //PV Move for first selected node

uint8_t history [2][120][120];
MoveNode killer [30][2];
boardHashTable bestTable;



//method
//static int zwSearch (int depth, Board &board, int alpha, int beta);
static int MaxValue(int depth, Board &board, int alpha, int beta , bool cut,int late);
static void setHistPv(const int &depth, Board b);
static int quiescentMaxSearch( Board &board , int alpha, int beta );


static void checkTimer(){
	if (timeInfo.useTimer && GameTime::getMs()> timeInfo.stopSearch){
		timeInfo.isStop=1;
	}

}


bool Board::quiescent(){

	if (captured || beingChecked || enPromoting()){

		return false;
	}
	return true;
}


static void swapMove(MoveList &a, int i ,int j){
	MoveNode temp = a.node[j];
	a.node[j]=a.node[i];
	a.node[i]=temp;
}

static void pick (MoveList &a , int i ){
	int max =-INIFINITY; int target;
	for (int j =i ; j <a.count; j++){
		if (a.node[j].priority >max){
			max = a.node[j].priority;
			target = j;
		}
	}
	swapMove(a, i, target);
}


static int findPV(int depth ,MoveList & a){
	if (depth <2){
		pvSearched =true;
		return -1;
	}
	for (int i =0 ; i <a.count; i++){

		if (pvList[destDepth-2][depth-2].equal( a.node+i)){

			return i;
		}
	}
	pvSearched=true;
	return -1;
}




static int MaxValue(int depth, Board &board, int alpha, int beta , bool cut , int late){

	int first; int head =0; MoveNode * historyBest=0; int local=-INIFINITY;
	MoveNode * bestMove; int score;
	int preAlpha = alpha;
	nodes++;
	if ((board.isRepeat() ||board.fiftymove >100) && board.plys >0){
		return 0;
	}


	if ((nodes & 1023)==0){
		checkTimer();
	}
	if (timeInfo.isStop){ //stop Searching
		return 0;
	}

    if (late && board.beingChecked){
        depth +=late;
        
    }

	if ( depth <= 0)
		return quiescentMaxSearch(board,alpha,beta);

	int hasht=bestTable.getRecord(board.hashcode, &historyBest, score,alpha,beta,depth);
	if (destDepth > 2&& pvSearched &&hasht==1){
		//validate	
		histCut++;
		return score;

	}


	Board copy = board;
	MoveList temp;
	board.genNodes(temp);


	//null move

	if (pvSearched && depth >R && cut && board.plys && !board.beingChecked &&board.majorNumber[board.side]>=2 ){

		board.performNull();
		score = - MaxValue(depth-1-R, board,-beta,1-beta,false ,0 );
		board.undoNull();
		//
		if (timeInfo.isStop){ //stop Searching
			return 0;
		}
		if (score >=beta){
			nullCut++;
			return beta;
		}
	}


	if( depth ==2 && !board.beingChecked && board.plys>4 && !board.enPromoting()&& !board.myPromoting()){
		int tsco = board.evaluation();
		if (tsco +500< alpha){
			futCut++;
			return alpha;
		}
	}





	//==============order arrange===================
	if (1^pvSearched){
		first = findPV(depth,temp);
		if (first >=0){

			swapMove(temp,first,head);
			head++;
		}
	}



	if (historyBest){
		for (int i=head; i <temp.count; i++){
			if (temp.node[i].equal(historyBest)){
				swapMove(temp,i,head);
				head ++;


				break;

			}
		}

	}
	//===========
	// multi-cut

	if (pvSearched && depth >R && cut && board.plys && !board.beingChecked && board.majorNumber[board.side]>=2){
		int cutNum=0; int firstNum ; uint8_t pieceCutted[C];


		firstNum= temp.count <=M ?temp.count : M;
		for (int i=0; i <firstNum; i++){
			if(i>=head)
				pick(temp,i);
			//===========
			bool skip=false;
			for (int p=0 ; p<cutNum ; p++){
				if(temp.node[i].from==pieceCutted[p]){
					skip=true;
					break;
				}

			}
			if (!skip){
				board.performMove(temp.node[i]);
				score = - MaxValue(depth-1-R, board,-beta,1-beta,false,0 );
				board = copy;
				//
				if (timeInfo.isStop){ //stop Searching
					return 0;
				}
				if (score >=beta){
					pieceCutted[cutNum]=temp.node[i].from;
					if (++cutNum==C){
						muCut++;
						return beta;
					}

				}
			}
		}

	}

	bool pvMoved=false;
	for (int i=0 ;i <temp.count; i++){


		if (i >= head)
			pick(temp,i);

		// null window search
		if (!pvMoved){
			board.performMove(temp.node[i]);
			score = -MaxValue(depth-1 ,board,-beta,-alpha,1,0);
		}
		else {


			board.performMove(temp.node[i]);
			//late move reduction
            if ( i > 5 && depth > 3 && !board.beingChecked && !temp.node[i].promote && !temp.node[i].capture){
                int lateR = 1;
                if ( depth > 7) lateR = depth/4;

                if (i >20) lateR ++;
                if (i>32) lateR++;
                
                score =  -MaxValue(depth-1 -lateR ,board,-alpha-1,-alpha,1,lateR);
            }else {
                score =  -MaxValue(depth-1 ,board,-alpha-1,-alpha,1,0);
			}
            if (score > alpha){
				score = -MaxValue(depth-1 ,board,-beta,-alpha,1,0);


			}
		}


		board = copy;

		if (timeInfo.isStop){			
			return 0;
		}
		if (score >local){
			local = score;		
			bestMove=&temp.node[i];
		}

		if (score >= beta){			
			if (i==0)
				fhf++;
			fh++;


			if (1^temp.node[i].capture){

				killer[board.plys][1]=killer[board.plys][0];
				killer[board.plys][0]=temp.node[i];

				history[board.side][temp.node[i].from][temp.node[i].to]+=depth;
			}
			CONFIRM(bestMove!=0);
			bestTable.addRecord(board.hashcode,*bestMove, score, betaFlag,depth);
			return score;

		}
		if (score > alpha){
			alpha= score;
			pvMoved=true;
			if (depth!=1){

				memcpy(pvList[depth-1],pvList[depth-2],sizeof(MoveNode)*(depth-1));
				pvList[depth-1][depth-1]=temp.node[i];
			}
			else{
				pvList[0][0]=temp.node[i];
			}


		}


	}

	if (!temp.count) {

		if (board.beingChecked){
			mateDepth=depth;
			return -CHECKMATE;
		}
		else return 0;

	}

	if (alpha > preAlpha){
		CONFIRM(bestMove!=0);
		bestTable.addRecord(board.hashcode,*bestMove, local, exactFlag,depth);

	}
	else{
		CONFIRM(bestMove!=0);
		bestTable.addRecord(board.hashcode,*bestMove, alpha, alphaFlag,depth);
	}

	return alpha;



}



static int quiescentMaxSearch(  Board& board , int alpha, int beta ){
	nodes++; 
	if (board.isRepeat()||board.fiftymove >100){
		return 0;
	}



	if (board.plys >=MAXDEPTH ){

		return board.evaluation();
	}

	if ((nodes & 1023)==0){
		checkTimer();
	}
	if (timeInfo.isStop){
		return 0;
	}


	int tscore =board.evaluation();
    if (!board.beingChecked){
        if (tscore >=beta)
            return tscore;

    }
	if (tscore > alpha)
		alpha = tscore;


	MoveList temp;
	board.genCaps(temp);
	Board copy = board;


	for (int i=0 ;i <temp.count; i++){

		pick(temp,i);
		CONFIRM(temp.node[i].capture==1);
		board.performMove(temp.node[i]);
		int score = -quiescentMaxSearch( board,-beta,-alpha);
		board = copy;


		if (timeInfo.isStop){
			return 0;
		}



		if (score >= beta){

			if (1^temp.node[i].capture){
				killer[board.plys][1]=killer[board.plys][0];
				killer[board.plys][0]=temp.node[i];
			}
			return score;
		}
		if (score > alpha){
			alpha= score;

		}
	}
	



	return alpha;
}

static int alphaBeta(int depth, Board board, int alpha, int beta){
	if (depth >1)
		memcpy(pvList[depth-1]+1,pvList[depth-2],sizeof(MoveNode)*(depth-1));
	int temp= MaxValue(depth,board,alpha,beta,true,0);

	setHistPv(depth,board);	
	return temp;
}



MoveNode  searchMove(int depth , Board board){
	// init ====
	int score; int final;
	memset(pvList[0],0,sizeof(MoveNode)*30*30);
	memset(killer[0],0,sizeof(MoveNode)*30*2);
	memset(history[0][0],0,sizeof(uint8_t)*28800);
	board.plys=0; final=0;
	
	bestTable.hit=0;
	bestTable.collision=0;
	timeInfo.isStop=0;
	mateDepth=0;

	Board copy;
	
	if (depth > MAXDEPTH)
		depth =MAXDEPTH;

	//================iterative depeening
	for (int i =1 ; i <= depth ; i ++){
		//=============init=================
		nodes=0; destDepth=i; muCut=0; histCut=0; nullCut=0;futCut=0;
		fh=0;
		fhf=0;
		pvSet=false;

		//==============start Search===============		
		copy = board;
		score=alphaBeta(i, board,-INIFINITY, +INIFINITY);
		board =copy;
		//========================
		if (timeInfo.isStop){
			final= i;

			break;
		}
		pvSearched=false;


		


		// checkmate ==========================
		if (score == CHECKMATE || score == -CHECKMATE){
			printf("info score cp %d depth %d nodes %llu time %llu ",score,i,nodes,GameTime::getMs()-timeInfo.startSearch);
			printf("pv ");
			for (int j = i-1; j>=mateDepth; j--){
				if (pvList[i-1][j].from==0) break;
				printf("%s ",pvList[i-1][j].uciPrint().c_str());

				if (pvList[i-1][j].capture)
					printf("(capture) ");
			}

			printf("checkmate\n");			
			//printf("FHFRate:%lf multi-cut:%ld nullCut:%ld histCut:%ld", fhf/fh , muCut, nullCut, histCut);
			//printf("\n");
			final=i;
			break;
		}
		
		//============ not checkmate ====================

		printf("info score cp %d depth %d nodes %llu time %llu ",score,i,nodes,GameTime::getMs()-timeInfo.startSearch);


		printf("pv ");
		for (int j = i-1; j>=0; j--){
			if (pvList[i-1][j].from==0) break;
			printf("%s ",(pvList[i-1][j].uciPrint()).c_str());

			if (pvList[i-1][j].capture)
				printf("(capture) ");

		}

		//printf("\n");

		//printf("FHFRate:%lf multi-cut:%ld nullCut:%ld histCut:%ld futCut:%ld ", fhf/fh , muCut, nullCut, histCut,futCut);

		printf("\n");

		//=======
		mateDepth=0;
		final=i;

	}
	//==================== 

	/*for (int i= final-1; i >=mateDepth; i--)
	pvList[final-1][i].printNode();*/
	//printf("score:%d \n", score);

	//printf("hit %d collision %d \n",bestTable.hit, bestTable.collision);

	printf("bestmove %s\n",pvList[final-1][final-1].uciPrint().c_str());



	return pvList[final-1][final-1];
}




static void setHistPv(const int &depth,  Board b){

	int index=1; MoveList temp; int ok=1;

	while (index <= depth && ok == 1){
		temp.count=0;
		b.genNodes(temp); 
		ok =0; 
		for (int i =0; i <temp.count; i++){
			if (temp.node[i].equal(pvList[depth-1]+(depth-index))){								
				b.performMove(temp.node[i]);
				index++;
				ok =1;
				break;
			}
		}

	}
	if (index <= depth){
		pvList[depth-1][depth -index].from =0;
	}



	return;

}