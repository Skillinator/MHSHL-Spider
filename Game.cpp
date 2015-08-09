#include <iostream>
#include <string>
#include <vector>
#include "mhshl.h"

Game::Game(int m, int d, int y, int st, std::string home, std::string away, bool var){
	month = m;
	day = d;
	year = y;
	startTime = st;

	homeTeam = home;
	awayTeam = away;

	awayScore = homeScore = homeShots = awayShots = 0;

	period = 1;
	time = 14*60;
	if(var){
		time = 17*60;
	}
	ID.append(twoPlace(month));
	ID.append(twoPlace(day));
	ID.append(twoPlace(year));
	ID.append("-");
	ID.append(homeTeam);
}

void Game::printHeading(){
	std::cout<<"\n  GameID   m  d  y  start  home  away  homeScore  awayScore  homeShots  awayShots  per  time\n";
}

void Game::printString(){
	spaceBuffer(10, ID);
	spaceBuffer(3, std::to_string(month));
	spaceBuffer(3, std::to_string(day));
	spaceBuffer(3, std::to_string(year));
	std::cout<<" "<<twoPlace(startTime/60)<<":"<<twoPlace(startTime%60);
	spaceBuffer(6, homeTeam);
	spaceBuffer(6, awayTeam);
	spaceBuffer(12, std::to_string(homeScore));
	spaceBuffer(11, std::to_string(awayScore));
	spaceBuffer(11, std::to_string(homeShots));
	spaceBuffer(11, std::to_string(awayShots));
	spaceBuffer(5, std::to_string(period));
	std::cout<<" "<<twoPlace(time/60)<<":"<<twoPlace(time%60)<<"\n";


}