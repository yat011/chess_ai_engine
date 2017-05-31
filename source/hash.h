#include "Board.h"
#include "MoveNode.h"

#ifndef HASH_H
#define HASH_H
#define MAXSIZE 15014323

void initHashData();

extern uint64_t pieceKey[120][13];
extern uint64_t castleKey [16];
extern uint64_t enpassKey[16];
extern uint64_t sideKey;
enum {alphaFlag=0,betaFlag,exactFlag};
class boardHashTable{

	
public:
	//unsigned int size;
	unsigned int hit;
	unsigned int collision;
	uint64_t boardKey[MAXSIZE];
	MoveNode node[MAXSIZE];
	uint8_t flag [MAXSIZE];
	uint8_t depth [MAXSIZE];
	int score [MAXSIZE];
	
	//uint8_t reference[MAXSIZE];
	//====================== method===============

	//void add (const uint64_t &key, const MoveNode & nod);
	
	
	boardHashTable(){
		this->clear();
	}
	MoveNode getMove(const uint64_t &key);
	int getRecord(const uint64_t &key, MoveNode ** move, int &score, const int &alpha, const int &beta, const int &depth);
	void addRecord(const uint64_t &key,  const MoveNode& best, const int &score , const int &flag, const int &depth); 
	void clear (){
		memset(this,0,sizeof(boardHashTable));
		
	}
};









#endif