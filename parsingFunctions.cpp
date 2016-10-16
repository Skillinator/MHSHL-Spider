#include <vector>
#include <curl/curl.h>
#include <string>
#include <iostream>
#include "mhshl.h"
#include "mhshlUtils.h"

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
		db_updatePlayer(p, league);
		std::cout<<"Successfully finished updating goalie\n";
	}

}

ScoringEvent processGoal(Game* game, League* league, int period, std::string rawGoalString){
	/*
	Initialize values to their default states
	*/
	std::string gID="null";
	std::string tID="null";
	int scorernum=0;
	int a1 = 0;
	int a2 = 0;
	int sec=0;
	int pp = 0;

	/*
	If no scoring occured, stop processing right here
	*/
	if(rawGoalString.find("no scoring") != std::string::npos)
		return ScoringEvent();

	/*
	If there's a <br> on the end, remove that
	*/
	if(rawGoalString.find("<br"))
		rawGoalString = split(rawGoalString, "<br")[0];
	if(rawGoalString.find("(power play)")!=std::string::npos)
		pp = 1;
	if(rawGoalString.find("(short handed)")!= std::string::npos)
		pp = -1;

	/*
	Handle power play, shorthanded, and empty net goals
	*/
	rawGoalString = removeSubstring(rawGoalString, "<i>(empty net)</i>");
	rawGoalString = removeSubstring(rawGoalString, "<i>(power play)</i>");
	rawGoalString = removeSubstring(rawGoalString, "<i>(short handed)</i>");
	rawGoalString = removeSubstring(rawGoalString, "<i>(penalty shot)</i>");

	/*
	Split by the first parenthesis
	*/
	std::vector<std::string> firstsplit = split(rawGoalString, "(");

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
	gID = game->id;
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
	sec = league->periodLength - (60*std::stoi(split(time, ":")[0]) + std::stoi(split(time, ":")[1]));

	ScoringEvent se = ScoringEvent(gID, tID, scorernum, a1, a2, period, sec);
	se.powerPlay = pp;
	return se;
}

PenaltyEvent processPenalty(Game* game, League* league, int period, std::string rawPenaltyString){
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
	if(rawPenaltyString.find("no penalties") != std::string::npos)
		return PenaltyEvent();

	if(rawPenaltyString.size() < 10)
		return PenaltyEvent();

	/*
	If there's a <br> on the end, remove that
	*/
	if(rawPenaltyString.find("<br")!=std::string::npos)
		rawPenaltyString = split(rawPenaltyString, "<br")[0];
	/*
	We don't really care about a misconduct. Just remove it.
	*/
	rawPenaltyString = removeSubstring(rawPenaltyString, "(Misconduct");
	rawPenaltyString = removeSubstring(rawPenaltyString, "(Game Misconduct");
	rawPenaltyString = removeSubstring(rawPenaltyString, "(Major");
	rawPenaltyString = removeSubstring(rawPenaltyString, "(Bench Minor");
	rawPenaltyString = removeSubstring(rawPenaltyString, "(Double Minor");

	/*
	Split by the first parenthesis
	*/
	std::vector<std::string> firstsplit = split(rawPenaltyString, "(");
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
	gID = game->id;
	tID = translateTeamID(team);
	duration = std::stoi(split(time, " min")[0]);
	sec = league->periodLength - (60*std::stoi(split(split(time, ":")[0], ", ")[1]) + std::stoi(split(time, ":")[1]));
	return PenaltyEvent(gID, tID, player, duration, period, sec, p);
}

void parsePenaltyEvents(std::string page, std::vector<PenaltyEvent> *penaltyEvents, Game *game, League *league){
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
  Cycle through penalties by period
  */
  for(int i = 0; i < penVec.size()-1; i+=2){

    /*
    Only if there was a penalty in that period
    */

    if(penVec[i+1].find("(no penalties)") == std::string::npos ){

      /*
      Get the actual number for this period
      */
      int per = getPeriod(penVec[i]);

      /*
      If there is more than one penalty in that period, split them up and process them seperately.
      If there is only one penalty process it directly.
      */
      if(penVec[i+1].find("<tr>") != std::string::npos){
        std::vector<std::string> firstsplit = split(penVec[i+1], "<tr>");
        for(int n = 0; n < firstsplit.size(); n++){
          /*
          If it wasn't the first penalty of the period, there will be another <td> tag on it. We can't have that, so strip it off.
          */
          if(firstsplit[n].find("<td") != std::string::npos)
            firstsplit[n] = extract(firstsplit[n], "td");
          penaltyEvents->push_back(processPenalty(game, league, per, firstsplit[n]));
        }
      }else{
        processPenalty(game, league, per, penVec[i+1]);
        penaltyEvents->push_back(processPenalty(game, league, per, penVec[i+1]));
      }
    }
  }
}

void parseShotsOnGoal(std::string page, Game *game){
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
  game->homeShots = std::stoi(homeShots[homeShots.size() - 1]);
  game->awayShots = std::stoi(awayShots[awayShots.size() - 1]);
}

void parseScoringEvents(std::string page, std::vector<ScoringEvent> *scoringEvents, Game *game, League *league){

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
					scoringEvents->push_back(processGoal(game, league, per, firstsplit[n]));
				}
			}else{
				scoringEvents->push_back(processGoal(game, league, per, goalVec[i+1]));
			}
		}
	}
}
