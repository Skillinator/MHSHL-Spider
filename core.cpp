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

	League l = League(true);
	l.addTeam("QCB", "Blues", "Quad City", 1);
	l.addTeam("DBQ", "Devils", "Dubuque", 2);
	l.addTeam("CDR", "Jr. Roughriders", "Cedar Rapids", 3);
	l.addTeam("WAT", "Warriors", "Waterloo", 4);
	l.addTeam("DMC", "Capitals", "Des Moines", 5);
	l.addTeam("DMO", "Oak Leafs", "Des Moines", 6);
	l.addTeam("AMS", "Little Cyclones", "Ames", 7);
	l.addTeam("MCM", "Mohawks", "Mason City", 8);
	l.addTeam("SCM", "Metros", "Sioux City", 9);
	l.addTeam("KCJ", "Jets", "Kansas City", 10);
	l.addTeam("OJL", "Jr. Lancers", "Omaha", 11);
	l.addTeam("LJS", "Jr. Stars", "Lincoln", 12);

	l.addPlayer("QCB", "Test Player 1", 99);
	l.addPlayer("QCB", "Test Player 2", 87);
	l.addPlayer("QCB", "Test Player 3", 66);

	l.addGame(11, 1, 15, 1200, "QCB", "CDR", true);
	showTeams(l);
	showPlayers(l);
	showGames(l);


	l.addScoringEvent("110115-QCB", "QCB", 66, 99, 87, 3, 60);

	showTeams(l);
	showPlayers(l);
	showGames(l);
	return 0;
}