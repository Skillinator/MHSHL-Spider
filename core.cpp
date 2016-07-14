#include <curl/curl.h>
#include <iostream>
#include <string>
#include "mhshl.h"
#include "mhshlUtils.h"

const int SEASON_2014_2015 = 13209;
const int SEASON_2015_2016 = 14757;

void updateGame(Game*, League*);

void getGames(Team team, League *league){

	std::cout<<"Fetching Games for team " << team.abbreviation << " during the " << league->season << " season.\n";

	std::string url = "midwest-league.stats.pointstreak.com/players-team-schedule.html?teamid=" + std::to_string(team.pointstreakID) + "&seasonid=" + std::to_string(league->season);
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
		std::cout<<"Processing game " << i << " of " << games.size()-1 << "\n";
		std::string homeID, awayID = " ";
		int	month, day, year, time, gameID;


		std::vector<std::string> elements = split(games[i], "<td");
		elements.erase(elements.begin());
		elements.erase(elements.end());

		for(int j=0; j < elements.size(); j++){
			elements[j] = extract("<td" + elements[j], "td");
		}

		homeID = league->getTeam(stoi(getValue(elements[1], "teamid")))->abbreviation;
		awayID = league->getTeam(stoi(getValue(elements[3], "teamid")))->abbreviation;

		month = getMonth(elements[4]);
		day = getDay(elements[4]);
		year = getYear(2015, month);
		time = getMinutes(elements[5]);
		gameID = stoi(getValue(elements[6], "gameid"));

		league->addGame(Game(month, day, year, time, homeID, awayID, league->periodLength, gameID));

	}

}

void getGames(League *league){
	for(int i = 0; i < league->teams.size(); i++){
		getGames(league->teams[i], league);
	}
}

void sort_games(League* league){
	std::cout<<"Sorting Games\n";
	// Sort games by date. I probably could move this somewhere else because currently it will repeat for each team. Lot of redundancy.

	bool sorted = false;
	while(!sorted){
		sorted = true;
		for(int i = 0; i < league->games.size()-1; i++){
			std::string date1 = std::to_string(league->games[i].year) + twoPlace(league->games[i].month) + twoPlace(league->games[i].day);
			std::string date2 = std::to_string(league->games[i+1].year) + twoPlace(league->games[i+1].month) + twoPlace(league->games[i+1].day);
			if(stoi(date1) > stoi(date2)){
				sorted = false;
				Game tmp = league->games[i];
				league->games[i] = league->games[i+1];
				league->games[i+1] = tmp;
			}
		}
	}

}

void getPlayers(Team team, League *league){
	std::cout<<"Fetching Players for team " << team.abbreviation << " during the " << league->season << " season.\n";

	std::string url = "midwest-league.stats.pointstreak.com/players-team-roster.html?teamid=" + std::to_string(team.pointstreakID) + "&seasonid=" + std::to_string(league->season);

	std::string page = fetchWebPage(url);
	page = split(page, "Player Stats")[1];
	page = split(page, "</table>")[1];

	/*
	Split by </tr>, remove first and last elements
	*/
	std::vector<std::string> players = split(page, "</tr>");
	players.erase(players.begin());
	players.erase(players.end());
	players.erase(players.end());
	for(int i = 0; i < players.size(); i++){
		/*
		Remove the opening <tr> element from everything too
		*/
		std::string currentPlayer = extract(players[i] + "</tr>", "tr");
		if(currentPlayer.find("DP") == currentPlayer.find("HC") && currentPlayer.find("HC") == currentPlayer.find("AC")){
		/*
			Get the player's number in one string, and his name and ID in another.
			*/
			std::string num = extract(split(currentPlayer, "</td>")[0] + "</td", "td");
			std::string nameID = extract(split(currentPlayer, "</td>")[1] + "</td", "td");

			int playerNum = 0;
			int playerID = 0;
			std::string name = "NULL";
			playerNum = std::stoi(num);
			playerID = std::stoi(getValue(nameID, "playerid"));
			name = split(extract(nameID, "a"), "\t\t\t\t")[1];

			league->addPlayer(Player(team.abbreviation, name, playerID, playerNum));
		}

	}

	page = fetchWebPage(url);
	page = split(page, "Goalie Stats")[1];
	page = split(page, "</table>")[1];

	/*
	Split by </tr>, remove first and last elements
	*/
	std::vector<std::string> goalies = split(page, "</tr>");
	goalies.erase(goalies.begin());
	goalies.erase(goalies.end());
	for(int i = 0; i < goalies.size(); i++){
		/*
		Remove the opening <tr> element from everything too
		*/
		std::string currentPlayer = extract(goalies[i] + "</tr>", "tr");
		if(currentPlayer.find("DP") == currentPlayer.find("HC") && currentPlayer.find("HC") == currentPlayer.find("AC") && currentPlayer.find("DG") == currentPlayer.find("AC")){
		/*
			Get the player's number in one string, and his name and ID in another.
			*/
			std::string num = extract(split(currentPlayer, "</td>")[0] + "</td", "td");
			std::string nameID = extract(split(currentPlayer, "</td>")[1] + "</td", "td");

			int playerNum = 0;
			int playerID = 0;
			std::string name = "NULL";
			playerNum = std::stoi(num);
			playerID = std::stoi(getValue(nameID, "playerid"));
			name = split(extract(nameID, "a"), "\t\t\t")[1];

			if(league->addPlayer(Player(team.abbreviation, name, playerID, playerNum)))
				league->players[league->players.size()-1].goalie = true;
		}

	}


	std::cout<<"Updating players...  ";
	for(int i = 0; i < league->players.size(); i++){
		// std::cout<<players[i]<< "  ";
		db_updatePlayer(league->players[i], *league);
	}
	std::cout<<" ...Done!\n";
}

void getRosters(League *league){
	for(int i = 0; i < league->teams.size(); i++){
		getPlayers(league->teams[i], league);
	}
}

// Procedurally updates every game in the database. Only use when a full rebuild is needed.
void hard_update(League* league){
	for(int i = 0; i < league->games.size(); i++){
		std::cout<<"Updating game " << i+1 << " of " << league->games.size() << "\n";
		std::cout<<league->games[i].id<<"\n";
		updateGame(&league->games[i], league);
	}

	sort_games(league);
}

void addHardCodedTeams(League *league){
	league->addTeam(Team("QCB", "Blues", "Quad City", 47180));
	league->addTeam(Team("DBQ", "Devils", "Dubuque", 47175));
	league->addTeam(Team("CDR", "Jr. Roughriders", "Cedar Rapids", 47172));
	league->addTeam(Team("WAT", "Warriors", "Waterloo", 47182));
	league->addTeam(Team("DMC", "Capitals", "Des Moines", 47173));
	league->addTeam(Team("DMO", "Oak Leafs", "Des Moines", 47174));
	league->addTeam(Team("AMS", "Little Cyclones", "Ames", 47171));
	league->addTeam(Team("MCM", "Mohawks", "Mason City", 47178));
	league->addTeam(Team("SCM", "Metros", "Sioux City", 47181));
	league->addTeam(Team("KCJ", "Jets", "Kansas City", 96071));
	league->addTeam(Team("OJL", "Jr. Lancers", "Omaha", 149736));
	league->addTeam(Team("LJS", "Jr. Stars", "Lincoln", 47177));
}

void initializeLeague(League *league){

	addHardCodedTeams(league);
	getRosters(league);
	getGames(league);
	sort_games(league);
	hard_update(league);
}

void update(League *league){
	/*

	Sort Scoring Events and Penalty Events by age

	While ScoringEvents + PenaltyEvents > 0:
		Add Earliest Event

	updateGameTime(t)

	*/
}

void processGoalie(Game* game, League* league, std::string rawGoalieString, bool done){
	/*
	Initialize values to their default states
	*/
	std::string gID = game->id;
	std::string tID="null";
	int player = 0;
	int shots = 0;
	int goals = 0;
	int seconds = 0;

	player = std::stoi(split(split(rawGoalieString, "playerid=")[1], "&")[0]);

	std::vector<std::string> items = split(rawGoalieString, "</td>");
	items.erase(items.begin());

	for(int i = 0; i < items.size(); i++){
		items[i] = extract(items[i]+"</td>", "td");
	}
	if(stripWhitespace(items[0]) == "")
		return;
	seconds = std::stoi(stripWhitespace(items[0]));
	shots = std::stoi(stripWhitespace(items[1]));
	goals = shots - std::stoi(stripWhitespace(items[2]));

	tID = league->getPlayer(player)->teamID;

	league->setGoaliePerformance(gID, tID, player, seconds, goals, shots);

	if(done){
		Player* p = league->getPlayer(player);
		p->gamesPlayed++;
		p->shots += shots;
		p->goalsAgainst += goals;
		p->minutesPlayed += seconds;
		p->saves += shots-goals;
		if(p->gamesPlayed == 0){
			p->goalsAgainstAverage = 0;
		}else{
			p->goalsAgainstAverage = p->goalsAgainst*1.0/p->gamesPlayed;
		}
		if(p->shots == 0){
			p->savePercentage = 0;
		}else{
			p->savePercentage = p->saves*1.0/p->shots;
		}
		std::cout<<"Updating Goalie \n";
		db_updatePlayer(*p, *league);
		std::cout<<"Successfully finished updating goalie\n";
	}

}

void updateGame(Game* game, League* league){
	std::string url = "midwest-league.stats.pointstreak.com/players-boxscore.html?gameid=" + std::to_string(game->number);
	std::string page = fetchWebPage(url);


	std::vector<PenaltyEvent> penaltyEvents;
	std::vector<ScoringEvent> scoringEvents;

	parseShotsOnGoal(page, game);
	parsePenaltyEvents(page, &penaltyEvents, game, league);
	parseScoringEvents(page, &scoringEvents, game, league);

	/*
	* Go through these in chronological order.
	*/
	while(scoringEvents.size() > 0 && penaltyEvents.size() > 0){
		// Get most recent score, most recent penalty
		ScoringEvent mrs = scoringEvents[0];
		PenaltyEvent mrp = penaltyEvents[0];

		if(mrs.period < mrp.period){
			league->addScoringEvent(mrs);
			scoringEvents.erase(scoringEvents.begin());
		}else if(mrs.period > mrp.period){
			league->addPenaltyEvent(mrp);
			penaltyEvents.erase(penaltyEvents.begin());
		}else if(mrs.time > mrp.time){
			league->addScoringEvent(mrs);
			scoringEvents.erase(scoringEvents.begin());
		}else{
			league->addPenaltyEvent(mrp);
			penaltyEvents.erase(penaltyEvents.begin());
		}
	}

	if(scoringEvents.size() > 0){
		while(scoringEvents.size() > 0){
			ScoringEvent mrs = scoringEvents[0];
			league->addScoringEvent(mrs);
			scoringEvents.erase(scoringEvents.begin());
		}
	}

	if(penaltyEvents.size() > 0){
		while(penaltyEvents.size() > 0){
			PenaltyEvent mrp = penaltyEvents[0];
			league->addPenaltyEvent(mrp);
			penaltyEvents.erase(penaltyEvents.begin());
		}
	}


	/*
	*
	*
	* PROCESS TIME INFORMATION
	*
	*
	*/

	// we still need to properly process time, I'll have to see on some NAHL games or something.

	if(page.find("FINAL") != std::string::npos){
		switch(game->period){
			case 4:
				game->time = -2;
				break;
			case 5:
				game->time = -3;
				break;
			default:
				game->time = -1;
				game->period = 3;
				break;


		}
		db_updateGame(*game, *league);
	}


	/*
	*
	*
	* PROCESS GOALIE INFORMATION
	*
	*
	*/

	/*
	* Skip straight to the goalies section. What remains of the first table goes into the g1 string,
	* and the second table goes into the g2 string.
	*/
	std::string goalies = split(page, "Goalies")[1];
	std::string g1 = split(goalies, "</table>")[0];
	std::string g2 = split(goalies, "</table>")[1];

	/*
	* Split these by <tr to separate the different goalies.
	*/
	std::vector<std::string> goalies1 = split(g1, "<tr");
	std::vector<std::string> goalies2 = split(g2, "<tr");

	/*
	* Erase the first element as that is just labeling
	*/
	goalies1.erase(goalies1.begin());
	goalies1.erase(goalies1.begin());
	goalies1.erase(goalies1.begin());
	goalies2.erase(goalies2.begin());
	goalies2.erase(goalies2.begin());

	/*
	* Strip off the remaining <tr> tag info, then process
	*/
	for(int i = 0; i < goalies1.size(); i++){
		processGoalie(game, league, extract("<tr"+goalies1[i], "tr"), game->time < 0);
	}
	for(int i = 0; i < goalies2.size(); i++){
		processGoalie(game, league, extract("<tr"+goalies2[i], "tr"), game->time < 0);
	}


	/*
	*
	*
	* PROCESS ROSTER INFORMATION
	*
	*
	*/

	if(!game->rosterTaken){
		game->rosterTaken = true;

		std::string rosterArea = split(split(page, "Players")[1], "Other facts")[0];

		std::vector<std::string> players = split(rosterArea, "playerid=");
		players.erase(players.begin());
		for(int i = 0; i < players.size(); i++){
			int p = std::stoi(split(players[i], "&")[0]);
			league->getPlayer(p)->gamesPlayed++;
			std::cout<<"Updating Player " << league->getPlayer(p)->name << "\n";
			if(league->getPlayer(p)->name != "NULL")
				db_updatePlayer(*league->getPlayer(p), *league);
		}
	}
}

void getLeagueHistory(bool varsity, int season){
	League league = League(14 + 3*varsity, varsity, season);
	initializeLeague(&league);
}

int main(){
	getLeagueHistory(true, SEASON_2014_2015);
	return 0;
}
