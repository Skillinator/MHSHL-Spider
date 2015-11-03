#include <curl/curl.h>
#include <iostream>
#include <string>
#include "mhshl.h"

League::League(int pL){
	periodLength = pL * 60;
}

void League::setGoaliePerformance(std::string gID, std::string tID, int pl, int f, int a, std::string res, int sec, int ga, int sa){
	if(sec < 1)
		return;
	for(int i = 0; i < goaliePerformances.size(); i++){
		if(goaliePerformances.size() == 0)
			break;
		GoaliePerformance temp = goaliePerformances[i];
		if(temp.teamID == tID && temp.gameID == gID && temp.player = pl){
			goaliePerformances[i].finalFor = f;
			goaliePerformances[i].finalAgainst = a;
			goaliePerformances[i].result = res;
			goaliePerformances[i].seconds = sec;
			goaliePerformances[i].goalsAgainst = ga;
			goaliePerformances[i].shotsAgainst = sa;
			goaliePerformances[i].saves = sa-ga;
			return;
		}
	}
	
	GoaliePerformance gp = GoaliePerformance(gID, tID, pl, f, a, res, sec, ga, sa);
	goaliePerformances.push_back(gp);
}

void League::updatePenalties(std::string gID, int dT){
	if(penalties.size() == 0)
		return;

	for(int i = 0; i < penalties.size(); i++){
		if(penalties[i].gameID == gID){
			int penaltyTime = periodLength*penalties[i].period - penalties[i].time;
			penalties[i].timeRemaining = penalties[i].duration * 60 - (dT - penaltyTime);
		}
		if(penalties[i].timeRemaining < 0)
			penalties[i].timeRemaining = 0;
		if(penalties[i].timeRemaining > penalties[i].duration*60)
			penalties[i].timeRemaining = 0;
		if(penalties[i].scoredOn)
			penalties[i].timeRemaining = 0;
	}
}

void League::removeOldestPenalty(std::string gID, std::string tID){
	if(penalties.size() == 0)
		return;
	for(int i = 0; i < penalties.size(); i++){
		PenaltyEvent *pe = &penalties[i];
		if(pe->gameID == gID && pe->teamID == tID && pe->timeRemaining > 0 && pe->timeRemaining < pe->duration*60 && !pe->scoredOn && pe->duration < 10){
			pe->goalsWhile++;
			if(pe->duration <= 2)
				pe->scoredOn = true;

			if(pe->duration <= 4 && pe->goalsWhile >= 2)
				pe->scoredOn = true;

			return;
		}
	}
}

int League::powerPlayGoal(std::string gID, std::string tID){
	int homePenalties = 0;
	int awayPenalties = 0;
		
	std::string homeID = getGame(gID)->homeTeam;
	std::string awayID = getGame(gID)->awayTeam;

	if(penalties.size() == 0)
		return 0;
	for(int i = 0; i < penalties.size(); i++){
		if(penalties[i].gameID == gID){
			PenaltyEvent pe = penalties[i];
			if(pe.timeRemaining > 0 && pe.timeRemaining < pe.duration*60 && !pe.scoredOn && pe.duration < 10){
				if(pe.teamID == homeID){
					homePenalties++;
				}else{
					awayPenalties++;
				}
			}
		}
	}
	
	if(homePenalties == awayPenalties)
		return 0;
	if(tID == homeID){
		if(homePenalties > awayPenalties)
			return -1;

		if(homePenalties < awayPenalties){
			removeOldestPenalty(gID, awayID);
			return 1;
		}
	}


	if(tID == awayID){
		if(homePenalties > awayPenalties){
			removeOldestPenalty(gID, homeID);
			return 1;
		}
		
		if(homePenalties < awayPenalties)
			return -1;

	}

	return 0;
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
	updatePenalties(gID, universalTime);
}

bool League::addTeam(std::string abbr, std::string name, std::string city, int identifier){
	
	if(getTeam(abbr)->abbreviation != "NUL")
		return false;

	Team t = Team(abbr, name, city, identifier);
	teams.push_back(t);
	return true;
}

bool League::addPlayer(std::string tID, std::string name, int identifier, int number){
	
	if(getPlayer(tID, number)->name != "NULL")
		return false;

	Player p = Player(tID, name, identifier, number);
	players.push_back(p);
	return true;
}

bool League::addGame(int m, int d, int y, int st, std::string home, std::string away, int gameID){
	std::string ID = twoPlace(m) + twoPlace(d) + twoPlace(y) + "-" + home;

	if(getGame(ID)->id != "NULL")
		return false;

	Game g = Game(m, d, y, st, home, away, periodLength, gameID);
	games.push_back(g);
	return true;
}

void League::addPenaltyEvent(std::string gID, std::string tID, int player, int duration, int per, int time, std::string penalty){
	std::cout<<player<<":"<<penalty<<":"<<duration<<"\n";
	if(player == 8162506)
		std::cout<<"BRENNEN PETERS\n";
	updateGameTime(gID, per, time);
	PenaltyEvent *peTest = getPenaltyEvent(gID, tID, player, per, time, duration, penalty);
	if(peTest->offense != "NULL")
		return;
	PenaltyEvent pe = PenaltyEvent(gID, tID, player, duration, per, time, penalty);
	getTeam(tID)->pim += duration;
	getPlayer(tID, player)->pim += duration;
	penalties.push_back(pe);
}

void League::addScoringEvent(std::string gID, std::string tID, int gs, int a1, int a2, int per, int sec, int pp){
	updateGameTime(gID, per, sec);	
	ScoringEvent *seTest = getScoringEvent(gID, per, sec);
	if(seTest->teamID != "NUL"){
		if(gs != seTest->scorer){
			getPlayer(tID, seTest->scorer)->g--;
			getPlayer(tID, gs)->g++;
			if(seTest->pp == 1){
				getPlayer(tID, seTest->scorer)->pp--;
				getPlayer(tID, gs)->pp++;
			}
			if(seTest->pp == -1){
				getPlayer(tID, seTest->scorer)->sh--;
				getPlayer(tID, gs)->sh++;
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
	}

	ScoringEvent se = ScoringEvent(gID, tID, gs, a1, a2, per, sec);
	se.pp = pp;
	goals.push_back(se);
	
	Game* game = getGame(gID);

	bool againstHome = false;
	
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
	
	// Do not count shootout goals
	if(per < 5){


		getTeam(tID)->gf++;

		getPlayer(tID, gs)->g++;
		
		if(se.pp == 1)
			getPlayer(tID, gs)->pp++;

		if(se.pp == -1){
			getPlayer(tID, gs)->sh++;
		}
		getPlayer(tID, a1)->a++;
		getPlayer(tID, a2)->a++;
	}

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


Team* League::getTeam(int num){
	if(teams.size() == 0)
		return new Team();

	for(int i = 0; i < teams.size(); i++){
		if(teams[i].id == num)
			return &teams[i];
	}

	return new Team();
}

Player* League::getPlayer(std::string tID, int num){
	if(players.size() == 0)
		return new Player();

	for(int i = 0; i < players.size(); i++){
		if(players[i].team == tID && players[i].id == num)
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
