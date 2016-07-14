#ifndef _mhshlUtils
#define _mhshlUtils

#include <curl/curl.h>
#include <vector>
#include <string>
#include <iostream>
#include "mhshl.h"

/*
 *
 * CORE DATABASE FUNCTIONS
 *
 * These functions handle all interaction between this program and the MySQL database. They are called by their associated
 * methods within the League class, and have the same arguments save the addition of two: a boolean for varsity status
 * (True signifies varsity, false signifies Junior Varsity) and the integer for season used in all relevant Pointstreak URLs.
 * Their use is otherwise the same. These methods should not be called anywhere outside of their respective League functions
 * (db_addTeam should not be called from anywhere other than League.addTeam).
 *
 * The behavior of the following functions should be self-explanatory. They have no return, and simply use the same parameters
 * used in their league functions.
 *
 * Implementations can all be found in databaseActions.cpp
 *
 */

void db_addTeam(Team team, League league);
void db_addPlayer(Player player, League league);
void db_updatePlayer(Player player, League league);
void db_addGame(Game game, League league);
void db_updateGame(Game game, League league);
void db_addPenalty(PenaltyEvent penalty, League league);
void db_updatePenalty(PenaltyEvent penalty, League league);
void db_addGoal(ScoringEvent goal, League league);

/*
 *
 * Parsing functions
 *
 * A collection of functions removed from core.cpp for better readability.
 *
 * Implementations found in parsingFunctions.cpp
 *
 */

void parsePenaltyEvents(std::string page, std::vector<PenaltyEvent> *penaltyEvents, Game *game);
void parseShotsOnGoal(std::string page, Game *game);
void parseScoringEvents(std::string page, std::vector<ScoringEvent> *scoringEvents, Game *game);

/*
 *
 * STRING PROCESSING FUNCTIONS
 *
 * The following functions are critical for formatting string input (making it easier to parse our data) as well as output
 * (printing nicer tables to the console). They are defined in mhshlUtils.cpp.
 *
 */

// A basic split function. The input string str will be split at each instance of the delimiter. The segments are returned in a vector.
std::vector<std::string> split(std::string str, std::string delim);

/*
 * An "extractor" for HTML parsing. Looks for the first instance of the given tag, then returns the full string between the next
 * instances each of ">" and "<", which is everything inside that tag.
 *
 * Example
 * string str = "<html> <body parameters = "even other notation within the tag won't hinder us">This is the part we want returned</body></html>"
 *
 * extract(str, "body") would return the value "This is the part we want returned"
 */
std::string extract(std::string str, std::string tag);

/*
 * A corrolary to the extract function. Instead of extracting the markup within the tag, it will extract the values of a given
 * parameter, for example in <script type="text/javascript" src="script.js" />
 *
 * If a getValue would be run on that string, with value "src", the function would return "script.js"
 */
std::string getValue(std::string str, std::string value);

// This is a simple function to center a string within spaces for cleaner output
void spaceBuffer(int len, std::string str);

// The spiritual opposite to spaceBuffer, this function removes spaces and tabs.
std::string stripWhitespace(std::string s);

// Puts a 0 in front of a single digit integer for cleaner, more consistent output.
std::string twoPlace(int num);

// Self-explanatory. Fetches the web page via CURL and returns its source as a string.
std::string fetchWebPage(std::string url);

// If a given substring exists within a larger string, it will be eliminated altogether.
std::string removeSubstring(std::string str, std::string substr);



/*
 *
 * DATA OUTPUT FUNCTIONS
 *
 * A bunch of simple display methods, set up to display the given collection of data in a pretty table.
 *
 */

void showTeams(League l);
void showPlayers(League l);
void showGames(League l);
void showPenalties(League l);
void showGoals(League l);
void showGoaliePerformances(League l);

// This basically triggers all of the above; a complete dump of league stats.
void dumpStatus(League l);



/*
 * DATE/TIME FUNCTIONS
 *
 * Some functions for dealing with time and date.
 */

// Determines the year of a game based on the start of the season and the month.
int getYear(int startYear, int month);

// Just extracts the day from a string date representation
int getDay(std::string date);

// Turns a three day string ("Jan", "Feb", etc.) into an integer
int getMonth(std::string date);

// Translates string representations of period (ie "Period 1", "Shootout", "OT") into integer representations
int getPeriod(std::string per);

// Converts a start time ("5:30 PM") into the number of minutes after midnight
int getMinutes(std::string time);



/*
 * ORPHANED FUNCTIONS
 *
 * Functions with nowhere else to go
 */

// takes the city of a team and translates it into their three-character team identifier
std::string translateTeamID(std::string s);

#endif
