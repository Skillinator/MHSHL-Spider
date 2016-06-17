#include <iostream>
#include <string>
#include <vector>
#include "mhshl.h"
#include "mhshlUtils.h"

Game::Game(){
	month = day = year = startTime = awayScore = homeScore = awayShots = homeShots = period = time = 0;
	homeTeam = awayTeam = "NUL";
	id = "NULL";
	rosterTaken = false;
}

Game::Game(int m, int d, int y, int st, std::string home, std::string away, int t, int num){
	month = m;
	day = d;
	year = y;
	startTime = st;

	homeTeam = home;
	awayTeam = away;

	awayScore = homeScore = homeShots = awayShots = 0;

	period = 1;

	time = t;
	number = num;
	rosterTaken = false;
	id = twoPlace(m) + twoPlace(d) + twoPlace(y) + "-" + home;
}

void Game::printHeading(){
	std::cout<<"\n     GameID      m  d    y   start   home    away   homeScore  awayScore  homeShots  awayShots  per  time    ID\n";
}

void Game::printString(){ 
	spaceBuffer(16, id);
	spaceBuffer(3, std::to_string(month));
	spaceBuffer(4, std::to_string(day));
	spaceBuffer(3, std::to_string(year));
	std::cout<<" "<<twoPlace(startTime/60)<<":"<<twoPlace(startTime%60);
	spaceBuffer(8, homeTeam);
	spaceBuffer(8, awayTeam);
	spaceBuffer(12, std::to_string(homeScore));
	spaceBuffer(11, std::to_string(awayScore));
	spaceBuffer(11, std::to_string(homeShots));
	spaceBuffer(11, std::to_string(awayShots));
	spaceBuffer(5, std::to_string(period));
	std::cout<<" "<<twoPlace(time/60)<<":"<<twoPlace(time%60);
	spaceBuffer(8, std::to_string(number));
	std::cout<<"\n";


}
