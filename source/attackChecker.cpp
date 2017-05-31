#include "definition.h"
#include "Board.h"
#include "MoveNode.h"


void Board::genKAttacked(){
	uint8_t kingPos = pieceList[side *6 +WKing][0];
	uint8_t attacker; uint8_t oppSide = 1^side;
	uint8_t oppType = oppSide *6;
	uint8_t block ;

	memset(attacked,0,sizeof(uint8_t)*120);
	beingChecked=0;
	//Pawn
	if (side){ //Black side
		for (int d =2 ; d <4 ; d++){
			attacker = kingPos +dir2[d];
			if (position[attacker]==Offboard) continue;
			if (position[attacker]==WPawn ){
				attacked[attacker]|=CHECKFLAG;
				attacked[kingPos]|=CHECKFLAG;
				beingChecked++;

			}
		}



	}
	else{
		for (int d =0 ; d <2 ; d++){
			attacker = kingPos +dir2[d];
			if (position[attacker]==Offboard) continue;
			if (position[attacker]==BPawn ){
				attacked[attacker]|=CHECKFLAG;
				attacked[kingPos]|=CHECKFLAG;
				beingChecked++;

			}
		}

	}
	//knight

	for (int d=0; d <8 ; d++){
		attacker =kingPos +dir3[d];
		if (position[attacker]==Offboard) continue;
		if (position[attacker]== oppType+WKnight){
			attacked[attacker]|=CHECKFLAG;
			attacked[kingPos]|=CHECKFLAG;
			beingChecked++;
		}

	}

	//rook and Queen

	for (int d=0 ; d <4; d++){
		attacker = kingPos +dir1[d];

		while (position[attacker]!=Offboard){
			if(position[attacker]==Empty) {
				attacker +=dir1[d];
			}
			else if (position[attacker]== oppType+WRook || position[attacker]==oppType+WQueen){
				attacked[attacker]|=CHECKFLAG;
				attacker -=dir1[d];
				beingChecked++;
				while(attacker != kingPos - dir1[d]){
					attacked[attacker]|=CHECKFLAG;
					attacked[attacker]|=ATTACKFLAG;
					attacker -=dir1[d];
				}
				attacked[attacker]|=KINGRETRICTFLAG;
				break;
			}
			else if (sideBoard[attacker]==side){
				// BLocking path????
				block = attacker;
				attacker +=dir1[d];
				while(position[attacker]!=Offboard){
					if(position[attacker]==Empty) {
						attacker +=dir1[d];
					}
					else if (position[attacker]== oppType+WRook || position[attacker]==oppType+WQueen){

						while(attacker != block - dir1[d]){
							attacked[attacker]|=BLOCKFLAG;
							//attacked[attacker]|=ATTACKFLAG;
							attacked[attacker]|=bdir1[d];
							attacker -=dir1[d];
						}
						while(attacker != kingPos - dir1[d]){
							attacked[attacker]|=BLOCKFLAG;
							attacked[attacker]|=bdir1[d];
							attacker -=dir1[d];
						}

						break;
					}else {
						break;
					}
				}
				break;
			}// other enermy
			else{
				break;
			}

		}


	}
	//bishop and queen
	for (int d=0 ; d <4; d++){
		attacker = kingPos +dir2[d];

		while (position[attacker]!=Offboard){
			if(position[attacker]==Empty) {
				attacker +=dir2[d];
			}
			else if (position[attacker]== oppType+WBishop || position[attacker]==oppType+WQueen){
				attacked[attacker]|=CHECKFLAG;
				attacker -=dir2[d];
				beingChecked++;
				while(attacker != kingPos - dir2[d]){
					attacked[attacker]|=CHECKFLAG;
					attacked[attacker]|=ATTACKFLAG;
					attacker -=dir2[d];
				}
				attacked[attacker]|=KINGRETRICTFLAG;
				break;
			}
			else if (sideBoard[attacker]==side){
				// BLocking path????
				block = attacker;
				attacker +=dir2[d];
				while(position[attacker]!=Offboard){
					if(position[attacker]==Empty) {
						attacker +=dir2[d];
					}
					else if (position[attacker]== oppType+WBishop || position[attacker]==oppType+WQueen){

						while(attacker != block - dir2[d]){
							attacked[attacker]|=BLOCKFLAG;
							//attacked[attacker]|=ATTACKFLAG;
							attacked[attacker]|=bdir2[d];
							attacker -=dir2[d];
						}
						while(attacker != kingPos - dir2[d]){
							attacked[attacker]|=BLOCKFLAG;
							attacked[attacker]|=bdir2[d];
							attacker -=dir2[d];
						}

						break;
					}else {
						break;
					}
				}
				break;
			}// other enermy
			else{
				break;
			}

		}


	}

	//King
	/* for (int d=0 ; d <4 ; d++){
	attacker = kingPos+dir1[d];

	if (position[attacker]==oppType+WKing){
	attacked[attacker]|=CHECKFLAG;
	attacked[kingPos]|=CHECKFLAG;

	}

	}
	for (int d=0 ; d <4 ; d++){
	attacker = kingPos+dir2[d];

	if (position[attacker]==oppType+WKing){
	attacked[attacker]|=CHECKFLAG;
	attacked[kingPos]|=CHECKFLAG;

	}

	}*/




}






bool Board::bposAttacked(uint8_t tposition){

	uint8_t attacker; uint8_t oppSide = 1^side;
	uint8_t oppType = oppSide *6;

	//Pawn
	if (side){ //Black side
		for (int d =2 ; d <4 ; d++){
			attacker = tposition +dir2[d];
			if (position[attacker]==Offboard) continue;
			if (position[attacker]==WPawn ){

				return true;


			}
		}



	}
	else{
		for (int d =0 ; d <2 ; d++){
			attacker = tposition +dir2[d];
			if (position[attacker]==Offboard) continue;
			if (position[attacker]==BPawn ){

				return true;
			}
		}

	}
	//knight

	for (int d=0; d <8 ; d++){
		attacker =tposition +dir3[d];
		if (position[attacker]==Offboard) continue;
		if (position[attacker]== oppType+WKnight){

			return true;

		}

	}

	//rook and Queen

	for (int d=0 ; d <4; d++){
		attacker = tposition +dir1[d];

		while (position[attacker]!=Offboard){
			if(position[attacker]==Empty) {
				attacker +=dir1[d];
			}
			else if (position[attacker]== oppType+WRook || position[attacker]==oppType+WQueen){

				return true;

			}// other piece
			else{
				break;
			}

		}


	}
	//bishop and queen
	for (int d=0 ; d <4; d++){
		attacker = tposition +dir2[d];

		while (position[attacker]!=Offboard){
			if(position[attacker]==Empty) {
				attacker +=dir2[d];
			}
			else if (position[attacker]== oppType+WBishop || position[attacker]==oppType+WQueen){


				return true;

			}// other piece
			else{
				break;
			}

		}


	}

	//King
	for (int d=0 ; d <4 ; d++){
		attacker = tposition+dir1[d];

		if (position[attacker]==oppType+WKing){
			return true;
		}

	}
	for (int d=0 ; d <4 ; d++){
		attacker = tposition+dir2[d];

		if (position[attacker]==oppType+WKing){


			return true;
		}

	}

	return false;

}