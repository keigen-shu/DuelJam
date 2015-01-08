#include "__zzCore.hpp"

#ifdef __USE_D3D
#include <ClanLib/d3d.h>
#else
#include <ClanLib/gl.h>
#endif

#include "Main.hpp"
#include "Game.hpp"

#include "MusicScanner.hpp"
#include "Models/Tracker.hpp"

#include "Scenes/MusicSelector.hpp"

clan::Application app(&App::main, false);

bool App::gDebug   = false;
bool App::gNoSound = false;
std::shared_ptr<Game> App::gGame = nullptr;

int App::main(const std::vector<std::string> & args)
{
	clan::SetupCore     _SetupCore;
	clan::SetupDisplay  _SetupDisplay;
#ifdef __USE_D3D
	clan::SetupD3D      _SetupD3DRender;
#else
	clan::SetupGL       _SetupGLRender;
#endif

	try
	{
		Game::create();

		// TODO Read console arguments.
		MusicList   ML = scan_music_dir("./Music");
		auto        MS = std::make_shared<UI::MusicSelector>(ML);
		std::shared_ptr<Tracker> CT;

		gGame->add_subview(MS);
		MS->set_focus();

		while(gGame->keep_alive)
		{
			clan::KeepAlive::process(20);
			if (MS->hidden() && MS->get())
			{
				if (CT) {
					if (CT->hasChartEnded())
					{
						MS->set_hidden(false);
						gGame->show();
						CT = nullptr;
					} else {
						gGame->hide();
						CT->update();
						gGame->am.play(CT->getNAs());
						CT->getNAs().clear();
					}
				} else {
					// Initiate chart
					auto chart = MS->get();
					launchChart(chart);
					CT = std::make_shared<Tracker>(chart, JHard, Tracker::KeyBindings{}, nullptr);
					CT->getClock()->start();
					clan::Console::write_line("Tracker clock started.");
				}
			}
		}
	}
	catch (clan::Exception& e)
	{
		clan::ConsoleWindow console { "DuelJam Console", 80, 160 };
		clan::Console::write_line("Exception caught: " + e.get_message_and_stack_trace());
		console.display_close_message();
	}
	return 0;
}

void App::launchChart(std::shared_ptr<Chart> chart)
{
	chart->load_chart();
	chart->load_samples();
	gGame->am.wipe_SampleMap( );
	gGame->am.swap_SampleMap(*chart->getSampleMap());
}
