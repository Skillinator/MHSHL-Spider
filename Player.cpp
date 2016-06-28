#include <iostream>
#include <string>
#include <vector>
#include "mhshl.h"
#include "mhshlUtils.h"

Player::Player(){
	teamID = "NUL";
	name = "NULL";
	goalie = false;
	jerseyNumber = 66;
	pointstreakID = 0;

	gamesPlayed = 0;
	goals = 0;
	assists = 0;
	powerPlayGoals = 0;
	shortHandedGoals = 0;
	penaltyMinutes = 0;
	goalStreak = 0;
	pointStreak = 0;
	minutesPlayed = 0;
	shots = 0;
	goalsAgainst = 0;
	goalsAgainstAverage = 0;
	saves = 0;
	savePercentage = 0;
}

Player::Player(std::string tID, std::string gname, int identifier, int num){
	teamID = tID;
	name = gname;
	jerseyNumber = num;
	pointstreakID = identifier;
	goalie = false;

	gamesPlayed = 0;
	goals = 0;
	assists = 0;
	powerPlayGoals = 0;
	shortHandedGoals = 0;
	penaltyMinutes = 0;
	goalStreak = 0;
	pointStreak = 0;
	minutesPlayed = 0;
	shots = 0;
	goalsAgainst = 0;
	goalsAgainstAverage = 0;
	saves = 0;
	savePercentage = 0;
}

void Player::printHeading(){
	std::cout<<"\n             name                 ID   #  team  gp  g  a  pp  sh  pim  goalStreak  pointStreak\n";
}

void Player::printString(){
	spaceBuffer(30, name);
	spaceBuffer(8, std::to_string(pointstreakID));
	spaceBuffer(3, std::to_string(jerseyNumber));
	spaceBuffer(6, teamID);
	spaceBuffer(4, std::to_string(gamesPlayed));
	spaceBuffer(3, std::to_string(goals));
	spaceBuffer(3, std::to_string(assists));
	spaceBuffer(4, std::to_string(powerPlayGoals));
	spaceBuffer(4, std::to_string(shortHandedGoals));
	spaceBuffer(5, std::to_string(penaltyMinutes));
	spaceBuffer(12, std::to_string(goalStreak));
	spaceBuffer(13, std::to_string(pointStreak));
	std::cout<<"\n";
}


void Player::printGoalieHeading(){
	std::cout<<"\n             name                 ID   #  team  gp   mins  ga  sa  sv    gaa       svpercent\n";
}

void Player::printGoalieString(){
	spaceBuffer(30, name);
	spaceBuffer(8, std::to_string(pointstreakID));
	spaceBuffer(3, std::to_string(jerseyNumber));
	spaceBuffer(6, teamID);
	spaceBuffer(4, std::to_string(gamesPlayed));
	spaceBuffer(5, std::to_string(minutesPlayed));
	spaceBuffer(5, std::to_string(goalsAgainst));
	spaceBuffer(5, std::to_string(shots));
	spaceBuffer(5, std::to_string(saves));
	spaceBuffer(10, std::to_string(goalsAgainstAverage));
	spaceBuffer(10, std::to_string(savePercentage));
	std::cout<<"\n";
}
