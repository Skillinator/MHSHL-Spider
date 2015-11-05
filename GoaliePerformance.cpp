#include <iostream>
#include <string>
#include <vector>
#include "mhshl.h"

GoaliePerformance::GoaliePerformance(){
	gameID="NULL";
	teamID="NUL";
	player = seconds = goalsAgainst = shotsAgainst = saves = 0;
}

GoaliePerformance::GoaliePerformance(std::string gID, std::string tID, int pl, int sec, int ga, int sa){
	gameID = gID;
	teamID = tID;
	player = pl;
	seconds = sec;
	goalsAgainst = ga;
	shotsAgainst = sa;
	saves = sa-ga;
}

void GoaliePerformance::printHeader(){
	std::cout<<"\n  GameID   team  player  time   ga  sa  sv \n";
}

void GoaliePerformance::printString(){
	spaceBuffer(15, gameID);
	spaceBuffer(6, teamID);
	spaceBuffer(8, std::to_string(player));
	std::cout<<"  "<<twoPlace(seconds/60)<<":"<<twoPlace(seconds%60)<<" ";
	spaceBuffer(4, std::to_string(goalsAgainst));
	spaceBuffer(4, std::to_string(shotsAgainst));
	spaceBuffer(4, std::to_string(saves));
	std::cout<<"\n";
}