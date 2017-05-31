#include "definition.h"
#include "Board.h"
#include "MoveNode.h"
#include "hash.h"
#ifndef Search_h
#define Search_h

#define MAXDEPTH 30
#define EXTRA 4

MoveNode  searchMove(int depth , Board board);

extern int finishFlag;
extern unsigned long long nodes;
extern int destDepth;
extern boardHashTable bestTable;
#endif
