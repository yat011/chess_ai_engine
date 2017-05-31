#include "definition.h"
#include <string>
#include <mutex>

#ifndef MOVENODE_H
#define MOVENODE_H







class MoveNode{
public:
	uint8_t sideOfMove:1;
	uint8_t enpass : 4;
	uint8_t castleMove:1;
	uint8_t capture:1;
	uint8_t promote:3; //null:0 ,Queen 1,Knight 2,Rook 3,Bishop 4
	uint8_t from :7;
	uint8_t to :7;

	int priority;
	MoveNode(){
		memset(this,0, sizeof(MoveNode));
	}
	MoveNode(uint8_t from, uint8_t to, uint8_t side){
		init (from,to,side);
	}
	void printNode(uint8_t type){
		//printf("From:%s TO:%s Side:%d Enpassant:%d castleMove:%d capture:%d promote:%d\n", prPos(from).c_str(), prPos(to).c_str(), sideOfMove,enpass, castleMove,capture, promote);
		printf("Piece:%c From:%s TO:%s promotion:%d\n", pieceName[type], prPos(from).c_str(), prPos(to).c_str(),promote);
	}
	void printNode(){
		//printf("From:%s TO:%s Side:%d Enpassant:%d castleMove:%d capture:%d promote:%d\n", prPos(from).c_str(), prPos(to).c_str(), sideOfMove,enpass, castleMove,capture, promote);
		printf("Side:%s From:%s TO:%s capture:%d promotion:%d\n",sideOfMove?"Black":"White", prPos(from).c_str(), prPos(to).c_str(),capture,promote);
	}
	std::string uciPrint(){
		std::string temp;


		temp =prPos(from)+prPos(to);
		if (promote){
			switch(promote){
			case QPromote:
				temp+='q';
				break;
			case NPromote:
				temp+='n';
				break;
			case BPromote:
				temp+='b';
				break;
			case RPromote:
				temp+='r';
				break;
			default:
				break;
			}
			

		}
		return temp;
	}
	void init(uint8_t from, uint8_t to, uint8_t side){
		memset(this,0,sizeof(MoveNode));
		this->from=from;this->to= to;this->sideOfMove =side;  enpass =NOENPASS;
		//capture=0; promote =0; priority =0;  castleMove=0; 
	}
	bool equal(const MoveNode *b){
		if (sideOfMove!=b->sideOfMove ||from !=b->from || to!=b->to||castleMove!=b->castleMove || enpass!= b->enpass || promote!=b->promote || capture != b->capture)
			return false;
		return true;
	}



};






class MoveList {
    
public:
	MoveNode node[LISTSIZE];
	uint8_t count;
    
	MoveList(){ count =0;}
	void push_back(MoveNode a){
		
        node[count++]=a;
       
	}


};


#endif