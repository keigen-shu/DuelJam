#include "MusicSelector.hpp"
#include "../Main.hpp" // Access to Main::gGame;
#include "../Game.hpp" // Access to Main::gGame;
#include "../clanExt_Alignment.hpp"

namespace UI {

const Music kRandomMusic = Music {
    .path   = "",
        .title  = "RANDOM",
        .artist = "Someone",
        .genre  = "",
        .charts = ChartMap{}
};

clan::ColorHSLf getChartColor(const Chart &chart) {
    float level = static_cast<float>( 48 - std::min(chart.cgetInfo().level, uint(64)) );
    level = pow(level / 48.0f, 2.0f / 3.0f);
    return clan::ColorHSLf(240.0f * level, 1.0f, 0.25f, 1.0f);
}


void MusicSelector::load_theme()
{
    JSONReader& skin = App::gGame->skin;

    mbrMEsize = skin.get_or_set(
            &JSONReader::getVec2i, "theme.music-selector.entry.size",
            vec2i(320, 40));
    mbrMEoffset = skin.get_or_set(
            &JSONReader::getVec2i, "theme.music-selector.entry.offset",
            vec2i(10, 10));

    mtrMEartist = skin.get_or_set(
            &JSONReader::getRectf, "theme.music-selector.entry.rect-artist",
            rectf(0.2f, 0.0f, 1.0f, 0.4f)
            );
    mtrMEtitle = skin.get_or_set(
            &JSONReader::getRectf, "theme.music-selector.entry.rect-title",
            rectf(0.2f, 0.4f, 1.0f, 1.0f)
            );
    mtrMElevel = skin.get_or_set(
            &JSONReader::getRectf, "theme.music-selector.entry.rect-level",
            rectf(0.0f, 0.0f, 0.2f, 1.0f)
            );

    mtfMEartist = skin.getFont("theme.music-selector.entry.font-artist");
    mtfMEtitle  = skin.getFont("theme.music-selector.entry.font-title");
    mtfMElevel  = skin.getFont("theme.music-selector.entry.font-level");
}

void MusicSelector::reset_wheel()
{
    milDelay    = 1;
    milCounter  = 0;

    mwsCount    = 10;
    mwsTop      = mwsPointer  = 0;
    mwsPrevious = mwsStorage  = -1;

    mwsLvlIndex = 0;
}

std::shared_ptr<Chart> MusicSelector::get() const
{
    if (mwsPointer < 0)
        return nullptr;

    MusicList::const_iterator ip = mMusicList.begin(); // Selected music
    for(int i = 0; i < mwsPointer ; ip++, i++);

    uchar c = std::min(mwsLvlIndex, uchar((*ip)->charts.size() - 1));
    return (*ip)->charts.at(c);
}



MusicSelector::MusicSelector(MusicList const &list)
    : clan::View()
    , mMusicList(list)
{
    load_theme();
    reset_wheel();

    box_style.set_fixed ();
    box_style.set_width (App::gGame->clCv.get_size().width );
    box_style.set_height(App::gGame->clCv.get_size().height);

    set_focus_policy(clan::FocusPolicy::accept);

    slots.connect(sig_key_press  (), this, &MusicSelector::on_key_press);
    slots.connect(sig_key_release(), this, &MusicSelector::on_key_release);
    slots.connect(sig_pointer_press(), this, &MusicSelector::on_pointer_press);
}

void MusicSelector::render_entry(clan::Canvas& canvas, const Music* music)
{
    size_t chIndex = std::min(mwsLvlIndex, uchar(music->charts.size() - 1));

    clan::Colorf    cTextShadow { 0.0f, 0.0f, 0.0f, 0.5f };
    clan::ColorHSLf cLevelGrade = getChartColor(*music->charts.at(chIndex));

    rectf rArtist(
            mbrMEsize.width * mtrMEartist.left , mbrMEsize.height * mtrMEartist.top,
            mbrMEsize.width * mtrMEartist.right, mbrMEsize.height * mtrMEartist.bottom
            );
    rectf rTitle(
            mbrMEsize.width * mtrMEtitle.left , mbrMEsize.height * mtrMEtitle.top   ,
            mbrMEsize.width * mtrMEtitle.right, mbrMEsize.height * mtrMEtitle.bottom
            );
    rectf rLevel(
            mbrMEsize.width * mtrMElevel.left , mbrMEsize.height * mtrMElevel.top,
            mbrMEsize.width * mtrMElevel.right, mbrMEsize.height * mtrMElevel.bottom
            );

    canvas.fill_rect(rArtist, clan::Colorf::grey);
    canvas.fill_rect(rTitle , clan::Colorf::grey);
    canvas.fill_rect(rLevel , cLevelGrade); cLevelGrade.s = 0.8f; cLevelGrade.l = 0.8f;

    canvas.draw_box (rectf(0.f, 0.f, mbrMEsize.width, mbrMEsize.height), clan::Colorf::white);
    canvas.draw_box (rLevel , clan::Colorf::white);

    rArtist.shrink(4);
    rTitle .shrink(4);
    rLevel .shrink(2);

    draw_aligned_text(canvas, mtfMEartist, rArtist, Alignment::LB, music->artist, cTextShadow); rArtist.translate( -1, -1 );
    draw_aligned_text(canvas, mtfMEartist, rArtist, Alignment::LB, music->artist);

    draw_aligned_text(canvas, mtfMEtitle, rTitle, Alignment::LB, music->title, cTextShadow); rTitle.translate( -1, -1 );
    draw_aligned_text(canvas, mtfMEtitle, rTitle, Alignment::LB, music->title);

    auto level = std::to_string(music->charts.at(chIndex)->cgetInfo().level);
    draw_aligned_text(canvas, mtfMElevel, rLevel, Alignment::CB, level, cTextShadow); rLevel.translate( -1, -1 );
    draw_aligned_text(canvas, mtfMElevel, rLevel, Alignment::CB, level, cLevelGrade);

    size_t i = 0;
    auto it = music->charts.cbegin();
    for(;it != music->charts.cend(); i++, it++)
    {
        rectf r( i    * 10 + 1, mbrMEsize.height - 10,
                (i+1) * 10 - 1, mbrMEsize.height - 2
               );

        if (i == chIndex) {
            quadf q(r); q.rotate(q.center(), clan::Angle::from_degrees(45.0f));
            canvas.fill_triangles(
                    { q.q, q.p, q.s, q.q, q.s, q.r },
                    cLevelGrade
                    );
            std::vector<vec2f> p = { q.p, q.q, q.r, q.s, q.p };
            canvas.draw_line_strip( p.data(), 5 );
        } else {
            canvas.fill_rect(
                    1 + i * 10, mbrMEsize.height - 10, (i+1) * 10 - 1, mbrMEsize.height - 2,
                    cLevelGrade
                    );
        }

    }
}

void MusicSelector::render_content(clan::Canvas& canvas)
{
    // Top-of-screen and currently-pointed iterators
    MusicList::const_iterator it = mMusicList.cbegin(); // Top / current iteration
    MusicList::const_iterator ip = mMusicList.cbegin(); // Selected item
    MusicList::const_iterator il = mwsPrevious == -1 ? mMusicList.cend() : mMusicList.cbegin(); // Previous item

    for(int i=0; i<mwsTop     ; it++, i++);
    for(int i=0; i<mwsPointer ; ip++, i++);
    for(int i=0; i<mwsPrevious; il++, i++);

    //  Update cover art if needed.
    if (ip == mMusicList.cend()) {
        // TODO Add random selection background image
        miCoverArt = clan::Image();
    } else if (ip != il) {
        if (il != mMusicList.cend()) {
            (*ip)->charts.begin()->second->load_cover_art();
        }

        mwsPrevious = mwsPointer;

        auto pbCoverArt = (*ip)->charts.begin()->second->getCoverArt();
        if (pbCoverArt.is_null() == false) {
            miCoverArt = clan::Image(canvas, pbCoverArt, pbCoverArt.get_size());
            miCoverArt.set_alpha(1.0f / 3.0f);
        }
    }

    //  Draw cover art
    if (miCoverArt.is_null() == false)
    {
        float scale = std::min(
            static_cast<float>(geometry().content.get_width ()) / static_cast<float>(miCoverArt.get_width ()),
            static_cast<float>(geometry().content.get_height()) / static_cast<float>(miCoverArt.get_height())
        );

        miCoverArt.draw( canvas, align(
            Alignment::CC,
            rectf(0.f, 0.f, geometry().content.get_size()),
            static_cast<sizef>(miCoverArt.get_size()) * scale
        ) );
    }

    auto matOriginal = canvas.get_transform();
    for(int i = 0; i < mwsCount; it++, i++)
    {
        vec2i entryPos(
                canvas.get_width() - mbrMEsize.width * ((it == ip) ? 1.0f : 0.75f),
                i * (mbrMEsize.height + mbrMEoffset.y)
                );
        canvas.set_transform(matOriginal * clan::Mat4f::translate(entryPos.x, entryPos.y, 0.0f));

        if (it != mMusicList.cend())
            render_entry(canvas, *it);
    }

    canvas.set_transform(matOriginal);

    set_needs_render();
}

void MusicSelector::on_key_press(clan::KeyEvent& event)
{
    switch (event.key())
    {
        case clan::Key::up:
            if (milCounter > 0) {
                milCounter--;
            } else {
                mwsPointer = mwsPointer > 0 ? mwsPointer - 1 : 0;
                milCounter = milDelay;
            }
            break;

        case clan::Key::down:
            if (milCounter > 0) {
                milCounter--;
            } else {
                mwsPointer = mwsPointer < mMusicList.size() ? mwsPointer + 1 : mMusicList.size();
                milCounter = milDelay;
            }
            break;

        case clan::Key::left:
            mwsLvlIndex -= mwsLvlIndex > 0 ? 1 : 0;
            break;

        case clan::Key::right:
            mwsLvlIndex++;
            break;

        default: // Do not consume
            return;
    }

    if((mwsPointer - mwsTop) > (mwsCount - 1))
        mwsTop = mwsPointer  - (mwsCount - 1);
    else if ((mwsPointer - mwsTop) <= 0)
        mwsTop = mwsPointer;

    event.stop_propagation();
}

void MusicSelector::on_key_release(clan::KeyEvent& event)
{
    switch (event.key())
    {
        case clan::Key::key_return:
            mwsStorage = mwsPointer;
            event.stop_propagation();
            set_hidden();
            break;
        default: // Do not consume
            milCounter--;
            return;
    }
}

void MusicSelector::on_pointer_press(clan::PointerEvent& event)
{
    clan::Console::write_line("MusicSelector::on_pointer_press");
}

}
