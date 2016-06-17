#include <iostream>
#include <string>
#include <vector>
#include "mhshl.h"
#include "mhshlUtils.h"

PenaltyEvent::PenaltyEvent(){
	gameID="NULL";
	teamID="NUL";
	duration = period = time = timeRemaining = 0;
	offense="NULL";
}

PenaltyEvent::PenaltyEvent(std::string gID, std::string tID, int pl, int dur, int per, int t, std::string charge){
	gameID = gID;
	teamID = tID;
	player = pl;
	duration = dur;
	period = per;
	time = t;
	offense = charge;
	timeRemaining = dur*60;
	scoredOn = false;
	goalsWhile = 0;
}

void PenaltyEvent::printHeading(){
	std::cout<<"\n  GameID   team  player  duration   penalty   period  time  remaining  scoredOn\n";
}

void PenaltyEvent::printString(){
	spaceBuffer(10, gameID);
	spaceBuffer(6, teamID);
	spaceBuffer(8, std::to_string(player));
	spaceBuffer(10, std::to_string(duration));
	spaceBuffer(11, offense);
	spaceBuffer(8, std::to_string(period));
	std::cout<<" "<<twoPlace(time/60)<<":"<<twoPlace(time%60);
	std::cout<<"   ";
	std::cout<<twoPlace(timeRemaining/60)<<":"<<twoPlace(timeRemaining%60)<<"  ";
	spaceBuffer(10, std::to_string(scoredOn));
	std::cout<<"\n";
}
