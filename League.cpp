#include <curl/curl.h>
#include <iostream>
#include <string>
#include "mhshl.h"

League::League(bool var){
	varsity = var;
}

void League::addTeam(std::string abbr, std::string name, std::string city, int identifier){
	Team t = Team(abbr, name, city, identifier);
	teams.push_back(t);
}

void League::addScoringEvent(std::string gID, std::string tID, int gs, int a1, int a2, int per, int sec){

	for(int i = 0; i < games.length(); i++){

		if(games[i].ID == gID){
			if(homeTeam == tID){
				games.homeScore++;
			}else{
				games.awayScore++;
			}
			break;
		}
	}

	for(int i = 0; i < players.length(); i++){
		if(players[i].team == tID){
			if(players[i].number == gs){
				players[i].g++;
			}
			if(players[i].number == a1 || players[i].number == a2){
				players[i].a++;
			}
		}
	}
}