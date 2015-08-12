#include <iostream>
#include <string>
#include <vector>
#include "mhshl.h"

Player::Player(){
	team = "NUL";
	name = "NULL";
	number = gp = g = a = pp = sh = pim = goalStreak = pointStreak = 0;
}

Player::Player(std::string tID, std::string gname, int num){
	team = tID;
	name = gname;
	number = num;
	gp = g = a = pp = sh = pim = goalStreak = pointStreak = 0;
}

void Player::printHeading(){
	std::cout<<"\n        name         #  team  gp  g  a  pp  sh  pim  goalStreak  pointStreak\n";
}

void Player::printString(){
	spaceBuffer(20, name);
	spaceBuffer(3, std::to_string(number));
	spaceBuffer(6, team);
	spaceBuffer(4, std::to_string(gp));
	spaceBuffer(3, std::to_string(g));
	spaceBuffer(3, std::to_string(a));
	spaceBuffer(4, std::to_string(pp));
	spaceBuffer(4, std::to_string(sh));
	spaceBuffer(5, std::to_string(pim));
	spaceBuffer(12, std::to_string(goalStreak));
	spaceBuffer(13, std::to_string(pointStreak));
	std::cout<<"\n";
}