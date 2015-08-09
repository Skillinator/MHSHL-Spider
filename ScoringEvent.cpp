#include <iostream>
#include <string>
#include <vector>
#include "mhshl.h"

ScoringEvent::ScoringEvent(std::string gID, std::string tID, int gs, int a1, int a2, int per, int t){
	gameID = gID;
	teamID = tID;
	scorer = gs;
	assist1 = a1;
	assist2 = a2;
	period = per;
	time = t;
}

void ScoringEvent::printHeading(){
	std::cout<<"\n  GameID   team  player  assists  period  time\n";
}

void ScoringEvent::printString(){
	spaceBuffer(10, gameID);
	spaceBuffer(6, teamID);
	spaceBuffer(8, std::to_string(scorer));
	spaceBuffer(4, std::to_string(assist1));
	spaceBuffer(5, std::to_string(assist2));
	spaceBuffer(8, std::to_string(period));
	spaceBuffer(2, std::to_string(time/60));
	std::cout<<":";
	if(time%60 < 10){
		std::cout<<0<<time%60;
	}else{
		std::cout<<time%60;
	}
	std::cout<<"\n";
}