#include "Game.hpp"
#include "Main.hpp"

JSONFile Game::conf("conf.json");
JSONFile Game::skin("skin.json");

Game::Game(clan::DisplayWindowDescription &_clDWD)
	: clan::WindowView(_clDWD)
	, clDW(get_display_window())
	, clGC(clDW.get_gc())
	, clCv(clDW)
	, am  (conf.getInteger("audio.frame-rate"), conf.getInteger("audio.sample-rate"))
	, im  (get_display_window().get_ic())
	, font(SimpleFont::fromJSON(clGC, "Theme/base_font.json"))
	, keep_alive(true)
{
	set_focus_policy(clan::FocusPolicy::accept);

	slots.connect(sig_close    (), this, &Game::on_close);
	slots.connect(sig_key_press(clan::EventUIPhase::bubbling), this, &Game::on_key_press);
}

void Game::on_close(clan::CloseEvent &e)
{
	clan::Console::write_line("WM requested for application closure.");
	keep_alive = false;
}

void Game::on_key_press(clan::KeyEvent &event)
{
	switch(event.key())
	{
		case clan::Key::escape:
			clan::Console::write_line("Escape requested.");
			keep_alive = false;
			break;
		case clan::Key::f12:
			App::gDebug = !App::gDebug;
			clan::Console::write_line(std::string{ "Toggling debug to " }.append(App::gDebug ? "ON" : "OFF"));
			break;
		default:
			return;
	}

	event.stop_propagation();
}

std::shared_ptr<Game> Game::create()
{
	if (App::gGame != nullptr)
	{
		clan::Console::write_line("Tearing down previous game session.");
		App::gGame = nullptr;
	}

	Game::conf.load();
	Game::skin.load();

	////    Configure global variables
	App::gDebug = conf.get_or_set(
			&JSONReader::getBoolean, "debug", false
			);

	////    Initialize display
	const sizei displayResolution = conf.get_if_else_set(
			&JSONReader::getVec2i, "video.resolution", vec2i{ 640, 480 },
			[] (const vec2i &value) -> bool {
				return value.x > 640 && value.y > 480;
			});

	clan::DisplayWindowDescription displayWD(
			"DuelJam", displayResolution, true
			);

	displayWD.set_allow_resize      (false);
	displayWD.set_allow_screensaver (false);
	displayWD.set_fullscreen        (conf.getBoolean("video.fullscreen"));
	displayWD.set_refresh_rate      (conf.getInteger("video.refresh-rate"));
	displayWD.set_swap_interval     (conf.getInteger("video.flip-interval"));

	App::gGame = std::make_shared<Game>(displayWD);

	return App::gGame;
}
