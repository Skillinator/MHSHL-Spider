#include <curl/curl.h>
#include <iostream>
#include <string>
#include "mhshl.h"

void spaceBuffer(int len, std::string str){
	for(int x = 0; x < (len-str.length())/2; x++){
		std::cout<<" ";
	}
	if((len-str.length())%2){
		std::cout<<" ";
	}
	std::cout<<str;
	for(int x = 0; x < (len-str.length())/2; x++){
		std::cout<<" ";
	}
}

std::string twoPlace(int num){
	if(num < 10){
		return "0" + std::to_string(num);
	}
	return std::to_string(num);
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

	ScoringEvent se = ScoringEvent("011115-QCB", "QCB", 16, 49, 97, 3, 126);
	se.printHeading();
	se.printString();

	PenaltyEvent pe = PenaltyEvent("011115-QCB", "QCB", 22, 10, 3, 480, "Elbowing");
	pe.printHeading();
	pe.printString();

	Game g = Game(11, 1, 15, 1205, "QCB", "DUB", true);
	g.printHeading();
	g.printString();

	return 0;
}