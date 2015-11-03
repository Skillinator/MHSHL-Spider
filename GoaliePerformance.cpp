#include <iostream>
#include <string>
#include <vector>
#include "mhshl.h"

GoaliePerformance::GoaliePerformance(){
	gameID="NULL";
	teamID="NUL";
	player = finalFor = finalAgainst = seconds = goalsAgainst = shotsAgainst = saves = 0;
	result = "NULL";
}

GoaliePerformance::GoaliePerformance(std::string gID, std::string tID, int pl, int f, int a, std::string res, int sec, int ga, int sa){
	gameID = gID;
	teamID = tID;
	player = pl;
	finalFor = f;
	finalAgainst = a;
	result = res;
	seconds = sec;
	goalsAgainst = ga;
	shotsAgainst = sa;
	saves = sa-ga;
}

void GoaliePerformance::printHeader(){
	std::cout<<"\n  GameID   team  player  for  against  RES   time   ga  sa  sv \n";
}

void GoaliePerformance::printString(){
	spaceBuffer(10, gameID);
	spaceBuffer(6, teamID);
	spaceBuffer(8, std::to_string(player));
	spaceBuffer(5, std::to_string(finalFor));
	spaceBuffer(9, std::to_string(finalAgainst));
	spaceBuffer(5,result);
	std::cout<<"  "<<twoPlace(seconds/60)<<":"<<twoPlace(seconds%60)<<" ";
	spaceBuffer(4, std::to_string(goalsAgainst));
	spaceBuffer(4, std::to_string(shotsAgainst));
	spaceBuffer(4, std::to_string(saves));
	std::cout<<"\n";
}