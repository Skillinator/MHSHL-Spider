#ifndef _mhshlUtils
#define _mhshlUtils

#include <curl/curl.h>
#include <vector>
#include <string>
#include <iostream>
#include "mhshl.h"

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string str);
std::string fetchWebPage(std::string url);

void spaceBuffer(int len, std::string str);

std::string twoPlace(int num);

void showTeams(League l);

void showPlayers(League l);

void showGames(League l);

void showPenalties(League l);

void showGoals(League l);

void dumpStatus(League l);


#endif