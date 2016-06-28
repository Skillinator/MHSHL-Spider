#include <iostream>
#include <string>
#include <vector>
#include "mhshl.h"
#include "mhshlUtils.h"

ScoringEvent::ScoringEvent(){
	gameID="NULL";
	teamID="NUL";
	scorer = assist1 = assist2 = period = time = 0;
}

ScoringEvent::ScoringEvent(std::string gID, std::string tID, int gs, int a1, int a2, int per, int t){
	gameID = gID;
	teamID = tID;
	scorer = gs;
	assist1 = a1;
	assist2 = a2;
	period = per;
	time = t;
	powerPlay = 0;
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
	std::cout<<" "<<twoPlace(time/60)<<":"<<twoPlace(time%60);
	std::cout<<"\n";
}
