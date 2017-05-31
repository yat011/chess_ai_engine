#include "Board.h"

static uint8_t checking;
void Board::genCaps (MoveList & moveList){





	checking=beingChecked;



	Board::genPawnMoveC(moveList);
	Board::genBishopMoveC(moveList);
	Board::genRookMoveC(moveList);
	Board::genKnightMoveC(moveList);
	Board::genQueenMoveC(moveList);
	Board::genKingMoveC(moveList);
	checking = checking;

}
void Board::genPawnMoveC(MoveList &moveList){

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


		}
	}






}

void Board::genBishopMoveC (MoveList& moveList){
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
				if (position[target] ==Empty ){
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
void Board::genRookMoveC (MoveList& moveList){
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
				if (position[target] ==Empty ){
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
void Board::genKnightMoveC (MoveList& moveList){
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

			//capture Move
			if (sideBoard[target]== (1^side)){

				MoveNode tempNode(tposition,target,side);
				tempNode.capture=1;
				tempNode.priority+=victimAttackPri[position[target]][WKnight];
				moveList.push_back(tempNode);


			}






		}
	}

}
void Board::genQueenMoveC (MoveList& moveList){
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
								if (position[target] ==Empty ){
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
								if (position[target] ==Empty ){
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
void Board::genKingMoveC (MoveList& moveList){
	uint8_t type = side*6 +WKing;

	for (int p = 0 ; p<pieceNumber[type]; p++){
		uint8_t tposition=pieceList[type][p];
		CONFIRM(tposition!=INVALID);


		for (int d =0; d<4; d++){
			uint8_t target = tposition+dir2[d];
			if (position[target] == Offboard) continue;
			if ( noKingMove(attacked[target])||bposAttacked(target)) continue;

				
			if (sideBoard[target]==(side^1) ){

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

			//capture Move
			if (sideBoard[target]==(side^1)){

				MoveNode tempNode(tposition,target,side);
				tempNode.capture=1;
				tempNode.priority+=victimAttackPri[position[target]][WKing];
				moveList.push_back(tempNode);

			}


		}



	}


}