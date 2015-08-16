#include <curl/curl.h>
#include <iostream>
#include <string>
#include "mhshl.h"

League::League(int pL){
	periodLength = pL * 60;
}

void League::updatePenalties(std::string gID, int dT){
	if(penalties.size() == 0)
		return;

	for(int i = 0; i < penalties.size(); i++){
		if(penalties[i].gameID == gID){
			penalties[i].timeRemaining += dT;
		}

		if(penalties[i].timeRemaining < 0)
			penalties[i].timeRemaining = 0;
	}
}

bool League::powerPlayGoal(std::string gID, std::string tID){
	return false;
}


void League::updateGameTime(std::string gID, int per, int sec){
	Game* g = getGame(gID);

	int gameTime = g->time;
	int gamePeriod = g->period;

	int universalTime = per*periodLength - sec;
	int gameUniversalTime = gamePeriod*periodLength - gameTime;

	if(gamePeriod == 4)
		gameUniversalTime = 3*periodLength + 5 - gameTime;

	int dT = gameUniversalTime - universalTime;

	g->time = sec;
	g->period = per;
	updatePenalties(gID, dT);
}

bool League::addTeam(std::string abbr, std::string name, std::string city, int identifier){
	
	if(getTeam(abbr)->abbreviation != "NUL")
		return false;

	Team t = Team(abbr, name, city, identifier);
	teams.push_back(t);
	return true;
}

bool League::addPlayer(std::string tID, std::string name, int number){
	
	if(getPlayer(tID, number)->name != "NULL")
		return false;

	Player p = Player(tID, name, number);
	players.push_back(p);
	return true;
}

bool League::addGame(int m, int d, int y, int st, std::string home, std::string away){
	std::string ID = twoPlace(m) + twoPlace(d) + twoPlace(y) + "-" + home;

	if(getGame(ID)->id != "NULL")
		return false;

	Game g = Game(m, d, y, st, home, away, varsity);
	games.push_back(g);
	return true;
}

void League::addPenaltyEvent(std::string gID, std::string tID, int player, int per, int time, int duration, std::string penalty){
	PenaltyEvent *peTest = getPenaltyEvent(gID, tID, player, per, time, duration, penalty);
	if(peTest->offense != "NULL")
		return;
	PenaltyEvent pe = PenaltyEvent(gID, tID, player, duration, per, time, penalty);
	getTeam(tID)->pim += duration;
	getPlayer(tID, player)->pim += duration;
	penalties.push_back(pe);
	updateGameTime(gID, per, time);
}

void League::addScoringEvent(std::string gID, std::string tID, int gs, int a1, int a2, int per, int sec){
	
	updateGameTime(gID, per, sec);	
	ScoringEvent *seTest = getScoringEvent(gID, per, sec);
	if(seTest->teamID != "NUL"){
		if(gs != seTest->scorer){
			getPlayer(tID, seTest->scorer)->g--;
			getPlayer(tID, gs)->g++;
			if(powerPlayGoal(gID, tID)){
				getPlayer(tID, seTest->scorer)->pp--;
				getPlayer(tID, gs)->pp++;
			}
		}
		if(a1 != seTest->assist1){
			getPlayer(tID, seTest->assist1)->a--;
			getPlayer(tID, a1)->a++;
		}
		if(a2 != seTest->assist2){
			getPlayer(tID, seTest->assist2)->a--;
			getPlayer(tID, a2)->a++;
		}
		return;
	}else{
		std::cout<<seTest->teamID<<"\n";
	}

	ScoringEvent se = ScoringEvent(gID, tID, gs, a1, a2, per, sec);
	goals.push_back(se);

	int gameid = -1;
	bool againstHome = false;


	getTeam(tID)->gf++;

	getPlayer(tID, gs)->g++;
	
	if(powerPlayGoal(gID, tID))
		getPlayer(tID, gs)->pp++;

	getPlayer(tID, a1)->a++;
	getPlayer(tID, a2)->a++;


	Game* game = getGame(gID);

	if(game->id == "NULL")
		return;

	if(game->homeTeam == tID){
		game->homeScore++;
	} else{
		game->awayScore++;
		againstHome = true;
	}
	
	if(againstHome)
		getTeam(game->homeTeam)->ga++;
	if(!againstHome)
		getTeam(game->awayTeam)->ga++;


}

Game* League::getGame(std::string gID){
	if(games.size() == 0)
		return new Game();
	for(int i=0; i < games.size(); i++){
		if(games[i].id == gID)
			return &games[i];
	}
	return new Game();
}

Team* League::getTeam(std::string tID){
	if(teams.size() == 0)
		return new Team();

	for(int i = 0; i < teams.size(); i++){
		if(teams[i].abbreviation == tID)
			return &teams[i];
	}

	return new Team();
}

Player* League::getPlayer(std::string tID, int num){
	if(players.size() == 0)
		return new Player();

	for(int i = 0; i < players.size(); i++){
		if(players[i].team == tID && players[i].number == num)
			return &players[i];
	}

	return new Player();
}

PenaltyEvent* League::getPenaltyEvent(std::string gID, std::string tID, int player, int per, int time, int duration, std::string penalty){
	if(penalties.size() == 0)
		return new PenaltyEvent();

	for(int i = 0; i < penalties.size(); i++){
		PenaltyEvent pe = penalties[i];
		if(pe.gameID == gID && pe.teamID == tID && pe.player == player && pe.period == per && pe.time == time && pe.duration == duration && pe.offense == penalty)
			return &penalties[i];
	}

	return new PenaltyEvent();
}

ScoringEvent* League::getScoringEvent(std::string gID, int per, int time){
	if(goals.size() == 0)
		return new ScoringEvent();

	for(int i = 0; i < goals.size(); i++){
		if(goals[i].gameID == gID && goals[i].period == per && goals[i].time == time)
			return &goals[i];
	}

	return new ScoringEvent();

}