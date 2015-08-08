#ifndef _mhshlSpider
#define _mhshlSpider

#include <vector>
#include <string>

class ScoringEvent{
public:
	ScoringEvent(std::string gID, std::string tID, int gs, int a1, int a2, int per, int sec);
	std::string gameID; // Format for this is MMDDYY-HomeAbbr. For example, 120515-QCB
	std::string teamID; // The same shorthand team identifier.
	int scorer;
	int assist1;
	int assist2;
	int period; // 1-3 normal, 4 OT, 5 SO
	int time; // Seconds remaining in the period.
};

class PenaltyEvent{
public:
	PenaltyEvent(std::string gID, std::string tID, int pl, int dur, int per, int sec, std::string charge);
	std::string gameID;
	std::string teamID;
	int player;
	int duration;
	int period;
	int time;
	std::string offense;
};

/*
League is the base container class for all the teams, games, and players.
*/
class League{
public:
	League();
	bool varsity;
};

/*
Game is a basic class to hold information about, well, a game.
*/

class Game{
public:
	Game();
	
	// Date related stuff
	int month;
	int day;
	int year;
	int time; // In minutes

	int homeTeam;
	int awayTeam;

	int homeScore;
	int awayScore;

	int homeShots;
	int awayShots;

	int period; // 1-3 self explanatory. 4 is OT. 5 is SO.
	int time; // Time is in seconds remaining in the period. -1 signifies a final.

	vector<PenaltyEvent> penaltyEvents;
	vector<ScoringEvent> scoringEvents; 
};

#endif
