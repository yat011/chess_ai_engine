#include "definition.h"
#include "Board.h"
#include "evaluationData.h"
const int dualBishop =20;
const int isolatedSc = 5;
const int mobility =5;
const int pawnMo =3;
const int defendedPawn =10;
const int passerSc[2][10] ={{0,0,0,5,10,15,25,50,100,0},{0,0,0,100,50,25,15,10,5,0}};
const int left4 [] ={200, 100,40,20, 10};

//Isolated and passer
uint64_t isolatedMask[120];
uint64_t passerMask[2][120];
uint64_t colMask[2][120];
uint64_t defendMask[2][120];
uint64_t promoteMask[2];
int Board::evaluation (){
	int numS= 0; uint8_t tpos;

	if (materialDraw()||isRepeat()){

		return 0;
	}
	

	numS = 10000*(pieceNumber[WKing]-pieceNumber[BKing])+ 1000*(pieceNumber[WQueen] - pieceNumber[BQueen])+ 500*(pieceNumber[WRook] - pieceNumber[BRook])+ 300 *(pieceNumber[WBishop]-pieceNumber[BBishop])+ 300*(pieceNumber[WKnight]-pieceNumber[BKnight])+100*(pieceNumber[WPawn]-pieceNumber[BPawn]) ;

	if (beingChecked){
		numS+= side?50:-50;

	}
	// two or more bishop
	if (pieceNumber[WBishop]>=2){
		numS+=dualBishop;
	}
	if (pieceNumber[BBishop]>=2){
		numS-=dualBishop;
	}



	//isolated and passer and doubled, defend (pawn)
	//White
	for (int i =0 ; i<pieceNumber[WPawn]; i++){
		tpos = pieceList[WPawn][i];
		if ( !(pawnBitBoard[White] & isolatedMask[tpos])){
			numS-=isolatedSc;
		}
		if ( !(pawnBitBoard[Black] & passerMask[White][tpos])){
			numS+=passerSc[White][tpos/10];
		}
		if ( !(pawnBitBoard[White] & colMask[White][tpos])){
			
			numS+=pawnMo;
		}
		if ( (pawnBitBoard[White] & defendMask[White][tpos])){
			numS+=defendedPawn;
		}
	}
	//Black
	for (int i =0 ; i<pieceNumber[BPawn]; i++){
		tpos = pieceList[BPawn][i];
		if ( !(pawnBitBoard[Black] & isolatedMask[tpos])){
			numS+=isolatedSc;
		}
		if ( !(pawnBitBoard[White] & passerMask[Black][tpos])){
			numS-=passerSc[Black][tpos/10];
		}
		if ( !(pawnBitBoard[Black] & colMask[Black][tpos])){
			numS-=pawnMo;
		}
		if ( (pawnBitBoard[Black] & defendMask[Black][tpos])){
			numS-=defendedPawn;
		}
	}


	//rook queen block by mobility
	for (int i =0 ; i<pieceNumber[WRook]; i++){
		tpos = pieceList[WRook][i];
		if (!(pawnBitBoard[White] & colMask[White][tpos])){
			numS+=mobility;
			if (!(pawnBitBoard[Black]&colMask[White][tpos])){
				numS+=mobility;

			}
		}
	}

	for (int i =0 ; i<pieceNumber[BRook]; i++){
		tpos = pieceList[BRook][i];
		if (!(pawnBitBoard[Black] & colMask[Black][tpos])){
			numS-=mobility;
			if (!(pawnBitBoard[White]&colMask[Black][tpos])){
				numS-=mobility;

			}
		}
	}
	//queen
	for (int i =0 ; i<pieceNumber[WQueen]; i++){
		tpos = pieceList[WQueen][i];
		if (!(pawnBitBoard[White] & colMask[White][tpos])){
			numS+=mobility;
			if (!(pawnBitBoard[Black]&colMask[White][tpos])){
				numS+=mobility;

			}
		}
	}

	for (int i =0 ; i<pieceNumber[BQueen]; i++){
		tpos = pieceList[BQueen][i];
		if (!(pawnBitBoard[Black] & colMask[Black][tpos])){
			numS-=mobility;
			if (!(pawnBitBoard[White]&colMask[Black][tpos])){
				numS-=mobility;

			}
		}
	}

	//important piece
	//white
	
	if ( majorNumber[White] <=4){
		numS -= left4[majorNumber[White]];

	}
	if (majorNumber[White]<=3 || totalNum[White] <= 8){
		numS+= kingEndScore[White][pieceList[WKing][0]];

	}
	else{
		numS+= posDataScore[WKing][pieceList[WKing][0]];
	}
	
	//black
	
	if ( majorNumber[Black]<=4){
		numS += left4[ majorNumber[Black]];

	}			
	if ( majorNumber[Black]<=3 ||totalNum[Black] <=8){
		numS+= kingEndScore[Black][pieceList[BKing][0]];
	}
	else{
		numS+= posDataScore[BKing][pieceList[BKing][0]];
	}


	numS+=stateScore;

	if (side){
		return -numS;
	}
	return numS;


}



int Board::getState(){
	MoveList temp;
	genNodes(temp);
    
    
	if (beingChecked && temp.count ==0){
		return MATESTATE;
	}
	else if ( fiftymove >100 ){
		return FIFTYDRAW;
	}else if (threefold()){
		return THREEFOLD;
        
	}else if (materialDraw()){
		return MATDRAW;
	}
	else if (temp.count==0 ){
		return STALEMATE;
	}
	return 0;
    
}



bool Board::isRepeat(){
	int i =halfmove-fiftymove-1;
	i = i >= 0? i :0;
	for ( ; i< halfmove; i++){
		if (keyHistory[i]==hashcode){
			return true;
		}

	}
	return false;
}

bool Board::threefold(){
	int count =0; int i =halfmove-fiftymove-1;
	i = i >= 0? i :0;
	for ( ; i< halfmove; i++){
		if (keyHistory[i]==hashcode){
			if ((++count) >=2)
				return true;
		}

	}
	return false;

}


bool Board::materialDraw(){

	if (pieceNumber	[WPawn] || pieceNumber[BPawn])
		return false;

	if (!pieceNumber[WQueen]&& !pieceNumber[BQueen] && !pieceNumber[WRook] && !pieceNumber[BRook]){
		// no bishop
		if (!pieceNumber[WBishop] && !pieceNumber[BBishop]){
			//king and knight versus king or king versus king
			if ((pieceNumber [WKnight] <=1 && !pieceNumber[BKnight])|| (!pieceNumber[WKnight] && pieceNumber[BKnight] <=1)){
				return true;

			}
		}
		//no knight
		if (!pieceNumber[BKnight] &&!pieceNumber[WKnight]){
			if ((pieceNumber [WBishop] <=1 && !pieceNumber[BBishop])|| (!pieceNumber[WBishop] && pieceNumber[BBishop] <=1)){
				return true;

			}


		}



	}

	return false;
}

bool Board::enPromoting(){
    uint8_t opside = 1^side;
    if (promoteMask[opside] & pawnBitBoard[opside]){
        
        return true;
        
    }
    return false;
}


bool Board::myPromoting(){
    
    if (promoteMask[side] & pawnBitBoard[side]){
        
        return true;
        
    }
    return false;
}



