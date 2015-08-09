#ifndef _mhshlSpider
#define _mhshlSpider

#include <vector>
#include <string>

void spaceBuffer(int len, std::string message);
void divide();

class ScoringEvent{
public:
	ScoringEvent(std::string gID, std::string tID, int gs, int a1, int a2, int per, int sec);
	void printString();
	void printHeading();
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
	void printString();
	void printHeading();
	std::string gameID;
	std::string teamID;
	int player; 
	int duration;
	int period;
	int time;
	bool scoredOn;
	std::string offense;
};

class Team{
public:
	Team(std::string abbr, std::string n, std::string c, int identifier);
	std::string abbreviation;
	std::string name;
	std::string city;
	int id;
	int gp;
	int w;
	int l;
	int ot;
	int so;
	int pts;
	int gf;
	int ga;
	int streak;
};

class Player{
public:
	Player(std::string tID, std::string name, int num);
	std::string team; // Abbreviation
	std::string name;
	int number;
	int gp;
	int g;
	int a;
	int pp;
	int sh;
	int pim;
	int goalStreak;
	int pointStreak;
};

/*
Game is a basic class to hold information about, well, a game.
*/

class Game{
public:
	Game();
	std::string toString();

	// Date related stuff
	int month;
	int day;
	int year;
	int startTime; // In minutes

	int homeTeam;
	int awayTeam;

	int homeScore;
	int awayScore;

	int homeShots;
	int awayShots;

	int period; // 1-3 self explanatory. 4 is OT. 5 is SO.
	int time; // Time is in seconds remaining in the period. -1 signifies a final.

	std::vector<PenaltyEvent> penaltyEvents;
	std::vector<ScoringEvent> scoringEvents; 
};


/*
League is the base container class for all the teams, games, and players.
*/
class League{
public:
	League();
	bool varsity;
	std::vector<Game> games;
	std::vector<Team> teams;
	std::vector<Player> players;
	std::vector<ScoringEvent> goals;
	std::vector<PenaltyEvent> penalties;
};


#endif
