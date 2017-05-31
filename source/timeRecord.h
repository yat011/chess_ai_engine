
#include <time.h>
#include <stdint.h>
#include <memory.h>
#include <string.h>
#include  <fcntl.h>
#include <fstream>

#include <stdio.h>

#ifndef TIMERECORD
#define TIMERECORD

class GameTime{
public:
	uint64_t startSearch;
	uint64_t stopSearch;
	uint8_t useTimer:1;
	uint8_t isStop:1;
    static uint64_t getMs(){
        return (float)clock()/CLOCKS_PER_SEC *1000;
        
    }
	GameTime(){
		memset(this,0,sizeof(GameTime));


	}
};
extern GameTime timeInfo;

//int InputWaiting();


//void ReadInput() ;



#endif

