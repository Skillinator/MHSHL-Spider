#include <stdio>
#include <string>
#include <vector>
#include "mhshl.h"

PenaltyEvent::PenaltyEvent(std::string gID, std::string tID, int pl, int dur, int per, int t, std::string charge){
	gameID = gID;
	teamID = tID;
	player = pl;
	duration = dur;
	period = per;
	time = t;
	offense = charge;
	scoredOn = false;
}

void PenaltyEvent::printHeading(){
	std::cout<<"  GameID   team  player  period  time\n";
}

void PenaltyEvent::printString(){
	spaceBuffer(10, gameID);
	spaceBuffer(6, teamID);
	spaceBuffer(8, player);
	spaceBuffer(4, assist1);
	spaceBuffer(5, assist2);
	spaceBuffer(8, period);
	spaceBuffer(2, time/60);
	std::cout<<":";
	spaceBuffer(2, time%60);
	std::cout<<"\n";
}