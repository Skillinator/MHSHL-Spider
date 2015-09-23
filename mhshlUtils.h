#ifndef _mhshlUtils
#define _mhshlUtils

#include <curl/curl.h>
#include <vector>
#include <string>
#include <iostream>
#include "mhshl.h"

std::vector<std::string> split(std::string str, std::string delim);
std::string extract(std::string str, std::string tag);

std::string fetchWebPage(std::string url);

void spaceBuffer(int len, std::string str);

std::string twoPlace(int num);

void showTeams(League l);

void showPlayers(League l);

void showGames(League l);

void showPenalties(League l);

void showGoals(League l);

void dumpStatus(League l);



int getYear(int startYear, int month);

std::string translateTeamID(std::string s);

int getDay(std::string date);

int getMonth(std::string date);

int getPeriod(std::string per);

int getMinutes(std::string time);

std::string getValue(std::string str, std::string value);



#endif