#include <curl/curl.h>
#include <iostream>
#include <string>
#include "mhshl.h"
#include "mhshlUtils.h"

League::League(int pL, bool var, int s){
	periodLength = pL * 60;
	varsity = var;
	season = s;
}

void League::setGoaliePerformance(std::string gID, std::string tID, int pl, int sec, int ga, int sa){
	if(sec < 1)
		return;
	for(int i = 0; i < goaliePerformances.size(); i++){
		if(goaliePerformances.size() == 0)
			break;

		GoaliePerformance temp = goaliePerformances[i];
		if(temp.teamID == tID && temp.gameID == gID && temp.player == pl){
			goaliePerformances[i].seconds = sec;
			goaliePerformances[i].goalsAgainst = ga;
			goaliePerformances[i].shotsAgainst = sa;
			goaliePerformances[i].saves = sa-ga;
			return;
		}
	}
	GoaliePerformance gp = GoaliePerformance(gID, tID, pl, sec, ga, sa);
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

		if(penalties[i].gameID == gID)
			game->updatePenalty(&penalties[i], this);
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
	db_updateGame(g, this);
	updatePenalties(gID, universalTime);
}

bool League::addTeam(Team *team){

	if(getTeam(team->abbreviation)->abbreviation != "NUL")
		return false;

	teams.push_back(*team);

	db_addTeam(team, this);

	return true;
}

bool League::addPlayer(Player *player){

	if(getPlayer(player->teamID, player->jerseyNumber)->name != "NULL")
		return false;

	players.push_back(*player);

	db_addPlayer(player, this);

	return true;
}

bool League::addGame(Game *game){

	if(getGame(game->id)->id != "NULL")
		return false;

	games.push_back(*game);
	db_addGame(game, this);

	return true;
}

void League::addPenaltyEvent(PenaltyEvent *penaltyEvent){

	updateGameTime(penaltyEvent->gameID, penaltyEvent->period, penaltyEvent->time);

	PenaltyEvent *peTest = getPenaltyEvent(penaltyEvent->gameID, penaltyEvent->teamID, penaltyEvent->player, penaltyEvent->period, penaltyEvent->time, penaltyEvent->duration, penaltyEvent->offense);
	if(peTest->offense != "NULL")
		return;

	getTeam(penaltyEvent->teamID)->penaltyMinutes += penaltyEvent->duration;
	getPlayer(penaltyEvent->teamID, penaltyEvent->player)->penaltyMinutes += penaltyEvent->duration;

	penalties.push_back(*penaltyEvent);
	db_addPenalty(penaltyEvent, this);
}

void League::addScoringEvent(ScoringEvent *scoringEvent){
	updateGameTime(scoringEvent->gameID, scoringEvent->period, scoringEvent->time);

	ScoringEvent *seTest = getScoringEvent(scoringEvent->gameID, scoringEvent->period, scoringEvent->time);
	if(seTest->teamID != "NUL"){
		if(scoringEvent->scorer != seTest->scorer){
			getPlayer(scoringEvent->teamID, seTest->scorer)->goals--;
			getPlayer(scoringEvent->teamID, scoringEvent->scorer)->goals++;
			if(seTest->powerPlay == 1){
				getPlayer(scoringEvent->teamID, seTest->scorer)->powerPlayGoals--;
				getPlayer(scoringEvent->teamID, scoringEvent->scorer)->powerPlayGoals++;
			}
			if(seTest->powerPlay == -1){
				getPlayer(scoringEvent->teamID, seTest->scorer)->shortHandedGoals--;
				getPlayer(scoringEvent->teamID, scoringEvent->scorer)->shortHandedGoals++;
			}
		}

		if(scoringEvent->assist1 != seTest->assist1){
			getPlayer(scoringEvent->teamID, seTest->assist1)->assists--;
			getPlayer(scoringEvent->teamID, scoringEvent.assist1)->assists++;
		}
		if(scoringEvent->assist2 != seTest->assist2){
			getPlayer(scoringEvent->teamID, seTest->assist2)->assists--;
			getPlayer(scoringEvent->teamID, scoringEvent->assist2)->assists++;
		}
		seTest->scorer = scoringEvent->scorer;
		seTest->assist1 = scoringEvent->assist1;
		seTest->assist2 = scoringEvent->assist2;
		return;
	}

	goals.push_back(*scoringEvent);

	Game* game = getGame(scoringEvent->gameID);

	bool againstHome = false;

	if(game->id == "NULL")
		return;
	if(game->homeTeam == scoringEvent->teamID){
		game->homeScore++;
	} else{
		game->awayScore++;
		againstHome = true;
	}

	if(againstHome)
		getTeam(game->homeTeam)->goalsAgainst++;
	if(!againstHome)
		getTeam(game->awayTeam)->goalsAgainst++;

	// Do not count shootout goals
	if(scoringEvent->period < 5){

		getTeam(scoringEvent->teamID)->goalsFor++;

		getPlayer(scoringEvent->teamID, scoringEvent->scorer)->goals++;

		if(scoringEvent->powerPlay == 1)
			getPlayer(scoringEvent->teamID, scoringEvent->scorer)->powerPlayGoals++;

		if(scoringEvent->powerPlay == -1){
			getPlayer(scoringEvent->teamID, scoringEvent->scorer)->shortHandedGoals++;
		}
		getPlayer(scoringEvent->teamID, scoringEvent->assist1)->assists++;
		getPlayer(scoringEvent->teamID, scoringEvent->assist2)->assists++;
	}

	db_addGoal(scoringEvent, this);
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
		if(teams[i].pointstreakID == num)
			return &teams[i];
	}

	return new Team();
}

Player* League::getPlayer(std::string tID, int num){
	if(players.size() == 0)
		return new Player();

	for(int i = 0; i < players.size(); i++){
		if(players[i].teamID == tID && players[i].pointstreakID == num)
			return &players[i];
	}

	return new Player();
}

Player* League::getPlayer(int id){

	if(players.size() == 0)
		return new Player();

	for(int i = 0; i < players.size(); i++){
		if(players[i].pointstreakID == id)
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
