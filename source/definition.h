
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "timeRecord.h"

#include <memory.h>



#ifndef DEFINITION
#define DEFINITION




//Constant
#define INVALID 121
#define NOENPASS 14
#define INIFINITY 10000000
#define NOTSCORE  10000001
#define CHECKMATE 1000000
#define LISTSIZE 150
#define VERSION "1.01"
enum {CONTINUE=0 , MATESTATE ,FIFTYDRAW ,THREEFOLD, MATDRAW, STALEMATE};
// FEN test case
#define startFEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define F1 "K7/8/8/3Q4/4q3/8/8/7k b - - 0 1" 
#define F2 "r2bk2r/1P4Q1/1R3B2/R7/8/P7/P7/R3K2R w KQq - 0 2 "
#define F3 "8/4p3/4p3/pP6/6pP/8/8/8 b - h6 0 2 "
#define F4 "rnbqkbnr/pppppppp/8/8/8/3P4/PPP1PPPP/RNBQKBNR b KQkq - 0 1 "
#define F5 "rnbqkbnr/pppppppp/8/8/8/2P5/PPQPPPPP/RNB1KBNR b KQkq 0 0 2   "
#define F6 "rnbqkbnr/ppp1pppp/3p4/8/8/5N2/PPPPPPPP/RNBQKB1R w KQkq - 0 2 "
#define F7 "rnbqkbnr/ppp1pppp/8/3p4/Q7/2P5/PP1PPPPP/RNB1KBNR b KQkq - 1 2 "
#define F8 "rnbqkbnr/ppppp1pp/5p2/3p3Q/8/2P5/PPPP1PPP/RNB1KBNR b KQkq - 1 2 "
#define F9 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define case2 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -"
#define case3 "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -"
#define PASSER "8/2p5/3p4/KP5r/1R5k/8/4P1P1/8 w - -"
#define importantcase3b "8/2p5/3p4/KP5r/1R2Pp1k/8/6P1/8 b - e3"
#define importantcase3c "8/2p5/3p4/KP5r/1R3pPk/8/4P3/8 b - g3"
#define importantcase3d "8/8/K2p4/1Pp4r/1R3p1k/8/4P1P1/8 w - c6"
#define importantcase3e "8/2p5/8/KP1p3r/1R3pPk/8/4P3/8 w - - 0 2 "
#define case3f "8/8/3p4/1Pp4r/1K5k/1R3p2/4P1P1/8 w - c6 0 2 "
#define case3g "8/2p5/K6r/1P1p4/3R1p1k/8/4P1P1/8 w - - 0 3 "
#define case3h "8/2p5/3p4/Kr6/1R3p1k/4P3/6P1/8 w - -"
#define case4 "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"
#define doubleblock "r4k1r/p1ppqpb1/bn2pQpB/3PN3/1p2P3/2N4p/PPP1BPPP/R3K2R b KQ -"
#define stepC "rnbqkbnr/pppppppp/8/8/8/2P5/PP1PPPPP/RNBQKBNR b KQkq - 0 2 "
#define step3 "rnbqkbnr/1ppppppp/p7/8/8/2P5/PP1PPPPP/RNBQKBNR w KQkq - 0 2 "
#define step4 "rnbqkbnr/1ppppppp/p7/8/Q7/2P5/PP1PPPPP/RNB1KBNR b KQkq - 0 2 "
#define checkMate "rnbqkbnr/pppp1ppp/4p3/8/6P1/5P2/PPPPP11P/RNBQKBNR b KQkq - 0 2 "
#define WAC1 "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"
#define WAC2 "r1b1k2r/1pppnpp1/2n2q2/2b5/3NP3/2P1B3/1P3PPP/RN1QKB1R w KQkq - 0 1"
#define promoteState "r3k2r/pPppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPpP/R3K2R b KQkq -"
#define castleEx "r3k2r/p1ppqp11/bn2pnp1/3PN3/1p2P3/2N2Q1p/P1PBBPpP/R3K2R w KQkq -"

#define BLOCKFLAG 2
#define ATTACKFLAG 1
#define KINGRETRICTFLAG 4
#define CHECKFLAG 8

//Macro
#define checkPath(q) (q&8)
#define blockPath(q) (q>>4)
#define isBlocking(q) (q&2)
#define canAttacked(q) (q&1)
#define noKingMove(q) (q&4)
#define sideType (s,t) (s*6+t)

//pos tranlate
#define pos120to64(p) (((p)/10-2)*8+((p)%10-1))
#define reverse64(q) ((7-(q)/8)*8 +(q)%8)
#define getPosition120(p) (((p)/8 +2) *10 +(p) % 8 +1)
//parse move to 120
#define move120(q) ((*(q+1) -'0'+1)*10+(*q)-'a'+1)

//64bit 0000 0000  .........   0<=q<64
#define bitPosSet(b,q) ((b)|=((uint64_t)1<<(q)))
#define bitPosClear(b,q) ((b) &= (0xffffffffffffffff ^ ((uint64_t)1<< (q))))
#define bitPosPeek(b,q) (((b)&((uint64_t)1<<(q)))?1:0)


//#define DEBUG
#ifndef DEBUG
#define CONFIRM(n)
#else
#define CONFIRM(n) \
	if(!(n)){\
	printf("%s Failed",#n);\
	exit(1);}
#endif





enum {  WKing=0,WQueen, WRook,WBishop,WKnight, WPawn,BKing,BQueen,BRook,BBishop,BKnight,BPawn, Empty, Offboard} ;
//white uppercase, black Lowercase

enum { White =0, Black};
enum { BQueenCastle =1, BKingCastle=2, WQueenCastle=4, WKingCastle=8};
enum {NoPromote=0, QPromote,NPromote,RPromote,BPromote};


extern char pieceName[];
//piece direction
extern const int8_t dir1[];
extern const int8_t dir2[];
extern const uint8_t bdir1[];
extern const uint8_t bdir2[];
extern const int8_t dir3[];


//For move order
extern const int promotePri[];
extern const int victimPri[];
extern int  victimAttackPri[][6];

// e.g. kingPiece[white] == WKing
extern const uint8_t kingPiece[]; 
extern const uint8_t queenPiece[];
extern const uint8_t bishopPiece[];
extern const uint8_t knightPiece[];
extern const uint8_t rookPiece[];
extern const uint8_t pawnPiece[];

extern const uint8_t pieceSide[];
extern GameTime timeInfo;
//Bit Mask
extern uint64_t isolatedMask[120];
extern uint64_t passerMask[2][120];
extern uint64_t colMask[2][120];
extern uint64_t defendMask[2][120];
extern uint64_t promoteMask[2];


//inline Funtion
inline std::string prPos(uint8_t a){
	std::string b="";
	 b+= (a %10 -1 +'a');
	b+=(a/10 -1) + '0';
	
	return  b;
}



inline void errorIfn(int i, char * msg){
    if (i ==0 ){
        fprintf(stderr,"Error: %s\n", msg);
        exit(1);
    }
    
}
//
class Board;



//function
void test(Board & a, int cut);

void printBitBoard( uint64_t a);
void initData();
#endif