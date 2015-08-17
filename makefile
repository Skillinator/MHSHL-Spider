all:
	g++ core.cpp mhshlUtils.cpp Game.cpp Team.cpp League.cpp Player.cpp ScoringEvent.cpp PenaltyEvent.cpp -lcurl -std=c++11
