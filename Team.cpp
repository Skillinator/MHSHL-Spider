#include <iostream>
#include <string>
#include <vector>
#include "mhshl.h"

Team::Team(){
	abbreviation = "NUL";
	name = city = "NULL";
	id = gp = w = l = ot = so = pts = gf = ga = pim = streak = 0;
}

Team::Team(std::string abbr, std::string n, std::string c, int identifier){
	abbreviation = abbr;
	name = n;
	city = c;
	id = identifier;
	gp = w = l = ot = so = pts = gf = ga = pim = streak = 0;
}

/*
Quad City 9
Dubuque
Waterloo
Cedar Rapids 12
Ames
Des Moines
Sioux City
Mason City
Kansas City
Lincoln
Omaha

Blues
Devils
Warriors
Jr. Roughriders
Little Cyclones
Capitals
Oak Leafs
Metros
Jets
Mohawks
Stars
Jr. Lancers
17
*/

void Team::printHeading(){
	std::cout<<"\n abbr      city            name        gp   w   l  ot  so  pts  gf  ga  pim  streak\n";
}

void Team::printString(){
	spaceBuffer(6, abbreviation);
	spaceBuffer(14, city);
	spaceBuffer(18, name);
	spaceBuffer(4, std::to_string(gp));
	spaceBuffer(4, std::to_string(w));
	spaceBuffer(4, std::to_string(l));
	spaceBuffer(4, std::to_string(ot));
	spaceBuffer(4, std::to_string(so));
	spaceBuffer(5, std::to_string(pts));
	spaceBuffer(4, std::to_string(gf));
	spaceBuffer(4, std::to_string(ga));
	spaceBuffer(5, std::to_string(pim));
	spaceBuffer(8, std::to_string(streak));
	std::cout<<"\n";
}