#include <vector>
#include <curl/curl.h>
#include <string>
#include <iostream>
#include "mhshl.h"
#include "mhshlUtils.h"


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

void dumpStatus(League l){

	showTeams(l);
	showPlayers(l);
	showGames(l);
	showPenalties(l);
	showGoals(l);

}