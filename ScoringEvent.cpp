#include <stdio>
#include <string>
#include <vector>
#include "mhshl.h"

ScoringEvent::ScoringEvent(std::string gID, std::string tID, int gs, int a1, int a2, int per, int t){
	gameID = gID;
	teamID = teamID;
	scorer = gs;
	assist1 = a1;
	assist2 = a2;
	period = per;
	time = t;
}

void ScoringEvent::printHeading(){
	std::cout<<"  GameID   team  player  assists  period  time\n";
}

void ScoringEvent::printString(){
	spaceBuffer(10, gameID);
	spaceBuffer(6, teamID);
	spaceBuffer(8, scorer);
	spaceBuffer(4, assist1);
	spaceBuffer(5, assist2);
	spaceBuffer(8, period);
	spaceBuffer(2, time/60);
	std::cout<<":";
	spaceBuffer(2, time%60);
	std::cout<<"\n";
}