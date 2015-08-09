#include <iostream>
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
	std::cout<<"\n  GameID   team  player  duration   penalty   period  time\n";
}

void PenaltyEvent::printString(){
	spaceBuffer(10, gameID);
	spaceBuffer(6, teamID);
	spaceBuffer(8, std::to_string(player));
	spaceBuffer(10, std::to_string(duration));
	spaceBuffer(11, offense);
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