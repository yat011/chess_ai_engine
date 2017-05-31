#include "definition.h"
#include "Board.h"
#include "MoveNode.h"
#include <stdint.h>


const int8_t dir1[]={+10,-10,+1,-1};
const uint8_t bdir1[]={0x10,0x20,0x30,0x40};
const int8_t dir2[]={+9,+11,-9,-11};
const uint8_t bdir2[]={0x50,0x60,0x70,0x80};
const int8_t dir3[]={+21,+12,-8,-19,-21,-12,+8,+19};
const int victimPri[]={10000,900,500,300,300,100};


const uint8_t kingPiece[]={WKing,BKing};
const uint8_t queenPiece[]={WQueen,BQueen};
const uint8_t bishopPiece[]={WBishop,BBishop};
const uint8_t knightPiece[]={WKnight,BKnight};
const uint8_t rookPiece[]={WRook,BRook};
const uint8_t pawnPiece[] = {WPawn, BPawn};


int victimAttackPri[12][6];

const int promotePri[]={450,150,250,150};


// heuristic 
extern MoveNode killer [12][2]; 
extern uint8_t history[][120][120];

uint8_t checking;

int heuristic(MoveNode &a, const int &plys){
	int result =0;

	if (a.equal(killer[plys])){
		result= 50;
	}
	else if (a.equal(killer[plys] +1)){
		result= 45;
	}
	result += history[a.sideOfMove][a.from][a.to];

	return result;

}

void Board::genNodes (MoveList & moveList){


	


	checking=beingChecked;

	
	
	Board::genPawnMove(moveList);
	Board::genBishopMove(moveList);
	Board::genRookMove(moveList);
	Board::genKnightMove(moveList);
	Board::genQueenMove(moveList);
	Board::genKingMove(moveList);
	
	
}

//
void Board::genPawnMove (MoveList & moveList){
	if (checking >=2) return;
	int type; uint8_t tposition; uint8_t target; uint8_t skipFlag=0;
	uint8_t blockDir;
	uint8_t tenpass=getEnpass();

	if (side == White){
		//===================================White==================================================
		type =WPawn;

		for (int i =0 ; i<pieceNumber[type] ; i++){//=================================

			tposition=pieceList[type][i];

			CONFIRM(tposition!=INVALID);


			uint8_t tRow = tposition/10;
			blockDir=blockPath(attacked[tposition]);


			//================capture Move===================
			for (int p=0; p <2; p++){
				target =tposition+dir2[p];


				//==================ENPASSANT!!!!==================
				if (target==tenpass ){
					if (checking && CHECKFLAG^checkPath(attacked[target-10]) ) continue; // being chessed;
					if (isBlocking(attacked[tposition]) ){
						if (BLOCKFLAG^isBlocking(attacked[target]) || blockDir!=blockPath(attacked[target])){
							continue;  // on blocking Path
						}
					}// on blocking Path
					if (capEnpassSafe( target -10 , tposition)){
						MoveNode tempNode(tposition,target,side);

						tempNode.capture=1;					
						tempNode.priority+=victimAttackPri[WPawn][WPawn];

						moveList.push_back(tempNode);
					}

				}
				else{
					if (checking && CHECKFLAG^checkPath(attacked[target]) ) continue; // being chessed;
					if (isBlocking(attacked[tposition]) ){
						if (BLOCKFLAG^isBlocking(attacked[target]) || blockDir!=blockPath(attacked[target])){
							continue;  // on blocking Path
						}
					}// on blocking Path
					if (sideBoard[target] == Black  ) {


						//enpass or exisit enemy
						MoveNode tempNode(tposition,target,side);
						tempNode.capture=1; 
						tempNode.priority+=victimAttackPri[position[target]][WPawn];

						//promotion!!!
						if(tRow ==8 ){
							int tpriority=tempNode.priority;
							for (int m = 1 ; m <=4; m++){
								tempNode.promote=m;
								tempNode.priority= tpriority+promotePri[m-1];
								moveList.push_back(tempNode);
							}
						}
						else  moveList.push_back(tempNode);

					}
				}

			}
			//======================Normal Move

			skipFlag=0;
			target =tposition+10;
			if (checking && CHECKFLAG^checkPath(attacked[target]) ) skipFlag=1; // being chessed;
			if (isBlocking(attacked[tposition])){
				if (BLOCKFLAG^isBlocking(attacked[target])|| blockDir!=blockPath(attacked[target])){
					skipFlag=1;   // on blocking Path
				}
			}


			if (1^skipFlag && position[target] == Empty ) {

				MoveNode tempNode(tposition,target,side);
				tempNode.priority+=heuristic(tempNode,plys);
				if(tRow ==8 ){
					int tpriority=tempNode.priority;
					for (int m = 1 ; m <=4; m++){
						tempNode.promote=m;
						tempNode.priority=tpriority+promotePri[m-1];
						moveList.push_back(tempNode);
					}
				}
				else moveList.push_back(tempNode);



			}
			//========================Move two

			skipFlag=0;
			target =tposition+20;

			if (checking && CHECKFLAG^checkPath(attacked[target]) ) skipFlag=1; // being chessed;
			if (isBlocking(attacked[tposition]) ){
				if (BLOCKFLAG^isBlocking(attacked[target]) || blockDir!=blockPath(attacked[target])){
					skipFlag=1;   // on blocking Path
				}// on blocking Path
			}


			if (1^skipFlag && tRow ==3 && position[target-10] == Empty && position[target] == Empty    ) {

				MoveNode tempNode(tposition,target,side);
				tempNode.priority+=heuristic(tempNode,plys);
				tempNode.priority+=80;
				tempNode.enpass = (tposition %10 );

				moveList.push_back(tempNode);

			}
		}



	}
	else{
		//========================================Black------------

		//=======================================pawn-------------
		type = BPawn;
		for (int i =0 ; i<pieceNumber[type]  ; i++){
			tposition=pieceList[type][i];
			CONFIRM(tposition!=INVALID);


			uint8_t tRow = tposition/10;
			blockDir=blockPath(attacked[tposition]);

			//capture Move
			for (int p=2; p <4; p++){
				target =tposition+dir2[p];

				if (target==tenpass ){
					if (checking && CHECKFLAG^checkPath(attacked[target+10]) ) continue; // being chessed;
					if (isBlocking(attacked[tposition]) ){
						if (BLOCKFLAG^isBlocking(attacked[target]) || blockDir!=blockPath(attacked[target])){
							continue;  // on blocking Path
						}
					}// on blocking Path
					if (capEnpassSafe( target +10 , tposition)){


						MoveNode tempNode(tposition,target,side);
						tempNode.capture=1;
						tempNode.priority+=victimAttackPri[WPawn][WPawn];
						moveList.push_back(tempNode);


					}

				}
				else{
					if (checking && CHECKFLAG^checkPath(attacked[target]) ) continue; // being chessed;
					if (isBlocking(attacked[tposition]) ){
						if (BLOCKFLAG^isBlocking(attacked[target]) || blockDir!=blockPath(attacked[target])){
							continue;
						}
					}


					if ( sideBoard[target]==White  ) {



						MoveNode tempNode(tposition,target,side);
						tempNode.capture=1;
						tempNode.priority+=victimAttackPri[position[target]][WPawn];
						if(tRow ==3 ){
							int tpriority=tempNode.priority;
							for (int m = 1 ; m <=4; m++){
								tempNode.promote=m;
								tempNode.priority=tpriority+promotePri[m-1];
								moveList.push_back(tempNode);
							}
						}
						else  moveList.push_back(tempNode);


					}
				}
			}



			//============Normal Move



			target =tposition-10;

			skipFlag=0;

			if (checking && CHECKFLAG^checkPath(attacked[target]) ) skipFlag=1; // being chessed;
			if (isBlocking(attacked[tposition])){  // on blocking Path
				if (BLOCKFLAG^isBlocking(attacked[target]) || blockDir!=blockPath(attacked[target])){
					skipFlag=1;   // on blocking Path
				}
			}

			if (1^skipFlag && position[target] == Empty ) {


				MoveNode tempNode(tposition,target,side);
				tempNode.priority+=heuristic(tempNode,plys);
				if(tRow ==3 ){
					int tpriority=tempNode.priority;
					for (int m = 1 ; m <=4; m++){
						tempNode.promote=m;
						tempNode.priority=tpriority+promotePri[m-1];
						moveList.push_back(tempNode);
					}
				}
				else moveList.push_back(tempNode);


			}
			//=========================move two

			target =tposition-20;

			skipFlag=0;

			if (checking && CHECKFLAG^checkPath(attacked[target]) ) skipFlag=1; // being chessed;
			if (isBlocking(attacked[tposition])){
				if ( BLOCKFLAG^isBlocking(attacked[target]) || blockDir!=blockPath(attacked[target])){
					skipFlag=1;   // on blocking Path
				}

			}

			if (1^skipFlag && tRow ==8  && position[target+10] == Empty && position[target] == Empty ){

				MoveNode tempNode(tposition,target,side);
				tempNode.priority+=heuristic(tempNode,plys);
				tempNode.priority+=80;
				tempNode.enpass = (tposition %10 );
				moveList.push_back(tempNode);

			}
		}
	}
}




void Board::genBishopMove (MoveList & moveList){
	if (checking >=2) return;
	uint8_t blockDir; 
	uint8_t type = side*6 +WBishop;
	for (int p = 0 ; p<pieceNumber[type]; p++){

		uint8_t tposition=pieceList[type][p];
		CONFIRM(tposition!=INVALID);

		blockDir=blockPath(attacked[tposition]);
		for (int d =0; d<4; d++){
			uint8_t target = tposition+dir2[d];


			while (position[target] !=Offboard){

				if (checking && CHECKFLAG^checkPath(attacked[target]) ) {
					if (position [target] != Empty)
						break;
					target +=dir2[d];
					continue; // being chessed;
				}
				if (isBlocking(attacked[tposition])) {
					if (BLOCKFLAG^isBlocking(attacked[target]) || blockDir!=blockPath(attacked[target])){

						break; 
					}
				}

				//Normal
				if (position[target] ==Empty ){

					MoveNode tempNode(tposition,target,side);
					tempNode.priority+=heuristic(tempNode,plys);
					moveList.push_back(tempNode);
					target +=dir2[d];

				}
				//capture Move
				else if (sideBoard[target]== (1^side)){

					MoveNode tempNode(tposition,target,side);
					tempNode.priority+=victimAttackPri[position[target]][WBishop];
					tempNode.capture=1;

					moveList.push_back(tempNode);
					break;

				}
				else{
					CONFIRM(sideBoard[target]==side);
					break;
				}


			}
		}


	}


}


void Board::genRookMove (MoveList & moveList){
	if (checking >=2) return;
	uint8_t type = side*6 +WRook;
	for (int p = 0 ; p<pieceNumber[type]; p++){
		uint8_t tposition=pieceList[type][p];
		CONFIRM(tposition!=INVALID);

		uint8_t blockDir=blockPath(attacked[tposition]);

		for (int d =0; d<4; d++){
			uint8_t target = tposition+dir1[d];
			while (position[target] !=Offboard ){

				if (checking && CHECKFLAG^checkPath(attacked[target]) ) {
					if (position [target] != Empty)
						break;
					target +=dir1[d];				
					continue; // being chessed;
				}
				if (isBlocking(attacked[tposition]) ) {
					if (BLOCKFLAG^isBlocking(attacked[target]) || blockDir!=blockPath(attacked[target])){
						break;
					}
				}
				//Normal
				if (position[target] ==Empty ){

					MoveNode tempNode(tposition,target,side);
					tempNode.priority+=heuristic(tempNode,plys);
					moveList.push_back(tempNode);
					target +=dir1[d];

				}
				//capture Move
				else if (sideBoard[target]== (1^side)) {

					MoveNode tempNode(tposition,target,side);
					tempNode.capture=1;
					tempNode.priority+=victimAttackPri[position[target]][WRook];
					moveList.push_back(tempNode);
					break;

				}
				else{
					CONFIRM(sideBoard[target]==side);
					break;
				}

			}
		}


	}

}

void Board::genKnightMove (MoveList & moveList){
	if (checking >=2) return;
	uint8_t type = side*6 +WKnight;
	for (int p = 0 ; p<pieceNumber[type]; p++){
		uint8_t tposition=pieceList[type][p];
		CONFIRM(tposition!=INVALID);


		uint8_t blockDir=blockPath(attacked[tposition]);
		for (int d =0; d<8; d++){
			uint8_t target = tposition+dir3[d];
			if (position[target] == Offboard) continue;
			if (checking && CHECKFLAG^checkPath(attacked[target]) ) {

				continue; // being chessed;
			}
			if (isBlocking(attacked[tposition])) {
				if (BLOCKFLAG^isBlocking(attacked[target]) || blockDir!=blockPath(attacked[target])){

					continue; // on blocking Path
				}
			}
			//Normal
			if (position[target] ==Empty ){

				MoveNode tempNode(tposition,target,side);
				tempNode.priority+=heuristic(tempNode,plys);
				moveList.push_back(tempNode);


			}
			//capture Move
			else if (sideBoard[target]== (1^side)){

				MoveNode tempNode(tposition,target,side);
				tempNode.capture=1;
				tempNode.priority+=victimAttackPri[position[target]][WKnight];
				moveList.push_back(tempNode);


			}






		}
	}


}

void Board::genQueenMove (MoveList & moveList){
	if (checking >=2) return;
	uint8_t type = side*6 +WQueen;
	for (int p = 0 ; p<pieceNumber[type]; p++){
		uint8_t tposition=pieceList[type][p];
		CONFIRM(tposition!=INVALID);

		uint8_t blockDir=blockPath(attacked[tposition]);
		for (int d =0; d<4; d++){
			uint8_t target = tposition+dir2[d];


			while (position[target] !=Offboard){

				if (checking && CHECKFLAG^checkPath(attacked[target]) ) {
					if (position [target] != Empty)
						break;
					target +=dir2[d];
					continue; // being chessed;
				}
				if (isBlocking(attacked[tposition])) {
					if (BLOCKFLAG^isBlocking(attacked[target]) || blockDir!=blockPath(attacked[target])){
						break;
					}
				}
				//Normal
				if (position[target] ==Empty){

					MoveNode tempNode(tposition,target,side);
					tempNode.priority+=heuristic(tempNode,plys);
					moveList.push_back(tempNode);
					target +=dir2[d];

				}
				//capture Move
				else if (sideBoard[target]== (1^side)){

					MoveNode tempNode(tposition,target,side);
					tempNode.capture=1;
					tempNode.priority+=victimAttackPri[position[target]][WQueen];
					moveList.push_back(tempNode);
					break;

				}
				else{
					CONFIRM(sideBoard[target]==side);
					break;
				}


			}

		}
		//vertical and hori
		for (int d =0; d<4; d++){
			uint8_t target = tposition+dir1[d];


			while (position[target] !=Offboard ){

				if (checking && CHECKFLAG^checkPath(attacked[target]) ) {
					if (position [target] != Empty)
						break;
					target +=dir1[d];
					continue; // being chessed;
				}
				if (isBlocking(attacked[tposition])) {
					if (BLOCKFLAG^isBlocking(attacked[target]) || blockDir!=blockPath(attacked[target])){
						break;
					}
				}
				//Normal
				if (position[target] ==Empty ){

					MoveNode tempNode(tposition,target,side);
					tempNode.priority+=heuristic(tempNode,plys);
					moveList.push_back(tempNode);
					target +=dir1[d];

				}
				//capture Move
				else if (sideBoard[target]== (1^side)) {

					MoveNode tempNode(tposition,target,side);
					tempNode.capture=1;
					tempNode.priority+=victimAttackPri[position[target]][WQueen];
					moveList.push_back(tempNode);
					break;

				}
				else{
					CONFIRM(sideBoard[target]==side);
					break;
				}

			}


		}
	}


}


void Board::genKingMove (MoveList & moveList){
	uint8_t type = side*6 +WKing;

	for (int p = 0 ; p<pieceNumber[type]; p++){
		uint8_t tposition=pieceList[type][p];
		CONFIRM(tposition!=INVALID);


		for (int d =0; d<4; d++){
			uint8_t target = tposition+dir2[d];
			if (position[target] == Offboard) continue;
			if ( noKingMove(attacked[target])||bposAttacked(target)) continue;
			//Normal
			if (position[target] ==Empty){
				MoveNode tempNode(tposition,target,side);
				tempNode.priority+=heuristic(tempNode,plys);
				moveList.push_back(tempNode);


			}//capture Move					
			else if (sideBoard[target]==(side^1) ){

				MoveNode tempNode(tposition,target,side);
				tempNode.capture=1;
				tempNode.priority+=victimAttackPri[position[target]][WKing];
				moveList.push_back(tempNode);


			}





		}


		for (int d =0; d<4; d++){
			uint8_t target = tposition+dir1[d];
			if (position[target] == Offboard) continue;
			if ( noKingMove(attacked[target])||bposAttacked(target) ) continue;
			//Normal
			if (position[target] ==Empty ){
				MoveNode tempNode(tposition,target,side);
				tempNode.priority+=heuristic(tempNode,plys);
				moveList.push_back(tempNode);


			}
			//capture Move
			else if (sideBoard[target]==(side^1)){

				MoveNode tempNode(tposition,target,side);
				tempNode.capture=1;
				tempNode.priority+=victimAttackPri[position[target]][WKing];
				moveList.push_back(tempNode);

			}


		}
		if (side ==White){

			if (castle & WQueenCastle){ // Queen Side Castle

				if (position[24]==Empty && position[23]==Empty && position[22]==Empty) {
					if(1^checking && (1^bposAttacked(24))&& (1^bposAttacked(23))) {

						uint8_t target = 23;
						MoveNode tempNode(tposition,target,side);
						tempNode.castleMove=1;
						tempNode.priority+=heuristic(tempNode,plys);
						moveList.push_back(tempNode);
					}
				}

			}
			// King Side Castle

			if (castle & WKingCastle){
				if (position[26]==Empty && position[27]==Empty ){
					if(1^checking&& 1^bposAttacked(26) && bposAttacked(27)^1) {


						uint8_t target = 27;
						MoveNode tempNode(tposition,target,side);
						tempNode.castleMove=1;
						tempNode.priority+=heuristic(tempNode,plys);
						moveList.push_back(tempNode);
					}
				}
			}

		}
		else{
			if (castle & BQueenCastle){  // Queen Side Castle
				if (position[92]==Empty && position[93]==Empty && position[94]==Empty) {
					if(1^checking && bposAttacked(94)^1 && bposAttacked(93)^1) {

						uint8_t target = 93;
						MoveNode tempNode(tposition,target,side);
						tempNode.castleMove=1;
						tempNode.priority+=heuristic(tempNode,plys);
						moveList.push_back(tempNode);
					}
				}

			}
			// King Side Castle

			if (castle & BKingCastle){
				if (position[96]==Empty && position[97]==Empty ) {
					if(1^checking&& bposAttacked(96)^1 &&  bposAttacked(97)^1) {


						uint8_t target = 97;
						MoveNode tempNode(tposition,target,side);
						tempNode.castleMove=1;
						tempNode.priority+=heuristic(tempNode,plys);
						moveList.push_back(tempNode);

					}
				}

			}
		}



	}
}



bool Board::capEnpassSafe(uint8_t capedPos ,uint8_t moverPos){

	uint8_t kingPos = pieceList[side*6 +WKing][0];
	uint8_t oppSide = 1^side;
	//same row
	if (capedPos /10 == kingPos/10){
		if (capedPos < kingPos){
			uint8_t target= kingPos-1;
			while (target !=Offboard){
				if (position[target]==rookPiece[oppSide] || position[target] == queenPiece[oppSide]){
					return false;
				}
				else if (target != moverPos && target != capedPos && position[target]!= Empty ){
					return true;
				}
				target--;
			}
			return true;

		}
		else{
			uint8_t target= kingPos+1;
			while (target !=Offboard){
				if (position[target]==rookPiece[oppSide] || position[target] == queenPiece[oppSide]){
					return false;
				}
				else if (target != moverPos && target != capedPos&& position[target]!= Empty){
					return true;
				}
				target++;
			}
			return true;
		}
	}

	/*if (capedPos %10 == kingPos%10){
	if (capedPos < kingPos){
	uint8_t target= capedPos-10;
	while (target !=Offboard){
	if (position[target]==rookPiece[oppSide] || position[target] == queenPiece[oppSide]){
	return false;
	}
	else if (target != moverPos &&position[target]!= Empty){
	return true;
	}
	target-=10;
	}
	return true;

	}
	else{
	uint8_t target= capedPos+10;
	while (target !=Offboard){
	if (position[target]==rookPiece[oppSide] || position[target] == queenPiece[oppSide]){
	return false;
	}
	else if (target != moverPos &&position[target]!= Empty){
	return true;
	}
	target+=10;
	}
	return true;
	}
	}	*/

	//same diagonal
	if ((capedPos -kingPos)%9 ==0){
		if (capedPos < kingPos){
			uint8_t target= kingPos-9;
			while (target !=Offboard){
				if (position[target]==bishopPiece[oppSide] || position[target] == queenPiece[oppSide]){
					return false;
				}
				else if (target != capedPos && position[target]!= Empty){
					return true;
				}
				target-=9;
			}
			return true;

		}
		else{
			uint8_t target= kingPos+9;
			while (target !=Offboard){
				if (position[target]==bishopPiece[oppSide] || position[target] == queenPiece[oppSide]){
					return false;
				}
				else if (target != capedPos && position[target]!= Empty){
					return true;
				}
				target+=9;
			}
			return true;
		}
	}	

	if ((capedPos -kingPos)%11 ==0){
		if (capedPos < kingPos){
			uint8_t target= kingPos-11;
			while (target !=Offboard){
				if (position[target]==bishopPiece[oppSide] || position[target] == queenPiece[oppSide]){
					return false;
				}
				else if (target != capedPos && position[target]!= Empty){
					return true;
				}
				target-=11;
			}
			return true;

		}
		else{
			uint8_t target= kingPos+11;
			while (target !=Offboard){
				if (position[target]==bishopPiece[oppSide] || position[target] == queenPiece[oppSide]){
					return false;
				}
				else if (target != capedPos && position[target]!= Empty){
					return true;
				}
				target+=11;
			}
			return true;
		}
	}	
	return true;

}