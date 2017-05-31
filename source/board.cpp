#include "definition.h"
#include "Search.h"
#include "evaluationData.h"
#include "hash.h"
#include "Board.h"
#include "MoveNode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <memory.h>

char pieceName[]="KQRBNPkqrbnp-X" ;
#define HISTORYSIZE 1000
MoveNode historyList[HISTORYSIZE];
uint8_t captureHistory[HISTORYSIZE]; // captured Pos
uint8_t fiftyHistory[HISTORYSIZE];
uint8_t castleHistory[HISTORYSIZE];
uint8_t enpassHistory[HISTORYSIZE];
uint64_t keyHistory[HISTORYSIZE];

int posDataScore[12][120];
int kingEndScore[2][120];
extern const int pawnTable[];
extern const int knightTable[];
extern const int bishopTable[];
extern const int rookTable[];
extern const int queenTable[];
extern const int kingTable[];
const uint8_t pieceSide[]={White,White,White,White,White,White,Black,Black,Black,Black,Black,Black};
const uint8_t majorGroup[]={2,White,White,White,White,2,2,Black,Black,Black,Black,2};
GameTime timeInfo;

Board::Board(){

	//Board::init();

}







void Board::parseFEN(char FEN[]){
	init();
	int count=0; int p =0;int col =0;
	//position--------------------
	while (count <=7){

		if (FEN[p] >= '0' &&  FEN[p] <='9'){
			for (int i=0; i < int(FEN[p]-'0'); i ++){

				position[getPosition120(col + (7-count)* 8)]=Empty;
				col ++;
			}
		}
		else{
			int temp =getPosition120(col + (7-count)* 8);

			switch (FEN[p]){	
			case 'K':
				addPiecePos(temp,WKing);
				col++;
				break;
			case 'Q':
				addPiecePos(temp,WQueen);
				col++;
				break;
			case 'R':
				addPiecePos(temp,WRook);                  
				col++;
				break;
			case 'B':
				addPiecePos(temp,WBishop);      

				col++;
				break;
			case 'N':
				addPiecePos(temp,WKnight);    

				col++;
				break;
			case 'P':
				addPiecePos(temp,WPawn);    

				col++;
				break;
			case 'k':
				addPiecePos(temp,BKing);    

				col++;
				break;
			case 'q':
				addPiecePos(temp,BQueen);    

				col++;
				break;
			case 'r':
				addPiecePos(temp,BRook);    

				col++;
				break;
			case 'b':
				addPiecePos(temp,BBishop);    

				col++;
				break;
			case 'n':
				addPiecePos(temp,BKnight);    

				col++;
				break;
			case 'p':
				addPiecePos(temp,BPawn);    

				col++;
				break;
			default:
				errorIfn(FEN[p]=='/'|| FEN[p]==' ', "not valid FEN (missing '/')");
				count ++;
				col=0;
				break;
			}

		}

		p++;


	}
	//Other-----------------


	if (FEN[p]=='w'){
		side=White;
	}
	else if (FEN[p]=='b'){
		side=Black;
		hashcode^=sideKey;
	}
	else errorIfn(0, "Not valid FEN (side)");

	p+=2;

	while (FEN[p]!=' '){
		switch (FEN[p]){
		case 'K':
			castle|=WKingCastle;
			break;
		case 'Q':
			castle|=WQueenCastle;
			break;
		case 'k':
			castle|=BKingCastle;
			break;
		case 'q':
			castle|=BQueenCastle;
			break;
		case '-':
			break;
		default:
			errorIfn(0,"Not valid FEN (Castle)");
			break;

		}
		++p;
	}
	hashcode^=castleKey[castle];

	//En passant
	p++;
	if (FEN[p]!='-'){
		errorIfn(FEN[p]<='h'||FEN[p] >='a',"Not valid FEN (en passant)");
		errorIfn(FEN[p+1]<='8'||FEN[p+1] >='1',"Not valid FEN (en passant)");
		enpass = uint8_t(FEN[p] -'a'+1);
		printf("%d",enpass);
		p++;
	}else{
		errorIfn(FEN[p]=='-',"Not valid FEN (en passant)");
		enpass=NOENPASS;
	}
	hashcode^=enpassKey[enpass];

	//fiftymove
	p+=2;
	std::string temp="";
	while(FEN[p]!=' '){
		temp+=FEN[p];
		p++;
	}
	fiftymove=atoi(temp.c_str());
	//Fullmove
	p++;
	temp="";
	while(FEN[p]!=' '&& FEN[p]!='\0'){
		temp+=FEN[p];
		p++;
	}
	fullmove = atoi(temp.c_str());
	//genAttacked();
	genKAttacked();
}

int Board::parseMove(char move[]){
	uint8_t from = move120(move), to = move120((move+2));
	MoveList temp;
	genNodes(temp);

	if (from <21 || from >98 ||to >98 || to <21)
		return 0;

	for (int i =0 ; i<temp.count; i++){

		if (temp.node[i].from ==from && temp.node[i].to==to){

			if (temp.node[i].promote){
				move +=4;
				switch(move[0]){
				case 'q':
					performMove(temp.node[i]);
					break;
				case 'n':
					performMove(temp.node[i+NPromote-1]);
					break;
				case 'b':
					performMove(temp.node[i+BPromote-1]);
					break;
				case 'r':
					performMove(temp.node[i+RPromote-1]);
					break;
				default:
					return 0;

				}
				return 5;
			}
			else{
				performMove(temp.node[i]);
				return 4;

			}

		}

	}
	//printf("Not invalid\n");
	return 0;


}

int Board::validateMove(char move[]){
	uint8_t from = move120(move), to = move120((move+2));
	MoveList temp;
	genNodes(temp);

	if (from <21 || from >98 ||to >98 || to <21)
		return 0;

	for (int i =0 ; i<temp.count; i++){

		if (temp.node[i].from ==from && temp.node[i].to==to){

			if (temp.node[i].promote){
				move +=4;
				switch(move[0]){
				case 'q':
					//performMove(temp.node[i]);
					break;
				case 'n':
					//performMove(temp.node[i+NPromote-1]);
					break;
				case 'b':
					//performMove(temp.node[i+BPromote-1]);
					break;
				case 'r':
					//performMove(temp.node[i+RPromote-1]);
					break;
				default:
					return 0;

				}
				return 5;
			}
			else{
				//performMove(temp.node[i]);
				return 4;

			}

		}

	}
	//printf("Not invalid\n");
	return 0;


}


void Board::performMove(MoveNode move){
	//castle
	keyHistory[halfmove]=hashcode;
	uint8_t fiftyClear=0;
	captured =0;	
	hashcode^=castleKey[castle];
	hashcode^=enpassKey[enpass];
	castleHistory[halfmove] = castle;
	
	if (position[move.from]==kingPiece[side]){
		castle &= (12>>(side*2)) ^ 0xf;
	}

	if (position[move.from]==(rookPiece[side])){
		if (move.from == (21+(side*70)))
			castle &= (WQueenCastle>>(side*2))^0xf;
		else if (move.from == (28+(side*70)) ){
			castle &= (WKingCastle>>(side*2))^0xf;

		}
	}
	//fiftyMove
	
	if (position[move.from]==side*6 + WPawn)
		fiftyClear=1;
	if (castleHistory[halfmove] !=castle)
		fiftyClear=1;
	if (move.enpass !=enpass)
		fiftyClear=1;
	// Normal Move
	if (!move.capture){ 


		movePiece(move.from,move.to);

		if (move.castleMove){
			if (move.to-move.from >0) { // king Side castle
				movePiece(side*70+28, side*70+26);

			}
			else { // Queen Side
				movePiece(side*70+21, side*70+24);

			}
		}


	}
	else{// capture Move
		fiftyClear=1;
		captured =1;

		//enpass capture;
		if (move.to == getEnpass() ){

			uint8_t cappos=move.to+ side*20 -10;
			captureHistory[halfmove] = position[cappos];
			CONFIRM(position[cappos]!=Empty);
			popPiece(cappos,position[cappos]);
		}
		else {
			captureHistory[halfmove] = position[move.to];
			CONFIRM(position[move.to]!=Empty);
			popPiece(move.to,position[move.to]);	
		}
		movePiece(move.from,move.to);


		//castle
		if (move.to == (91-(side*70)))
			castle &= (BQueenCastle<<(side*2))^0xf;
		else if (move.to == (98-(side*70)) )
			castle &= (BKingCastle<<(side*2))^0xf;

	}
	//Promotion
	if (move.promote){
		switch(move.promote){
		case 1:
			popPiece(move.to,position[move.to]);
			addPiecePos(move.to,queenPiece[side]);
			break;
		case 2:
			popPiece(move.to,position[move.to]);
			addPiecePos(move.to,knightPiece[side]);
			break;
		case 3:
			popPiece(move.to,position[move.to]);
			addPiecePos(move.to,rookPiece[side]);
			break;
		case 4:
			popPiece(move.to,position[move.to]);
			addPiecePos(move.to,bishopPiece[side]);
			break;
		}

	}
	/*if(bposAttacked(pieceList[(side*6)+WKing][0])){
	printBoard();
	side =side;
	}*/


	if (side ==Black)
		fullmove++;
	fiftyHistory[halfmove]=fiftymove;
	
	historyList[halfmove] = move;
	enpassHistory[halfmove]=enpass;
	enpass = move.enpass;

	side =!side;
	plys++;

	halfmove++;
	hashcode ^=sideKey;
	hashcode^=castleKey[castle];
	hashcode^=enpassKey[enpass];
	fiftymove= fiftyClear?0: fiftymove+1;

	genKAttacked();
	


}

void Board::undoMove(){
	if (halfmove ==0)
		return;
	halfmove --;
	plys--;
	hashcode ^=sideKey;
	hashcode^=castleKey[castle];
	hashcode^=enpassKey[enpass];
	MoveNode move = historyList[halfmove];
	if (side ==White)
		fullmove--;
	side =!side;
	enpass =enpassHistory[halfmove];
	fiftymove=fiftyHistory[halfmove];
	castle = castleHistory[halfmove];

	hashcode^=castleKey[castle];
	hashcode^=enpassKey[enpass];
    if (halfmove>0){
        captured = historyList[halfmove-1].capture;
    }
    else {
        captured =0;
    }

	if (move.promote){
		switch(move.promote){
		case 1:
			popPiece(move.to,WQueen + side *6);
			addPiecePos(move.to,WPawn + side *6);
			break;
		case 2:
			popPiece(move.to,WKnight + side *6);
			addPiecePos(move.to,WPawn + side *6);
			break;
		case 3:
			popPiece(move.to,WRook + side *6);
			addPiecePos(move.to,WPawn + side *6);
			break;
		case 4:
			popPiece(move.to,WBishop + side *6);
			addPiecePos(move.to,WPawn + side *6);
			break;
		}

	}

	if (!move.capture){

		movePiece(move.to, move.from);

		if (move.castleMove){
			if (move.to-move.from >0) { // king Side castle
				movePiece(side*70+26,side*70+28 );

			}
			else { // Queen Side
				movePiece(side*70+24,side*70+21 );

			}

		}
	}
	else{
		movePiece(move.to,move.from);
		//enpass capture;
		if (move.to == getEnpass() ){

			uint8_t cappos=move.to+ side*20 -10;

			addPiecePos(cappos,captureHistory[halfmove]);
		}
		else {

			addPiecePos(move.to,captureHistory[halfmove]);
		}


	}

	genKAttacked();


}

void initData(){

	for (int v =0 ; v <6; v++){
		for (int a =0 ; a<6; a++){
			victimAttackPri[v][a] = victimPri[v] - victimPri[a]/100 ;
			victimAttackPri[v+6][a] = victimPri[v] - victimPri[a]/100;
		}
	}

	memset(posDataScore[0],0,sizeof(int) * 12 *120);

	for (int pos=0; pos <64 ; pos++){

		int tposition = getPosition120(pos);


		//==============

		posDataScore[WPawn][tposition]= pawnTable[reverse64(pos)];
		posDataScore[WKnight][tposition]= knightTable[reverse64(pos)];
		posDataScore[WRook][tposition]= rookTable[reverse64(pos)];
		posDataScore[WQueen][tposition] = queenTable[reverse64(pos)];
		posDataScore[WKing][tposition]= kingTable[reverse64(pos)];
		posDataScore[WBishop][tposition] = bishopTable[reverse64(pos)];

		posDataScore[BPawn][tposition]= -pawnTable[pos];
		posDataScore[BKnight][tposition]= -knightTable[pos];
		posDataScore[BRook][tposition] = -rookTable[pos];
		posDataScore[BQueen][tposition]= -queenTable[pos];
		posDataScore[BKing][tposition]= -kingTable[pos];
		posDataScore[BBishop][tposition]= -bishopTable[pos];

		kingEndScore[White][tposition]=kingEndTable[reverse64(pos)];
		kingEndScore[Black][tposition]=-kingEndTable[pos];
	}


	//======================Bit Mask===========================
	//passer
	memset(passerMask[0],0,sizeof(uint64_t)*2*120);
	uint64_t temp;

	for (int row = 30 ; row <= 90 ; row +=10){
		for (int col = 1; col <=8 ; col ++ ){
			temp =0;
			for (int hor = col -1 ; hor <= col +1 ; hor ++){
				if (hor <1 || hor > 8) continue;
				for (int v = hor +row +10; v<=98 ; v+=10){
					bitPosSet(temp,pos120to64(v));

				}

			}
			passerMask[White][row+col]=temp;

		}
	}
	for (int row = 80 ; row >= 20 ; row -=10){
		for (int col = 1; col <=8 ; col ++ ){
			temp =0;
			for (int hor = col -1 ; hor <= col +1 ; hor ++){
				if (hor <1 || hor > 8) continue;
				for (int v = hor +row -10; v>=21 ; v-=10){
					bitPosSet(temp,pos120to64(v));

				}

			}
			passerMask[Black][row+col]=temp;

		}
	}
	//isolated
	memset(isolatedMask,0,sizeof(uint64_t)*120);
	for (int col = 1; col <=8 ; col ++ ){
		temp =0;
		for (int hor = col -1 ; hor <= col +1 ; hor +=2){
			if (hor <1 || hor > 8) continue;
			for (int v =20 ; v <=90 ;  v+=10){
				bitPosSet(temp,pos120to64(v+hor));

			}

		}
		for (int row =20 ; row <= 90 ; row +=10){
			isolatedMask[row +col] =temp;
		}
		//printBitBoard(isolatedMask[30+col]);
	}
	// column Mask
	memset(colMask[0],0,sizeof(uint64_t)*2*120);
	for (int col = 1; col <=8 ; col ++ ){

		for (int v =20 ; v <=90 ;  v+=10){
			temp =0;
			for (int row = v+10; row <=90 ; row +=10){
				bitPosSet(temp,pos120to64(row+col));

			}
			colMask[White][col+v] =temp;

		}

	}

	for (int col = 1; col <=8 ; col ++ ){

		for (int v =90 ; v >=20 ;  v-=10){
			temp =0;
			for (int row = v-10; row >=20 ; row -=10){
				bitPosSet(temp,pos120to64(row+col));

			}
			colMask[Black][col+v] =temp;

		}

	}

	//defend Mask
	memset(defendMask[0],0,sizeof(uint64_t)*2*120);
	for (int row = 30 ; row <= 90 ; row +=10){
		for (int col = 1; col <=8 ; col ++ ){
			temp =0;
			for (int hor = col -1 ; hor <= col +1 ; hor +=2){
				if (hor <1 || hor > 8) continue;
				int v = hor +row -10;
				bitPosSet(temp,pos120to64(v));



			}
			defendMask[White][row+col]=temp;
			
		}
	}

	for (int row = 80 ; row >= 20 ; row -=10){
		for (int col = 1; col <=8 ; col ++ ){
			temp =0;
			for (int hor = col -1 ; hor <= col +1 ; hor +=2){
				if (hor <1 || hor > 8) continue;
				int v = hor +row +10;
					bitPosSet(temp,pos120to64(v));

			}
			defendMask[Black][row+col]=temp;
			
		}
	}
    
    //promote mask
    memset(promoteMask,0,sizeof(uint64_t)*2);
    temp =0; int temp2=0;
    for (int col =1; col <= 8 ; col++){
        bitPosSet(temp, pos120to64(30+col));
        bitPosSet(temp2, pos120to64(80+col));
        
    }
    promoteMask[White]=temp2;
    promoteMask[Black]=temp;
}

void Board::init(){


	memset(position, Offboard, sizeof(uint8_t) *120);
	memset(sideBoard, Offboard, sizeof(uint8_t) *120);
	for (int i =0; i<64; i++){

		position[getPosition120(i)]=Empty;
		sideBoard[getPosition120(i)]=Empty;
	}
	castle =0;
	enpass=NOENPASS;
	fullmove=0;
	fiftymove=0;
	halfmove=0;
	plys=0;
	stateScore=0;
	beingChecked=0;
	hashcode=0;
	memset(majorNumber,0, sizeof (uint8_t)*3);
	memset (pawnBitBoard,0,sizeof(uint64_t)*3);
	memset(pieceNumber, 0, sizeof(uint8_t) *12);
	memset (pieceList,INVALID,sizeof(uint8_t)*120); // 121 = not recording a piece)
	memset(totalNum, 0,sizeof(uint8_t)*2);




}

void Board::printBoard(){
	for(int row =7 ; row >=0; row--){

		printf ("\n%3d",row+1);

		for (int col= 0; col < 8 ; col++){
			printf("%3c",pieceName[position[getPosition120(row*8 + col)]]);

		}
	}
	printf("\n\n%3c",' ');
	for (int i = 'A' ; i <'A'+8; i++){
		printf("%3c",i);

	}
	std::string castleperm ="";
	if (castle&WKingCastle)
		castleperm+="K";
	if (castle&WQueenCastle)
		castleperm+="Q";
	if (castle&BKingCastle)
		castleperm+="k";
	if (castle&BQueenCastle)
		castleperm+="q";

	if (getEnpass()){
		printf("\ncastle:%s enpass:%s fullmove:%d fiftymove:%d Side:%s\n", castleperm.c_str(), prPos(getEnpass()).c_str(),fullmove,fiftymove/2,side?"Black":"White");
	}
	else{
		printf("\nCastle:%s Enpass:%s fullmove:%d fiftymove:%d Side:%s\n", castleperm.c_str(), "No" ,fullmove,fiftymove/2,side?"Black":"White");
	}

	if (fiftymove>100)
		printf("Draw\n");


	/*for (int i =0; i<10 ; i++){
	printf(" WPawn:%d\n",pieceList[WPawn][i]);
	}	
	for (int i =0; i<10 ; i++){
	printf(" BPawn:%d\n",pieceList[BPawn][i]);
	}*/
}
void Board::printAttacked(){
	for(int row =7 ; row >=0; row--){

		printf ("\n%3d",row+1);

		for (int col= 0; col < 8 ; col++){
			printf("%3d",attacked[getPosition120(row*8 + col)]);

		}
	}
	printf("\n\n%3c",' ');
	for (int i = 'A' ; i <'A'+8; i++){
		printf("%3c",i);

	}

}

void Board::printSideBoard(){
	for(int row =7 ; row >=0; row--){

		printf ("\n%3d",row+1);

		for (int col= 0; col < 8 ; col++){
			printf("%3d",sideBoard[getPosition120(row*8 + col)]);

		}
	}
	printf("\n\n%3c",' ');
	for (int i = 'A' ; i <'A'+8; i++){
		printf("%3c",i);

	}

}
void Board::movePiece(int from, int to){
	uint8_t type = position[from];
	position[from]=Empty;
	sideBoard[from]=Empty;
	position[to]=type;
	sideBoard[to]=pieceSide[type];

	if ((type != WKing && type !=BKing)){
		stateScore+= posDataScore[type][to];
		stateScore-= posDataScore[type][from];
	}

	hashcode^=pieceKey[from][type];
	hashcode^=pieceKey[to][type];
	//
	if (type == WPawn){
		bitPosClear(pawnBitBoard[White],pos120to64(from));
		bitPosSet(pawnBitBoard[White],pos120to64(to));
	}
	if (type == BPawn){
		bitPosClear(pawnBitBoard[Black],pos120to64(from));
		bitPosSet(pawnBitBoard[Black],pos120to64(to));
	}
	//


	for (int i =0 ; i<10 ; i++){
		if (pieceList[type][i] == from){
			pieceList[type][i]=to;
			return;
		}
	}
	errorIfn(0,"move nothing!!");

}

void Board::popPiece(int pos, int PType){
	if ((PType != WKing && PType !=BKing)){
		stateScore-= posDataScore[PType][pos];
	}

	totalNum[pieceSide[PType]]--;
	majorNumber[majorGroup[PType]]--;
	if (PType == WPawn){
		bitPosClear(pawnBitBoard[White],pos120to64(pos));
	}
	if (PType == BPawn){
		bitPosClear(pawnBitBoard[Black],pos120to64(pos));
	}

	hashcode^=pieceKey[pos][PType];
	for (int i =0 ; i<10 ; i++){
		if (pieceList[PType][i] == pos){			
			position[pos]=Empty;			
			pieceList[PType][i]=pieceList[PType][--pieceNumber[PType]];		
			pieceList[PType][pieceNumber[PType]]=INVALID;
			sideBoard[pos]=Empty;
			return;
		}
	}
	printBoard();
	printf("pos %d type %c", pos, pieceName[PType]);
	errorIfn(0,"pop nothing!!");
}


void Board::addPiecePos(int pos, int PType){
	//assume type valid
	CONFIRM(pieceList[PType][pieceNumber[PType]]==INVALID);
	position[pos]=PType;
	pieceList[PType][pieceNumber[PType]++]=pos;
	sideBoard[pos]=pieceSide[PType];
	majorNumber[majorGroup[PType]]++;

	if ((PType != WKing && PType !=BKing)){
		stateScore+= posDataScore[PType][pos];
	}

	hashcode^=pieceKey[pos][PType];
	totalNum[pieceSide[PType]]++;
	//=======
	if (PType == WPawn){
		bitPosSet(pawnBitBoard[White],pos120to64(pos));
	}
	if (PType == BPawn){
		bitPosSet(pawnBitBoard[Black],pos120to64(pos));
	}
}


uint8_t Board::getEnpass(){
	if (enpass==NOENPASS) return 0; 
	return (side? 40 + enpass: 70 +enpass);
}






void printBitBoard( uint64_t a){


	printf("\n");
	for (int row =7 ; row >=0 ; row --){
		for (int col =0 ; col <8 ; col++){
			printf ("%2d", bitPosPeek(a,col+row *8));
		}
		printf("\n");
	}
	printf("====================\n");






}





void Board::performNull(){
	//castle
	MoveNode nul= MoveNode();
	captured =0;
	hashcode^=castleKey[castle];
	hashcode^=enpassKey[enpass];
	historyList[halfmove]=nul;
	castleHistory[halfmove] = castle;
	fiftyHistory[halfmove]=fiftymove;	
	enpassHistory[halfmove]=enpass;
	keyHistory[halfmove]=hashcode;

	enpass = NOENPASS;
	side =!side;
	plys++;
	fiftymove= fiftymove+1;	
	halfmove++;
	hashcode ^=sideKey;
	hashcode^=castleKey[castle];
	hashcode^=enpassKey[enpass];

	genKAttacked();


}
void Board::undoNull(){
	if (halfmove ==0)
		return;
	halfmove --;
	plys--;
	hashcode ^=sideKey;
	hashcode^=castleKey[castle];
	hashcode^=enpassKey[enpass];
	captured = historyList[halfmove-1].capture;

	side =!side;
	enpass =enpassHistory[halfmove];
	fiftymove=fiftyHistory[halfmove];
	castle = castleHistory[halfmove];

	hashcode^=castleKey[castle];
	hashcode^=enpassKey[enpass];



	genKAttacked();





}