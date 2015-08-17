#include <curl/curl.h>
#include <iostream>
#include <string>
#include "mhshl.h"

void spaceBuffer(int len, std::string str){
	for(int x = 0; x < (len-str.length())/2; x++){
		std::cout<<" ";
	}
	
	if((len-str.length())%2)
		std::cout<<" ";

	std::cout<<str;
	for(int x = 0; x < (len-str.length())/2; x++){
		std::cout<<" ";
	}
}

std::string twoPlace(int num){
	if(num < 10)
		return "0" + std::to_string(num);
	return std::to_string(num);
}

void showTeams(League l){
	if(l.teams.size() == 0)
		return;

	l.teams[0].printHeading();
	for(int i = 0; i < l.teams.size(); i++){
			l.teams[i].printString();
	}
}

void showPlayers(League l){
	if(l.players.size() == 0)
		return;

	l.players[0].printHeading();
	for(int i = 0; i < l.players.size(); i++){
		l.players[i].printString();
	}
}
void showGames(League l){
	if(l.games.size() == 0)
		return;

	l.games[0].printHeading();
	for(int i = 0; i < l.games.size(); i++){
		l.games[i].printString();
	}
}

void showPenalties(League l){
	if(l.penalties.size() == 0)
		return;

	l.penalties[0].printHeading();
	for(int i = 0; i < l.penalties.size(); i++){
		l.penalties[i].printString();
	}
}
void showGoals(League l){
	if(l.goals.size() == 0)
		return;

	l.goals[0].printHeading();
	for(int i = 0; i < l.goals.size(); i++){
		l.goals[i].printString();
	}
}

void initMHSHL(League l){
	std::cout<<l.addTeam("QCB", "Blues", "Quad City", 1);
	std::cout<<l.addTeam("DBQ", "Devils", "Dubuque", 2);
	std::cout<<l.addTeam("CDR", "Jr. Roughriders", "Cedar Rapids", 3);
	std::cout<<l.addTeam("WAT", "Warriors", "Waterloo", 4);
	std::cout<<l.addTeam("DMC", "Capitals", "Des Moines", 5);
	std::cout<<l.addTeam("DMO", "Oak Leafs", "Des Moines", 6);
	std::cout<<l.addTeam("AMS", "Little Cyclones", "Ames", 7);
	std::cout<<l.addTeam("MCM", "Mohawks", "Mason City", 8);
	std::cout<<l.addTeam("SCM", "Metros", "Sioux City", 9);
	std::cout<<l.addTeam("KCJ", "Jets", "Kansas City", 10);
	std::cout<<l.addTeam("OJL", "Jr. Lancers", "Omaha", 11);
	std::cout<<l.addTeam("LJS", "Jr. Stars", "Lincoln", 12);
}

void dumpStatus(League l){

	showTeams(l);
	showPlayers(l);
	showGames(l);
	showPenalties(l);
	showGoals(l);

}

int main(){
	/*
	CURL *curl;
	CURLcode res;

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	if(curl){
		curl_easy_setopt(curl, CURLOPT_URL, "http://midwest-league.stats.pointstreak.com/players-leagues.html?leagueid=301");

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

		res = curl_easy_perform(curl);

		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	*/

	League l = League(17);
	initMHSHL(l);

	std::cout<<l.addPlayer("QCB", "Test Player 1", 99);
	std::cout<<l.addPlayer("QCB", "Test Player 2", 87);
	std::cout<<l.addPlayer("QCB", "Test Player 3", 66);
	std::cout<<l.addPlayer("CDR", "Test Player 4", 71);

	std::cout<<l.addGame(11, 1, 15, 1200, "QCB", "CDR");


	l.updateGameTime("110115-QCB", 1, 930);
	l.addScoringEvent("110115-QCB", "QCB", 66, 99, 87, 1, 960);
	
	l.addPenaltyEvent("110115-QCB", "QCB", 99, 1, 90, 2, "slashing");
	

	dumpStatus(l);

	l.updateGameTime("110115-QCB", 1, 900);
	
	dumpStatus(l);

	l.updateGameTime("110115-QCB", 1, 925);
	
	dumpStatus(l);

	return 0;
}