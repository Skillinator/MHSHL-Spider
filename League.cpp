#include <curl/curl.h>
#include <iostream>
#include <string>
#include "mhshl.h"

League::League(bool var){
	varsity = var;
}

bool League::addTeam(std::string abbr, std::string name, std::string city, int identifier){
	for(int i = 0; i < teams.size(); i++){
		if(teams.size() > 0 && teams[i].abbreviation == abbr)
			return false;
	}
	Team t = Team(abbr, name, city, identifier);
	teams.push_back(t);
	return true;
}

bool League::addPlayer(std::string tID, std::string name, int number){
	for(int i = 0; i < players.size(); i++){
		if(players.size() > 0 && players[i].team == tID && players[i].number == number)
			return false;
	}
	Player p = Player(tID, name, number);
	players.push_back(p);
	return true;
}

bool League::addGame(int m, int d, int y, int st, std::string home, std::string away, bool var){
	std::string ID;

	ID.append(twoPlace(m));
	ID.append(twoPlace(d));
	ID.append(twoPlace(y));
	ID.append("-");
	ID.append(home);

	if(games.size() > 0){
		for(int i = 0; i < games.size(); i++){
			if(games[i].ID == ID)
				return false;
		}
	}
	Game g = Game(m, d, y, st, home, away, var);
	games.push_back(g);
	return true;
}

void League::addScoringEvent(std::string gID, std::string tID, int gs, int a1, int a2, int per, int sec){
	
	int gameid = -1;
	bool againstHome = false;

	for(int i = 0; i < games.size(); i++){

		if(games.size() > 0 && games[i].ID == gID){
			if(games[i].homeTeam == tID){
				games[i].homeScore++;
				gameid = i;
			}else{
				games[i].awayScore++;
				gameid = i;
				againstHome = true;
			}
			break;
		}
	}

	for(int i = 0; i < teams.size(); i++){
		if(teams.size() > 0 && teams[i].abbreviation == tID)
			teams[i].gf++;
	}

	if(gameid != -1){
		std::string against = games[gameid].homeTeam;
		if(againstHome == false)
			against = games[gameid].awayTeam;

		for(int i=0; i < teams.size(); i++){
			if(teams.size() > 0 && teams[i].abbreviation == against)
				teams[i].ga++;
		}
		
	}

	for(int i = 0; i < players.size(); i++){
		if(players.size() > 0 && players[i].team == tID){
			if(players[i].number == gs){
				players[i].g++;
			}
			if(players[i].number == a1 || players[i].number == a2){
				players[i].a++;
			}
		}
	}
}