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

void processGoal(Game* g, League* l, int p, std::string s){
	/*
	Initialize values to their default states
	*/
	std::string gID="null";
	std::string tID="null";
	int scorernum=0;
	int a1 = 0;
	int a2 = 0;
	int per = p;
	int sec=0;
	
	/*
	If no scoring occured, stop processing right here 
	*/
	if(s.find("no scoring") != std::string::npos)
		return;
	
	/*
	If there's a <br> on the end, remove that
	*/
	if(s.find("<br"))
		s = split(s, "<br")[0];
	
	/*
	Handle power play, shorthanded, and empty net goals
	*/
	s = removeSubstring(s, "<i>(empty net)</i>");
	s = removeSubstring(s, "<i>(power play)</i>");
	s = removeSubstring(s, "<i>(short handed)</i>");
	s = removeSubstring(s, "<i>(penalty shot)</i>");
	
	/*
	Split by the first parenthesis
	*/
	std::vector<std::string> firstsplit = split(s, "(");
	
	/*
	This willl give us the team name and the scorer ID
	The team name still needs to be run through the translate function since it's not how they're stored in our database
	*/
	std::vector<std::string> teamscorer = split(firstsplit[0]," - ");
	
	/*
	Separate the assists from the time
	*/
	std::string assists = split(firstsplit[1], ")")[0];
	std::string time = split(split(firstsplit[1], ")")[1], "<br")[0];
	
	/*
	Break the team and scorer values out of their bolding tags
	*/
	std::string team = extract(teamscorer[0], "b");
	std::string scorer = extract(teamscorer[1], "b");
	
	/*
	Set the first three values. 
	gameID is found directly from the game. 
	PlayerID is found from the playerid value passed in the href to the player's page.
	teamID is found by translating the team name into our style teamID.
	*/
	gID = g->id;
	scorernum = std::stoi(getValue(scorer, "playerid"));
	tID = translateTeamID(team);
	
	/*
	Process assists
	*/
	if(assists == "unassisted"){
		// do nothing, no assists
	}else if(assists.find(", ") == std::string::npos){
		/*
		Only one assist, process it accordingly.
		*/
		a1 = std::stoi(getValue(assists, "playerid"));		
	}else{
		// Split the assists apart before processing
		std::vector<std::string> assistvec = split(assists, ", ");
		
		/*
		Now it's basically the same as processing one assist, we just do it twice.
		*/
		a1 = std::stoi(getValue(assistvec[0], "playerid"));
		a2 = std::stoi(getValue(assistvec[1], "playerid"));
	}
	
	std::cout<<split(time, ":")[0]<<"\n";
	std::cout<<split(time, ":")[1]<<"\n";
	sec = l->periodLength - (60*std::stoi(split(time, ":")[0]) + std::stoi(split(time, ":")[1]));
	
	l->addScoringEvent(gID, tID, scorernum, a1, a2, per, sec);
	std::cout<<"addScoringEvent("<<gID<<", "<<tID<<", "<<scorernum<<", "<<a1<<", "<<a2<<", "<<per<<", "<<sec<<")\n";
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
