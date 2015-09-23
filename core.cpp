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

void processGoal(Game* g, League* l, int p, std::string s){
	std::string gID="null";
	std::string tID="null";
	int scorernum=0;
	int a1 = 0;
	int a2 = 0;
	int per = p;
	int sec=0;
	
	if(s.find("no scoring") != std::string::npos)
		return;

	std::vector<std::string> firstsplit = split(s, "(");
	std::vector<std::string> teamscorer = split(firstsplit[0]," - ");
	/*
	std::string assists = split(firstsplit[1], ")")[0];
	std::string time = split(split(firstsplit[1], ")")[1], "<br")[0];
	*/	
	std::string team = extract(teamscorer[0], "b");
	std::string scorer = extract(teamscorer[1], "b");

	gID = g->id;
	scorernum = std::stoi(getValue(scorer, "playerid"));
	tID = translateTeamID(team);
	/*
	if(assists == "unassisted"){
		// do nothing
	}else if(assists.find(", ") == std::string::npos){
		a1 = std::stoi(getValue(assists, "playerid"));		
	}else{
		std::vector<std::string> assistvec = split(assists, ", ");
		a1 = std::stoi(getValue(assistvec[0], "playerid"));
		a2 = std::stoi(getValue(assistvec[1], "playerid"));
	}
	*/
	std::cout<<team<<"\n";
	if(tID == g->homeTeam){
		std::cout<<"Team " << tID << " == " << g->homeTeam << "\n";
		g->homeScore++;
	}else{
		g->awayScore++;
	}
	
}

void updateGame(Game* g, League* l){
	std::string url = "midwest-league.stats.pointstreak.com/players-boxscore.html?gameid=" + std::to_string(g->number);
	std::string page = fetchWebPage(url);
	
	std::string goals = split(page, "Scoring Summary")[1];
	
	goals = split(goals, "</table>")[0];

	std::vector<std::string> goalVec = split(goals, "</td>");
	goalVec.erase(goalVec.begin());
	goalVec.erase(goalVec.end());

	
	for(int i = 0; i < goalVec.size(); i++){
		goalVec[i] = extract(goalVec[i] + "</td>", "td");
	}
	
	for(int i = 0; i < goalVec.size(); i+=2){
		if(goalVec[i+1].find("(no scoring)") == std::string::npos ){
			// Here we do the actual interpretation
			int per = getPeriod(goalVec[i]);
			if(goalVec[i+1].find("<tr>") != std::string::npos){
				std::vector<std::string> firstsplit = split(goalVec[i+1], "<tr>");
				for(int n = 0; n < firstsplit.size(); n++){
					if(firstsplit[n].find("<td") != std::string::npos)
						firstsplit[n] = extract(firstsplit[n], "td");
					processGoal(g, l, per, firstsplit[n]);
				}
			}else{
				processGoal(g, l, per, goalVec[i+1]);
			}
		}
	}	
}

void getGames(int season, int team, League *l){
	
	std::cout<<"Fetching Games for team " << team << " during the " << season << " season.\n";

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
	
	std::cout<< "Extracted Games\n";

	for(int i = 1; i < games.size(); i++){
		std::cout<<"Processing game " << i+1 << " of " << games.size() << "\n";
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

void sort_games(League* l){
	std::cout<<"Sorting Games\n";	
	// Sort games by date. I probably could move this somewhere else because currently it will repeat for each team. Lot of redundancy.

	bool sorted = false;
	while(!sorted){
		sorted = true;
		for(int i = 0; i < l->games.size()-1; i++){
			std::string date1 = std::to_string(l->games[i].year) + twoPlace(l->games[i].month) + twoPlace(l->games[i].day);
			std::string date2 = std::to_string(l->games[i+1].year) + twoPlace(l->games[i+1].month) + twoPlace(l->games[i+1].day);
			if(stoi(date1) > stoi(date2)){
				sorted = false;
				Game tmp = l->games[i];
				l->games[i] = l->games[i+1];
				l->games[i+1] = tmp;
			}
		}
	}
	
}

// Procedurally updates every game in the database. Only use when a full rebuild is needed.
void hard_update(League* l){
	for(int i = 0; i < l->games.size(); i++){
		std::cout<<"Updating game " << i+1 << " of " << l->games.size() << "\n";
		std::cout<<l->games[i].id<<"\n";
		updateGame(&l->games[i],l);
	}
	
	sort_games(l);
}

const int SEASON_2014_2015 = 13209;
const int SEASON_2015_2015 = 14757;

int main(){
	League l = League(17);
	initMHSHL(&l);
	for(int i = 0; i < l.teams.size(); i++){
		getGames(SEASON_2014_2015, l.teams[i].id, &l);
	}
	sort_games(&l);
	hard_update(&l);
	showGames(l);
	std::cout<<l.games.size();
	return 0;
}
