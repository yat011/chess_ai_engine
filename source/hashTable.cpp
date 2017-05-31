#include "definition.h"
#include "hash.h"
#include "Board.h"
#include "MoveNode.h"
#include <time.h>
#include <stdlib.h>


#define PRIME 25999

uint64_t pieceKey[120][13];
uint64_t castleKey [16];
uint64_t enpassKey[16];
uint64_t sideKey;
void initHashData(){
	srand(time(NULL));
	for (int i =0 ; i<64; i++){
		uint8_t pos = getPosition120(i);
		for (int j =0; j <13; j++){
			pieceKey[pos][j] = (uint64_t) rand() ;			
			pieceKey[pos][j] |= (uint64_t)rand()<<15 ;			
			pieceKey[pos][j] |= (uint64_t)rand()<<30;			
			pieceKey[pos][j] |= (uint64_t)rand()<<45;						
		}
		
	}
	for (int i =0 ; i<16; i++){
		castleKey [i]=(uint64_t)rand();
		castleKey [i]|= (uint64_t)rand()<<15 ;
		castleKey [i]|= (uint64_t)rand()<<30;
		castleKey [i]|= (uint64_t)rand()<<45;
		
	}
	memset(enpassKey ,0 , sizeof (uint64_t)*16);
	for (int i =1 ; i<=8; i++){
		enpassKey [i]=(uint64_t)rand();
		enpassKey[i]|= (uint64_t)rand()<<15 ;
		enpassKey [i]|= (uint64_t)rand()<<30;
		enpassKey [i]|= (uint64_t)rand()<<45;
		
	}

	sideKey=(uint64_t)rand();
	sideKey|= (uint64_t)rand()<<15 ;
	sideKey|= (uint64_t)rand()<<30;
	sideKey|= (uint64_t)rand()<<45;
}





MoveNode boardHashTable::getMove(const uint64_t &key){
	uint64_t index = key%MAXSIZE;
	if (key == boardKey[index] ){
		return node[index];

	}

    return MoveNode();

}



int boardHashTable::getRecord(const uint64_t &key, MoveNode ** move, int &score, const int &alpha, const int &beta, const int &depth){
	uint64_t index = key%MAXSIZE;
	if (key == boardKey[index] ){
		
		*move =node+index;
		
		if (this->depth[index] >=depth){
			hit++;
			CONFIRM(this->depth[index]>=depth);
			score = this->score[index];
			switch (flag[index]){
			case alphaFlag: 
				if (score <alpha){
					score = alpha;
					return 1;
				}
				break;
			case betaFlag:
				if (score >beta){
					score = beta;
					return 1;
				}
                    return 2;
				break;
			case exactFlag:
				return 1;
				break;
			default:
				CONFIRM(0);
				return 0;


			}

		}
		return 0;



	}

	return 0;
}



void boardHashTable::addRecord(const uint64_t &key,const MoveNode& best, const int &score , const int &flag, const int &depth){
	uint64_t index = key % MAXSIZE;
	if (boardKey[index]){
		collision++;
	}
    if (boardKey[index]==key && this->flag[index]==flag && this->depth[index] > depth){
        return ;
        
    }
	boardKey[index] =key;
	node[index]=best;
	this->flag[index] = flag;
	this ->score[index] = score;
	this->depth[index]=depth;
	


}

