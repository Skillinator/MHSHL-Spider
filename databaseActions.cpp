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

void setSchema(bool varsity){
	if(!varsity){
		con->setSchema("jv");
	}else{
		con->setSchema("varsity");
	}
}

void db_addTeam(Team team, League league){
	startConnection();

	sql::Statement *stmt;
	sql::ResultSet *res;

	setSchema(league.varsity);

	stmt = con->createStatement();
	bool exists = false;

	res = stmt->executeQuery("SELECT * FROM teams WHERE abbr='" + team.abbreviation + "' AND season='" + std::to_string(league.season) + "'");

	while(res->next()){
		exists = true;
	}

	if(!exists){
		stmt->execute("INSERT INTO teams(abbr, name, city, id, season) VALUES('" + team.abbreviation + "','" + team.name + "','" + team.city + "'," + std::to_string(team.pointstreakID) + "," + std::to_string(league.season) + ")");
	}

	endConnection();
	delete stmt;
	delete res;
}

void db_addPlayer(Player player, League league){
	startConnection();

	sql::Statement *stmt;
	sql::ResultSet *res;

	setSchema(league.varsity);

	stmt = con->createStatement();
	bool exists = false;

	res = stmt->executeQuery("SELECT * FROM players WHERE id=" + std::to_string(player.pointstreakID) + " AND season=" + std::to_string(league.season));

	while(res->next()){
		exists = true;
	}

	if(!exists){
		stmt->execute("INSERT INTO players(team, name, id, number, season) VALUES('" + player.teamID + "','" + player.name + "','" + std::to_string(player.pointstreakID) + "'," + std::to_string(player.jerseyNumber) + "," + std::to_string(league.season) + ")");
	}

	endConnection();
	delete stmt;
	delete res;
}

void db_updatePlayer(Player player, League league){
	startConnection();
	sql::Statement *stmt;
	sql::ResultSet *res;

	setSchema(league.varsity);

	stmt = con->createStatement();
	bool exists = false;

	res = stmt->executeQuery("SELECT * FROM players WHERE id=" + std::to_string(player.pointstreakID) + " AND season=" + std::to_string(league.season));

	while(res->next()){
		exists = true;
	}

	if(!exists){
		db_addPlayer(player, league);
	}

	stmt->execute("UPDATE players SET gp = " + std::to_string(player.gamesPlayed) + " WHERE id=" + std::to_string(player.pointstreakID));

	if(player.goalie){
		stmt->execute("UPDATE players SET goalie = 1 WHERE id=" + std::to_string(player.pointstreakID));
	}else{
		stmt->execute("UPDATE players SET goalie = 0 WHERE id=" + std::to_string(player.pointstreakID));
	}

	endConnection();
	delete stmt;
	delete res;


}

void db_addGame(Game game, League league){
	startConnection();

	sql::Statement *stmt;
	sql::ResultSet *res;

	setSchema(league.varsity);

	stmt = con->createStatement();
	bool exists = false;

	res = stmt->executeQuery("SELECT * FROM games WHERE id='" + game.id + "'");

	while(res->next()){
		exists = true;
	}

	if(!exists){
		std::cout<<"Adding game";
		stmt->execute("INSERT INTO games(id, month, day, year, startTime, home, away, number, season) VALUES('" + game.id + "'," + std::to_string(game.month) + "," + std::to_string(game.day) + "," + std::to_string(game.year) + "," + std::to_string(game.startTime) + ",'" + game.homeTeam + "','" + game.awayTeam + "'," + std::to_string(game.number) + "," + std::to_string(league.season) + ")");
	}

	endConnection();
	delete stmt;
	delete res;
}

void db_updateGame(Game game, League league){
	startConnection();
	sql::Statement *stmt;
	sql::ResultSet *res;

	setSchema(league.varsity);

	stmt = con->createStatement();
	bool exists=false;
	res = stmt->executeQuery("SELECT * FROM games WHERE id='" + game.id + "'");

	while(res->next()){
		exists = true;
	}

	if(!exists){
		db_addGame(game, league);
	}

	stmt->execute("UPDATE games SET time=" + std::to_string(game.time) + " WHERE id='" + game.id + "'");
	stmt->execute("UPDATE games SET period=" + std::to_string(game.period) + " WHERE id='" + game.id + "'");
	stmt->execute("UPDATE games SET homeShots=" + std::to_string(game.homeShots) + " WHERE id='" + game.id + "'");
	stmt->execute("UPDATE games SET awayShots=" + std::to_string(game.awayShots) + " WHERE id='" + game.id + "'");

	endConnection();
	delete stmt;
	delete res;
}

void db_addPenalty(PenaltyEvent penalty, League league){
	startConnection();
	sql::Statement *stmt;
	sql::ResultSet *res;

	setSchema(league.varsity);

	stmt = con->createStatement();
	bool exists = false;
	res=stmt->executeQuery("SELECT * FROM penaltyEvents WHERE GameID='"+penalty.gameID+"'&&player="+std::to_string(penalty.player)+"&&time="+std::to_string(penalty.time));

	while(res->next()){
		exists = true;
	}

	if(!exists){
		stmt->execute("INSERT INTO penaltyEvents(GameID, TeamID, player, duration, period, time, timeRemaining, offense, season) VALUES('" + penalty.gameID + "', '" + penalty.teamID + "', " + std::to_string(penalty.player) + ", " + std::to_string(penalty.duration) + ", " + std::to_string(penalty.period) + ", " + std::to_string(penalty.time) + ", " + std::to_string(penalty.timeRemaining) + ", '" + penalty.offense + "'," + std::to_string(league.season) + ")");
	}

	endConnection();
	delete stmt;
	delete res;
}

void db_updatePenalty(PenaltyEvent penalty, League league){
	startConnection();
	sql::Statement *stmt;
	sql::ResultSet *res;

	setSchema(league.varsity);

	stmt = con->createStatement();
	bool exists = false;
	res=stmt->executeQuery("SELECT * FROM penaltyEvents WHERE GameID='"+penalty.gameID+"'&&player="+std::to_string(penalty.player)+"&&time="+std::to_string(penalty.time));

	while(res->next()){
		exists = true;
	}

	if(!exists){
		db_addPenalty(penalty, league);
	}


	stmt->execute("UPDATE penaltyEvents SET timeRemaining=" + std::to_string(penalty.timeRemaining) + " WHERE GameID='" + penalty.gameID + "'&&player=" + std::to_string(penalty.player) + "&&time=" + std::to_string(penalty.time));
	stmt->execute("UPDATE penaltyEvents SET goalsWhile=" + std::to_string(penalty.goalsWhile) + " WHERE GameID='" + penalty.gameID + "'&&player=" + std::to_string(penalty.player) + "&&time=" + std::to_string(penalty.time));
	endConnection();
	delete stmt;
	delete res;
}

void db_addGoal(ScoringEvent goal, League league){
	startConnection();
	sql::Statement *stmt;
	sql::ResultSet *res;

	setSchema(league.varsity);

	stmt = con->createStatement();
	bool exists=false;
	res=stmt->executeQuery("SELECT * FROM scoringEvents WHERE GameID='" + goal.gameID + "'&&period="+std::to_string(goal.period)+"&&time="+std::to_string(goal.time));

	while(res->next()){
		exists = true;
	}

	if(!exists){
		stmt->execute("INSERT INTO scoringEvents(GameID, TeamID, scorer, a1, a2, period, time, pp, season) VALUES('" + goal.gameID + "', '" + goal.teamID + "', " + std::to_string(goal.scorer) + ","+std::to_string(goal.assist1)+","+std::to_string(goal.assist2)+","+std::to_string(goal.period) + "," + std::to_string(goal.time) + "," + std::to_string(goal.powerPlay) + "," + std::to_string(league.season) + ")");
	}

	endConnection();
	delete stmt;
	delete res;

}
