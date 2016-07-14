#include <vector>
#include <curl/curl.h>
#include <string>
#include <iostream>
#include "mhshl.h"
#include "mhshlUtils.h"

void parsePenaltyEvents(std::string page, std::vector<PenaltyEvents> *penaltyEvents, Game *game, League *league){
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
          penaltyEvents.push_back(processPenalty(game, league, per, firstsplit[n]));
        }
      }else{
        processPenalty(game, league, per, penVec[i+1]);
        penaltyEvents.push_back(processPenalty(game, league, per, penVec[i+1]));
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

void parseScoringEvents(std::string page, std::vector<ScoringEvents> *scoringEvents, Game *game, League *league){

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
					scoringEvents.push_back(processGoal(game, league, per, firstsplit[n]));
				}
			}else{
				scoringEvents.push_back(processGoal(game, league, per, goalVec[i+1]));
			}
		}
	}
}
