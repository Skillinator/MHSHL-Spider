all:
	g++ -I/usr/include/cppconn core.cpp mhshlUtils.cpp parsingFunctions.cpp Game.cpp Team.cpp GoaliePerformance.cpp databaseActions.cpp League.cpp Player.cpp ScoringEvent.cpp PenaltyEvent.cpp -lcurl -L/usr/lib -lmysqlcppconn -std=c++11 -o mhshl_backend
