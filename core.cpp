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

ScoringEvent processGoal(Game* g, League* l, int p, std::string s){
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
	int pp = 0;
	
	/*
	If no scoring occured, stop processing right here 
	*/
	if(s.find("no scoring") != std::string::npos)
		return ScoringEvent();
	
	/*
	If there's a <br> on the end, remove that
	*/
	if(s.find("<br"))
		s = split(s, "<br")[0];
	if(s.find("(power play)")!=std::string::npos)
		pp = 1;
	if(s.find("(short handed)")!= std::string::npos)
		pp = -1;			

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
	std::string time = split(firstsplit[1], ") , ")[1];
	
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
	sec = l->periodLength - (60*std::stoi(split(time, ":")[0]) + std::stoi(split(time, ":")[1]));	
	
	ScoringEvent se = ScoringEvent(gID, tID, scorernum, a1, a2, per, sec);
	se.pp = pp;
	return se;
}

GoaliePerformance processGoalie(Game* G, League* l, std::string g){
	/*
	Initialize values to their default states
	*/
	std::string gID="null";
	std::string tID="null";
	int player = 0;
	int shots = 0;
	int goals = 0;
	int seconds = 0;
	gID = G->id;
	
	player = std::stoi(split(split(g, "playerid=")[1], "&")[0]);
	
	std::vector<std::string> items = split(g, "</td>");
	items.erase(items.begin());
	
	for(int i = 0; i < items.size(); i++){
		items[i] = extract(items[i]+"</td>", "td");
	}
	
	if(stripWhitespace(items[0]) == "")
		return GoaliePerformance();
	std::cout<<g<<"\n";
	seconds = std::stoi(stripWhitespace(items[0]));
	shots = std::stoi(stripWhitespace(items[1]));
	goals = shots - std::stoi(stripWhitespace(items[2]));
	
	tID = l->getPlayer(player)->team;
	
	l->setGoaliePerformance(gID, tID, player, seconds, goals, shots);
}

PenaltyEvent processPenalty(Game* g, League* l, int per, std::string s){
	/*
	Initialize values to their default states
	*/
	std::string gID="null";
	std::string tID="null";
	int player = 0;
	int duration = 0;
	std::string p = "null";
	int sec = 0;
	
	/*
	If no penalties occured, stop processing right here 
	*/
	if(s.find("no penalties") != std::string::npos)
		return PenaltyEvent();
	
	if(s.size() < 10)
		return PenaltyEvent();
	
	/*
	If there's a <br> on the end, remove that
	*/
	if(s.find("<br")!=std::string::npos)
		s = split(s, "<br")[0];
	/*
	We don't really care about a misconduct. Just remove it.
	*/
	s = removeSubstring(s, "(Misconduct");
	s = removeSubstring(s, "(Game Misconduct");
	s = removeSubstring(s, "(Major");
	s = removeSubstring(s, "(Bench Minor");
	s = removeSubstring(s, "(Double Minor");
	
	/*
	Split by the first parenthesis
	*/
	std::vector<std::string> firstsplit = split(s, "(");
	/*
	This willl give us the team name and the player ID
	The team name still needs to be run through the translate function since it's not how they're stored in our database
	*/
	std::vector<std::string> teamplayer = split(firstsplit[0]," - ");
	
	/*
	Separate the charge from the time and duration
	*/
	p = split(firstsplit[1], ")")[0];
	std::string time = split(firstsplit[1], "), ")[1];
	/*
	Break the team and player values out of their bolding tags
	*/
	std::string team = extract(teamplayer[0], "b");
	player = std::stoi(getValue(extract(teamplayer[1], "b"), "playerid"));
	
	/*
	Set the first three values. 
	gameID is found directly from the game. 
	PlayerID is found from the playerid value passed in the href to the player's page.
	teamID is found by translating the team name into our style teamID.
	*/
	gID = g->id;
	tID = translateTeamID(team);
	duration = std::stoi(split(time, " min")[0]);
	sec = l->periodLength - (60*std::stoi(split(split(time, ":")[0], ", ")[1]) + std::stoi(split(time, ":")[1]));
	return PenaltyEvent(gID, tID, player, duration, per, sec, p);
}


void updateGame(Game* g, League* l){
	std::string url = "midwest-league.stats.pointstreak.com/players-boxscore.html?gameid=" + std::to_string(g->number);
	std::string page = fetchWebPage(url);
	
	std::vector<PenaltyEvent> penaltyEvents;
	std::vector<ScoringEvent> scoringEvents;
	
	/*
	*
	*
	* PROCESS PENALTY INFORMATION
	*
	*
	*/
	
	
	/*
	For Penalties, delete everything above the "Penalties" header. Once again, delete all that follows </table>.
	*/
	std::string penalties = split(page, "Penalties")[1];
	penalties = split(penalties, "</table>")[0];

	/*
	Split by </td> to break up the periods. The first and last elements are garbage, just toss them.
	*/
	std::vector<std::string> penVec = split(penalties, "</td>");
	penVec.erase(penVec.begin());
	penVec.erase(penVec.end());

	/*
	Go through and remove the opening <td> tag now from every element, it's unnecessary.
	*/
	for(int i = 0; i < penVec.size(); i++){
		penVec[i] = extract(penVec[i] + "</td>", "td");
	}
	
	
	/*
	Cycle through goals by period
	*/
	for(int i = 0; i < penVec.size()-1; i+=2){
		
		/*
		Only if there was a goal in that period
		*/
		
		if(penVec[i+1].find("(no penalties)") == std::string::npos ){
			
			/*
			Get the actual number for this period
			*/
			int per = getPeriod(penVec[i]);
			
			/*
			If there are more than one goals in that period, split them up and process them seperately.
			If there is only one goal, process it directly.
			*/
			if(penVec[i+1].find("<tr>") != std::string::npos){
				std::vector<std::string> firstsplit = split(penVec[i+1], "<tr>");
				for(int n = 0; n < firstsplit.size(); n++){
					/*
					If it wasn't the first goal of the period, there will be another <td> tag on it. We can't have that, so strip it off.
					*/
					if(firstsplit[n].find("<td") != std::string::npos)
						firstsplit[n] = extract(firstsplit[n], "td");
					penaltyEvents.push_back(processPenalty(g, l, per, firstsplit[n]));
				}
			}else{
				processPenalty(g, l, per, penVec[i+1]);
				penaltyEvents.push_back(processPenalty(g, l, per, penVec[i+1]));
			}
		}
	}	
	
	
	/*
	*
	*
	* PROCESS SCORING INFORMATION
	*
	*
	*/
	
	/*
	Take the page after "Shots on Goal", before "</div>", then after "</a>";
	*/
	std::string shots = split(split(split(page, "Shots on Goal")[1], "</div>")[0], "</a>")[1];
	
	/*
	Home team's is first, away second. Also split by " "
	*/
	std::vector<std::string> homeShots = split(split(shots, "<br>")[0], " ");
	std::vector<std::string> awayShots = split(split(split(shots, "<br>")[1],  "  ")[0], " ");
	
	/*
	Just take the last element and stoi it, and set as the appropriate team's shot total
	*/
	g->homeShots = std::stoi(homeShots[homeShots.size() - 1]);
	g->awayShots = std::stoi(awayShots[awayShots.size() - 1]);
	
	/*
	For goals, we dont' want anything above the scoring summary, or below the next </table>
	*/
	std::string goals = split(page, "Scoring Summary")[1];
	goals = split(goals, "</table>")[0];

	/*
	Split by </td> to break up the periods. The first and last elements are garbage, just toss them.
	*/
	std::vector<std::string> goalVec = split(goals, "</td>");
	goalVec.erase(goalVec.begin());
	goalVec.erase(goalVec.end());

	/*
	Go through and remove the opening <td> tag now from every element, it's unnecessary.
	*/
	for(int i = 0; i < goalVec.size(); i++){
		goalVec[i] = extract(goalVec[i] + "</td>", "td");
	}
	
	
	/*
	Cycle through goals by period
	*/
	for(int i = 0; i < goalVec.size(); i+=2){
		
		/*
		Only if there was a goal in that period
		*/
		
		if(goalVec[i+1].find("(no scoring)") == std::string::npos ){
			
			/*
			Get the actual number for this period
			*/
			int per = getPeriod(goalVec[i]);
			
			/*
			If there are more than one goals in that period, split them up and process them seperately.
			If there is only one goal, process it directly.
			*/
			if(goalVec[i+1].find("<tr>") != std::string::npos){
				std::vector<std::string> firstsplit = split(goalVec[i+1], "<tr>");
				for(int n = 0; n < firstsplit.size(); n++){
					/*
					If it wasn't the first goal of the period, there will be another <td> tag on it. We can't have that, so strip it off.
					*/
					if(firstsplit[n].find("<td") != std::string::npos)
						firstsplit[n] = extract(firstsplit[n], "td");
					scoringEvents.push_back(processGoal(g, l, per, firstsplit[n]));
				}
			}else{
				scoringEvents.push_back(processGoal(g, l, per, goalVec[i+1]));
			}
		}
	}
	
	/*
	* Go through these in chronological order.
	*/
	while(scoringEvents.size() > 0 && penaltyEvents.size() > 0){
		// Get most recent score, most recent penalty
		ScoringEvent mrs = scoringEvents[0];
		PenaltyEvent mrp = penaltyEvents[0];
		
		if(mrs.period < mrp.period){
			l->addScoringEvent(mrs.gameID, mrs.teamID, mrs.scorer, mrs.assist1, mrs.assist2, mrs.period, mrs.time, mrs.pp);
			scoringEvents.erase(scoringEvents.begin());
		}else if(mrs.period > mrp.period){
			l->addPenaltyEvent(mrp.gameID, mrp.teamID, mrp.player, mrp.duration, mrp.period, mrp.time, mrp.offense);
			penaltyEvents.erase(penaltyEvents.begin());
		}else if(mrs.time > mrp.time){
			l->addScoringEvent(mrs.gameID, mrs.teamID, mrs.scorer, mrs.assist1, mrs.assist2, mrs.period, mrs.time, mrs.pp);
			scoringEvents.erase(scoringEvents.begin());
		}else{
			l->addPenaltyEvent(mrp.gameID, mrp.teamID, mrp.player, mrp.duration, mrp.period, mrp.time, mrp.offense);			
			penaltyEvents.erase(penaltyEvents.begin());
		}
	}
	
	if(scoringEvents.size() > 0){		
		while(scoringEvents.size() > 0){
			ScoringEvent mrs = scoringEvents[0];
			l->addScoringEvent(mrs.gameID, mrs.teamID, mrs.scorer, mrs.assist1, mrs.assist2, mrs.period, mrs.time, mrs.pp);
			scoringEvents.erase(scoringEvents.begin());
		}
	}
	
	if(penaltyEvents.size() > 0){
		while(penaltyEvents.size() > 0){
			PenaltyEvent mrp = penaltyEvents[0];
			l->addPenaltyEvent(mrp.gameID, mrp.teamID, mrp.player, mrp.duration, mrp.period, mrp.time, mrp.offense);
			penaltyEvents.erase(penaltyEvents.begin());
		}
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
	goalies2.erase(goalies2.begin());
	
	/*
	* Strip off the remaining <tr> tag info, then process
	*/
	for(int i = 0; i < goalies1.size(); i++){
		processGoalie(g, l, extract("<tr"+goalies1[i], "tr"));
	}
	for(int i = 0; i < goalies2.size(); i++){
		processGoalie(g, l, extract("<tr"+goalies2[i], "tr"));
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
		switch(g->period){
			case 4:
				g->time = -2;
				break;
			case 5:
				g->time = -3;
				break;
			default:
				g->time = -1;
				g->period = 3;
				break;
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

void getPlayers(int season, int team, League *l){
	std::cout<<"Fetching Players for team " << team << " during the " << season << " season.\n";

	std::string url = "midwest-league.stats.pointstreak.com/players-team-roster.html?teamid=" + std::to_string(team) + "&seasonid=" + std::to_string(season);

	
	Team* t = l->getTeam(team);
	std::string teamID = t->abbreviation;
	
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
			
			l->addPlayer(teamID, name, playerID, playerNum);
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
			
			if(l->addPlayer(teamID, name, playerID, playerNum))
				l->players[l->players.size()].goalie = true;
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
	
	getPlayers(SEASON_2014_2015, 47180, &l);
	
	for(int i = 0; i < l.teams.size(); i++){
		//getGames(SEASON_2014_2015, l.teams[i].id, &l);
	}
	getGames(SEASON_2014_2015, l.teams[0].id, &l);
	sort_games(&l);
	hard_update(&l);
//	showGames(l);
	
//	showPlayers(l);
	showGoaliePerformances(l);
	return 0;
}
