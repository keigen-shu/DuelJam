//  Main.hpp :: ClanLib application definition
//  Copyright 2013 Keigen Shu

#ifndef MAIN_H
#define MAIN_H

#include <ClanLib/application.h>
#include <memory>

class Game;
class Chart;

class App
{
public:
	static bool  gDebug;
	static bool  gNoSound;

	static std::shared_ptr<Game> gGame;

	static int main(std::vector<std::string> const &args);

	static void launchChart(std::shared_ptr<Chart> chart);
};

#endif
