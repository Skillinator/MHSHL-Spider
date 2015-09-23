#include <vector>
#include <curl/curl.h>
#include <string>
#include <iostream>
#include "mhshl.h"
#include "mhshlUtils.h"

/*
String Functions

Functions that help us process strings in the scraping process
*/

std::vector<std::string> split(std::string str, std::string delim){
	std::vector<std::string> tmp;

	int  startFrom = 0;

	while(startFrom < str.size() && str.find(delim, startFrom) != std::string::npos){
		int end = str.find(delim, startFrom);
		tmp.push_back(str.substr(startFrom, end-startFrom));
		startFrom = end+delim.size();
	}
	tmp.push_back(str.substr(startFrom, str.size()-1));

	return tmp;

}

std::string extract(std::string str, std::string tag){
	int start = str.find(">", str.find("<"+tag)) + 1;
	int end = str.find("</"+tag);
	return str.substr(start, end-start);
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
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
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
	if(len < str.size()){
		std::cout<<str;
		return;
	}
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

std::string getValue(std::string str, std::string value){
	int start = str.find(value + "=") + value.size()+1;
	int end = str.find("&", start);
	if(str.find('"', start) < end)
		end = str.find('"', start);

	return str.substr(start, end-start);
}

std::string removeSubstring(std::string str, std::string substr){
	if(str.find(substr) != std::string::npos)
		str.replace(s.find(substr), substr.size(), "");
	return str;
}

/*
Output functions

Functions that will batch output the data stored within the database
*/

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


/*
Time Functions

Functions to help us interpret, store, and read time/date

*/
int getMinutes(std::string time){
	bool pm = (split(time, " ")[1] == "pm");
	std::string raw = split(time, " ")[0];
	int minutes = stoi(split(raw, ":")[0]) * 60 + stoi(split(raw, ":")[1]) + 12*60*pm;
	return minutes;
}

int getPeriod(std::string per){
	if(per == "Period 1")
		return 1;
	if(per == "Period 2")
		return 2;
	if(per == "Period 3")
		return 3;
	if(per == "OT")
		return 4;
	if(per == "Shootout")
		return 5;
	return 0;
}

int getMonth(std::string date){
	std::string month = split(date, " ")[1];
	int m;
	if(month == "Jan")
		m = 1;
	if(month == "Feb")
		m = 2;
	if(month == "Mar")
		m = 3;
	if(month == "Apr")
		m = 4;
	if(month == "May")
		m = 5;
	if(month == "Jun")
		m = 6;
	if(month == "Jul")
		m = 7;
	if(month == "Aug")
		m = 8;
	if(month == "Sep")
		m = 9;
	if(month == "Oct")
		m = 10;
	if(month == "Nov")
		m = 11;
	if(month == "Dec")
		m = 12;
	return m;
}

int getDay(std::string date){
	return stoi(split(date, " ")[2]);
}

int getYear(int startYear, int month){
	if(month <= 6)
		return startYear+1;
	return startYear;
}


/*
Unsorted functions

Things that don't yet have any comrades.
*/
std::string translateTeamID(std::string s){

	if(s == "Ames")
		return "AMS";
	if(s == "DM Oak Leafs")
		return "DMO";
	if(s == "DM Capitals")
		return "DMC";
	if(s == "Sioux City")
		return "SCM";
	if(s == "Kansas City")
		return "KCJ";
	if(s == "Omaha")
		return "OJL";
	if(s == "Quad City")
		return "QCB";
	if(s == "Lincoln")
		return "LJS";
	if(s == "Dubuque")
		return "DBQ";
	if(s == "Mason City")
		return "MCM";
	if(s == "Waterloo")
		return "WAT";
	if(s == "Cedar Rapids")
		return "CDR";
	return "NUL";

}
