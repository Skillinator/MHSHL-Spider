#include <curl/curl.h>
#include <iostream>
#include <string>
#include "mhshl.h"
#include "mhshlUtils.h"

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


void initializeLeague(League L){
	/*
	initMHSHL
	getRosters
	getGames
	serverSync
	*/
}

void update(League l){
	/*

	Sort Scoring Events and Penalty Events by age
	
	While ScoringEvents + PenaltyEvents > 0:
		Add Earliest Event

	updateGameTime(t)
	
	*/
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *str){
	((std::string*)str)->append((char*)contents, size*nmemb);
	return size*nmemb;
}

std::string fetchWebPage(std::string url){

	CURL *curl;
	CURLcode res;

	std::string str;

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	if(curl){
		curl_easy_setopt(curl, CURLOPT_URL, "http://midwest-league.stats.pointstreak.com/players-leagues.html?leagueid=301");

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		res = curl_easy_perform(curl);

		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();

	return str;
}

int main(){
	

	League l = League(17);
	initMHSHL(l);

	std::cout<<l.addPlayer("QCB", "Test Player 1", 99);
	std::cout<<l.addPlayer("QCB", "Test Player 2", 87);
	std::cout<<l.addPlayer("QCB", "Test Player 3", 66);
	std::cout<<l.addPlayer("CDR", "Test Player 4", 71);
	std::cout<<l.addPlayer("CDR", "Test Player 5", 16);
	std::cout<<l.addPlayer("CDR", "Test Player 6", 31);

	std::cout<<l.addGame(11, 1, 15, 1200, "QCB", "CDR");

	l.addPenaltyEvent("110115-QCB", "CDR", 31, 2, 1, 1000, "Asshole3");
	l.addPenaltyEvent("110115-QCB", "CDR", 16, 2, 1, 1000, "Asshole2");
	l.addPenaltyEvent("110115-QCB", "QCB", 99, 2, 1, 1000, "Asshole");
	showGames(l);
	showPenalties(l);

	l.updateGameTime("110115-QCB", 2, 17*60-40);
	showGames(l);
	showPenalties(l);

	l.addScoringEvent("110115-QCB", "CDR", 71, 0, 0, 2, 60);
	showPlayers(l);
	showGames(l);
	showPenalties(l);

	l.addScoringEvent("110115-QCB", "CDR", 71, 0, 0, 2, 61);
	showPlayers(l);
	showGames(l);
	showPenalties(l);

	return 0;
}