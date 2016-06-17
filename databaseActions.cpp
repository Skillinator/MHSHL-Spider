#include <iostream>
#include "mhshl.h"
#include "mhshlUtils.h"
#include <string>
#include <mysql_connection.h>
#include <driver.h>
#include <exception.h>
#include <resultset.h>
#include <statement.h>

#include "databasePasswords.h"

sql::Driver *driver;
sql::Connection *con;

void startConnection(){
	driver = get_driver_instance();
	con = driver->connect("tcp://127.0.0.1:3306", user, pass);
}
void endConnection(){
	delete con;
}

void db_addTeam(bool varsity, std::string abbr, std::string name, std::string city, int id, int season){
	startConnection();
	
	sql::Statement *stmt;
	sql::ResultSet *res;
	if(!varsity){
		con->setSchema("jv");
	}else{
		con->setSchema("varsity");
	}
	stmt = con->createStatement();
	bool exists = false;
	
	res = stmt->executeQuery("SELECT * FROM teams WHERE abbr='" + abbr + "' AND season='" + std::to_string(season) + "'");
	
	while(res->next()){
		exists = true;
	}
	
	if(!exists){
		stmt->execute("INSERT INTO teams(abbr, name, city, id, season) VALUES('" + abbr + "','" + name + "','" + city + "'," + std::to_string(id) + "," + std::to_string(season) + ")");
	}
	
	endConnection();
	delete stmt;
	delete res;
}


void db_addPlayer(bool varsity, std::string tID, std::string name, int identifier, int number, int season){
	startConnection();
	
	sql::Statement *stmt;
	sql::ResultSet *res;
	if(!varsity){
		con->setSchema("jv");
	}else{
		con->setSchema("varsity");
	}
	stmt = con->createStatement();
	bool exists = false;
	
	res = stmt->executeQuery("SELECT * FROM players WHERE id=" + std::to_string(identifier) + " AND season=" + std::to_string(season));
	
	while(res->next()){
		exists = true;
	}
	
	if(!exists){
		stmt->execute("INSERT INTO players(team, name, id, number, season) VALUES('" + tID + "','" + name + "','" + std::to_string(identifier) + "'," + std::to_string(number) + "," + std::to_string(season) + ")");
	}
	
	endConnection();
	delete stmt;
	delete res;
}

void db_updatePlayer(bool varsity, int season, Player *p){
	startConnection();
	sql::Statement *stmt;
	sql::ResultSet *res;
	
	
	if(varsity){
		con->setSchema("varsity");
	}else{
		con->setSchema("jv");
	}
	
	stmt = con->createStatement();
	bool exists = false;
	
	res = stmt->executeQuery("SELECT * FROM players WHERE id=" + std::to_string(p->id) + " AND season=" + std::to_string(season));
	
	while(res->next()){
		exists = true;
	}
	
	if(!exists){
		db_addPlayer(varsity, p->team, p->name, p->id, p->number, season);
	}
	
	stmt->execute("UPDATE players SET gp = " + std::to_string(p->gp) + " WHERE id=" + std::to_string(p->id));
	
	if(p->goalie){
		stmt->execute("UPDATE players SET goalie = 1 WHERE id=" + std::to_string(p->id));
	}else{
		stmt->execute("UPDATE players SET goalie = 0 WHERE id=" + std::to_string(p->id));
	}
	
	endConnection();
	delete stmt;
	delete res;
	
	
}

void db_addGame(bool varsity, std::string id, int m, int d, int y, int start, std::string home, std::string away, int num){
	startConnection();
	
	sql::Statement *stmt;
	sql::ResultSet *res;
	if(!varsity){
		con->setSchema("jv");
	}else{
		con->setSchema("varsity");
	}
	stmt = con->createStatement();
	bool exists = false;
	
	res = stmt->executeQuery("SELECT * FROM games WHERE id='" + id + "'");
	
	while(res->next()){
		exists = true;
	}
	
	if(!exists){
		std::cout<<"Adding game";
		stmt->execute("INSERT INTO games(id, month, day, year, startTime, home, away, number) VALUES('" + id + "'," + std::to_string(m) + "," + std::to_string(d) + "," + std::to_string(y) + "," + std::to_string(start) + ",'" + home + "','" + away + "'," + std::to_string(num) + ")");
	}
	
	endConnection();
	delete stmt;
	delete res;
}

void db_updateGame(bool varsity, int season, Game *g){
	startConnection();
	sql::Statement *stmt;
	sql::ResultSet *res;
	
	if(varsity){
		con->setSchema("varsity");
	}else{
		con->setSchema("jv");
	}
	
	stmt = con->createStatement();
	bool exists=false;
	res = stmt->executeQuery("SELECT * FROM games WHERE id='" + g->id + "'");
	
	while(res->next()){
		exists = true;
	}
	
	if(!exists){
		db_addGame(varsity, g->id, g->month, g->day, g->year, g->startTime, g->homeTeam, g->awayTeam, g->number);
	}
	
	stmt->execute("UPDATE games SET time=" + std::to_string(g->time) + " WHERE id='" + g->id + "'");
	stmt->execute("UPDATE games SET period=" + std::to_string(g->period) + " WHERE id='" + g->id + "'");
	stmt->execute("UPDATE games SET homeShots=" + std::to_string(g->homeShots) + " WHERE id='" + g->id + "'");
	stmt->execute("UPDATE games SET awayShots=" + std::to_string(g->awayShots) + " WHERE id='" + g->id + "'");
	
	endConnection();
	delete stmt;
	delete res;
}

void db_addPenalty(bool varsity, std::string gID, std::string tID, int p, int d, int per, int t, std::string offense){
	startConnection();
	sql::Statement *stmt;
	sql::ResultSet *res;
	
	if(varsity){
		con->setSchema("varsity");
	}else{
		con->setSchema("jv");
	}
	
	stmt = con->createStatement();
	bool exists = false;
	res=stmt->executeQuery("SELECT * FROM penaltyEvents WHERE GameID='"+gID+"'&&player="+std::to_string(p)+"&&time="+std::to_string(t));
	
	while(res->next()){
		exists = true;
	}
	
	if(!exists){
		stmt->execute("INSERT INTO penaltyEvents(GameID, TeamID, player, duration, period, time, timeRemaining, offense) VALUES('" + gID + "', '" + tID + "', " + std::to_string(p) + ", " + std::to_string(d) + ", " + std::to_string(per) + ", " + std::to_string(t) + ", " + std::to_string(d) + ", '" + offense +"')");
	}	

	endConnection();
	delete stmt;
	delete res;
}

void db_updatePenalty(bool varsity, int season, PenaltyEvent *p){
	startConnection();
	sql::Statement *stmt;
	sql::ResultSet *res;
	
	if(varsity){
		con->setSchema("varsity");
	}else{
		con->setSchema("jv");
	}
	
	stmt = con->createStatement();
	bool exists = false;
	res=stmt->executeQuery("SELECT * FROM penaltyEvents WHERE GameID='"+p->gameID+"'&&player="+std::to_string(p->player)+"&&time="+std::to_string(p->time));

	stmt->execute("UPDATE penaltyEvents SET timeRemaining=" + std::to_string(p->timeRemaining) + " WHERE GameID='" + p->gameID + "'&&player=" + std::to_string(p->player) + "&&time=" + std::to_string(p->time));
	stmt->execute("UPDATE penaltyEvents SET goalsWhile=" + std::to_string(p->goalsWhile) + " WHERE GameID='" + p->gameID + "'&&player=" + std::to_string(p->player) + "&&time=" + std::to_string(p->time));
	endConnection();
	delete stmt;
	delete res;
}

void db_addGoal(bool varsity, int season, std::string GameID, std::string TeamID, int s, int a1, int a2, int per, int t, int pp){
	startConnection();
	sql::Statement *stmt;
	sql::ResultSet *res;
	
	if(varsity){
		con->setSchema("varsity");
	}else{
		con->setSchema("jv");
	}
	
	stmt = con->createStatement();
	bool exists=false;
	res=stmt->executeQuery("SELECT * FROM scoringEvents WHERE GameID='" + GameID + "'&&period="+std::to_string(per)+"&&time="+std::to_string(t));
	
	while(res->next()){
		exists = true;
	}
	
	if(!exists){
		stmt->execute("INSERT INTO scoringEvents(GameID, TeamID, scorer, a1, a2, period, time, pp) VALUES('" + GameID + "', '" + TeamID + "', " + std::to_string(s) + ","+std::to_string(a1)+","+std::to_string(a2)+","+std::to_string(per) + "," + std::to_string(t) + "," + std::to_string(pp) + ")");
	}
	
	endConnection();
	delete stmt;
	delete res;

}
