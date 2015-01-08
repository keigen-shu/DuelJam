//  UI/MusicSelector.hpp :: Music selection UI component
//  Copyright 2013 - 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef SCENE_MUSIC_SELECTOR_H
#define SCENE_MUSIC_SELECTOR_H

#include "../__zzCore.hpp"
#include "../Music.hpp"

namespace UI {

class MusicSelector : public clan::View
{
private:
	MusicList   mMusicList; //!< List of musics available to put on the wheel.

	////    Music Entry Style Elements    ////
	sizei mbrMEsize;        //!< Music entry bounding box size.
	vec2i mbrMEoffset;      //!< Music entry render offset. `<` shape
	clan::Font mtfMEartist; //!< Artist text font.
	clan::Font mtfMEtitle;  //!< Title text font.
	clan::Font mtfMElevel;  //!< Difficulty level text font.
	rectf mtrMEartist;      //!< Artist text rect, normalized to bounding box.
	rectf mtrMEtitle;       //!< Title text rect, normalized to bounding box.
	rectf mtrMElevel;       //!< Difficulty level text rect, normalized to bounding box.

	////    Selected Chart Style Elements    ////
	clan::Image miCoverArt;

	////    Input response variables    ////
	int milDelay;      //!< Pointer movement locking period ; time taken before pointer can move to the next element.
	int milCounter;    //!< Pointer movement locking counter; time elapsed since pointer last moved.

	int mwsCount;      //!< Maximum number of elements to display on UI
	int mwsTop;        //!< Offset to top visible element
	int mwsPointer;    //!< Offset to currently selected element
	int mwsPrevious;   //!< Offset to previously selected element
	int mwsStorage;    //!< Storage for if user cancels

	uchar mwsLvlIndex; //!< Selected chart level/index

public:
	MusicSelector(MusicList const &list);

	void load_theme();
	void reset_wheel();

	std::shared_ptr<Chart> get() const;

	void render_entry(clan::Canvas& canvas, const Music* music);

	////    clan::View overrides
	void render_content(clan::Canvas& canvas) override;

private:
	////    clan::View callbacks
	void on_key_press(clan::KeyEvent& event);
	void on_key_release(clan::KeyEvent& event);
	void on_pointer_press(clan::PointerEvent& event);
};

}

#endif
