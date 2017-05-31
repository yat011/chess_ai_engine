#include "definition.h"
#include "Board.h"
#include "MoveNode.h"

void preft(Board &a, int depth);
unsigned int leaf=0; int cutoffdep =1; 
static long startTime;
void test(Board & a, int cut){
	int depth =0; cutoffdep = cut;
	
	leaf =0;
	//preft(a,0);
	//printf("\n%d capture:%d check:%d checkmate:%d\n ", leaf,capture,check,checkmate);

	MoveList temp;
	a.genNodes(temp);
	Board copy = a;
	startTime = GameTime::getMs();
	for (int i = 0; i<temp.count; i++){
		
		
		
		unsigned int last = leaf;
		a.performMove(temp.node[i]);
		preft(a,depth+1);
		a=copy;
		temp.node[i].printNode(a.position[temp.node[i].from]);
		printf("%d\n", leaf- last);
	}
	printf("\n%d\n", leaf);
	printf("Time %llu \n", GameTime::getMs()-startTime);
	

}


void preft(Board &a, int depth){
	
	
	if (depth == cutoffdep) {
		
		/*if ((chessPath(a.attacked[a.pieceList[BKing][0]])|| chessPath(a.attacked[a.pieceList[WKing][0]]))){
				check++;
				
				
		}
		if (temp.size()==0){
			checkmate++;
			/*a.printSideBoard();
			a.printAttacked();
			a.printBoard();
			system("pause");
			std::vector<MoveNode> temp2;

			a.genNodes(temp2);
			printf("%d\n",temp2.size());
			system("pause");

		}*/
		leaf++;
		return ;
	}
	MoveList temp;

	a.genNodes(temp);
	int b=0;
	Board copy = a;
	for (int i = 0; i<temp.count; i++){
		
		if (depth==cutoffdep-1){
			
	
		}
		a.performMove(temp.node[i]);
		
		preft(a,depth+1);
		a=copy;
		
		
	
	
	}
	return ;

}