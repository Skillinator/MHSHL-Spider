#include <curl/curl.h>
#include <iostream>
#include <string>
#include "mhshl.h"
#include "mhshlUtils.h"

void initMHSHL(League *l){
	l->addTeam("QCB", "Blues", "Quad City", 47180);
	l->addTeam("DBQ", "Devils", "Dubuque", 47175);
	l->addTeam("CDR", "Jr. Roughriders", "Cedar Rapids", 47172);
	l->addTeam("WAT", "Warriors", "Waterloo", 47182);
	l->addTeam("DMC", "Capitals", "Des Moines", 47173);
	l->addTeam("DMO", "Oak Leafs", "Des Moines", 47174);
	l->addTeam("AMS", "Little Cyclones", "Ames", 47171);
	l->addTeam("MCM", "Mohawks", "Mason City", 47178);
	l->addTeam("SCM", "Metros", "Sioux City", 47181);
	l->addTeam("KCJ", "Jets", "Kansas City", 96071);
	l->addTeam("OJL", "Jr. Lancers", "Omaha", 149736);
	l->addTeam("LJS", "Jr. Stars", "Lincoln", 47177);
}


void initializeLeague(League *L){
	/*
	initMHSHL
	getRosters
	getGames
	serverSync
	*/
}

void update(League *l){
	/*

	Sort Scoring Events and Penalty Events by age
	
	While ScoringEvents + PenaltyEvents > 0:
		Add Earliest Event

	updateGameTime(t)
	
	*/
}

int getMinutes(std::string time){
	bool pm = (split(time, " ")[1] == "pm");
	std::string raw = split(time, " ")[0];
	int minutes = stoi(split(raw, ":")[0]) * 60 + stoi(split(raw, ":")[1]) + 12*60*pm;
	return minutes;
}

std::string getValue(std::string str, std::string value){
	int start = str.find(value + "=") + value.size()+1;
	int end = str.find("&", start);
	if(str.find('"', start) < end)
		end = str.find('"', start);

	return str.substr(start, end-start);
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

void getGames(int season, int team, League *l){
	std::string url = "midwest-league.stats.pointstreak.com/players-team-schedule.html?teamid=" + std::to_string(team) + "&seasonid=" + std::to_string(season);

	std::string page = fetchWebPage(url);
	std::vector<std::string> games = split(extract("<table" + split(page, "<table")[4], "table"), "<tr");

	games.erase(games.begin());
	games.erase(games.end());

	if(games.size() == 0)
		return;

	for(int i = 0; i < games.size(); i++){
		games[i] = extract("<tr" + games[i], "tr");
	}

	for(int i = 1; i < games.size(); i++){
		
		std::string homeID, awayID = " ";
		int	month, day, year, time, gameID;
		
		
		std::vector<std::string> elements = split(games[i], "<td");
		elements.erase(elements.begin());
		elements.erase(elements.end());

		for(int j=0; j < elements.size(); j++){
			elements[j] = extract("<td" + elements[j], "td");
		}
		homeID = l->getTeam(stoi(getValue(elements[0], "teamid")))->abbreviation;
		awayID = l->getTeam(stoi(getValue(elements[1], "teamid")))->abbreviation;
		
		month = getMonth(elements[2]);
		day = getDay(elements[2]);
		year = getYear(2015, month);
		time = getMinutes(elements[3]);
		gameID = stoi(getValue(elements[4], "gameid"));
	
		l->addGame(month, day, year, time, homeID, awayID, gameID);
	}

}

int main(){
	League l = League(17);
	initMHSHL(&l);
	for(int i = 0; i < l.teams.size(); i++){
		getGames(14757, l.teams[i].id, &l);
	}
	showGames(l);
	std::cout<<l.games.size();
	return 0;
}