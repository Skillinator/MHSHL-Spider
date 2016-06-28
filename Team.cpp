#include <iostream>
#include <string>
#include <vector>
#include "mhshl.h"
#include "mhshlUtils.h"

Team::Team(){
	abbreviation = "NUL";
	name = city = "NULL";
	pointstreakID = 0;
	gamesPlayed = 0;
	wins = 0;
	losses = 0;
	overtimeLosses = 0;
	shootoutLosses = 0;
	points = 0;
	goalsFor = 0;
	goalsAgainst = 0;
	penaltyMinutes = 0;
	winLosstreak = 0;
}

Team::Team(std::string abbr, std::string n, std::string c, int identifier){
	abbreviation = abbr;
	name = n;
	city = c;
	pointstreakID = identifier;
	gamesPlayed = 0;
	wins = 0;
	losses = 0;
	overtimeLosses = 0;
	shootoutLosses = 0;
	points = 0;
	goalsFor = 0;
	goalsAgainst = 0;
	penaltyMinutes = 0;
	winLosstreak = 0;
}

void Team::printHeading(){
	std::cout<<"\n abbr      city            name        gp   w   l  ot  so  pts  gf  ga  pim  streak\n";
}

void Team::printString(){
	spaceBuffer(6, abbreviation);
	spaceBuffer(14, city);
	spaceBuffer(18, name);
	spaceBuffer(4, std::to_string(gamesPlayed));
	spaceBuffer(4, std::to_string(wins));
	spaceBuffer(4, std::to_string(losses));
	spaceBuffer(4, std::to_string(overtimeLosses));
	spaceBuffer(4, std::to_string(shootoutLosses));
	spaceBuffer(5, std::to_string(points));
	spaceBuffer(4, std::to_string(goalsFor));
	spaceBuffer(4, std::to_string(goalsAgainst));
	spaceBuffer(5, std::to_string(penaltyMinutes));
	spaceBuffer(8, std::to_string(winLossStreak));
	std::cout<<"\n";
}
