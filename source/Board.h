

#include "MoveNode.h"

#ifndef BOARD_H
#define BOARD_H

int heuristic(const MoveNode &a, const int &plys);

extern MoveNode historyList[];
extern uint8_t captureHistory[]; 
extern uint8_t fiftyHistory[];
extern uint8_t castleHistory[];
extern uint8_t enpassHistory[];
extern uint64_t keyHistory[];




class Board {	
private:
	void genPawnMove(MoveList &moveList);
	void genBishopMove (MoveList& moveList);
	void genRookMove (MoveList& moveList);
	void genKnightMove (MoveList& moveList);
	void genQueenMove (MoveList& moveList);
	void genKingMove (MoveList& moveList);
	
	void genPawnMoveC(MoveList &moveList);
	void genBishopMoveC (MoveList& moveList);
	void genRookMoveC (MoveList& moveList);
	void genKnightMoveC (MoveList& moveList);
	void genQueenMoveC (MoveList& moveList);
	void genKingMoveC (MoveList& moveList);

    bool capEnpassSafe(uint8_t capedPos ,uint8_t moverPos);
	void movePiece(int from, int to);
    void popPiece(int pos, int PType);
	void addPiecePos(int pos, int PType);
public:
	
    
	uint8_t position[120];
	uint8_t attacked[120];
	uint8_t sideBoard[120];
	uint64_t pawnBitBoard[3];
	uint64_t hashcode;
    uint8_t enpass : 4; // column
    uint8_t castle:4;// 0 0 0 0 ; WkingSide WqueenSide BK BQ
    uint8_t side:1;
	uint8_t fiftymove;
	uint8_t pieceNumber[12];
	uint8_t majorNumber[3]; // 0: White 1: black 2:others
	uint8_t totalNum[2];
    uint8_t pieceList[12][10];// e.g. pieceList[WPawn][1] = 30; record each piece position
	uint8_t captured:1;
    uint8_t beingChecked;
	
	unsigned int fullmove;
	unsigned int halfmove;
	unsigned int plys;
	int stateScore;
	

	//-------------------------method-----------------------------
	Board();
    void parseFEN(char FEN[]);
    int parseMove(char move[]);
	int validateMove(char move[]);
	void init();
    void startGame();
	// move
    void performMove(MoveNode move);
	void undoMove();

    
   // gen Node
	void genNodes (MoveList& moveList);	
	void genCaps (MoveList & moveList);
    void genKAttacked(); // King Attacked Board(for generating move)
    bool bposAttacked(uint8_t tposition); //check a square if can be attacked
    
	//for null move pruning
	void performNull();
	void undoNull();
    //
    int evaluation();   
    bool quiescent();    
    bool isRepeat();
	bool materialDraw();
	bool threefold();
    bool enPromoting();
	bool myPromoting();
   


	//Draw? Mate? 
    int getState();
	//print function
	void printBoard();
	void printAttacked();
	void printSideBoard();

	//Enpassant
	uint8_t getEnpass();
	
} ;







#endif